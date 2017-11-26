#include <cstddef>
#include <string>
#include "ecc.cpp"


int main(){
  std::string infile = "testImage.img";
  std::string outfile = "results.txt";

  int decodeResults = decode(infile, outfile);
}
