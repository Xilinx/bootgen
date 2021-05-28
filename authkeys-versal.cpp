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

#include "authkeys-versal.h"
#include "authentication-versal.h"
#include "options.h"
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
VersalKey::VersalKey(const std::string& name0)
    : Key(name0)
{
    if (keySize == RSA_4096_KEY_LENGTH)
    {
        //Do Nothing
        eckey = nullptr;
    }
    else if (keySize == EC_P384_KEY_LENGTH)
    {
        keySizeX = keySizeY = keySize;
        eckey = EC_KEY_new_by_curve_name(NID_secp384r1);
    }
    else
    {
        eckey = EC_KEY_new_by_curve_name(NID_secp521r1);
    }
    x = nullptr;
    y = nullptr;
}


/******************************************************************************/
VersalKey::VersalKey(const Key& otherKey)
    :Key(otherKey)
{
    // ECDSA
    if (keySize == RSA_4096_KEY_LENGTH)
    {
        //Do Nothing
    }
    else if (keySize == EC_P384_KEY_LENGTH)
    {
        eckey = EC_KEY_new_by_curve_name(NID_secp384r1);
    }
    else
    {
        eckey = EC_KEY_new_by_curve_name(NID_secp521r1);
    }

    memcpy(this, &otherKey, sizeof(Key));
}

/******************************************************************************/
VersalKey::~VersalKey()
{
    if (x != NULL)
    {
        delete[] x;
        x = nullptr;
    }

    if (y != NULL)
    {
        delete[] y;
        y = nullptr;
    }

    if (eckey != NULL)
    {
        EC_KEY_free(eckey);
        eckey = nullptr;
    }
}

/******************************************************************************/
void Key4096Sha3Padding::Export(void* acKey)
{
    ACKey4096Sha3Padding* key = (ACKey4096Sha3Padding*)acKey;
    if (!Loaded)
    {
        std::string pubsec = (isSecret) ? " (Secret)" : " (Public)";
        LOG_ERROR("%s - $s is not loaded", name.c_str(), pubsec.c_str());
    }

    memset(key, 0, VERSAL_ACKEY_STRUCT_SIZE);
    memcpy(key->N, N, keySize);
    memcpy(key->N_extension, N_ext, keySize);
    memcpy(key->E, E, 4);
}


/******************************************************************************/
void Key4096Sha3Padding::Import(const void* acKey, const std::string& name0)
{
    ACKey4096Sha3Padding* key = (ACKey4096Sha3Padding*)acKey;
    Loaded = true;
    isSecret = false;
    name = name0;

    memcpy(N, key->N, keySize);
    memcpy(N_ext, key->N_extension, keySize);
    memcpy(E, key->E, sizeof(uint32_t));
    memset(P, 0, keySize / 2);
    memset(Q, 0, keySize / 2);

    // Fill secret exponent with zeros
    memset(D, 0, keySize);
}

/******************************************************************************/
void KeyECDSA::Export(void * acKey)
{
    ACKeyECDSA* key = (ACKeyECDSA*)acKey;
    if (!Loaded)
    {
        std::string pubsec = (isSecret) ? " (Secret)" : " (Public)";
        LOG_ERROR("%s - $s is not loaded", name.c_str(), pubsec.c_str());
    }

    memset(key, 0, sizeof(ACKeyECDSA));
    memcpy(key->x, x, keySize);
    memcpy(key->y, y, keySize);
    memset(key->pad, 0, sizeof(key->pad));
}

/******************************************************************************/
void KeyECDSA::Import(const void * acKey, const std::string & name0)
{
    ACKeyECDSA* key = (ACKeyECDSA*)acKey;
    Loaded = true;
    isSecret = false;
    name = name0;

    x = new uint8_t[keySize];
    memset(x, 0, keySize);
    y = new uint8_t[keySize];
    memset(y, 0, keySize);

    memcpy(x, key->x, keySize);
    memcpy(y, key->y, keySize);
    memset(key->pad, 0, sizeof(key->pad));

    // Fill secret exponent with zeros
    memset(D, 0, keySize);
}

/******************************************************************************/
void KeyECDSAp521::Export(void * acKey)
{
    ACKeyECDSAP521* key = (ACKeyECDSAP521*)acKey;
    if (!Loaded)
    {
        std::string pubsec = (isSecret) ? " (Secret)" : " (Public)";
        LOG_ERROR("%s - $s is not loaded", name.c_str(), pubsec.c_str());
    }

    memset(key, 0, sizeof(ACKeyECDSAP521));
    if (keySizeX == EC_P521_KEY_LENGTH1)
    {
        memcpy((key->x) + 1, x, keySizeX);
    }
    else
    {
        memcpy(key->x, x, keySizeX);
    }
    if (keySizeY == EC_P521_KEY_LENGTH1)
    {
        memcpy((key->y)+1, y, keySizeY);
    }
    else
    {
        memcpy(key->y, y, keySizeY);
    }
    memset(key->pad, 0, sizeof(key->pad));
}


/******************************************************************************/
void KeyECDSAp521::Import(const void * acKey, const std::string & name0)
{
    ACKeyECDSAP521* key = (ACKeyECDSAP521*)acKey;
    Loaded = true;
    isSecret = false;
    name = name0;

    x = new uint8_t[keySizeX];
    y = new uint8_t[keySizeY];
    memset(x, 0, keySizeX);
    memset(y, 0, keySizeY);

    memcpy(x, key->x, keySizeX);
    memcpy(y, key->y, keySizeY);
    memset(key->pad, 0, sizeof(key->pad));

    // Fill secret exponent with zeros
    memset(D, 0, keySize);
}

/******************************************************************************/
static void RearrangeEndianess(uint8_t *array, uint32_t size)
{
    uint32_t lastIndex = size - 1;
    char tempInt = 0;

    // If array is NULL, return
    if (!array)
    {
        return;
    }

    for (uint32_t loop = 0; loop <= (lastIndex / 2); loop++)
    {
        tempInt = array[loop];
        array[loop] = array[lastIndex - loop];
        array[lastIndex - loop] = tempInt;
    }
}

/******************************************************************************/
uint8_t VersalKey::ParseECDSAOpenSSLKey(const std::string& filename)
{
    OpenSSL_add_all_algorithms();
    BIGNUM *X = BN_new();
    X->flags = 0;
    X->neg = 0;
    X->top = 0;
    BIGNUM *Y = BN_new();
    Y->flags = 0;
    Y->neg = 0;
    Y->top = 0;
    uint32_t keySzRdX;
    uint32_t keySzRdY;

    FILE* file;
    file = fopen(filename.c_str(), "r");
    if (file == NULL)
    {
        LOG_ERROR("Cannot open key %s", filename.c_str());
    }

    if (isSecret)
    {
        eckey = PEM_read_ECPrivateKey(file, NULL, NULL, NULL);
        if (!EC_KEY_check_key(eckey))
        {
            LOG_TRACE("Failed to check EC Key\n");
        }
        else
        {
            EC_GROUP *ecgroup;
            if (keySize == EC_P384_KEY_LENGTH)
            {
                x = new uint8_t[keySize];
                y = new uint8_t[keySize];
                memset(x, 0, keySize);
                memset(y, 0, keySize);
                X->d = (BN_ULONG*)x;
                Y->d = (BN_ULONG*)y;
                X->dmax = keySize / sizeof(BN_ULONG);
                Y->dmax = keySize / sizeof(BN_ULONG);
                ecgroup = EC_GROUP_new_by_curve_name(NID_secp384r1);
                const EC_POINT *pub = EC_KEY_get0_public_key(eckey);
                if (EC_POINT_get_affine_coordinates_GFp(ecgroup, pub, X, Y, NULL))
                {
                    keySzRdX = BN_num_bytes(X);
                    if (keySzRdX != keySize)
                    {
                        LOG_ERROR("Incorrect Key Size !!!\n\t   Key Size is %d bits. Expected key size is %d bits", BN_num_bits(X), keySize * 8);
                    }
                    keySzRdY = BN_num_bytes(Y);
                    if (keySzRdY != keySize)
                    {
                        LOG_ERROR("Incorrect Key Size !!!\n\t   Key Size is %d bits. Expected key size is %d bits", BN_num_bits(Y), keySize * 8);
                    }
                    RearrangeEndianess(x, keySize);
                    RearrangeEndianess(y, keySize);
                }
                else
                {
                    LOG_ERROR("Failed to parse key %s for ECDSAp384",filename.c_str());
                }
            }
            else
            {
                ecgroup = EC_GROUP_new_by_curve_name(NID_secp521r1);
                const EC_POINT *pub = EC_KEY_get0_public_key(eckey);
                if (EC_POINT_get_affine_coordinates_GFp(ecgroup, pub, X, Y, NULL))
                {
                    keySizeX = BN_num_bytes(X);
                    if (keySizeX != EC_P521_KEY_LENGTH1 && keySizeX != EC_P521_KEY_LENGTH2)
                    {
                        LOG_ERROR("Incorrect Key Size !!!\n\t   Key Size is %d bits. Expected key size is %d bits", BN_num_bits(X), 521);
                    }
                    keySizeY = BN_num_bytes(Y);
                    if (keySizeY != EC_P521_KEY_LENGTH1 && keySizeY != EC_P521_KEY_LENGTH2)
                    {
                        LOG_ERROR("Incorrect Key Size !!!\n\t   Key Size is %d bits. Expected key size is %d bits", BN_num_bits(Y), 521);
                    }

                    x = new uint8_t[keySizeX];
                    y = new uint8_t[keySizeY];
                    memset(x, 0, keySizeX);
                    memset(y, 0, keySizeY);
                    memcpy(x, X->d, keySizeX);
                    memcpy(y, Y->d, keySizeY);

                    RearrangeEndianess(x, keySizeX);
                    RearrangeEndianess(y, keySizeY);
                }
                else
                {
                    LOG_ERROR("Failed to parse key %s for ECDSAp521",filename.c_str());
                }
            }
        }
    }
    else
    {
        eckey = PEM_read_EC_PUBKEY(file, NULL, NULL, NULL);
        if (!EC_KEY_check_key(eckey))
        {
            LOG_TRACE("Failed to check EC Key\n");
        }
        else
        {
            EC_GROUP *ecgroup;
            if (keySize == EC_P384_KEY_LENGTH)
            {
                x = new uint8_t[keySize];
                y = new uint8_t[keySize];
                memset(x, 0, keySize);
                memset(y, 0, keySize);
                X->d = (BN_ULONG*)x;
                Y->d = (BN_ULONG*)y;
                X->dmax = keySize / sizeof(BN_ULONG);
                Y->dmax = keySize / sizeof(BN_ULONG);
                ecgroup = EC_GROUP_new_by_curve_name(NID_secp384r1);
                const EC_POINT *pub = EC_KEY_get0_public_key(eckey);
                if (EC_POINT_get_affine_coordinates_GFp(ecgroup, pub, X, Y, NULL))
                {
                    keySzRdX = BN_num_bytes(X);
                    if (keySzRdX != keySize)
                    {
                        LOG_ERROR("Incorrect Key Size !!!\n\t   Key Size is %d bits. Expected key size is %d bits", BN_num_bits(X), keySize * 8);
                    }
                    keySzRdY = BN_num_bytes(Y);
                    if (keySzRdY != keySize)
                    {
                        LOG_ERROR("Incorrect Key Size !!!\n\t   Key Size is %d bits. Expected key size is %d bits", BN_num_bits(Y), keySize * 8);
                    }
                    RearrangeEndianess(x, keySize);
                    RearrangeEndianess(y, keySize);
                }
                else
                {
                    LOG_ERROR("Failed to parse key %s for ECDSAp384",filename.c_str());
                }
            }
            else
            {
                ecgroup = EC_GROUP_new_by_curve_name(NID_secp521r1);
                const EC_POINT *pub = EC_KEY_get0_public_key(eckey);
                if (EC_POINT_get_affine_coordinates_GFp(ecgroup, pub, X, Y, NULL))
                {
                    keySizeX = BN_num_bytes(X);
                    if (keySizeX != EC_P521_KEY_LENGTH1 && keySizeX != EC_P521_KEY_LENGTH2)
                    {
                        LOG_ERROR("Incorrect Key Size !!!\n\t   Key Size is %d bits. Expected key size is %d bits", BN_num_bits(X), 521);
                    }
                    keySizeY = BN_num_bytes(Y);
                    if (keySizeY != EC_P521_KEY_LENGTH1 && keySizeY != EC_P521_KEY_LENGTH2)
                    {
                        LOG_ERROR("Incorrect Key Size !!!\n\t   Key Size is %d bits. Expected key size is %d bits", BN_num_bits(Y), 521);
                    }

                    x = new uint8_t[keySizeX];
                    y = new uint8_t[keySizeY];
                    memset(x, 0, keySizeX);
                    memset(y, 0, keySizeY);
                    memcpy(x, X->d, keySizeX);
                    memcpy(y, Y->d, keySizeY);

                    RearrangeEndianess(x, keySizeX);
                    RearrangeEndianess(y, keySizeY);
                }
                else
                {
                    LOG_ERROR("Failed to parse key %s for ECDSAp521",filename.c_str());
                }
            }
        }
    }
    fclose(file);
    return 0;
}

/******************************************************************************/
void VersalKey::Parse(const std::string& filename, bool isSecret0)
{
    uint8_t errCode = 0;
    isSecret = isSecret0;
    std::string basefile = StringUtils::BaseName(filename);

    std::ifstream File(filename.c_str());
    std::string word;
    File >> word;
    File >> word;

    if (!isSecret)
    {
        FILE* f;
        f = fopen(filename.c_str(), "r");
        if (f == NULL)
        {
            LOG_ERROR("Cannot open key %s", filename.c_str());
        }
        RSA* rsa = PEM_read_RSA_PUBKEY(f, NULL, NULL, NULL);
        fclose(f);
        f = fopen(filename.c_str(), "r");
        if (f == NULL)
        {
            LOG_ERROR("Cannot open key %s", filename.c_str());
        }
        EC_KEY *eckeyLocal = PEM_read_EC_PUBKEY(f, NULL, NULL, NULL);
        fclose(f);
        if (rsa != NULL)
        {
            if(authType == Authentication :: RSA)
            {
                keySize = 0x200;
                RSA_free(rsa);
                FILE* f;
                f = fopen(filename.c_str(), "r");
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

                    fseek(f, 0, 0); // rewind

                    /* If the file starts with 'N', then it is Xilinx Format
                       If it starts with '-', then it is OpenSSL format */
                    if (name == 'N')
                    {
                        LOG_INFO("Parsing RSA key (Xilinx Format)");
                        errCode = ParseXilinxRsaKey(f);
                    }
                    else if (name == '-')
                    {
                        LOG_INFO("Parsing RSA key (OpenSSL Format)");
                        errCode = ParseOpenSSLKey(f);
                    }
                    else
                    {
                        LOG_ERROR("Unsupported key file - %s\n           Supported key formats: Xilinx Format & OpenSSL format", basefile.c_str());
                    }
                    fclose(f);

                    if (errCode != 0)
                    {
                        LOG_ERROR("RSA authentication key parsing failed - %s", basefile.c_str());
                    }

                    /* Calculate the modulus extension, i.e. Montgomery Reduction term RR
                    and some sanity check for the keys passed */
                    {
                        BIGNUM m; // modulus
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

                catch (...)
                {
                    fclose(f);
                    throw;
                }
            }
            else
            {
                LOG_ERROR("RSA keys are specified for ECDSAp384/ECDSAp521 authentication. Keyname is -%s",filename.c_str());
            }
        }
        else if (eckeyLocal != NULL)
        {
            if(authType != Authentication::RSA)
            {
                LOG_INFO("Parsing EC key (OpenSSL Format)");
                if (authType == Authentication::ECDSA)
                {
                    keySize = 0x30;
                }
                errCode = ParseECDSAOpenSSLKey(filename);
                Loaded = true;
            }
            else
            {
                LOG_ERROR("ECDSA keys are specified for RSA authentication. Keyname is - %s",filename.c_str());
            }
        }
    }
    else
    {
        // EC key
        if (word == "EC" )
        {
            if(authType != Authentication :: RSA)
            {
                LOG_INFO("Parsing EC key (OpenSSL Format)");
                errCode = ParseECDSAOpenSSLKey(filename);
                Loaded = true;
            }
            else
            {
                LOG_ERROR("ECDSA keys are specified for RSA authentication. Keyname is - %s",filename.c_str());
            }
        }
        // RSA key
        else
        {
            if(authType == Authentication :: RSA)
            {
                FILE* f;
                f = fopen(filename.c_str(), "r");
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

                    fseek(f, 0, 0); // rewind

                    /* If the file starts with 'N', then it is Xilinx Format. If it starts with '-', then it is OpenSSL format */
                    if (name == 'N')
                    {
                        LOG_INFO("Parsing RSA key (Xilinx Format)");
                        errCode = ParseXilinxRsaKey(f);
                    }
                    else if (name == '-')
                    {
                        LOG_INFO("Parsing RSA key (OpenSSL Format)");
                        errCode = ParseOpenSSLKey(f);
                    }
                    else
                    {
                        LOG_ERROR("Unsupported key file - %s\n           Supported key formats: Xilinx Format & OpenSSL format", basefile.c_str());
                    }
                    fclose(f);

                    if (errCode != 0)
                    {
                        LOG_ERROR("RSA authentication key parsing failed - %s", basefile.c_str());
                    }

                    /* Calculate the modulus extension, i.e. Montgomery Reduction term RR
                    and some sanity check for the keys passed */
                    {
                        BIGNUM m; // modulus
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

                catch (...)
                {
                    fclose(f);
                    throw;
                }
            }
            else
            {
                LOG_ERROR("RSA keys are specified for ECDSAp384/ECDSAp521 authentication. Keyname is -%s",filename.c_str());
            }
        }
    }
}
