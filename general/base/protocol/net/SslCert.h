#ifndef SSLCERT_H
#define SSLCERT_H

#include <iostream>
#include <stdio.h>
#include <string>

namespace CatchChallenger
{
    class SslCert
    {
        std::string key;
        std::string pem;

        public:
            SslCert(const char* key = nullptr, const char* pem = nullptr) {
                this->key.copy(const_cast<char*>(key), sizeof(key));
                this->pem.copy(const_cast<char*>(pem), sizeof(pem));
            }

            bool validated() {
                //todo verify the keys themselves
                return !key.empty() && !pem.empty();
            }
    };
}

#endif //SSLCERT_H
