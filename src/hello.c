/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

/** @file
 *
 * minimal example filesystem using high-level API
 *
 * Compile with:
 *
 *     gcc -Wall hello.c `pkg-config fuse3 --cflags --libs` -o hello
 *
 * ## Source code ##
 * \include hello.c
 */


#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <endian.h>
 #include <openssl/hmac.h>


struct metadata_header {
    char name[256];
    uint64_t length;
    uint64_t time;
    uint64_t offset;
    uint32_t type;
};
typedef struct metadata_header m_hdr;

/*
 * Command line options
 *
 * We can't set default values for the char* fields here because
 * fuse_opt_parse would attempt to free() them when the user specifies
 * different values on the command line.
 */
static struct options {
	const char *filename;
	const char *contents;
	int show_help;
} options;

#define OPTION(t, p)                           \
    { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
	OPTION("--name=%s", filename),
	OPTION("--contents=%s", contents),
	OPTION("-h", show_help),
	OPTION("--help", show_help),
	FUSE_OPT_END
};

char* image; 
FILE* fp;

static void *hello_init(struct fuse_conn_info *conn,
			struct fuse_config *cfg)
{
	(void) conn;
	cfg->kernel_cache = 1;
	return NULL;
}

static m_hdr* readHeader(uint64_t curr_offset) {

	fseek(fp, curr_offset, SEEK_SET);

	char* root_name = (char*) malloc(255);
	fread((void*)root_name, 1, 256, fp);
	//printf("root_name: %s \n", root_name);

	fseek(fp, (long) curr_offset+256, SEEK_SET);

	uint64_t length;
	//printf("sizeof: %d", sizeof(uint64_t));

	int status = fread((void*)&length, 8, 1, fp);
	uint64_t length2 = htobe64(length);
	//printf("length: %d \n", length2);

	uint64_t time;
	fread((void*)&time, 8, 1, fp);
	uint64_t time2 = htobe64(time);
	//printf("time: %d \n", time2);

	uint64_t offset;
	fread((void*)&offset, 8, 1, fp);
	uint64_t offset2 = htobe64(offset);
	//printf("offset: %d \n", offset2);

	uint64_t type;
	fread((void*)&type, 4, 1, fp);
	uint64_t type2 = htobe32(type);
	//printf("type: %d \n", type2);


	m_hdr* header = (m_hdr*) malloc(sizeof(m_hdr));

	//header -> name = root_name;
	strncpy(header -> name, root_name, 256);
	//printf("headerName: %s \n", header -> name);
	header-> length = length2;
	header -> offset = offset2;
	header -> time = time2;
	header -> type = type2;
	return header;
}

static uint64_t readOffset(uint64_t offset) {
	fseek(fp, (long) offset, SEEK_SET);
	uint64_t o;
	fread((void*)&o, 8, 1, fp);
	return htobe64(o);	
}

static m_hdr* find(const char* path) {
	if (path == NULL) {
		return NULL;
	}
	int print =0;
	if (strcmp(path, "/test/lev1")==0) {
		print = 1;
	}
	if (print) 
		printf("CALLING FIND on : %s \n", path);

	char* pathCopy = (char*) malloc(strlen(path) + 1);
	strcpy(pathCopy, path);
	char* token = strtok(pathCopy, "/");

	m_hdr* root = readHeader(0);
	m_hdr* current = root;

	while (token != NULL) {
		if (print)
			printf("token: %s \n", token);
		if (strcmp(current -> name, token) !=0) {
			if (print)
				printf("ERROR: %s not equal %s", current-> name, token);
			return NULL;
		}
		token = strtok(NULL, "/");
		if (token == NULL) {
			break;
		}

		if (current -> type == 1) {
			//printf("I am a file \n");
			//printf("FINAL CURRENT: %s \n", current -> name);
			return current;
		}

		uint64_t initOffset = current -> offset;
		if (print)
			printf("Length of current: %d \n", current -> length);
		int childFound = 0;
		for (int i =0; i< current -> length; i++) {
			if (print)
				printf("i: %d", i);
			uint64_t nextOffset = initOffset + i*sizeof(uint64_t);
			uint64_t next_header_block = readOffset(nextOffset);
			m_hdr* child = readHeader(next_header_block);
			if (print)
				printf("Child: %s \n", child-> name);
			if (strcmp(child -> name, token) == 0) {
				current = child;
				childFound = 1;
				break;
			}
		}
		if (!childFound) {
			if (print)
					printf("DID NOT FIND A CHILD \n");
				return NULL;
		}

	}

	//printf("FINAL CURRENT: %s \n", current -> name);

	return current;

}

static int hello_getattr(const char *path, struct stat *stbuf,
			 struct fuse_file_info *fi)
{
	(void) fi;
	int res = 0;

	if(!path) { return -ENOENT; }

	if (strcmp(path, "/test/lev1") == 0) {
		printf("Get attribute called on: %s \n", path);
	}
	//readHeader(fp, 0);
	//find("/test/ryan.txt", fp);


	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		//printf("Get attribute called on: %s", path);
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	}

	m_hdr* head = find(path);
	if (head == NULL) {
		return -ENOENT;
	}
	if (head -> type == 0) {
		// dir 
		stbuf -> st_mode = S_IFDIR | 0444;
		stbuf -> st_nlink = 1;

	} else if (head -> type == 1) {
		//printf("Get attribute called on: %s", path);
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_size = head -> length;
		printf("TIME: %d \n", head->time);
		stbuf->st_mtime = head -> time;
	} else {
		res = -ENOENT;
	}

	return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags)
{
	(void) offset;
	(void) fi;
	(void) flags;

	//printf("Hello readdir on: %s \n", path);

	m_hdr* dir_header;
	if (strcmp(path, "/") == 0) {
		//root
		//printf("Attempting to read the root directory \n");
		dir_header = readHeader(0);
		filler(buf, ".", NULL, 0, 0);
		filler(buf, "..", NULL, 0, 0);
		filler(buf, dir_header -> name, NULL, 0, 0);
		//printf("Added %s to the buffer and returned \n", dir_header -> name);
		return 0;
	} else {
		dir_header = find(path);
	}

	if (dir_header == NULL) {
		return -ENOENT;
	}

	if (dir_header -> type != 0) {
		//not a directory
		return -ENOENT;
	}

	filler(buf, ".", NULL, 0, 0);
	filler(buf, "..", NULL, 0, 0);

	uint64_t initOffset = dir_header -> offset;
	//printf("Length of current: %d \n", dir_header -> length);
	for (int i =0; i< dir_header -> length; i++) {
		//printf("i: %d \n", i);
		uint64_t nextOffset = initOffset + i*sizeof(uint64_t);
		uint64_t next_header_block = readOffset(nextOffset);
		m_hdr* child = readHeader(next_header_block);
		//printf("Child: %s \n", child-> name);
		filler(buf, child->name, NULL, 0, 0);
	}

	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	m_hdr* file_header = find(path);

	if (file_header == NULL) {
		// file not found
		return -ENOENT;
	}

	if (file_header -> type != 1) {
		// not a file
		return -ENOENT;
	}

	if ((fi->flags & O_ACCMODE) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	printf("Calling read on %s: \n", path);
	
	m_hdr* file_header = find(path);

	if (file_header == NULL) {
		return -ENOENT;
	}

	if (file_header -> type != 1) {
		// not a file
		return -ENOENT;
	}

	size_t length = file_header -> length;
	printf("File length: %d \n", length);
	uint64_t data_block_offset = file_header -> offset;
	fseek(fp, data_block_offset, SEEK_SET);
	//char* data_buffer = malloc(len+1);
	char* data_buffer = malloc(length);
	int status = fread((void*)data_buffer, 1, length, fp);
	len = strlen(data_buffer);
	printf("Data buffer: %s", data_buffer);
	printf("Length: %d \n", len);

	printf("fread status: %d \n", status);

	
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, data_buffer + offset, size);
	} else
		size = 0;

	free(data_buffer);
	printf("Size: %d \n", size);
	return size;
}

static struct fuse_operations hello_oper = {
	.init           = hello_init,
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
};

static void show_help(const char *progname)
{
	printf("usage: %s [options] <mountpoint>\n\n", progname);
	printf("File-system specific options:\n"
	       "    --name=<s>          Name of the \"hello\" file\n"
	       "                        (default: \"hello\")\n"
	       "    --contents=<s>      Contents \"hello\" file\n"
	       "                        (default \"Hello, World!\\n\")\n"
	       "\n");
}

int checkHash() {
	char* file_name = "/home/ras70/mounting/WriteOnceFileSystem/src/test.wofs";
	struct stat st;
 	stat(file_name, &st);
  	long file_size = st.st_size;
  	printf("File size %d\n", file_size);

  	int hashSize = 32;

  	long hashStart = file_size - hashSize;
  	fseek(fp, hashStart, SEEK_SET);

  	char hash[32];
  	int hashSuccess = fread(hash, hashSize, 1, fp);

  	printf("Successful read %d\n", hashSuccess);
  	printf("hash: %x\n", hash[0]);

  	
  	fseek(fp, 0, SEEK_SET);
  	unsigned char buffer[hashStart];
  	int bytes_read = fread(buffer, sizeof(char), hashStart, fp);


  	// Make Hash
  	const char* key = "stranger";
  	unsigned char* digest;
  	digest = HMAC(EVP_sha256(), key, strlen(key), buffer, hashStart, NULL, NULL);

  	//Print the Hash
  	// Be careful of the length of string with the choosen hash engine. SHA1 produces a 20-byte hash value which rendered as 40 characters.
  	// Change the length accordingly with your choosen hash engine
  	printf("digest[0] %x \n", digest[0]);

  	int r = 1;

  	for (int i =0; i< 32; i++) {
    	printf("i: %d file: %x hash: %x \n", i, (unsigned char) hash[i], digest[i]);
    	if ((unsigned char) hash[i] != digest[i]) {
      	r = 0;
    	}
  	}
  	return r;
	
}

int main(int argc, char *argv[])
{
	char** argsFuse = malloc(3 * sizeof(char*));
	argsFuse[0] = argv[0];
	argsFuse[1] = argv[1];
	argsFuse[2] = argv[2];
	struct fuse_args args = FUSE_ARGS_INIT(argc-1, argsFuse);

	fp = fopen("/home/ras70/mounting/WriteOnceFileSystem/src/test.wofs", "r");

	int hashCorrect = checkHash();
	printf("Hash correct: %d\n", hashCorrect);
	exit(0);
	/* Set defaults -- we have to use strdup so that
	   fuse_opt_parse can free the defaults if other
	   values are specified */
	//options.filename = strdup("hello");
	options.filename = strdup("test");
	options.contents = strdup("Hello World!\n");

	/* Parse options */
	if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
		return 1;

	/* When --help is specified, first print our own file-system
	   specific help text, then signal fuse_main to show
	   additional help (by adding `--help` to the options again)
	   without usage: line (by setting argv[0] to the empty
	   string) */
	if (options.show_help) {
		show_help(argv[0]);
		assert(fuse_opt_add_arg(&args, "--help") == 0);
		args.argv[0] = (char*) "";
	}

	return fuse_main(args.argc, args.argv, &hello_oper, NULL);
}
