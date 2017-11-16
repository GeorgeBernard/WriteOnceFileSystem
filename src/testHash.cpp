// SHA2 test program
#include "sha256.h"
#include <iostream> // for std::cout only, not needed for hashing library
int main(int, char**)
{
  // create a new hashing object
  SHA256 sha256;
  // hashing an std::string
  std::cout << sha256("Hello World") << std::endl;
  // => a591a6d40bf420404a011733cfb7b190d62c65bf0bcda32b57b277d9ad9f146e
  // hashing a buffer of bytes
  return 0;
}