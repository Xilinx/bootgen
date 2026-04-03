#if !defined( LMS_UTILS_H_ )
#define LMS_UTILS_H_

extern int LmsSign(const char *keyname, const unsigned char *buffer, unsigned long int buffer_len, unsigned char* result0, size_t* sig_len, uint32_t* err);
extern int LmsVerify(const char *keyname, const unsigned char *buffer, size_t buffer_len, unsigned char* sig, size_t sig_len);

extern int GetLmsSignLength(const char *keyname, bool lmsOnly);
extern int GetLmsSignLengthFromPubKey(const char *keyname, bool lmsOnly);
extern int GetLmsSignLengthFromPubKeyParam(int *keyParam, int keyParamSize);
extern int GetLmsSignatureLength(int *keyParam,  int keyParamSize, const char *secretKey, const char *publicKey, bool lmsOnly);
extern bool update_private_key(unsigned char *private_key, size_t len_private_key, void *filename);
//extern int LmsKeyGeneration(const char *keyname, const char *parm_set);
extern int LmsKeyGeneration(const char *keyname, int* hash, int* h, int* w, int levels);

#endif /* LMS_UTILS_H_ */