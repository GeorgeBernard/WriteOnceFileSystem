#include "OnDiskStructure.h"

// Std lib includes
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <endian.h>
#include <bitset>

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
*/
uint64_t readOffset(std::fstream& input, uint64_t offset);

/** 
*/
m_hdr* readHeader(std::fstream& input, uint64_t offset);

/** 
*/
void printString(char* s);

int main(int argc, char* argv[])
{
    if (argc < 1) {
        std::cout << "please provide name of file" << std::endl;
        return 0;
    }

    char* fileName = argv[1];
    m_hdr* root_ptr = new m_hdr;

    std::fstream input;
    input.open(fileName, std::ios::in | std::ios::binary);

    input.seekg(0);
    root_ptr = readHeader(input, 0);
    print_metadata(input, root_ptr, 0);
    return 0;
}

m_hdr* readHeader(std::fstream& input, uint64_t offset) {

    m_hdr* header = new m_hdr;
    input.seekg(offset);
    input.read((char*)header, sizeof(m_hdr));

    header -> type = (file_type) toBigEndian32(header -> type);
    header -> length = toBigEndian64(header -> length);
    header -> time = toBigEndian64(header -> time);
    header -> offset = toBigEndian64(header -> offset);

    return header;
}

uint64_t readOffset(std::fstream& input, uint64_t offset) {
    input.seekg(offset);
    uint64_t* read_offset = new uint64_t;
    input.read((char*)read_offset, sizeof(uint64_t));
    return toBigEndian64(*read_offset);
}

uint64_t toBigEndian64(uint64_t value) {
    return htobe64(value);
}

uint32_t toBigEndian32(uint32_t value) {
    return htobe32(value);
}

void print_metadata(std::fstream& input, m_hdr* hdr, unsigned int depth){

    // Prepend line with dashes to indicate depth
    for (auto i = 0U; i < depth; i++) {
        std::cout << '-';
    }
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
        for(auto i = 0U; i < hdr->length; ++i){
            uint64_t next_offset = init_offset + i*sizeof(uint64_t);
            uint64_t x = readOffset(input, next_offset);

            m_hdr* sub = new m_hdr;
            sub = readHeader(input, x);
            print_metadata(input, sub, depth+1);
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