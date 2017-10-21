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

    // m_test* test_ptr = new m_test;
    // std::fstream i;
    // i.open("ryanTest.wfs", std::ios::in | std::ios::binary);
    // i.read((char*)test_ptr, sizeof(m_test));
    // std :: cout << sizeof(test_ptr -> c) << "\n";
    // std::cout << std::hex << "a: " << test_ptr->a  << "\n"
    //         << std::hex << "b: " << test_ptr->b << "\n"
    //         << std::hex << "c: " << test_ptr->c << "\n"
    //         << std::endl;
    // // std::cout << std::hex << "b: " << test_ptr->b << "\n"
    // //         << std::endl;
    // return 0;


    m_hdr* root_ptr = new m_hdr;

    std::fstream input;
    input.open("example.wfs", std::ios::in | std::ios::binary);

    input.seekg(0);
    
    // uint64_t* length_pointer = new uint64_t;
    // input.read((char*)length_pointer, sizeof(uint64_t));
    // std::cout << std::hex << *length_pointer << "\n";
    // uint64_t reversed = htobe64(*length_pointer);
    // std::cout << std::hex << reversed << "\n";
    // return 0;

    // char name [256];
    // input.read((char*)name, 256);
    // std::cout << name << std::endl;
    // return 0;
    // uint32_t reversed2 = htobe32(root_ptr -> type);
    // //std::cout << "before reverse" << std::hex << root_ptr -> length << "\n";
    // std::cout << "before reverse: " << std::bitset<32> (root_ptr -> type) << "\n";
    // //std::cout << "after reverse" << std::hex << reversed << "\n";
    // std::cout << "after reverse: " <<  std::bitset<32> (reversed2) << "\n";

    // uint64_t reversed64 = htobe64(root_ptr -> length);
    // //std::cout << "before reverse" << std::hex << root_ptr -> length << "\n";
    // std::cout << "before reverse: " << std::bitset<64> (root_ptr -> length) << "\n";
    // //std::cout << "after reverse" << std::hex << reversed << "\n";
    // std::cout << "after reverse: " <<  std::bitset<64> (reversed64) << "\n";
    
    root_ptr = readHeader(input, 0);
    print_metadata(input, root_ptr, 0);
    return 0;

    std::cout << root_ptr->name << "\n"
            << "type: " << toBigEndian32(root_ptr -> type) << "\n"
            << "length: " << toBigEndian64(root_ptr -> length) << "\n"
            << "time: " << toBigEndian64(root_ptr->time) << "\n"
            << "offset: " << toBigEndian64(root_ptr->offset) << "\n"
            << std::endl;

    //print_metadata(input, root_ptr, 0);

    uint64_t* firstOffset = new uint64_t;
    std::cout << "seeked to: " << toBigEndian64(root_ptr -> offset) << std::endl;
    input.seekg(toBigEndian64(root_ptr->offset));

    input.read((char*)firstOffset, sizeof(uint64_t));

    uint64_t reversed64 = toBigEndian64(*firstOffset);
    std::cout << "before reverse: " << std::hex << *firstOffset << "\n";
    std::cout << "before reverse: " << std::bitset<64> (*firstOffset) << "\n";
    std::cout << "after reverse: " << std::hex << reversed64 << "\n";
    std::cout << "after reverse: " <<  std::bitset<64> (reversed64) << "\n";

    std::cout << "offset: " << toBigEndian64(*firstOffset) << std::endl;
    input.close();
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