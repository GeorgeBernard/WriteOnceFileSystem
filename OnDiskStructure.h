#ifndef OnDiskStructure_H_   /* Include guard */
#define OnDiskStructure_H_

#include <cstdint>

// structure for metadata on disk
struct metadata_parse {
	char name[255];       // name of the file or directory
	unsigned int type;    // indicates file or directory 0 - file 1 - directory
	unsigned long length; // for file the length of the file in bytes, for directory the number of sub files/directories
	unsigned long time;   // The time of access in UNIX time - saved as an unsigned long
	void* p;              // Pointer to either the start of the file or the subfiles/directories
						   // if pointing to a file be sure to cast before using
};
typedef struct metadata_parse m_prs;

enum file_type : uint32_t {DIRECTORY = 0, PLAIN_FILE = 1, SYM_LINK = 2};

struct metadata_header {
    char name[256];
    uint64_t length;
    uint64_t time;
    uint64_t offset;
    enum file_type type;
};
typedef struct metadata_header m_hdr;

struct metadata_test {
    uint64_t a; 
    uint32_t b;
    uint64_t c;

};
typedef struct metadata_test m_test;

#endif 