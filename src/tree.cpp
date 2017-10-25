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
void print_metadata(std::fstream& input, const m_hdr& hdr, unsigned int depth);

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
m_hdr readHeader(std::fstream& input, uint64_t offset);

/**
    Prints just the header and optionally file metadata/data
    
    @param s: the pointer to the first character in the space padded buffer
    @return the payload string within the space padded buffer   
*/
void printHeader(std::fstream& input, const m_hdr& hdr, const int depth);

/**
    Trims spaces from beginning and end of a string

    @param s: the string input
    @return the trimmed string
*/
std::string trimSpaces(const std::string& s);

/**
    Converts a string of unix time to human readable Y-M-D H:M:S

    @param time: the date time object
    @return the human readable date time string
*/
std::string unixTimeToHumanTime(const time_t& time);


//============================== Static Globals ==============================//

static bool disp_verbose;
static bool disp_content;

static const std::string empty = "    ";
static const std::string child_line = "|___";

//=========================== Function Definitions ===========================//


int main(int argc, char* argv[])
{
    // Sanity check arguments and ensure the user has given us a filename
    if (argc < 2) {
        std::cout << "please provide name of file" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc > 3) {
        std::cout << "Too many arguments!" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 3){
        std::string flag = argv[1];
        disp_verbose = (flag == "-v") || (flag == "-vc");
        disp_content = (flag == "-vc");
    }

    // Create file stream, and open with user given filename
    std::fstream input;
    const std::string filename = (argc == 2) 
                               ? argv[1] 
                               : argv[2];
    input.open(filename, std::ios::in | std::ios::binary);

    // Check if file was properly opened
    if (!input.is_open()) {
        std::cout << "failed to open " << filename << std::endl;
        return EXIT_FAILURE;
    }

    // Set stream position to zero, and get first header 
    input.seekg(0);
    m_hdr root_ptr = readHeader(input, 0);
    print_metadata(input, root_ptr, 0);

    // Cleanup and return
    return EXIT_SUCCESS;
} // end main

m_hdr readHeader(std::fstream& input, uint64_t offset) {

    // Instantiate new header
    m_hdr header;

    // Read the header data starting at the offset
    input.seekg(offset);
    input.read(reinterpret_cast<char*>(&header), sizeof(m_hdr));

    // Set data values in output header
    header.type   = (file_type) toBigEndian32(header.type);
    header.length = toBigEndian64(header.length);
    header.time   = toBigEndian64(header.time);
    header.offset = toBigEndian64(header.offset);

    return header;
} // end readHeader

uint64_t readOffset(std::fstream& input, uint64_t offset) {
    input.seekg(offset);
    uint64_t read_offset;
    input.read((char*)&read_offset, sizeof(uint64_t));
    return toBigEndian64(read_offset);
} // end readOffset

uint64_t toBigEndian64(uint64_t value) {
    return htobe64(value);
} // end toBigEndian64

uint32_t toBigEndian32(uint32_t value) {
    return htobe32(value);
} // end toBigEndian32

void print_metadata(std::fstream& input, const m_hdr& hdr, unsigned int depth){

    // Prepend line with dashes to indicate depth
    for (auto i = 1U; i < depth; i++) {
        std::cout << empty; 
    }
    if(depth > 0) { std::cout << child_line; }

    // Then print name of current header
    printHeader(input, hdr, depth);

    const file_type& type = hdr.type;
    
    // If the file type is a normal file, no need to recurse
    if(type == file_type::PLAIN_FILE)
    {
        return;
    }

    // If the file is a directory, print the contents
    if(type == file_type::DIRECTORY)
    {
        uint64_t init_offset = hdr.offset;

        for(auto i = 0U; i < hdr.length; ++i){
            uint64_t next_offset = init_offset + i*sizeof(uint64_t);
            uint64_t next_header_block = readOffset(input, next_offset);
            
            m_hdr sub = readHeader(input, next_header_block);
            print_metadata(input, sub, depth+1);
        }
    }
} // end print_metadata

void printHeader(std::fstream& input, const m_hdr& hdr, const int depth) {

    std::cout << trimSpaces(hdr.name) << std::endl;

    if (disp_verbose) {
        for (auto i = 0U; i < depth; i++) {
            std::cout << empty; 
        }
        std::cout << " *Time: " << unixTimeToHumanTime(hdr.time) << std::endl;

        if (hdr.type == PLAIN_FILE) {
            for (auto i = 0U; i < depth; i++) {
                std::cout << empty; 
            }
            std::cout << " *Size: " << hdr.length << " B" << std::endl;
        }
    }
    if (disp_content && (hdr.type == PLAIN_FILE)) {
        
        // Create and reserve size of file
        char buffer[hdr.length];

        // Seek to beginning of file
        input.seekg(hdr.offset);
        input.read(reinterpret_cast<char*>(&buffer), sizeof(buffer));

        for (auto i = 0U; i < depth; i++) {
            std::cout << empty; 
        }

        std::cout << " *Contents: " << trimSpaces(buffer) << std::endl;
    }
}

std::string unixTimeToHumanTime(const time_t& time)
{
    struct tm * datetime;
    char buffer [30];
    datetime = localtime(&time);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", datetime);
    return std::string(buffer);
}

std::string trimSpaces(const std::string& s){

    const auto firstScan = s.find_first_not_of(' ');
    const auto first = firstScan == std::string::npos ? s.length() : firstScan;

    const auto last = s.find_last_not_of(' ');

    return s.substr(first, last-first+1);
}