#include "sha224.h"

SHA224::SHA(const char* string) {
    SHA224_Init(&ctx);
    SHA224_Update(&ctx, string, strlen(string));
}

void SHA224::addData(const char* string) {
    SHA224_Update(&ctx, string, strlen(string));
}

void SHA224::execute() {
    SHA224_Final(m_digest, &ctx);
    m_digest[SHA224_DIGEST_LENGTH] = 0;
}

void SHA224::getDigest(char* out_digest) {
    strcpy(out_digest, (const char *)m_digest);
    out_digest[SHA224_DIGEST_LENGTH] = 0;
}

void SHA224::getDigestHex(char* out_digestHex) {
    char mdString[SHA224_DIGEST_LENGTH * 2 + 1];

    for (int i = 0; i < SHA224_DIGEST_LENGTH; i++) {
        sprintf(&mdString[i * 2], "%02x", (unsigned int)m_digest[i]);
    }

    strcpy(out_digestHex, mdString);
    out_digest[SHA224_DIGEST_LENGTH * 2 + 1] = 0;
}