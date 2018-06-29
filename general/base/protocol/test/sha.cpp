#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
 
int main()
{
    unsigned char digest[SHA224_DIGEST_LENGTH];
    char string[] = "rootAwjDvPIzfJPTTgHsroot";
    
    SHA224((unsigned char*)&string, strlen(string), (unsigned char*)&digest);

    printf("digest: %s\n", digest);
 
    char mdString[SHA224_DIGEST_LENGTH*2+1];
 
    for(int i = 0; i < SHA224_DIGEST_LENGTH; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
 
    printf("SHA224 digest: %s\n", mdString);
 
    return 0;
}
