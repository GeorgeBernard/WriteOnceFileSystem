
#include <stdio.h>
#include <string>
#include <vector>
#include <openssl/hmac.h>

int main()
{
    // The key to hash
    char key[] = "01234ggc";

    // The data that we're going to hash using HMAC
    char data[] = "hello world testitty testitty test";

    unsigned char* digest;

    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc

    std::ifstream file("myfile", std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)){
        digest = HMAC(EVP_sha256(), key, strlen(key), string(begin(buffer), end(buffer)).c_str(), strlen(data), NULL, NULL);
    }

    

    // Be careful of the length of string with the choosen hash engine. SHA1 produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine
    char mdString[32];
    for(int i = 0; i < 32; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    printf("HMAC digest: %s\n", mdString);

    return 0;
}
