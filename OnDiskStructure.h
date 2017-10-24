#ifndef OnDiskStructure_H_   /* Include guard */
#define OnDiskStructure_H_

#include <cstdint>

enum file_type : uint32_t {DIRECTORY = 0, PLAIN_FILE = 1, SYM_LINK = 2};

// structure for metadata on disk
struct metadata_parse {
	char name[255];       	// name of the file or directory
	enum file_type type;    // indicates file or directory 0 - file 1 - directory
	uint64_t length; 		// for file the length of the file in bytes, for directory the number of sub files/directories
	uint64_t time;   		// The time of access in UNIX time - saved as an unsigned long
	void* p;              	// Pointer to either the start of the file or the subfiles/directories
						  		// if pointing to a file be sure to cast before using
};
typedef struct metadata_parse m_prs;


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

struct tree_node {
    m_prs* data;
    tree_node* children;
    uint32_t fill;
};
typedef struct tree_node node;

#define M_HDR_SIZE sizeof(m_hdr::name) + sizeof(m_hdr::type) + sizeof(m_hdr::length) + sizeof(m_hdr::time) + sizeof(m_hdr::offset);

#endif
