#include "pqc_openssl_req.h"
#include "logger.h"

extern "C" {
#include "pqc_openssl.h"
}

void RequirePqcOpenSsl(const char* feature)
{
    if (bootgen_pqc_openssl_available() != 0) {
        return;
    }
    const char* name = (feature != nullptr && feature[0] != '\0') ? feature : "mldsa/slhdsa";
    LOG_ERROR("%s requires OpenSSL 3.5 or newer. This bootgen was built or is running with an older OpenSSL.",
              name);
}
