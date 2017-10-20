#include "OnDiskStructure.h"

// Std lib includes
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>

#include <stdio.h>
int main(int argc, char* argv[])
{
    // Intend to create directory with one file
    m_hdr* root_ptr = new m_hdr;
    std::string root_str = "root";
    std::string buffer = std::string(255, ' ');
    std::copy(begin(root_str), end(root_str), begin(buffer));
    std::copy(begin(buffer), end(buffer), root_ptr->name);
    root_ptr->type = file_type::DIRECTORY;
    root_ptr->length = 1;
    root_ptr->time = 0;
    root_ptr->offset = sizeof(m_hdr);

    uint64_t* offset_arr = new uint64_t[1];
    offset_arr[0] = sizeof(uint64_t) + sizeof(m_hdr);

    m_hdr* file_ptr = new m_hdr;
    std::string file_name = "test.txt";
    strcpy(file_ptr->name, file_name.c_str());
    file_ptr->type = file_type::PLAIN_FILE;
    file_ptr->time = 0;
    file_ptr->length = 11;
    file_ptr->offset = 2*sizeof(m_hdr)+sizeof(uint64_t);
    
    std::ofstream output;
    output.open("example.wfs", std::ios::out | std::ios::trunc | std::ios::binary);

    // append root
    if(output.is_open()){
        output.write(root_ptr->name, 255);
        char term = '\0';
        output.write(&term, sizeof(char));
        std::cout << sizeof(file_type) << root_ptr->type << std::endl;
        output.write((char*) &root_ptr->type, sizeof(file_type));
        output.write((char*) &root_ptr->length, sizeof(uint64_t) );
        output.write((char*) &root_ptr->time, sizeof(uint64_t) );
        output.write((char*) &root_ptr->offset, sizeof(uint64_t) );
        
        output.write((char*) &offset_arr[0], sizeof(uint64_t));

        output.write(file_ptr->name, 255);
        output.write(&term, sizeof(char));
        output.write((char*) &file_ptr->type, sizeof(file_type));
        output.write((char*) &file_ptr->length, sizeof(uint64_t) );
        output.write((char*) &file_ptr->time, sizeof(uint64_t) );
        output.write((char*) &file_ptr->offset, sizeof(uint64_t) );

        output << "I am a file";
        output.close();
    }
    else
    {
        std::cout << "fail" << std::endl;
    }

    return 0;
}