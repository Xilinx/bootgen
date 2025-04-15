
/******************************************************************************
* Copyright 2015-2022 Xilinx, Inc.
* Copyright 2022-2023 Advanced Micro Devices, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "hss.h"
#include "hss_verify_inc.h"
#include "hss_sign_inc.h"
#include "lms_hash.h"
#include "hss_zeroize.h"
#include "lms-utils.h"

/*
------------------------------------------------------------------------------
****************************************************   F U N C T I O N S   ***
------------------------------------------------------------------------------
*/
/* When we generate a key, these are the parameters we use (unless the */
   /* user has specified otherwise). For signature generation/verification, */
   /* we use the parameters from the private key (for signature */
   /* generation) or the public key and signature (for verification) */

   /* By default, we use Merkle trees with two levels */
   /* Top tree has 20 levels, bottom tree has 10 (and so it's 20/10 in the */
   /* notation we use elsewhere) */
   /* We use Winternitz 8 for both trees; this minimizes the signature size */
   /* This gives us a reasonable genkey time (3 minutes with threading), */
   /* good load times (perhaps 1 second), and a billion signatures per key */
const char *default_parm_set = "20/8/sha256,10/8/sha256";

#define DEFAULT_AUX_DATA 10916   /* Use 10+k of aux data (which works well */
                            /* with the above default parameter set) */

static const char *seedbits = 0;
static const char *i_value = 0;
static bool convert_specified_seed_i_value( void *, size_t );


/*
 * This is a function that is supposed to generate truly random values.
 * This is a hideous version of this; this needs to be replaced by something
 * secure in a real product
 */
bool do_rand( void *output, size_t len ) {
    if (seedbits) {
        /* The seed was specified on the command line */
        /* Return that exact seed and i */
        /* This is not something a real application should do */
        return convert_specified_seed_i_value( output, len );
    }
    struct {
        unsigned char dev_random_output[32];
        int rand_output[16];
        /* Potentially more random sources here */
        unsigned count;
    } buffer;
    int i;

    /* Try to grab a sammple of /dev/urandom output */
    /* We use /dev/urandom because there's no point in blocking; this is a */
    /* demo program */
    FILE *f = fopen( "/dev/urandom", "r" );
    if (f) {
         (void)fread( buffer.dev_random_output, 1, 32, f );
         fclose(f);
    }

    /* Also try to grab some output from rand */
    /* It's not great, but if the /dev/urandom output fails, at least we */
    /* have something */
    /* In a real program, we'd want to fail if we don't have enough */
    /* entropy, but hey, this is a demo */
    static int set_seed = 0;
    if (!set_seed) {
        srand( time(0) );
        set_seed = 1;
    }
    for (i = 0; i<16; i++) {
        buffer.rand_output[i] = rand();
    }


    /* If we had more random sources, we'd sample them here */

    unsigned output_buffer[32];
    for (i=0; len>0; i++) {
        buffer.count = i;

        /* Ok, hash all our random samples together to generate the random */
        /* string that was asked for */
        hss_hash( output_buffer, HASH_SHA256, &buffer, sizeof buffer );

        /* Copy that hash to the output buffer */
        int this_len = 32;
        if (this_len > len) this_len = len;
        memcpy( output, output_buffer, this_len );

        /* Advance pointers */
        output = (unsigned char *)output + this_len; len -= this_len;
    }

    /* Clean up after ourselves.  Yes, this is a demo program; doesn't mean */
    /* we get to be sloppy */
    hss_zeroize( output_buffer, sizeof output_buffer );
    hss_zeroize( &buffer, sizeof buffer );

    return true;
}

static int fromhex(char c) {
    if (isdigit(c)) return c - '0';
    switch (c) {
    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
    default: return 0;  /* Turn any nonhexdigit into a 0 */
    }
}

/*
 * This is used if the user maually specified the seed and the
 * i values
 * This converts what the user specified into the format that
 * the library expects
 */
static bool convert_specified_seed_i_value( void *buffer, size_t len) {
    int i;
    const char *in = seedbits; 
    unsigned char *out = buffer;
    for (i=0; i<len; i++) {
        /* After 32 bytes of seed, then comes the i value */
        if (i == 32) {
            in = i_value;
        }
        int c = fromhex(*in); if (*in) in++;
        int d = fromhex(*in); if (*in) in++;
        *out++ = 16*c + d;
    }

    return true;
}
/*
 * This parses the parameter set; this is provided so we can try different
 * sets without recompiling the program each time.  This is placed here
 * because it's ugly parsing code that has nothing to do with how to use
 * HSS
 */
static int parse_param_set(param_set_t *lm_array, param_set_t *ots_array, int *hash, int* h, int* w, int levels)
{
	int i;
	int hash_type;

	if (levels > 2) {
		printf("[ERROR]  : Telluride supports only 2 HSS levels\n");
		return 0;
	}
	for (i = 0; i < levels; i++)
	{
		switch (hash[i])
		{
		case 1:	hash_type = HASH_SHA256; break;
		case 2:	hash_type = HASH_SHAKE256; break;
		default: printf("[ERROR]  : Unsupported Hash algorithm for LMS - %s\n", (hash_type == HASH_SHA256) ? "SHA256" : "SHAKE256");
			printf("           Supported hash algorithms = SHA256, SHAKE256\n");
			return 0;
		}
		param_set_t lm;
		switch (h[i])
		{
		case 5:  lm = (hash_type == HASH_SHA256) ? LMS_SHA256_N32_H5 : LMS_SHAKE256_N32_H5;  break;
		case 10: lm = (hash_type == HASH_SHA256) ? LMS_SHA256_N32_H10 : LMS_SHAKE256_N32_H10;  break;
		case 15: lm = (hash_type == HASH_SHA256) ? LMS_SHA256_N32_H15 : LMS_SHAKE256_N32_H15; break;
		case 20: lm = (hash_type == HASH_SHA256) ? LMS_SHA256_N32_H20 : LMS_SHAKE256_N32_H20; break;
		default: printf("[ERROR]  : Unsupported Merkle tree height (h) - %d\n", h[i]);
			printf("           Supported heights = 5, 10, 15, 20\n");
			return 0;
		}
		/* Now see if we can get the Winternitz parameter */
		param_set_t ots = LMOTS_SHA256_N32_W8;
		switch (w[i])
		{
		case 2: ots = (hash_type == HASH_SHA256) ? LMOTS_SHA256_N32_W2 : LMOTS_SHAKE256_N32_W2; break;
		case 4: ots = (hash_type == HASH_SHA256) ? LMOTS_SHA256_N32_W4 : LMOTS_SHAKE256_N32_W4; break;
		case 8: ots = (hash_type == HASH_SHA256) ? LMOTS_SHA256_N32_W8 : LMOTS_SHAKE256_N32_W8; break;
		default: printf("[ERROR]  : Unsupported Winternitz parameter (w) - %d\n", w[i]);
			printf("           Supported parameters = 2, 4, 8\n");
			return 0;
		}
		lm_array[i] = lm;
		ots_array[i] = ots;
	}
	return 1;
}

static void list_parameter_set(int levels, const param_set_t *lm_array,
                           const param_set_t *ots_array, size_t aux_size, int* hash ) {
	printf("[INFO]   : LMS parameter set: \n");
	printf("\t\tMerkel Tree Levels - %d\n", levels );
    int i;
    for (i=0; i<levels; i++) {
        printf( "\t\t   Level %d: hash function = %s; ", i, (hash[i] == 1) ? "SHA256" : "SHAKE256");
        int h = 0;
        switch (lm_array[i]) {
        case LMS_SHA256_N32_H5: case LMS_SHAKE256_N32_H5: h = 5; break;
		case LMS_SHA256_N32_H10: case LMS_SHAKE256_N32_H10: h = 10; break;
        case LMS_SHA256_N32_H15: case LMS_SHAKE256_N32_H15: h = 15; break;
        case LMS_SHA256_N32_H20: case LMS_SHAKE256_N32_H20: h = 20; break;
        case LMS_SHA256_N32_H25: h = 25; break;
        }
        printf( "Merkle tree height (h) = %d; ", h );
        int w = 0;
        switch (ots_array[i]) {
        case LMOTS_SHA256_N32_W1: w = 1; break;
        case LMOTS_SHA256_N32_W2: case LMOTS_SHAKE256_N32_W2: w = 2; break;
        case LMOTS_SHA256_N32_W4: case LMOTS_SHAKE256_N32_W4: w = 4; break;
        case LMOTS_SHA256_N32_W8: case LMOTS_SHAKE256_N32_W8: w = 8; break;
        }
        printf( "Winternitz param (w) = %d\n", w );
    }
    if (aux_size > 0) {
        //printf( "Maximum of %lu bytes of aux data\n", (unsigned long)aux_size );
    } else {
        //printf( "Aux data disabled\n" );
    }
}

/*
 * This function implements the 'genkey' command
 *
 * It generates the key, and writes the private_key, public key and the aux
 * data to disk.  The private key is also written to disk by the
 * update_private_key function; we write out the public key and the aux data
 * explicitly
 *
 * With the default parameters, this takes quite a while if we're not
 * in threaded mode; in threaded mode, it takes 3 minutes on my test
 * equipment
 */ 
int LmsKeyGeneration(const char *keyname, int* hash, int* h, int* w, int levels)
{
	/* Parse the parameter set */
	param_set_t lm_array[MAX_HSS_LEVELS];
	param_set_t ots_array[MAX_HSS_LEVELS];
	size_t aux_size = DEFAULT_AUX_DATA;
	
	if (!parse_param_set(lm_array, ots_array, hash, h, w, levels)) {
		return 0;
	}

	/* Tell the user how we interpreted the parameter set he gave us */
	list_parameter_set(levels, lm_array, ots_array, aux_size, hash);

	/* We'll place the private key here */
	size_t private_key_filename_len = strlen(keyname) + sizeof(".prv") + 1;
	char *private_key_filename = malloc(private_key_filename_len);
	if (!private_key_filename) return 0;
	sprintf(private_key_filename, "%s.prv", keyname);

	/* We'll place the public key in this array */
	unsigned len_public_key = hss_get_public_key_len(levels,
		lm_array, ots_array);
	if (len_public_key == 0) { free(private_key_filename); return 0; }
	unsigned char public_key[HSS_MAX_PUBLIC_KEY_LEN];

	/* And we'll place the aux data in this array */
	unsigned aux_len;
	if (aux_size > 0) {
		aux_len = hss_get_aux_data_len(aux_size, levels,
			lm_array, ots_array);
		//printf( "aux_len = %d\n", aux_len );
	}
	else {
		aux_len = 1;
	}
	unsigned char *aux = malloc(aux_len);
	if (!aux) {
		printf("error mallocing aux; not generating aux\n");
		aux_len = 0;
		aux = 0;
	}

	printf("[INFO]   : Generating private key %s\n", private_key_filename);
	if (!hss_generate_private_key(
		do_rand,       /* Routine to generate randomness */
		levels,        /* # of Merkle levels */
		lm_array, ots_array,  /* The LM and OTS parameters */
		update_private_key, private_key_filename, /* Routine to write out */
												  /* the genearted private key */
		public_key, len_public_key,  /* The public key is placed here */
		aux_size > 0 ? aux : 0, aux_len, /* Where to place the aux data */
		0)) {            /* Use the defaults for extra info */
		free(private_key_filename);
		free(aux);
		return 0;
	}
	free(private_key_filename); private_key_filename = 0;

	size_t public_key_filename_len = strlen(keyname) + sizeof(".pub") + 1;
	char *public_key_filename = malloc(public_key_filename_len);
	if (!public_key_filename) {
		free(aux);
		return 0;
	}
	sprintf(public_key_filename, "%s.pub", keyname);

	printf("[INFO]   : Generating public key %s\n", public_key_filename);
	FILE *f = fopen(public_key_filename, "w");
	free(public_key_filename); public_key_filename = 0;
	if (!f) {
		fprintf(stderr, "[ERROR]  : Unable to write public key - %s\n", public_key_filename);
		free(aux);
		return 0;
	}
	if (1 != fwrite(public_key, len_public_key, 1, f)) {
		/* Write failed */
		fclose(f);
		free(aux);
		return 0;
	}
	if (0 != fclose(f)) {
		fprintf(stderr, "[ERROR]  : Unable to close public key file - %s\n", public_key_filename);
		/* Close failed (possibly because pending write failed) */
		free(aux);
		return 0;
	}

	/* If the key was specified manually, put in our warning */
	if (seedbits) {
		fprintf(stderr,
			"*** Warning: the key was not generated manually\n"
			"    This key should not be used for real security\n");
	}

	if (aux_size > 0) {
		size_t aux_filename_len = strlen(keyname) + sizeof(".aux") + 1;
		char *aux_filename = malloc(aux_filename_len);
		if (!aux_filename) {
			fprintf(stderr, "[WARNING] : malloc failure writing to aux file - %s\n", aux_filename);
			free(aux);
			return 1;
		}
		sprintf(aux_filename, "%s.aux", keyname);

		/* Attempt to write the aux file.  Note that if we fail, we'll still */
		/* claim to have succeeded (as the aux file is optional) */
		printf("[INFO]   : Generating aux data %s\n", aux_filename);
		f = fopen(aux_filename, "w");
		free(aux_filename); aux_filename = 0;
		if (!f) {
			fprintf(stderr, "[WARNING] : Unable to write aux file - %s\n", aux_filename);
			free(aux);
			return 1;
		}
		if (1 != fwrite(aux, aux_len, 1, f)) {
			fprintf(stderr, "[WARNING] : Unable to write aux file - %s\n", aux_filename);
			/* Write failed */
			fclose(f);
			free(aux);
			return 1;
		}
		if (0 != fclose(f)) {
			fprintf(stderr, "[WARNING] : Close failed writing aux file - %s\n", aux_filename);
			/* Close failed (possibly because pending write failed) */
			free(aux);
			return 1;
		}
	}
	free(aux);

	return 1;
}

#if 0
int LmsKeyGeneration(const char *keyname, const char *parm_set)
{
    /* Parse the parameter set */
    int levels;
	int hash_type;
    param_set_t lm_array[ MAX_HSS_LEVELS ];
    param_set_t ots_array[ MAX_HSS_LEVELS ];
    size_t aux_size;
    if (!parm_set) parm_set = default_parm_set;
    if (!parse_param_set( &levels, lm_array, ots_array, &aux_size, &hash_type, parm_set)) {
        return 0;
    }

    /* Tell the user how we interpreted the parameter set he gave us */
    list_parameter_set( levels, lm_array, ots_array, aux_size );

    /* We'll place the private key here */
    size_t private_key_filename_len = strlen(keyname) + sizeof (".prv" ) + 1;
    char *private_key_filename = malloc(private_key_filename_len);
    if (!private_key_filename) return 0;
    sprintf( private_key_filename, "%s.prv", keyname );

    /* We'll place the public key in this array */
    unsigned len_public_key = hss_get_public_key_len(levels,
                                                lm_array, ots_array);
    if (len_public_key == 0) { free(private_key_filename); return 0; }
    unsigned char public_key[HSS_MAX_PUBLIC_KEY_LEN];

    /* And we'll place the aux data in this array */
    unsigned aux_len;
    if (aux_size > 0) {
        aux_len = hss_get_aux_data_len( aux_size, levels,
                                               lm_array, ots_array);
        //printf( "aux_len = %d\n", aux_len );
    } else {
        aux_len = 1;
    }
    unsigned char *aux = malloc(aux_len);
    if (!aux) {
        printf( "error mallocing aux; not generating aux\n" );
        aux_len = 0;
        aux = 0;
    }

    printf("[INFO]   : Generating private key %s\n", private_key_filename );
    if (!hss_generate_private_key(
        do_rand,       /* Routine to generate randomness */
        levels,        /* # of Merkle levels */
        lm_array, ots_array,  /* The LM and OTS parameters */
        update_private_key, private_key_filename, /* Routine to write out */
                                       /* the genearted private key */
        public_key, len_public_key,  /* The public key is placed here */
        aux_size > 0 ? aux : 0, aux_len, /* Where to place the aux data */
        0)) {            /* Use the defaults for extra info */
            free(private_key_filename);
            free(aux);
            return 0;
    }
    free(private_key_filename); private_key_filename = 0;

    size_t public_key_filename_len = strlen(keyname) + sizeof (".pub" ) + 1;
    char *public_key_filename = malloc(public_key_filename_len);
    if (!public_key_filename) {
        free(aux);
        return 0;
    }
    sprintf( public_key_filename, "%s.pub", keyname );

	printf("[INFO]   : Generating public key %s\n", public_key_filename);
    FILE *f = fopen( public_key_filename, "w" );
    free(public_key_filename ); public_key_filename = 0;
    if (!f) {
        fprintf( stderr, "[ERROR]  : Unable to write public key - %s\n", public_key_filename);
        free(aux);
        return 0;
    }
    if (1 != fwrite( public_key, len_public_key, 1, f )) {
        /* Write failed */
        fclose(f);
        free(aux);
        return 0;
    }
    if (0 != fclose(f)) {
        fprintf( stderr, "[ERROR]  : Unable to close public key file - %s\n", public_key_filename );
        /* Close failed (possibly because pending write failed) */
        free(aux);
        return 0;
    }

    /* If the key was specified manually, put in our warning */
    if (seedbits) {
        fprintf( stderr,
               "*** Warning: the key was not generated manually\n"
               "    This key should not be used for real security\n" );
    }

    if (aux_size > 0) {
        size_t aux_filename_len = strlen(keyname) + sizeof (".aux" ) + 1;
        char *aux_filename = malloc(aux_filename_len);
        if (!aux_filename) {
            fprintf( stderr, "[WARNING] : malloc failure writing to aux file - %s\n", aux_filename);
            free(aux);
            return 1;
        }
        sprintf( aux_filename, "%s.aux", keyname );

        /* Attempt to write the aux file.  Note that if we fail, we'll still */
        /* claim to have succeeded (as the aux file is optional) */
        printf( "[INFO]   : Generating aux data %s\n", aux_filename );
        f = fopen( aux_filename, "w" );
        free(aux_filename); aux_filename = 0;
        if (!f) {
            fprintf( stderr, "[WARNING] : Unable to write aux file - %s\n", aux_filename);
            free(aux);
            return 1;
        }
        if (1 != fwrite( aux, aux_len, 1, f )) {
            fprintf( stderr, "[WARNING] : Unable to write aux file - %s\n", aux_filename);
            /* Write failed */
            fclose(f);
            free(aux);
            return 1;
        }
        if (0 != fclose(f)) {
            fprintf( stderr, "[WARNING] : Close failed writing aux file - %s\n", aux_filename);
            /* Close failed (possibly because pending write failed) */
            free(aux);
            return 1;
        }
    }
    free(aux);

    return 1;
}
#endif
