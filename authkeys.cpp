/******************************************************************************
* Copyright 2015-2021 Xilinx, Inc.
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
#include <iomanip>
#include <iostream>

#include "authkeys.h"
#include "authentication.h"
#include "options.h"
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
 Key::Key(const std::string& name0)
    : Loaded(false)
    , isSecret(false)
    , name(name0)
{
    keySize = AuthenticationContext::GetRsaKeyLength();
    N = new uint8_t [keySize];
    N_ext = new uint8_t [keySize];
    D = new uint8_t [keySize];
    E = new uint8_t [WORD_SIZE_IN_BYTES];
    P = new uint8_t [keySize/2];
    Q = new uint8_t [keySize/2];
    memset(N, 0, keySize);
    memset(N_ext, 0, keySize);
    memset(E, 0, WORD_SIZE_IN_BYTES);
    memset(P, 0, keySize/2);
    memset(Q, 0, keySize/2);
    memset(D, 0, keySize); 
}

/******************************************************************************/
Key::Key(const Key& otherKey)
{ 
    keySize = AuthenticationContext::GetRsaKeyLength();
    N = new uint8_t [keySize];
    N_ext = new uint8_t [keySize];
    D = new uint8_t [keySize];
    E = new uint8_t [WORD_SIZE_IN_BYTES];
    P = new uint8_t [keySize/2];
    Q = new uint8_t [keySize/2];
    memcpy(this,&otherKey,sizeof(Key)); 
}

/******************************************************************************/
Key::~Key()
{
    if(N != NULL)
    {
        delete[] N;
        N = nullptr;
    }
    if(N_ext != NULL)
    {
        delete[] N_ext;
        N_ext = nullptr;
    }
    if(D != NULL)
    {
        delete[] D;
        D = nullptr;
    }
    if(E != NULL)
    {
        delete[] E;
        E = nullptr;
    }
    if(P != NULL)
    {
        delete[] P;
        P = nullptr;
    }
    if(Q != NULL)
    {
        delete[] Q;
        Q = nullptr;
    }
}

/******************************************************************************/
void Key::ParsePublic(const std::string& filename)
{
    Parse(filename,false); 
}

/******************************************************************************/
void Key::ParseSecret(const std::string& filename)
{ 
    Parse(filename,true);
}

/******************************************************************************/
void Key::Parse(const std::string& filename, bool isSecret0) 
{
    uint8_t errCode = 0;
    isSecret = isSecret0;
    std::string basefile = StringUtils::BaseName(filename);

    FILE* f;
    f = fopen(filename.c_str(),"r");
    if (f == NULL)
    {
        LOG_ERROR("Cannot open key %s", filename.c_str());
    }

    try 
    {
        int name;
        do 
        {
            name = getc(f); 
        } while (name >= 0 && isspace(name));
        
        fseek(f,0,0);
    
        /* If the file starts with 'N', then it is Xilinx Format
           If it starts with '-', then it is OpenSSL format */
        if (name == 'N') 
        {
            errCode = ParseXilinxRsaKey(f);
        } 
        else if (name == '-')
        {
            errCode = ParseOpenSSLKey(f);
        } 
        else
        {
            LOG_ERROR("Unsupported key file - %s\n           Supported key formats: Xilinx Format & OpenSSL format", basefile.c_str());
        }
        fclose(f);

        if(errCode != 0)
        {
            LOG_ERROR("RSA authentication key parsing failed - %s", basefile.c_str());
        }
       
        /* Calculate the modulus extension, i.e. Montgomery Reduction term RR 
           and some sanity check for the keys passed */
        {
            BIGNUM m;
            m.d = (BN_ULONG*)N;
            m.dmax = keySize / sizeof(BN_ULONG);
            m.top = keySize / sizeof(BN_ULONG);
            m.flags = 0;
            m.neg = 0;

            BN_CTX_Class ctxInst;
            BN_MONT_CTX_Class montClass(ctxInst);
            montClass.Set(m);
            montClass.GetModulusExtension(N_ext, m, keySize);
        }
        Loaded = true;
    }
    catch(...) 
    {
        fclose(f);
        throw;
    }
}

/******************************************************************************/
uint8_t Key::ParseOpenSSLKey(FILE* f) 
{
    RSA_Class rsaInst;
    OpenSSL_add_all_algorithms();
    uint32_t keySzRd;
    if (isSecret) 
    {
        rsaInst.rsa = PEM_read_RSAPrivateKey(f, NULL, NULL, NULL);
        if(rsaInst.rsa == NULL)
        {
            return 1;
        }

#if OPENSSL_VERSION_NUMBER > 0x10100000L
        keySzRd = BN_num_bytes(RSA_get0_n(rsaInst.rsa));
#else
        keySzRd = BN_num_bytes(rsaInst.rsa->n);
#endif
        if (keySzRd != keySize)
        {
            LOG_ERROR("Incorrect Key Size !!!\n\t   Key Size is %d bits. Expected key size is %d bits", keySzRd * 8, keySize * 8);
        }
#if OPENSSL_VERSION_NUMBER > 0x10100000L
        memcpy(D, RSA_get0_d(rsaInst.rsa)->d, keySize);
#else
        memcpy(D, rsaInst.rsa->d->d, keySize);
#endif
    }
    else
    {
        rsaInst.rsa = PEM_read_RSA_PUBKEY(f, NULL, NULL, NULL);
        if(rsaInst.rsa == NULL)
        {
            return 1;
        }
#if OPENSSL_VERSION_NUMBER > 0x10100000L
        keySzRd = BN_num_bytes(RSA_get0_n(rsaInst.rsa));
#else
        keySzRd = BN_num_bytes(rsaInst.rsa->n);
#endif
        if (keySzRd != keySize)
        {
            LOG_ERROR("Incorrect Key Size !!!\n\t   Key Size is %d bits. Expected key size is %d bits", keySzRd * 8, keySize * 8);
        }
        memset(D,0,keySize);
    }
#if OPENSSL_VERSION_NUMBER > 0x10100000L
    memcpy(N, RSA_get0_n(rsaInst.rsa)->d, keySize);
    memcpy(E, RSA_get0_e(rsaInst.rsa)->d, sizeof(uint32_t));
#else
    memcpy(N, rsaInst.rsa->n->d, keySize);
    memcpy(E, rsaInst.rsa->e->d, sizeof(uint32_t));
#endif
    return 0;
}

/******************************************************************************/
uint8_t Key::ParseXilinxRsaKey(FILE* f)
{
    int name;

    /* Check for N in the key file followed by '=', Once got, parse the value of N */
    do 
    {
        name = getc(f); 
    } while (name >= 0 && isspace(name));
    if (name != 'N')
    {
        LOG_DEBUG(DEBUG_STAMP, "Bad Key file, expected N, got %c", name);
        return 1;
    }
    do 
    {
        name = getc(f);
    } while (name >= 0 && isspace(name));
    if (name != '=') 
    {
        LOG_DEBUG(DEBUG_STAMP, "Bad Key file, expected =, got %c", name);
        return 1;
    }
    LOG_TRACE("Parsing 'N' from key file");
    Hex2Byte(f, N, keySize);

    /* Check for E in the key file followed by '=' Once got, parse the value of E */
    do
    {
        name = getc(f); 
    } while (name >= 0 && isspace(name));
    if (name != 'E')
    {
        LOG_DEBUG(DEBUG_STAMP, "Bad Key file, expected E, got %c",name);
        return 1;
    }
     
    do 
    {
        name = getc(f); 
    } while (name >= 0 && isspace(name));
    if (name != '=')
    {
        LOG_DEBUG(DEBUG_STAMP, "Bad Key file, expected =, got %c",name);
        return 1;
    }

    uint32_t temp;
    LOG_TRACE("Parsing 'E' from key file");
    if(EOF != fscanf(f,"%X",&temp))
    {
        E[0] = (uint8_t)(temp);
        E[1] = (uint8_t)(temp>>8);
        E[2] = (uint8_t)(temp>>16);
        E[3] = (uint8_t)(temp>>24);
    }

    if (isSecret) 
    {
        /* Check for D in the key file followed by '=' Once got, parse the value of D */
       do
        {
            name = getc(f); 
        } while (name >= 0 && isspace(name));
        if(name != 'D') 
        {
            LOG_DEBUG(DEBUG_STAMP, "Bad Key file, expected D, got %c",name);
            return 1;
        }
        
        do
        {
            name = getc(f); 
        } while (name >= 0 && isspace(name));
        if (name != '=')
        {
            LOG_DEBUG(DEBUG_STAMP, "Bad Key file, expected =, got %c",name);
            return 1;
        }
        LOG_TRACE("Parsing 'D' from key file");
        Hex2Byte(f, D, keySize);

        /* The rest of parsing is for sanity checking purpose 
           Not used for signing or verfication */

        /* Check for P in the key file followed by '=' Once got, parse the value of P */
        do 
        {
            name = getc(f); 
        } while (name >= 0 && isspace(name));
        if(name == 'P')
        {
            do 
            {
                name = getc(f); 
            } while (name >= 0 && isspace(name));
            if (name != '=')
            {
                LOG_DEBUG(DEBUG_STAMP, "Bad Key file, expected =, got %c",name);
                return 1;
            }
            LOG_TRACE("Parsing 'P' from key file");
            Hex2Byte(f, P, keySize/2);

            /* Check for Q in the key file followed by '=' Once got, parse the value of Q */
            do
            {
                name = getc(f);
            } while (name >= 0 && isspace(name));
            if(name != 'Q')
            {
                LOG_DEBUG(DEBUG_STAMP, "Bad Key file, expected Q, got %c",name);
                return 1;
            }
           
            do
            {
                name = getc(f); 
            } while (name >= 0 && isspace(name));
            if(name != '=') 
            {
                LOG_DEBUG(DEBUG_STAMP, "Bad Key file, expected =, got %c",name);
                return 1;
            }
            LOG_TRACE("Parsing 'Q' from key file");
            Hex2Byte(f, Q, keySize/2);

            /* Sanity check block, N = PxQ */
            {
                uint8_t *Ncheck = new uint8_t [keySize];
                Multiply_p_q(P, Q, Ncheck);

                if (memcmp(N, Ncheck, keySize)) 
                {
                    delete[] Ncheck;
                    LOG_DEBUG(DEBUG_STAMP, "Inconsistency found in key file, P * Q != N");
                    return 1;
                }
                delete[] Ncheck;
            }
        }
    } 
    else
    {
        /* If it is a public key, D=0 */
        memset(D, 0, keySize);
    }
    return 0;
}

/******************************************************************************/
void Key::WriteRsaFile(std::string filename, const RSA* rsa, bool secret, uint16_t keyLength)
{
    std::ofstream file(filename.c_str());
    bool fileWritten = false;

    if (file)
    {
        uint8_t *temp = new uint8_t[keyLength];

        file << "N = ";

#if OPENSSL_VERSION_NUMBER > 0x10100000L
        memcpy(temp, RSA_get0_n(rsa)->d, keyLength);
#else
        memcpy(temp, rsa->n->d, keyLength);
#endif
        for (uint32_t index = keyLength; index != 0; index--)
        {
            file << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << int(temp[index - 1]);
        }

        file << "\n\nE = ";
#if OPENSSL_VERSION_NUMBER > 0x10100000L
        uint32_t* temp_e = (uint32_t*)RSA_get0_e(rsa)->d;
#else
        uint32_t* temp_e = (uint32_t*)rsa->e->d;
#endif    
        file << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << *temp_e;
        if (secret)
        {
            file << "\n\nD = ";
#if OPENSSL_VERSION_NUMBER > 0x10100000L
            memcpy(temp, RSA_get0_d(rsa)->d, keyLength);
#else
            memcpy(temp, rsa->d->d, keyLength);
#endif        
            for (uint32_t index = keyLength; index != 0; index--)
            {
                file << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << int(temp[index - 1]);
            }
        }
        delete[] temp;
        fileWritten = !file.bad();
        file.close();
    }

    if (!fileWritten)
    {
        LOG_ERROR("Failure writing authentication key file : %s", filename.c_str());
    }
    else
    {
        LOG_INFO("%s key file generation in RSA format successful", filename.c_str());
    }
}

/******************************************************************************/
void Key::WritePemFile(std::string filename, RSA * rsa, EC_KEY* eckey, bool secret)
{
    FILE* file = fopen(filename.c_str(), "wb");
    bool fileWritten = false;
    if (file)
    {
        if (rsa != NULL)
        {
            if (!secret)
            {
               if (PEM_write_RSA_PUBKEY(file, rsa))
               {
                   fileWritten = true;
               }
            }
            else
            {
               if (PEM_write_RSAPrivateKey(file, rsa, NULL, NULL, 0, NULL, NULL))
               {
                  fileWritten = true;
               }
            }
            fclose(file);
        }
        if (eckey != NULL)
        {
            if (!secret)
            {
                if (PEM_write_EC_PUBKEY(file, eckey))
                {
                   fileWritten = true;
                }
            }
            else
            {
                if (PEM_write_ECPrivateKey(file, eckey, NULL, NULL, 0, NULL, NULL))
                {
                   fileWritten = true;
                }
            }
            fclose(file);
        }
    }

    if (!fileWritten)
    {
        LOG_ERROR("Failure writing authentication key file : %s", filename.c_str());
    }
    else
    {
        LOG_INFO("%s key file generation in PEM format successful",filename.c_str());
    }
}

/******************************************************************************/
void Key::GenerateRsaKeys(KeyGenerationStruct* keygen)
{
    RSA *rsa = NULL;
    OpenSSL_add_all_algorithms();

    if (!(keygen->ppk_file != "" || keygen->psk_file != "" || keygen->spk_file != "" || keygen->ssk_file != ""))
    {
        LOG_ERROR("Failed to generate authentication keys. Please specify the key paths in BIF file");
    }

    if (keygen->ppk_file != "" || keygen->psk_file != "")
    {
        if ((rsa = RSA_generate_key(keygen->keyLength * 8, RSA_F4, NULL, NULL)) == NULL)
        {
            LOG_ERROR("Failure creating authentication Keys");
        }
        else
        {
            if (keygen->format == GenAuthKeys::PEM)
            {
                if (keygen->ppk_file != "")
                {
                    WritePemFile(keygen->ppk_file, rsa, NULL, false);
                }

                if (keygen->psk_file != "")
                {
                    WritePemFile(keygen->psk_file, rsa, NULL, true);
                }
            }
            else
            {
                if (keygen->ppk_file != "")
                {
                    WriteRsaFile(keygen->ppk_file, rsa, false, keygen->keyLength);
                }

                if (keygen->psk_file != "")
                {
                    WriteRsaFile(keygen->psk_file, rsa, true, keygen->keyLength);
                }
            }
        }
        rsa = NULL;
    }

    if (keygen->spk_file != "" || keygen->ssk_file != "")
    {
        if ((rsa = RSA_generate_key(keygen->keyLength * 8, RSA_F4, NULL, NULL)) == NULL)
        {
            LOG_ERROR("Failure creating authentication Keys");
        }
        else
        {
            if (keygen->format == GenAuthKeys::PEM)
            {
                if (keygen->spk_file != "")
                {
                    WritePemFile(keygen->spk_file, rsa, NULL, false);
                }

                if (keygen->ssk_file != "")
                {
                    WritePemFile(keygen->ssk_file, rsa, NULL, true);
                }
            }
            else
            {
                if (keygen->spk_file != "")
                {
                    WriteRsaFile(keygen->spk_file, rsa, false, keygen->keyLength);
                }

                if (keygen->ssk_file != "")
                {
                    WriteRsaFile(keygen->ssk_file, rsa, true, keygen->keyLength);
                }
            }
        }
        rsa = NULL;
    }
}

/************************************************************************************/
void Key::GenerateEcdsaKeys(KeyGenerationStruct* keygen)
{
    EC_KEY* eckey = NULL;
    OpenSSL_add_all_algorithms();

    if (!(keygen->ppk_file != "" || keygen->psk_file != "" || keygen->spk_file != "" || keygen->ssk_file != ""))
    {
        LOG_ERROR("Failed to generate authentication keys. Please specify the key paths in BIF file");
    }


    if (keygen->format == GenAuthKeys::ECDSA)
    {
        eckey = EC_KEY_new_by_curve_name(NID_secp384r1);
    }
    else if(keygen->format == GenAuthKeys::ECDSAP521)
    {
        eckey = EC_KEY_new_by_curve_name(NID_secp521r1);
    }
    else
    {
        eckey = NULL;
    }


    if (keygen->ppk_file != "" || keygen->psk_file != "")
    {
        if((EC_KEY_generate_key(eckey)) == 0)
        {
            LOG_ERROR("Failure creating authentication Keys");
        }
        else
        {
            if (keygen->ppk_file != "")
            {
                WritePemFile(keygen->ppk_file, NULL, eckey, false);
            }

            if (keygen->psk_file != "")
            {
                WritePemFile(keygen->psk_file,NULL, eckey, true);
            }
        }
    }

    if (keygen->spk_file != "" || keygen->ssk_file != "")
    {
        if ((EC_KEY_generate_key(eckey)) == 0)
        {
            LOG_ERROR("Failure creating authentication Keys");
        }
        else
        {
            if (keygen->spk_file != "")
            {
                WritePemFile(keygen->spk_file, NULL, eckey, false);
            }

            if (keygen->ssk_file != "")
            {
                WritePemFile(keygen->ssk_file, NULL, eckey, true);
            }
        }
    }
}

/******************************************************************************/
void Key2048::Export(void* acKey) 
{
    ACKey2048* key = (ACKey2048*)acKey;
    if (!Loaded) 
    {
        std::string pubsec = (isSecret) ? " (Secret)" : " (Public)";
        LOG_ERROR("%s - $s is not loaded", name.c_str(), pubsec.c_str());
    }

    memset(key,0,sizeof(ACKey2048));
    memcpy(key->N, N, 256);
    memcpy(key->N_extension, N_ext, 256);
    memcpy(key->E, E, 4);
}

/******************************************************************************/
void Key2048::Import(const void* acKey, const std::string& name0) 
{
    ACKey2048* key = (ACKey2048*)acKey;
    Loaded = true;
    isSecret = false;
    name = name0;
    memcpy(N,key->N,keySize);
    memcpy(N_ext,key->N_extension,keySize);
    memcpy(E,key->E,sizeof(uint32_t));
    memset(P,0,keySize/2);
    memset(Q,0,keySize/2);

    /* Fill secret exponent with zeros */
    memset(D,0,keySize); 
}

/******************************************************************************/
void Key4096::Export(void* acKey) 
{
    ACKey4096* key = (ACKey4096*)acKey;
    if (!Loaded) 
    {
        std::string pubsec = (isSecret) ? " (Secret)" : " (Public)";
        LOG_ERROR("%s - $s is not loaded", name.c_str(), pubsec.c_str());
    }
    memset(key, 0, sizeof(ACKey4096));
    memcpy(key->N, N, keySize);
    memcpy(key->N_extension, N_ext, keySize);
    memcpy(key->E, E, 4);
}

/******************************************************************************/
void Key4096::Import(const void* acKey, const std::string& name0) 
{
    ACKey4096* key = (ACKey4096*)acKey;
    Loaded = true;
    isSecret = false;
    name = name0;
    memcpy(N, key->N, keySize);
    memcpy(N_ext, key->N_extension, keySize);
    memcpy(E, key->E, sizeof(uint32_t));
    memset(P, 0, keySize/2);
    memset(Q, 0, keySize/2);

    /* Fill secret exponent with zeros */
    memset(D, 0, keySize); 
}

/******************************************************************************/
void Key::SetKeyName(std::string keyname)
{
    name = keyname;
}

/******************************************************************************/
void Key::Multiply_p_q(uint8_t p[], uint8_t q[], uint8_t n[]) 
{
    memset(n,0,256);

    /* Multiply p * q, and store in n */
    for(int i = 0; i < 128; i++) 
    {
        for(int j = 0; j < 128; j++) 
        {
            uint16_t prod = p[i] * q[j];
            int k = i+j;
            while (prod && k < 256) 
            {
                uint16_t sum = n[k]  + (prod & 0xFF);
                n[k] = sum & 0xFF;
                prod >>= 8;
                sum >>= 8;
                prod = (prod + sum);
                k++;
            }
        }
    }
}

/******************************************************************************/
void Key::Hex2Byte(FILE* f, uint8_t* data, int count) 
{
    char *buf;
    buf = (char *) malloc(2000);
    if (buf != NULL)
    {
        char *tempbuf = buf;
        if (fscanf(f, "%s", tempbuf) != 1) 
        {
            LOG_ERROR("Error parsing key");
        }
    
        int len = strlen(tempbuf)/2;
        if(len != count)
        {
            LOG_ERROR("Key size is %d bytes, expected size is %d bytes", len, count);
        }
        for(int i = count-1; i >= 0; i--) 
        {
            int v;
            if (sscanf(tempbuf, "%2X", &v) != 1) 
            {
                LOG_DEBUG(DEBUG_STAMP, "Failure reading the authentication key file");
                LOG_ERROR("Failure in key parsing !!!");
            }
            if (v < 0 || v > 255)
            {
                LOG_DEBUG(DEBUG_STAMP, "Failure converting the hex characters from key file");
                LOG_ERROR("Failure in key parsing !!!");
            }
            if(i > 0)
            {
                tempbuf += 2;
            }
            data[i] = v;
        }
    }
    else
    {
        LOG_DEBUG(DEBUG_STAMP, "Memory Allocation error while reading keys");
        LOG_ERROR("Failure in key parsing !!!");
    }
    free(buf);
}
