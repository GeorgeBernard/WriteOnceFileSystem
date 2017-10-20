#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>
#include <iostream>
#include <stdint.h>
#include <string>

#include "OnDiskStructure.h"


static int s_builder(const char *, const struct stat *, int, struct FTW *);
int image();
static int display_info(const char *, const struct stat *, int, struct FTW *);
const char* parse_name(const char *);



const int MAX_METADATA = 100;

m_prs* meta;

int metadataPointer = 0;
int header_count = 0;
int subitems_count = 0;

int main(int argc, char **argv){
	// initialize our structure
	meta = (m_prs*) malloc(MAX_METADATA * sizeof(m_prs)); 
	// ensure input is appropriate
    if(argc < 2){
    	std::cout << "Please input a directory to master." << "\n";
    	return 0;
    }
    else if(argc > 2){
    	std::cout << "This program currently only supports the mastering of a single directory.  Please package the directories into an additional folder and pass that in.  Thanks." << "\n";
    	return 0;
    }

    // Using the nftw() funtion to update global structure
    //	see here: https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rnftw.htm
            // filepath
            // function to call on each directory/file
            // max number of directories that can be used
            // flags to specialize usage, we aren't using any right now  
    int result =  nftw(argv[1], s_builder, MAX_METADATA, 0);		

    for(int i = 0; i < metadataPointer; i++){
        std::cout << meta[i].name << "\n";
        std::cout << meta[i].type << "\n";
        std::cout << meta[i].length << "\n";
    }
    //std::cout << header_count << "\n";
    //std::cout << subitems_count << "\n";


    // Now write the file to a structure
    image();
	return 0;
}

// function called on each sub directory/file, updates the global information
static int s_builder(const char * path_name, const struct stat * object_info, int ftw, struct FTW * data){
    //std::cout << "Called s_builder on:" << "\n"; 
    //std::cout << path_name << "\n";
    header_count++;
	// store directory metadata
	if(ftw == FTW_D){
		//get filename
        const char* file_name = parse_name(path_name);

		// get number of files in directory algo from: https://stackoverfloxw.com/questions/1723002/how-to-list-all-subdirectories-in-a-given-directory-in-c?answertab=votes#tab-top
        int dir_length = 0;
    	struct dirent* d;
    	DIR* rdir = opendir(path_name);
    	while((d = readdir(rdir)) != NULL)
    	{
        	struct stat st;
			if (fstatat(dirfd(rdir), d->d_name, &st, 0) < 0){
            	perror(d->d_name);
        	}
        	else{
        		dir_length++;        		
        	}
    	}
    	closedir(rdir);

		// assign all values
        subitems_count = subitems_count + dir_length;
        for(int i =0; i < 256; i++){
            meta[metadataPointer].name[i] = file_name[i];
        }

		meta[metadataPointer].type = DIRECTORY;
		meta[metadataPointer].length = dir_length;
		meta[metadataPointer].time = object_info->st_mtime;
		meta[metadataPointer].p = (void*) opendir(path_name);
		metadataPointer++;
        return 0;
	}
	// Store File Metadata - possibly add in FTW_NS and FTW_SNL functionality for failed symbolic links
	else if((ftw == FTW_F) || (ftw == FTW_SL)){
		//get filename
        const char* file_name = parse_name(path_name);
		// assign all values
        for(int i =0; i < 256; i++){
            meta[metadataPointer].name[i] = file_name[i];
        }		

        meta[metadataPointer].type = PLAIN_FILE;
		meta[metadataPointer].length = object_info->st_size;
		meta[metadataPointer].time = object_info->st_mtime; // time of last modification, could also use atime for last access or ctime for last status change
		meta[metadataPointer].p = fopen(path_name, "r"); // open the file for reading, when writing to img use this stream
		metadataPointer++;
        return 0;
	}
    return 7; // indicates an error
}

//returns final token separated by /
const char* parse_name(const char * path_name){
    std::string s = path_name;
    std::string d = "/";
    std::string token;
    size_t pos = 0;
    while ((pos = s.find(d)) != std::string::npos) {
        token = s.substr(0, pos);
        s.erase(0, pos + d.length());
    }

    return s.c_str();
}

// function to write the file 
int image(){
    uint64_t header_offset = 0;
    uint64_t file_offset = find_header_size();
    for(int i = 0; i < metadataPointer; i++){
        std::cout << meta[i].name << "\n";
        std::cout << meta[i].type << "\n";
        std::cout << meta[i].length << "\n";
    }
    
	return 0;
}


uint64_t find_header_size(){
    uint64_t h_size = header_count * sizeof(m_hdr) + subitems_count * sizeof(uint64_t);
} 













