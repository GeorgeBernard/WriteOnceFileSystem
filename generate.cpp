#include "OnDiskStructure.h"

// Std lib includes
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <endian.h>

#include <stdio.h>

int writeHeader(m_hdr* header, std::ofstream& output);

int main(int argc, char* argv[])
{
    // std::cout << "Creating a test file" << std::endl;
    // // Intend to create directory with one file

    // m_test* ptr = new m_test;
    // ptr -> a = 1;
    // ptr -> b = 999999999;
    // ptr -> c = 9999999999;
    // std::ofstream o;
    // o.open("ryanTest.wfs", std::ios::out | std::ios::trunc | std::ios::binary);
    // uint64_t reversedA = htobe64(ptr->a);
    // o.write((char*) &reversedA, sizeof(uint64_t) );
    // uint32_t reversedB = htobe32(ptr->b);
    // o.write((char*) &reversedB, sizeof(uint32_t) );
    // uint64_t reversedC = htobe64(ptr->c);
    // o.write((char*) &reversedC, sizeof(uint64_t) );
    // return 0;

    int m_hdrSize = 284;
    m_hdr* root_ptr = new m_hdr;
    std::string root_str = "root";
    std::string buffer = std::string(255, ' ');
    std::copy(begin(root_str), end(root_str), begin(buffer));
    std::copy(begin(buffer), end(buffer), root_ptr->name);
    root_ptr->type = file_type::DIRECTORY;
    root_ptr->length = 1;
    //root_ptr->length = 55;
    root_ptr->time = 0;
    root_ptr->offset = m_hdrSize;

    uint64_t* offset_arr = new uint64_t[1];
    offset_arr[0] = sizeof(uint64_t) + m_hdrSize;

    m_hdr* file_ptr = new m_hdr;
    std::string file_name = "test.txt";
    strcpy(file_ptr->name, file_name.c_str());
    file_ptr->type = file_type::PLAIN_FILE;
    file_ptr->time = 0;
    file_ptr->length = 1;
    file_ptr->offset = 2*m_hdrSize+sizeof(uint64_t);

    std::ofstream output;
    output.open("example.wfs", std::ios::out | std::ios::trunc | std::ios::binary);

    // append root
    if(output.is_open()){
        writeHeader(root_ptr, output);
        char term = '\0';

        std::cout << "offset: " << offset_arr[0] << std::endl;
        std::cout << "offset: " << std::hex << offset_arr[0] << std::endl;

        uint64_t reversed = htobe64(offset_arr[0]);
        output.write((char*) &reversed, sizeof(uint64_t));

        writeHeader(file_ptr, output);
        // output.write(file_ptr->name, 255);
        // output.write(&term, sizeof(char));
        // output.write((char*) &file_ptr->type, sizeof(file_type));
        // output.write((char*) &file_ptr->length, sizeof(uint64_t) );
        // output.write((char*) &file_ptr->time, sizeof(uint64_t) );
        // output.write((char*) &file_ptr->offset, sizeof(uint64_t) );

        output << "I am a file";
        output.close();
    }
    else
    {
        std::cout << "fail" << std::endl;
    }

    return 0;
}

int writeHeader(m_hdr* header, std::ofstream& output) {
    
    std::cout << "writing header" << std::endl;
    output.write(header->name, 255);
    char term = '\0';
    output.write(&term, sizeof(char));
    //std::cout << sizeof(file_type) << root_ptr->type << std::endl;
    //output.write((char*) &root_ptr->type, sizeof(file_type));
    //output.write((char*) &root_ptr->length, sizeof(uint64_t) );
    //printf("0x%16x", &root_ptr->length);
    uint64_t reversed = htobe64(header->length);
    output.write((char*) &reversed, sizeof(uint64_t) );
    
    output.write((char*) &header->time, sizeof(uint64_t) );
    uint64_t reversedOffset = htobe64(header->offset);
    output.write((char*) &reversedOffset, sizeof(uint64_t) );
    uint32_t reversedType = htobe32(header->type);
    output.write((char*) &reversedType, sizeof(uint32_t) );
    return 0;

}

