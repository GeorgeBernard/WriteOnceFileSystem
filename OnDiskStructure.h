#ifndef OnDiskStructure_H_   /* Include guard */
#define OnDiskStructure_H_


enum file_type {DIRECTORY = 0, PLAIN_FILE = 1, SYM_LINK = 2};


// structure to store infomration while parsing
struct metadata_parse {
	char name[256];       // name of the file or directory
	enum file_type type;    // indicates file or directory 0 - file 1 - directory
	uint64_t length; // for file the length of the file in bytes, for directory the number of sub files/directories
	uint64_t time;   // The time of access in UNIX time - saved as an unsigned long
	void* p;              // Pointer to either the start of the file or the subfiles/directories
						   // if pointing to a file be sure to cast before using
};
typedef struct metadata_parse m_prs;

// structure to store infomration while parsing
struct metadata_header {
	char name[256];       // name of the file or directory
	enum file_type type;    // indicates file or directory 0 - file 1 - directory
	uint64_t length;
	uint64_t time;
	uint64_t offset;

};
typedef struct metadata_header m_hdr;

#endif
