
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <openssl/hmac.h>
#include <sys/stat.h>

int main()
{

    // The key to hash
    char key[] = "secret key num2";

    // The data that we're going to hash using HMAC
    char data[] = "hello world testitty testitty test";

    unsigned char* digest;

    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc

    // Get File Size
    struct stat st;
    stat("testfile.txt", &st);
    long size = st.st_size;

    //Open up file and read into a buffer
    FILE* f = fopen("testfile.txt", "a+");
    unsigned char buffer[size];
    int bytes_read = fread(buffer, sizeof(char), size, f);

    // Make Hash
     digest = HMAC(EVP_sha256(), key, strlen(key), buffer, strlen(data), NULL, NULL);


    //Print the Hash
    // Be careful of the length of string with the choosen hash engine. SHA1 produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine
    char mdString[32];
    for(int i = 0; i < 32; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    // Append the Hash to the file
    fwrite (digest, sizeof(char), sizeof(mdString), f);
    fclose (f);
    return 0;
}
