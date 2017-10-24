#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <string>
#include <endian.h>
#include <stack>
#include <queue>

#include "OnDiskStructure.h"


static int s_builder(const char *, const struct stat *, int, struct FTW *);
int image();
int imageDFS(node* root);
static int display_info(const char *, const struct stat *, int, struct FTW *);
std::string parse_name(const std::string& path_name);
uint64_t find_header_size();

uint64_t header_off;
uint64_t file_off;

const int MAX_METADATA = 1000;

m_prs* meta;

int metadataPointer = 0;
int header_count = 0;
int subitems_count = 0;
std::stack<node> directories;

int main(int argc, char **argv){

	// ensure input is appropriate
    if(argc < 2){
    	std::cout << "Please input a directory to master." << "\n";
    	return 0;
    }
    else if(argc > 2){
    	std::cout << "This program currently only supports the mastering of a single directory.  Please package the directories into an additional folder and pass that in.  Thanks." << "\n";
    	return 0;
    }

    //make dummy head node to store ptr to root of the tree
    m_prs* h = new m_prs;
    h->type = DIRECTORY;
    h->length = 1;

    // make dummy head to store first root
    node* head = new node;
    head->data = h;
    head->fill = 0;
    head->children = new node[1];

    directories.push(*head);

    // Using the nftw() funtion to update global structure
    //	see here: https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rnftw.htm
            // filepath
            // function to call on each directory/file
            // max number of directories that can be used

            // flags to specialize usage, we aren't using any right now
    int result =  nftw(argv[1], s_builder, MAX_METADATA, 0);
    std::cout << "after nftw" << std::endl;


    //iterate to real root
    node root = head->children[0];

    // initialize our array structure
    meta = (m_prs*) malloc(metadataPointer * sizeof(m_prs));
    int i = 0;

    // write the tree to an array  this should be put in a separate method later
     std::cout << "before we convert" << std::endl;
    std::queue<node> q;
    q.push(root);
    while(!q.empty()){
        std::cout << "before got data" <<std::endl;
        node t = q.front();
        std::cout << "got node" << std::endl;
        q.pop();
        std::cout << "q.pop" <<std::endl;
        meta[i] = *t.data;
        std::cout << "name: " << t.data -> name << std::endl;
        std::cout << "got the data" << std::endl;
        i++;
        // if not a file recurs through sub files/directories
        if(t.fill != -1){
            std::cout << "i am not a file " << std::endl;
            for(int j = 0; j < t.data->length; j++){
                std::cout << "pushing: " << t.children[j].data -> name << std::endl;
                q.push(t.children[j]);
            }
            std::cout << "after loop" << std::endl;
        }
    }
    std::cout << "after we convert" << std::endl;

    //return 0;
    // Write the BFS structure for testing purposes.
    for(int i = 0; i < metadataPointer; i++){
        //std::cout << meta[i].name << "\n";
        //std::cout << meta[i].type << "\n";
        //std::cout << meta[i].length << "\n";
    }
    //std::cout << header_count << "\n";
    //std::cout << subitems_count << "\n";

    //return 0;
    // Now write the file to a structure
    node* r = new node;
    *r =  head->children[0];
    //std::cout << "before we image" << std::endl;
    int imageStatus = imageDFS(r);
      // std::cout << "after we image" << std::endl;
	return 0;
}

// function called on each sub directory/file, updates the global information
static int s_builder(const char * path_name, const struct stat * object_info, int ftw, struct FTW * data){

    std::cout << path_name << std::endl;
    const auto name = parse_name(path_name);
    const char* dir_file_name = name.c_str();
    std::cout << dir_file_name << std::endl;
    
	if (name == "." || name == "..") {
		return 0;
	}

    header_count++;
	// store directory metadata
	if(ftw == FTW_D){

        // std::cout << "im a directory" << std::endl;
		//get filename
      const char* dir_name = dir_file_name;
			char* buffer = new char[255];
 			std::string str (dir_name);
 			std::size_t length = str.copy(buffer,255,0);
 			buffer[length]='\0';


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
						char firstChar = (d-> d_name)[0];
						if (firstChar == '.') {
							continue;
						}

        		dir_length++;
        	}
    	}
    	closedir(rdir);

		// assign all values
        m_prs* h = new m_prs;
        subitems_count = subitems_count + dir_length;
        for(int i =0; i < 255; i++){
            h->name[i] = buffer[i];
        }
		h->type = DIRECTORY;
		h->length = dir_length;
		h->time = object_info->st_mtime;
		h->p = (void*) opendir(path_name);

        // insert into parent if there is one
        if(!directories.empty()){
            node parent = directories.top();
            directories.pop();

            //write child information
            // child = parent.children[parent.fill]
            // std::cout << "added child: " << h -> name << " to: " 
                // << parent.data -> name << " at " << parent.fill << std::endl;
            parent.children[parent.fill].data = h;
            parent.children[parent.fill].fill = 0;
            parent.children[parent.fill].children = new node[h->length];

            //update the "fill" value
            parent.fill = parent.fill + 1;

            // if not parent not full add parent back to the stack
            if(parent.fill != parent.data->length){
                // std::cout << "adding " << parent.data -> name << " back to stack " << std::endl;
                directories.push(parent);
            }
             // add this directory to the stack

            // Add ourself to the stack only if we have children
            if(parent.children[parent.fill-1].data->length != 0){
                directories.push(parent.children[parent.fill-1]);
            }
           
        }

        // std::cout << "end of im a dir" << std::endl;
                //keep total size
		metadataPointer++;
        return 0;
	}
	// Store File Metadata - possibly add in FTW_NS and FTW_SNL functionality for failed symbolic links
	else if((ftw == FTW_F) || (ftw == FTW_SL)){
		//get filename
        const char* file_name = dir_file_name;
        char* buffer = new char[255];
 			std::string str (file_name);
 			std::size_t length = str.copy(buffer,255,0);
 			buffer[length]='\0';
 			//std::cout << "buffer: " << buffer << std::endl;
        //std::cout << "file name: " << file_name << std::endl;
        
       
		// assign all values
        m_prs* h = new m_prs;
        for(int i =0; i < 255; i++){
            h->name[i] = buffer[i];
        }
        h->type = PLAIN_FILE;
		h->length = object_info->st_size;
		h->time = object_info->st_mtime; // time of last modification, could also use atime for last access or ctime for last status change
		h->p = fopen(path_name, "r"); // open the file for reading, when writing to img use this stream
		//std::cout << "h name: " << h->name << std::endl;
        // add to the tree
        if(!directories.empty()){
            // get parent node
            node parent = directories.top();
            directories.pop();

            // add in this node's data
            parent.children[parent.fill].data = h;
            parent.children[parent.fill].fill = -1;
            parent.children[parent.fill].children = NULL;

            //update the parent fill value
            parent.fill = parent.fill + 1;

            // if the parent is not full add it back
            if(parent.fill != parent.data->length){
                directories.push(parent);
            }
        }
		metadataPointer++;
        return 0;
	}
    return 7; // indicates an error
}

void write64(uint64_t item, FILE* output) {
	uint64_t bigEndian = htobe64(item);
	fwrite((char*) &bigEndian, sizeof(uint64_t), 1, output);
}

void write32(uint32_t item, FILE* output) {
	uint32_t bigEndian = htobe32(item);
	fwrite((char*) &bigEndian, sizeof(uint32_t), 1, output);
}


//returns final token separated by /
std::string parse_name(const std::string& path_name){
    std::string s = path_name;
    std::string d = "/";
    std::string token;
    size_t pos = 0;
    while ((pos = s.find(d)) != std::string::npos) {
        token = s.substr(0, pos);
        s.erase(0, pos + d.length());
    }

    return s;
}

uint64_t writeDFS(node* node, FILE* output) {

	// std::cout << "node name: " << node -> data->name << std::endl;
	uint64_t currentOffset = header_off;

	fseek(output, currentOffset, SEEK_SET);
	char* buffer = new char[256];
	for(auto i = 0U; i < 256; i++){
		buffer[i] = ' ';
	}
	std::string str (node -> data -> name);
	std::size_t length = str.copy(buffer,str.size(),0);
	buffer[255]='\0';

	fwrite(buffer, sizeof(char), 256, output);

	if (node -> fill == -1) {
		// File
		// std::cout << "im a file" << std::endl;
		write64(node->data->length, output);
		write64(node->data->time, output);
		write64(file_off, output);
		write32(node->data->type, output );
		std::cout << "file offset: " << file_off << std::endl;
		fseek(output, file_off, SEEK_SET); // start at header

		uint64_t fileSize = node -> data -> length;
		char buffer[fileSize];
		size_t bytes;

		while (0 < (bytes = fread(buffer, 1, sizeof(buffer), (FILE*) node -> data -> p))){
			fwrite(buffer, 1, bytes, output);
		}
		file_off += fileSize;
		header_off += M_HDR_SIZE;
		
	} else if (node -> fill == 0) {
		// DIR
		std::cout << "im a dir" << std::endl;

		write64(node->data->length, output);
		write64(node->data->time, output);
		uint64_t endOffset = currentOffset + M_HDR_SIZE;
		write64(endOffset, output);
		write32(node->data->type, output );

		uint64_t numChildren = node -> data -> length;
		header_off = endOffset + sizeof(uint64_t) * numChildren;

		for (int i = 0; i<numChildren; i++) {
			tree_node child = (node -> children)[i];
			uint64_t childOffset = writeDFS(&child, output);
			uint64_t desiredSeekLoc = endOffset + i * sizeof(uint64_t);
			fseek(output, desiredSeekLoc, SEEK_SET); 
			write64(childOffset, output);
		}
	}
	return currentOffset;
}

int imageDFS(node* root) {
	header_off = 0;
	file_off = find_header_size();
	std::cout << "original final off: " << file_off << std::endl;

	FILE *output;
	output = fopen("./output.wofs", "wb");

	std::cout << "root name: " << root -> data->name << std::endl;
	writeDFS(root, output);
	fclose(output);
}

// function to write the file
int image(){
	std::cout << "in image" << std::endl;
	uint64_t header_offset = 0;
	uint64_t file_offset = find_header_size();
	uint64_t end_offset = 0;

	std::cout << "file offset: " << file_offset << std::endl;

	FILE *output;
	output = fopen("./output.wofs", "wb");


	//write type dependent information
	for(int i = 0; i < metadataPointer; i++){

		std::cout << "i: " << i << std::endl;
		std::cout << "name: " << meta[i].name << std::endl;
		//Write universal information

		char* buffer = new char[256];
		for(auto i = 0U; i < 256; i++){
			buffer[i] = ' ';
		}
		std::string str (meta[i].name);
		std::size_t length = str.copy(buffer,str.size(),0);
		buffer[255]='\0';

		fwrite(buffer, sizeof(char), 256, output);
		std::cout << std::dec << "length: " << meta[i].length << std::endl;
		std::cout << std::hex << "length: " << meta[i].length << std::endl;
		std::cout << std::dec << "time: " << meta[i].time << std::endl;
		std::cout << std::hex << "time: " << meta[i].time << std::endl;
		std::cout << std::dec << "type: " << meta[i].type << std::endl;
		std::cout << std::hex << "type: " << meta[i].type << std::endl;
		uint64_t reversedLength = htobe64(meta[i].length);
		uint64_t reversedTime = htobe64(meta[i].time);
		uint32_t reversedType = htobe32(meta[i].type);
		fwrite((char*) &reversedLength, sizeof(uint64_t), 1, output);
		fwrite((char*) &reversedTime, sizeof(uint64_t), 1, output);


		if(meta[i].type == PLAIN_FILE){
			write64(file_offset, output);
			write32(meta[i].type, output );
			//fwrite((char*) &file_offset, sizeof(uint64_t), 1, output);
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
			std::cout << "in directory" << std::endl;
			// Move header offset one header downward
			header_offset += M_HDR_SIZE;
			// Write the header offset to point to next block
			write64(header_offset, output);
			write32(meta[i].type, output );
			//fwrite((char*) &header_offset, sizeof(uint64_t), 1, output);

			if (end_offset < header_offset) {
					end_offset = header_offset;
					end_offset += meta[i].length * sizeof(uint64_t);
			}

			header_offset += meta[i].length * sizeof(uint64_t);

			// Write Directory Array Values
			struct dirent* d;
			DIR* rdir = (DIR*) meta[i].p;
			while((d = readdir(rdir)) != NULL){
				std::cout << "here" << std::endl;
				struct stat st;
				if (fstatat(dirfd(rdir), d->d_name, &st, 0) < 0){
					perror(d->d_name);
				}
				// Store address to correct array header
				else{
					std::cout << "name: " << d->d_name << std::endl;
					// check whether current or parent directory
					if(d->d_name[0] == '.') {
						std::cout << "I am continuoing" << std::endl;
						bool tf = d->d_name[0] == '.';
						std::cout << "TF: " << tf << std::endl;
						continue;
					}
					else {
						std::cout << "else" << std::endl;
						bool is_dir = d->d_type == DT_DIR;
						bool is_file = d->d_type == DT_REG;
						if (is_file) {
							std::cout << std::dec << "end offset: " << end_offset <<  std::endl;
							write64(end_offset, output);
							//fwrite((char*) &end_offset, sizeof(uint64_t), 1, output);
							end_offset += M_HDR_SIZE;
						}
						if (is_dir) {
							std::cout << std::dec <<  "end offset: " << end_offset << std::endl;
							write64(end_offset, output);
							//fwrite((char*) &end_offset, sizeof(uint64_t), 1, output);
							end_offset += M_HDR_SIZE;
							for (uint64_t j = i; j< i+meta[i].length; j++) {
								const auto nameSafe = parse_name(d->d_name);
								if (strstr(nameSafe.c_str(), meta[j].name)) {
									std::cout << std::dec << "increment" << meta[j].length << std::endl;
									 end_offset += meta[j].length * sizeof(uint64_t);
									 std::cout << std::dec << "after end offset " << end_offset << std::endl;
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
		std::cout << "header_count: " << header_count << std::endl;
		std::cout << "subitems_count: " << subitems_count << std::endl;
		std::cout << "header size: " << (M_HDR_SIZE) << std::endl;
    	uint64_t h_size = header_count * M_HDR_SIZE + subitems_count * sizeof(uint64_t);
    	std::cout << "h size: " << h_size << std::endl;
		return h_size;
}
