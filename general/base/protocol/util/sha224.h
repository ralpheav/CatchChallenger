#ifndef CATCHCHALLENGER_SHA224_H
#define CATCHCHALLENGER_SHA224_H

#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

namespace CatchChallenger
{
    class SHA224
    {
        private:

            SHA256_CTX ctx;
            unsigned char m_digest[SHA224_DIGEST_LENGTH];

        public:

            SHA224(const char* input_string);

            int addData(const char* input_string);
            int execute();
            void getDigest(char* out_digest);
            void getDigestHex(char* out_digestHex);
    };
}
#endif //CATCHCHALLENGER_SHA224_H
