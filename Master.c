#include "OnDiskStructure.h"
#include <stdio.h>
#include <iostream.h>
#include <ftw.h>

const int MAX_METADATA = 2000;

metadata* meta;

int main(int argc, char **argv){
	// initialize our structure
	meta = (metadata*) malloc(MAX_METADATA * sizeof(metadata)); 
	// ensure input is appropriate
    if(argc < 2){
    	cout << "Please input a directory to master." << "\n";
    	return 0;
    }
    else if(argc > 2){
    	cout << "This program currently only supports the mastering of a single directory.  
    		Please package the directories into an additional folder and pass that in.  Thanks." << "\n";
    	return 0;
    }

    // Using the nftw() funtion to update global structure
    //	see here: https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rnftw.htm
    int result =  nftw64(argv[1], 	// filepath
    				s_builder,      // function to call on each directory/file
    				MAX_METADATA, 	// max number of directories that can be used
    				0);				// flags to specialize usage, we aren't using any right now  


    // Now write the file to a structure

	return 0;
}

// function called on each sub directory/file, updates the global information
int s_builder(const char *, const struct stat64 *, int, struct FTW *){

}

// function to write the file 

