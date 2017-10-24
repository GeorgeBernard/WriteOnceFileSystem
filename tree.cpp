#include "OnDiskStructure.h"

// Std lib includes
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <endian.h>
#include <bitset>

//========================== Function Declarations ===========================//

/** 
    Prints metadata tree from root of hdr
    
    @param input: the input stream from which to load the metadata.
    @param hdr: the root metadata header from which to print down from
    @param depth: the depth for which you call the print recursion
*/
void print_metadata(std::fstream& input, m_hdr* hdr, unsigned int depth);

/** 
    Converts value from little endian to big endian (i.e. reverses it)
*/
uint64_t toBigEndian64(uint64_t value);

/** 
    Converts value from little endian to big endian (i.e. reverses it)
*/
uint32_t toBigEndian32(uint32_t value);

/** 
    Reads value from input file at offset
    
    @param input: the input stream from which to load the metadata.
    @param offset: the offset from the beginning of the file that addresses the 
        value
    @return value at offset
*/
uint64_t readOffset(std::fstream& input, uint64_t offset);

/** 
    Reads header metadata struct from input file at offset

    @param input: the input stream from which to load the metadata.
    @param offset: the offset from the beginning of the file that addresses the 
        value
    @return header at offset
*/
m_hdr* readHeader(std::fstream& input, uint64_t offset);

/**
    Prints just the payload string in a space padded buffer
    
    @param s: the pointer to the first character in the space padded buffer
    @return the payload string within the space padded buffer   
*/
void printString(char* s);

//=========================== Function Definitions ===========================//

int main(int argc, char* argv[])
{
    // Sanity check arguments and ensure the user has given us a filename
    if (argc < 2) {
        std::cout << "please provide name of file" << std::endl;
        return EXIT_FAILURE;
    }

    // Create file stream, and open with user given filename
    std::fstream input;
    char* filename = argv[1];
    input.open(filename, std::ios::in | std::ios::binary);

    // Check if file was properly opened
    if (!input.is_open()) {
        std::cout << "failed to open " << filename << std::endl;
        return EXIT_FAILURE;
    }

    // Set stream position to zero, and get first header 
    input.seekg(0);
    m_hdr* root_ptr = readHeader(input, 0);
    print_metadata(input, root_ptr, 0);

    // Cleanup and return
    delete root_ptr;
    return EXIT_SUCCESS;
}

m_hdr* readHeader(std::fstream& input, uint64_t offset) {

    // Instantiate new header
    m_hdr* header = new m_hdr;

    // Read the header data starting at the offset
    input.seekg(offset);
    input.read((char*)header, sizeof(m_hdr));

    // Set data values in output header
    header -> type   = (file_type) toBigEndian32(header -> type);
    header -> length = toBigEndian64(header -> length);
    header -> time   = toBigEndian64(header -> time);
    header -> offset = toBigEndian64(header -> offset);

    return header;
}

uint64_t readOffset(std::fstream& input, uint64_t offset) {
    input.seekg(offset);
    uint64_t read_offset;
    input.read((char*)&read_offset, sizeof(uint64_t));
    return toBigEndian64(read_offset);
}

uint64_t toBigEndian64(uint64_t value) {
    return htobe64(value);
}

uint32_t toBigEndian32(uint32_t value) {
    return htobe32(value);
}

void print_metadata(std::fstream& input, m_hdr* hdr, unsigned int depth){

    static const std::string empty = "    ";
    static const std::string child_line = "|___";

    // Prepend line with dashes to indicate depth
    for (auto i = 0U; i < depth; i++) {
        if(i > 0) { std::cout << empty; }
    }
    std::cout << child_line;

    // Then print name of current header
    printString(hdr-> name);

    const file_type& type =  hdr->type;
    //std::cout << "file type: " << type << std::endl;
    
    // If the file type is a normal file, no need to recurse
    if(type == file_type::PLAIN_FILE)
    {
        return;
    }

    // If the file is a directory, print the contents
    if(type == file_type::DIRECTORY)
    {
        uint64_t init_offset = hdr->offset;
       // std::cout << "init_offset: " << init_offset << std::endl;
        for(auto i = 0U; i < hdr->length; ++i){
            uint64_t next_offset = init_offset + i*sizeof(uint64_t);
            uint64_t x = readOffset(input, next_offset);
           // std::cout << "next_offset: " << x << std::endl;
            m_hdr* sub = new m_hdr;
            sub = readHeader(input, x);
            print_metadata(input, sub, depth+1);

            // Cleanup
            delete sub;
        }
    }
}

void printString(char* s) {

    // iterate back to front to find first non-space character
    // i.e. finds the end of the payload string
    int firstNonspace = 255;
    for (int i=254; i>=0; i--) {
        if (s[i] != ' ') {
            firstNonspace = i+1;
            break;
        }
    }

    // Create a buffer that is the same length as the payload string
    char buffer[firstNonspace];

    // copy the argument string to the buffer
    std::string str (s);
    std::size_t length = str.copy(buffer, firstNonspace, 0);
    
    // Null-Terminate the buffer
    buffer[length] = '\0';

    // Finally, print the string
    std::cout << buffer << std::endl;
}