#include <cstdio>
#include <iostream>
#include <openssl/pem.h>
#include <openssl/x509.h>

EVP_PKEY * generate_key(int keySize);

X509 * generate_x509(EVP_PKEY * pkey);

bool write_to_stdout(EVP_PKEY * pkey, X509 * x509);

int doTest(void);
