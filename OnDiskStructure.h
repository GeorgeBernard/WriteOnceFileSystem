#ifndef OnDiskStructure_H_   /* Include guard */
#define OnDiskStructure_H_

// structure for metadata on disk
typedef struct metaData {
	char name[255];       // name of the file or directory
	unsigned int type;    // indicates file or directory 0 - file 1 - directory
	unsigned long length; // for file the length of the file in bytes, for directory the number of sub files/directories
	unsigned long time;   // The time of access in UNIX time - saved as an unsigned long
	void* p;              // Pointer to either the start of the file or the subfiles/directories
						   // if pointing to a file be sure to cast before using
} metadata;




#endif 