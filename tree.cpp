#include "OnDiskStructure.h"

// Std lib includes
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <endian.h>
#include <bitset>

void print_metadata(std::fstream& input, m_hdr* hdr, unsigned int depth);
uint64_t toBigEndian64(uint64_t value);
uint32_t toBigEndian32(uint32_t value);
uint64_t readOffset(std::fstream& input, uint64_t offset);
m_hdr* readHeader(std::fstream& input, uint64_t offset);
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

    for (int i = 0; i<depth; i++) {
        std::cout << '-';
    }
    printString(hdr-> name);

    file_type type =  hdr->type;
    //std::cout << "file type: " << type << std::endl;
    
    if(type == file_type::PLAIN_FILE)
    {
        return;
    }
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
    int firstNonspace = 255;
    for (int i=254; i>=0; i--) {
        if (s[i] != ' ') {
            firstNonspace = i+1;
            break;
        }
    }

    char buffer[firstNonspace];
    std::string str (s);
    std::size_t length = str.copy(buffer, firstNonspace, 0);
    buffer[length] = '\0';
    std::cout << buffer << std::endl;
}