#include <cstddef>
#include <string>
#include <iostream>
#include <fstream>
#include "../libraries/schifra/schifra_reed_solomon_block.hpp"
#include "../libraries/schifra/schifra_fileio.hpp"
#include "../libraries/schifra/schifra_galois_field.hpp"
#include "../libraries/schifra/schifra_sequential_root_generator_polynomial_creator.hpp"
#include "../libraries/schifra/schifra_reed_solomon_decoder.hpp"
#include "fileDecoder.cpp"
#include "config/decodeConstants.c"

int decode(std::string inFile, std::string outFile)
{
   const std::size_t field_descriptor    = FIELD_DESCRIPTOR;
   const std::size_t gen_poly_index      = GEN_POLY_INDEX;
   const std::size_t code_length         = CODE_LENGTH;
   const std::size_t fec_length          =   FEC_LENGTH;
   const std::string input_file_name     = inFile;
   const std::string output_file_name    = outFile;

   typedef schifra::reed_solomon::decoder<code_length,fec_length> decoder_t;
   typedef schifra::reed_solomon::file_decoder<code_length,fec_length> file_decoder_t;

   const schifra::galois::field field(field_descriptor,
                                      schifra::galois::primitive_polynomial_size06,
                                      schifra::galois::primitive_polynomial06);

   const decoder_t rs_decoder(field,gen_poly_index);

   file_decoder_t* fd = new file_decoder_t();
   int decode_success = fd -> decode_file(rs_decoder, input_file_name, output_file_name);
   //std::cout << rs_decoder.errors_corrected << std::endl;
   free(fd);
   return decode_success;
}
