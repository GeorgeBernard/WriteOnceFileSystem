#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <algorithm>

#include "OnDiskStructure.h"


static int s_builder(const char *, const struct stat *, int, struct FTW *);
int image();
static int display_info(const char *, const struct stat *, int, struct FTW *);
const char* parse_name(const char *);
uint64_t find_header_size();


const int MAX_METADATA = 1000;

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
		std::cout << "This program currently only supports mastering a single directory."
		"Package the directories into a single directory and pass that in.  Thanks." << "\n";
		return 0;
	}

	// Using the nftw() funtion to update global structure
	// see here: https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rnftw.htm
	// filepath
	// function to call on each directory/file
	// max number of directories that can be used
	// flags to specialize usage, we aren't using any right now
	int result =  nftw(argv[1], s_builder, MAX_METADATA, 0);

	// print out metadata properties for testing purposes
	for(int i = 0; i < metadataPointer; i++){
		std::cout << meta[i].name << "\n";
		std::cout << meta[i].type << "\n";
		std::cout << meta[i].length << "\n";
	}
	//std::cout << header_count << "\n";
	//std::cout << subitems_count << "\n";


	// Now write the file to a structure
	int imageStatus = image();
	return 0;
}

// function called on each sub directory/file, updates the global information
static int s_builder(const char * path_name, const struct stat * object_info, int ftw, struct FTW * data){
	std::cout << "Called s_builder on:" << "\n";
	std::cout << path_name << "\n";
	std::cout << "ftw: " << ftw << std::endl;
	header_count++;

	std::cout << "dir: " << FTW_D << std::endl;
	std::cout << "file: " << FTW_F << std::endl;

	// store directory metadata
	if (ftw == FTW_D) { //if directory
		//get filename
		const char* file_name = path_name;
		//const char* file_name = path_name;
		std::cout << "Dir name: " << file_name << std::endl;
		// get number of files in directory algo from: https://stackoverflow.com/questions/1723002/how-to-list-all-subdirectories-in-a-given-directory-in-c?answertab=votes#tab-top
		int dir_length = 0;

		struct dirent* d;
		DIR* rdir = opendir(path_name);
		while ((d = readdir(rdir)) != NULL) {
			struct stat st;
			if (fstatat(dirfd(rdir), d->d_name, &st, 0) < 0){
				perror(d->d_name);
			}
			dir_length++;
			// else if(strstr(d->d_name, ".") != NULL && strstr(d->d_name, "..") != NULL){
			// 	dir_length++;
			// }
		}
		closedir(rdir);

		// assign all values
		subitems_count += dir_length;

		for (int i = 0; i < 255; i++) {
			meta[metadataPointer].name[i] = file_name[i]; // copy over all chars in dir name
		}
		meta[metadataPointer].type = DIRECTORY;
		meta[metadataPointer].length = dir_length;
		meta[metadataPointer].time = object_info->st_mtime;
		meta[metadataPointer].p = (void*) opendir(path_name);
		std::cout << "type: " << meta[metadataPointer].type << std::endl;
		std::cout << "length: " << meta[metadataPointer].length << std::endl;
		std::cout << "time: " << meta[metadataPointer].time << std::endl;
		std::cout << "p: " << meta[metadataPointer].p << std::endl;
		metadataPointer++;
		return 0;
	}

	// not a directory
	// Store File Metadata - possibly add in FTW_NS and FTW_SNL functionality for failed symbolic links
	else if ((ftw == FTW_F) || (ftw == FTW_SL)) {
		//get filename
		const char* file_name = path_name;
		// assign all values
		for(int i = 0; i < 255; i++){
			meta[metadataPointer].name[i] = file_name[i];
		}
		meta[metadataPointer].type = PLAIN_FILE;
		meta[metadataPointer].length = object_info->st_size; // need the size
		meta[metadataPointer].time = object_info->st_mtime; // time of last modification, could also use atime for last access or ctime for last status change
		meta[metadataPointer].p = fopen(path_name, "r"); // open the file for reading, when writing to img use this stream

		std::cout << "type: " << meta[metadataPointer].type << std::endl;
		std::cout << "length: " << meta[metadataPointer].length << std::endl;
		std::cout << "time: " << meta[metadataPointer].time << std::endl;
		std::cout << "p: " << meta[metadataPointer].p << std::endl;
		metadataPointer++;
		return 0;
	}

	return 7; // indicates an error

}

// returns final token separated by /
const char* parse_name(const char * path_name){
	std::cout << "in parse name: " << path_name << std::endl;
	std::string s = path_name;
	std::string d = "/";
	std::string token;
	size_t pos = 0;
	while ((pos = s.find(d)) != std::string::npos) { //until end of string is reached
		token = s.substr(0, pos);
		s.erase(0, pos + d.length());
	}
	std::cout << "end parse name: " << path_name << std::endl;
	return s.c_str(); //convert from cpp to c formatted string
	// char* buffer = new char[s.size()];
	// std::size_t length = s.copy(buffer, s.size(), 0);
	// buffer[length]='\0';
	// std::cout << "end parse name: " << path_name << std::endl;
	// return buffer;

}

// function to write the file
int image(){
	uint64_t header_offset = 0;
	uint64_t file_offset = find_header_size();
	uint64_t end_offset = 0;

	FILE *output;
	output = fopen("./output.wofs", "a"); // open in append mode

	//write type dependent information
	for(int i = 0; i < metadataPointer; i++){
		//Write universal information
		fwrite(meta[i].name, sizeof(char), 255, output);
		char lastChar = '\0';
		fwrite(&lastChar, sizeof(char), 1, output);
		fwrite((char*) meta[i].length, sizeof(uint64_t), 1, output);
		fwrite((char*) meta[i].time, sizeof(uint64_t), 1, output);
		fwrite((char*) meta[i].type, sizeof(uint32_t), 1, output );

		if(meta[i].type == PLAIN_FILE){
			fwrite((char*) &file_offset, sizeof(uint64_t), 1, output);
			header_offset += M_HDR_SIZE;

			// Write actual file to memory
			fseek(output, file_offset, SEEK_SET); // start at header
			char buffer[meta[i].length];
			size_t bytes;

			while (0 < (bytes = fread(buffer, 1, sizeof(buffer), (FILE*) meta[i].p))){
				fwrite(buffer, 1, bytes, output);
			}
			file_offset += meta[i].length;
			// Move back to header
			fseek(output, header_offset, SEEK_SET);
			fclose((FILE*) meta[i].p);
		}

		else if (meta[i].type == DIRECTORY){
			// Move header offset one header downward
			header_offset += M_HDR_SIZE;
			// Write the header offset to point to next block
			fwrite((char*) header_offset, sizeof(uint64_t), 1, output);

			if (end_offset < header_offset) {
					end_offset = header_offset;
					end_offset += meta[i].length * sizeof(uint64_t);
			}

			header_offset += meta[i].length * sizeof(uint64_t);

			// Write Directory Array Values
			struct dirent* d;
			DIR* rdir = (DIR*) meta[i].p;
			while((d = readdir(rdir)) != NULL){
				struct stat st;
				if (fstatat(dirfd(rdir), d->d_name, &st, 0) < 0){
					perror(d->d_name);
				}
				// Store address to correct array header
				else{
					// check whether current or parent directory
					if(strstr(d->d_name, ".") != NULL || strstr(d->d_name, "..") != NULL) {
						continue;
					}
					else {
						bool is_dir = d->d_type == DT_DIR;
						bool is_file = d->d_type == DT_REG;
						if (is_file) {
							fwrite((char*) &end_offset, sizeof(uint64_t), 1, output);
							end_offset += M_HDR_SIZE;
						}
						if (is_dir) {
							fwrite((char*) &end_offset, sizeof(uint64_t), 1, output);
							end_offset += M_HDR_SIZE;
							for (uint64_t j = i; j<= i+meta[i].length; j++) {
								const char* nameSafe = parse_name(d->d_name);
								if (strstr(nameSafe, meta[j].name)) {
									 end_offset += meta[j].length * sizeof(uint64_t);
								}
							}
						}
					}
				}
			}
			closedir(rdir);
		}
	}
	fclose(output);
	return 0;
}

uint64_t find_header_size(){
	uint64_t h_size = header_count * sizeof(m_hdr) + subitems_count * sizeof(uint64_t);
	return h_size;
}
