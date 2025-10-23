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
/*
* This will read in the file into a malloc'ed area
* The hss routines assume that everything public keys, auxilary data and
* signatures are in contiguous memory; this is used to read them in.
*
* This isn't used to read in the files being signed/verified; we read
* those in chunks within the sign()/verify() routines below.
*/
void *read_file(const char *filename, size_t *len) {
	FILE *f = fopen(filename, "r");
	if (!f) return 0;

#define FILE_INCREMENT 20000
	unsigned alloc_len = FILE_INCREMENT;
	unsigned char *p = malloc(FILE_INCREMENT);
	if (!p) return 0;

	unsigned cur_len = 0;
	for (;;) {
		unsigned delta = alloc_len - cur_len;
		if (delta == 0) {
			unsigned char *q = realloc(p, alloc_len + FILE_INCREMENT);
			if (!q) {
				free(p);
				return 0;
			}
			p = q;
			alloc_len += FILE_INCREMENT;
			delta = FILE_INCREMENT;
		}
		int n = fread(p + cur_len, 1, delta, f);
		if (n <= 0) break;
		cur_len += n;
	}

	if (len) *len = cur_len;
	return p;
}

/*
* This saves the private key to secure storage; in this case, a file on the
* filesystem.  The context pointer we use here is the filename
*/
bool update_private_key(unsigned char *private_key,
	size_t len_private_key, void *filename) {
	FILE *f = fopen(filename, "r+");
	if (!f) {
		/* Open failed, possibly because the file didn't exist */
		f = fopen(filename, "w");
		if (!f) {
			/* Unable to open file */
			return false;
		}
	}
	if (1 != fwrite(private_key, len_private_key, 1, f)) {
		/* Write failed */
		fclose(f);
		return false;
	}
	if (0 != fclose(f)) {
		/* Close failed (possibly because pending write failed) */
		return false;
	}

	/* Everything succeeded */
	return true;
}

/*
* This retrieves the private key from secure storage; in this case, a file on
* the filesystem.  The context pointer we use here is the filename
*/
static bool read_private_key(unsigned char *private_key,
	size_t len_private_key, void *filename) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		return false;
	}
	if (1 != fread(private_key, len_private_key, 1, f)) {
		/* Read failed */
		fclose(f);
		return false;
	}
	fclose(f);

	/* Everything succeeded */
	return true;
}

int LmsVerify(const char *keyname, const unsigned char *buffer, size_t buffer_len, unsigned char* sig, size_t sig_len) {
    size_t public_key_filename_len = strlen(keyname) + 1;
    char *public_key_filename = malloc(public_key_filename_len);
    if (!public_key_filename) {
        printf("Error: malloc failure\n");
        return 0;
    }
    sprintf(public_key_filename, "%s", keyname);
    unsigned char *pub = read_file(public_key_filename, 0);
    if (!pub) {
        printf("Error: unable to read %s\n", keyname);
        free(public_key_filename);
        return 0;
    }

    bool success = hss_validate_signature(pub, buffer, buffer_len, sig, sig_len, NULL);
    if (success) {
        //printf("Verifed");
        return 1;
    }
    else {
        //printf("Not Verifed");
        return 0;
    }
}

/*
 This function implements the 'sign' command; it loads the private key, and
 then for each file, loads it into memory, generates the signature, and
 writes the signature out to disk
 */
 int LmsSign(const char *keyname, const unsigned char *buffer, unsigned long int buffer_len, unsigned char* sig, size_t* sig_len, uint32_t* err) {
	int private_key_filename_len = strlen(keyname) + 1;
	char *private_key_filename = malloc(private_key_filename_len);
	if (!private_key_filename) {
		printf("Malloc failure\n");
		return 0;
	}
	sprintf(private_key_filename, "%s", keyname);
	struct hss_extra_info info = { 0 };

	/* Read in the auxilliary file */
	size_t aux_filename_len = strlen(keyname) + sizeof(".aux") + 1;
	char *aux_filename = malloc(aux_filename_len);
	if (!aux_filename) {
		printf("Malloc failure\n");
		free(private_key_filename);
		return 0;
	}
	sprintf(aux_filename, "%s.aux", keyname);
	size_t len_aux_data = 0;
	void *aux_data = read_file(aux_filename, &len_aux_data);
	if (aux_data != 0) {
		//printf("Processing with aux data\n");
	}
	else {
		/* We don't have the aux data; proceed without it */
		//printf("Processing without aux data\n");
	}

	/* Load the working key into memory */
	//printf("Loading private key\n");
	fflush(stdout);
	struct hss_working_key *w = hss_load_private_key(
		read_private_key, private_key_filename, /* How to load the */
												/* private key */
		0,                          /* Use minimal memory */
		aux_data, len_aux_data,     /* The auxiliary data */
		&info);                         /* Use the defaults for extra info */
	if (!w) {
		printf("[ERROR]  : Error loading private key\n");
		*err = info.error_code;
		free(aux_data);
		hss_free_working_key(w);
		free(aux_filename);
		free(private_key_filename);
		return 0;
	}
	free(aux_data);

//	printf("Loaded private key\n");  /* printf here mostly so the user */
	fflush(stdout);              /* gets a feel for how long this step took */
								 /* compared to the signing steps below */

								 /* Now, go through the file list, and generate the signatures for each */

								 /* Look up the signature length */
	*sig_len = hss_get_signature_len_from_working_key(w);
	if (*sig_len == 0) {
		printf("Error getting signature len\n");
		hss_free_working_key(w);
		free(aux_filename);
		free(private_key_filename);
		return 0;
	}

	if (!sig) {
		printf("Error during malloc\n");
		hss_free_working_key(w);
		free(aux_filename);
		free(private_key_filename);
		return 0;
	}

	//printf("Signing the buffer\n");
	
	bool success = hss_generate_signature(w,
		update_private_key, private_key_filename,
		buffer, buffer_len,
		sig, *sig_len, &info);

	if (!success){
		printf("Error signing");
		*err = info.error_code;
		return 0;
	}
#if 0
	static int i = 1;
	char* oldName = "datafile";
	int fName_len = strlen(oldName) + sizeof(".sig") + 1 + 2;
	char *fName = malloc(fName_len);
	sprintf(fName, "%s_%d%s", oldName, i++, ".sig");
	FILE* f = fopen( fName, "w" );
    if (!f) {
        printf( "   unable to create\n");
    }
    if (1 != fwrite( sig, sig_len, 1, f )) {
        printf( "    unable to write %s\n", fName);
        fclose(f);
    }
    fclose(f);
#endif
	hss_free_working_key(w);
	free(aux_filename);
	free(private_key_filename);
	//free(sig);
	return 1;
}

int GetLmsSignLength(const char *keyname, bool lmsOnly)
{
	int private_key_filename_len = strlen(keyname) + 1;
	char *private_key_filename = malloc(private_key_filename_len);
	if (!private_key_filename) {
		printf("Malloc failure\n");
		return 0;
	}
	sprintf(private_key_filename, "%s", keyname);

	unsigned levels;
	param_set_t lm[MAX_HSS_LEVELS];
	param_set_t ots[MAX_HSS_LEVELS];
	if (!hss_get_parameter_set(&levels, lm, ots, read_private_key, private_key_filename, NULL)) {
		/* Can't read private key, or private key invalid */
		return 0;
	}
    int hss_size = 0;
	hss_size = hss_get_signature_len(levels, lm, ots);

    if (lmsOnly)
    {
        /*Reducing by 4 bytes, to make 1 Level HSS Sign to LMS Sign*/
        return hss_size - 4;
    }
    else
    {
        return hss_size;
    }
}
