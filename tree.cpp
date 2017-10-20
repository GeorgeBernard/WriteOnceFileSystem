#include "OnDiskStructure.h"

// Std lib includes
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <cstddef>

void print_metadata(std::fstream& input, m_hdr* hdr, unsigned int depth);

int main(int argc, char* argv[])
{
    m_hdr* root_ptr = new m_hdr;

    std::fstream input;
    input.open("example.wfs", std::ios::in | std::ios::binary);

    //input.seekg(0);
    input.read((char*)root_ptr, sizeof(m_hdr));

    std::cout << root_ptr->name << "\n"
            << "type: " << std::hex << root_ptr->type  << "\n"
            << "length: " << std::hex << root_ptr->length << "\n"
            << "time: " << std::hex << root_ptr->time << "\n"
            << "offset: " << std::hex << root_ptr->offset << "\n"
            << std::endl;

    //print_metadata(input, root_ptr, 0);

    input.close();
    return 0;
}


void print_metadata(std::fstream& input, m_hdr* hdr, unsigned int depth){


    std::cout << std::string(depth*2, '-') 
    << hdr->name 
    << std::endl;

    file_type type =  hdr->type;
    std::cout << type << std::endl;
    if(type == file_type::PLAIN_FILE)
    {
        return;
    }
    if(type == file_type::DIRECTORY)
    {
        uint64_t init_offset = hdr->offset;
        std::cout << hdr->length << std::endl;
        for(auto i = 0U; i < hdr->length; ++i){
            std::cout << i << std::endl;
            m_hdr* sub = new m_hdr;
            input.seekg(init_offset + i*sizeof(uint64_t));
            input.read((char*)sub, sizeof(m_hdr));
            print_metadata(input, sub, depth+1);
            delete sub;
        }
    }
}