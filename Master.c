#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>
#include <iostream>
#include <stdint.h>

#include "OnDiskStructure.h"


int s_builder(const char *, const struct stat *, int, struct FTW *);
int image();

const int MAX_METADATA = 2000;

m_prs* meta;
int metadataPointer = 0;

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

    // Now write the file to a structure
    image();
	return 0;
}

// function called on each sub directory/file, updates the global information
int s_builder(const char * path_name, const struct stat * object_info, int ftw, struct FTW * data){
    std::cout << "Called s_builder on " << path_name << "\n";
	// store directory metadata
	if(ftw == FTW_D){
		//get filename
        char* path_name2;
        strncpy(path_name2, path_name, strlen(path_name)); 
        char* buf;
        buf = std::strtok(path_name2, "/");
        char* buff;
        while (buf != NULL){   
            buff = buf;
            buf = std::strtok (NULL, "/");
        }

		char* file_name = (char*) malloc(sizeof(char)*256);
        for(int i =0; i < sizeof(buff); i++){
            file_name[i] = buff[i];
        }
        std::cout << file_name <<"\n";

		// get number of files in directory algo from: https://stackoverfloxw.com/questions/1723002/how-to-list-all-subdirectories-in-a-given-directory-in-c?answertab=votes#tab-top
		int dir_length = 0;
    	struct dirent* d;
    	// TODO Consider this- 2 loops or one loop and then rebuilding loop?
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
        char* path_name2;
        strncpy(path_name2, path_name, strlen(path_name)); 
        char* buf;
        buf = std::strtok(path_name2, "/");
        char* buff;
        while (buf != NULL){   
            buff = buf;
            buf = std::strtok (NULL, "/");
        }

        char* file_name = (char*) malloc(sizeof(char)*256);
        for(int i =0; i < sizeof(buff); i++){
            file_name[i] = buff[i];
        }
        std::cout << file_name <<"\n";

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
}

// function to write the file 
int image(){
	

}














