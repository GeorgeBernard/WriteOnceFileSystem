#include "OnDiskStructure.h"

// Std lib includes
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <endian.h>

#include <stdio.h>

int writeHeader(m_hdr* header, std::ofstream& output);
void createTest1();
void createTest2();
void createTest3();
m_hdr* createHeader(std::string str,
    file_type type,
    uint64_t length,
    uint64_t time,
    uint64_t offset);

m_hdr* createHeader(std::string str,
    file_type type,
    uint64_t length,
    uint64_t time,
    uint64_t offset
    ) 
{
    m_hdr* header = new m_hdr;
    std::string buffer = std::string(255, ' ');
    std::copy(begin(str), end(str), begin(buffer));
    std::copy(begin(buffer), end(buffer), header->name);
    header->type = type;
    header->length = length;
    header->time = time;
    header->offset = offset;
    return header;
}

int main(int argc, char* argv[]) {
    createTest1();
    createTest2();
    createTest3();
    return 0;
}

void createTest1() {
    int m_hdrSize = 284;
    std::string root_str = "root";
    m_hdr* root_ptr = createHeader(root_str, file_type::DIRECTORY, 1, 0, m_hdrSize);

    uint64_t* offset_arr = new uint64_t[1];
    offset_arr[0] = sizeof(uint64_t) + m_hdrSize;

    std::string file_name = "test.txt";
    uint64_t file_offset = 2*m_hdrSize+sizeof(uint64_t);
    m_hdr* file_ptr = createHeader(file_name, file_type::PLAIN_FILE, 11, 0, file_offset);
    
    std::ofstream output;
    output.open("test1.wfs", std::ios::out | std::ios::trunc | std::ios::binary);

    if(output.is_open()){
        writeHeader(root_ptr, output);
        uint64_t reversed = htobe64(offset_arr[0]);
        output.write((char*) &reversed, sizeof(uint64_t));
        writeHeader(file_ptr, output);
        output << "I am a file";
        output.close();
    }
    else
    {
        std::cout << "fail" << std::endl;
    }
}

void createTest2() {
    int m_hdrSize = 284;
    int file1Size = 16;
    int file2Size = 16;

    std::string root_str = "root";
    m_hdr* root_ptr = createHeader(root_str, file_type::DIRECTORY, 2, 0, m_hdrSize);

    uint64_t* offset_arr = new uint64_t[2];
    uint64_t endArrayOffset = 2 * sizeof(uint64_t) + m_hdrSize;
    offset_arr[0] = endArrayOffset;
    offset_arr[1] = endArrayOffset + m_hdrSize;

    std::string file_name = "a.txt";
    uint64_t endHeadersOffset = endArrayOffset + 2 * m_hdrSize;
    m_hdr* file_ptr = createHeader(file_name, file_type::PLAIN_FILE, file1Size, 0, endHeadersOffset);
    
    std::string file2_name = "b.txt";
    m_hdr* file_ptr2 = createHeader(file2_name, file_type::PLAIN_FILE, file2Size, 0, endHeadersOffset + file1Size);
    
    std::ofstream output;
    output.open("test2.wfs", std::ios::out | std::ios::trunc | std::ios::binary);

    if(output.is_open()){
        writeHeader(root_ptr, output);
        uint64_t reversed = htobe64(offset_arr[0]);
        output.write((char*) &reversed, sizeof(uint64_t));
        uint64_t reversed2 = htobe64(offset_arr[1]);
        output.write((char*) &reversed2, sizeof(uint64_t));
        writeHeader(file_ptr, output);
        writeHeader(file_ptr2, output);
        output << "I am a file     ";
        output << "ryan            ";
        output.close();
    }
    else
    {
        std::cout << "fail" << std::endl;
    }
}

void createTest3() {
    int m_hdrSize = 284;
    int file1Size = 16;
    int numSubDirectories = 1;

    std::string root_str = "root";
    m_hdr* root_ptr = createHeader(root_str, file_type::DIRECTORY, numSubDirectories, 0, m_hdrSize);

    uint64_t* offset_arr = new uint64_t[numSubDirectories];
    uint64_t endArrayOffset = sizeof(uint64_t) + m_hdrSize;
    offset_arr[0] = endArrayOffset;

    std::string sub_dir_name = "Desktop";
    uint64_t endHeadersOffset = endArrayOffset + m_hdrSize;
    m_hdr* sub_dir_ptr = createHeader(sub_dir_name, file_type::DIRECTORY, 1, 0, endHeadersOffset);
    
    uint64_t* desktop_offset_arr = new uint64_t[1];
    desktop_offset_arr[0] = endHeadersOffset + sizeof(uint64_t);

    std::string file_name = "hw.txt";
    uint64_t file1Offset = endHeadersOffset + sizeof(uint64_t) + m_hdrSize;
    m_hdr* file_ptr = createHeader(file_name, file_type::PLAIN_FILE, file1Size, 0, file1Offset);
    
    std::ofstream output;
    output.open("test3.wfs", std::ios::out | std::ios::trunc | std::ios::binary);

    if(output.is_open()){
        writeHeader(root_ptr, output);
        uint64_t reversed = htobe64(offset_arr[0]);
        output.write((char*) &reversed, sizeof(uint64_t));
        writeHeader(sub_dir_ptr, output);
        uint64_t reversed2 = htobe64(desktop_offset_arr[0]);
        output.write((char*) &reversed2, sizeof(uint64_t));
        writeHeader(file_ptr, output);
        output << "Important hw!!!!";
        output.close();
    }
    else
    {
        std::cout << "fail" << std::endl;
    }
}

int writeHeader(m_hdr* header, std::ofstream& output) {
    output.write(header->name, 255);
    char term = '\0';
    output.write(&term, sizeof(char));
    uint64_t reversed = htobe64(header->length);
    output.write((char*) &reversed, sizeof(uint64_t) );
    output.write((char*) &header->time, sizeof(uint64_t) );
    uint64_t reversedOffset = htobe64(header->offset);
    output.write((char*) &reversedOffset, sizeof(uint64_t) );
    uint32_t reversedType = htobe32(header->type);
    output.write((char*) &reversedType, sizeof(uint32_t) );
    return 0;

}

