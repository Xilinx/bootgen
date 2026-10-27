#ifndef PQC_OPENSSL_REQ_H
#define PQC_OPENSSL_REQ_H

/* LOG_ERROR (throws) if ML-DSA / SLH-DSA is requested without OpenSSL 3.5+. */
void RequirePqcOpenSsl(const char* feature);

#endif
