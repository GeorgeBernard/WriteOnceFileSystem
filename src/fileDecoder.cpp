/*
(**************************************************************************)
(* Based on the file_decoder in the Schifra library                       *)
(* The Schifra file was edited to return a status on decode               *)
(*                                                                        *)
(* Release Version 0.0.1                                                  *)
(* http://www.schifra.com                                                 *)
(* Copyright (c) 2000-2017 Arash Partow, All Rights Reserved.             *)                             *)
(*                                                                        *)
(**************************************************************************)
*/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "schifra/schifra_reed_solomon_block.hpp"
#include "schifra/schifra_reed_solomon_decoder.hpp"
#include "schifra/schifra_fileio.hpp"

namespace schifra
{
   namespace reed_solomon
   {

      template <std::size_t code_length, std::size_t fec_length, std::size_t data_length = code_length - fec_length>
      class file_decoder
      {

      enum return_type : int {SUCCESS = 0, ZERO_SIZE = 1, ERR_OPEN = 2, ERR_CREATE = 3, ERR_DECODE = 4};

      public:

         typedef decoder<code_length,fec_length> decoder_type;
         typedef typename decoder_type::block_type block_type;
         int errors_corrected;
         int errors_detected;

      /*
      // Public exposed API for decoding file
      // Decode the entire file: return return_type based on status of decode
      */
      public:
         inline int decode_file(const decoder_type& decoder,
                                    const std::string& input_file_name,
                                    const std::string& output_file_name) {
            const char* input_display = strrchr(input_file_name.c_str(), '/');
            const char* output_display = strrchr(output_file_name.c_str(), '/');
            std::cout << "Decoding " << input_display << " ..." <<std::endl;

            std::size_t remaining_bytes = schifra::fileio::file_size(input_file_name);
             if (remaining_bytes == 0)
            {
               std::cout << "Error: input file has ZERO size." << std::endl;
               return ZERO_SIZE;
            }

            std::ifstream in_stream(input_file_name.c_str(),std::ios::binary);
            if (!in_stream)
            {
               std::cout << "Error: input file could not be opened." << std::endl;
               return ERR_OPEN;
            }

            std::ofstream out_stream(output_file_name.c_str(),std::ios::binary);
            if (!out_stream)
            {
               std::cout << "Error: output file could not be created." << std::endl;
               return ERR_CREATE;
            }

            current_block_index_ = 0;

            while (remaining_bytes >= code_length)
            {
               int process_success = process_complete_block(decoder,in_stream,out_stream);
               if (process_success) {
                  return ERR_DECODE;
               }
               remaining_bytes -= code_length;
               current_block_index_++;
            }

            if (remaining_bytes > 0)
            {
               int process_success = process_partial_block(decoder,in_stream,out_stream,remaining_bytes);
               if (process_success) {
                  return ERR_DECODE;
               }
            }

            in_stream.close();
            out_stream.close();

            std::cout << std::endl << "REPORT " << std::endl;
            std::cout << "Decoded " << input_display << " into " << output_display << std::endl; 
            std::cout << "Errors detected: " << errors_detected << std::endl;
            std::cout << "Errors corrected: " << errors_corrected << std::endl;
            std::cout << "Recoverable: " << 1 << std::endl << std::endl;
            return SUCCESS;   
      }

      private:

         inline int process_complete_block(const decoder_type& decoder,
                                            std::ifstream& in_stream,
                                            std::ofstream& out_stream)
         {
            in_stream.read(&buffer_[0],static_cast<std::streamsize>(code_length));
            copy<char,code_length,fec_length>(buffer_,code_length,block_);

            int res = decoder.decode(block_);
            errors_detected = errors_detected + block_.errors_detected;
            errors_corrected = errors_corrected + block_.errors_corrected;
            
            if (!res)
            {
               std::cout << "Error during decoding of block " << current_block_index_ << "!" << std::endl;
               return ERR_DECODE;
            }

            for (std::size_t i = 0; i < data_length; ++i)
            {
               buffer_[i] = static_cast<char>(block_[i]);
            }

            out_stream.write(&buffer_[0],static_cast<std::streamsize>(data_length));
            return SUCCESS;
         }

         inline int process_partial_block(const decoder_type& decoder,
                                           std::ifstream& in_stream,
                                           std::ofstream& out_stream,
                                           const std::size_t& read_amount)
         {
            if (read_amount <= fec_length)
            {
               std::cout << "Error during decoding of block " << current_block_index_ << "!" << std::endl;
               return ERR_DECODE;
            }

            in_stream.read(&buffer_[0],static_cast<std::streamsize>(read_amount));

            for (std::size_t i = 0; i < (read_amount - fec_length); ++i)
            {
               block_.data[i] = static_cast<typename block_type::symbol_type>(buffer_[i]);
            }

            if ((read_amount - fec_length) < data_length)
            {
               for (std::size_t i = (read_amount - fec_length); i < data_length; ++i)
               {
                  block_.data[i] = 0;
               }
            }

            for (std::size_t i = 0; i < fec_length; ++i)
            {
               block_.fec(i) = static_cast<typename block_type::symbol_type>(buffer_[(read_amount - fec_length) + i]);
            }

            if (!decoder.decode(block_))
            {
               std::cout << "Error during decoding of block " << current_block_index_ << "!" << std::endl;
               return ERR_DECODE;
            }

            for (std::size_t i = 0; i < (read_amount - fec_length); ++i)
            {
               buffer_[i] = static_cast<char>(block_.data[i]);
            }

            out_stream.write(&buffer_[0],static_cast<std::streamsize>(read_amount - fec_length));
            return SUCCESS;
         }

         block_type block_;
         std::size_t current_block_index_;
         char buffer_[code_length];
      };
   } // namespace reed_solomon
} // namespace schifra
