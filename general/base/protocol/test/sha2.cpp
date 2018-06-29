#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>


class SHA
{
    SHA256_CTX ctx;
    unsigned char m_digest[SHA224_DIGEST_LENGTH];

public:

    SHA(const char* string) {
        SHA224_Init(&ctx);
        SHA224_Update(&ctx, string, strlen(string));
    }

    void addData(const char* string) {
        SHA224_Update(&ctx, string, strlen(string));
    }

    void execute() {
        SHA224_Final(m_digest, &ctx);
        m_digest[SHA224_DIGEST_LENGTH] = 0;
    }

    void getDigest(char* out_digest) {
        strcpy(out_digest, (const char *)m_digest);
        out_digest[SHA224_DIGEST_LENGTH] = 0;
    }

    void getDigestHex(char* out_digestHex) {
        char mdString[SHA224_DIGEST_LENGTH * 2 + 1];

        for (int i = 0; i < SHA224_DIGEST_LENGTH; i++) {
            sprintf(&mdString[i * 2], "%02x", (unsigned int)m_digest[i]);
        }

        strcpy(out_digestHex, mdString);
        out_digestHex[SHA224_DIGEST_LENGTH * 2 + 1] = 0;
    }
};

int pri(char* temp) {
    char digest[64];
    char mdString[64];
    const char* string = "rootAwjDvPIzfJPTTgHsroot";
 
    SHA sha(string);
    sha.execute();
    sha.getDigest(digest);
    sha.getDigestHex(mdString);

    printf("digest: %s\n", digest);
    printf("SHA224 digest: %s\n", mdString);

    strcpy(temp, (const char *)digest);

    return 0;
}


int sec() {
    char digest[SHA224_DIGEST_LENGTH];
    char mdString[SHA224_DIGEST_LENGTH*2+1];
    const char* string = "otro";

    char hashpass[] = "";
    pri(hashpass);

    printf("still SHA224 digest: %s\n", hashpass);

    SHA sha(hashpass);
    sha.addData(string);
    sha.execute();
    sha.getDigest(digest);
    sha.getDigestHex(mdString);

    printf("digest: %s\n", digest);
    printf("SHA224 digest: %s\n", mdString);

    return 0;
}


int main() {

    sec();

    return 0;
}
