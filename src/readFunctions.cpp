#include <cstring>

static m_hdr* readHeader(FILE* fp, uint64_t curr_offset);
static uint32_t read32(FILE* fp, uint64_t offset);
static uint32_t read32_pure(FILE* fp); // no offset seek beforehand. Take care when using
static uint64_t read64(FILE* fp, uint64_t offset);
static uint64_t read64_pure(FILE* fp); // no offset seek beforehand. Take care when using

static uint64_t read64(FILE* fp, uint64_t offset) {
	fseek(fp, (long) offset, SEEK_SET);
	return read64_pure(fp);	
}

static uint64_t read64_pure(FILE* fp) {
	uint64_t data;
	fread((void*)&data, sizeof(uint64_t), 1, fp);
	return htobe64(data);	
}

static uint32_t read32(FILE* fp, uint64_t offset) {
    fseek(fp, offset, SEEK_SET);
    return read32_pure(fp);
}

static uint32_t read32_pure(FILE* fp) {
	uint32_t data;
	fread((void*)&data, sizeof(uint32_t), 1, fp);
	return htobe32(data);	
}

static m_hdr* readHeader(FILE* fp, uint64_t curr_offset) {

	fseek(fp, curr_offset, SEEK_SET);
	int name_length = 255;
	char* root_name = (char*) malloc(name_length);
	fread((void*)root_name, 1, name_length+1, fp);
	fseek(fp, (long) curr_offset+name_length+1, SEEK_SET);
	m_hdr* header = (m_hdr*) malloc(sizeof(m_hdr));
	strncpy(header -> name, root_name, name_length+1);
	header-> length = read64_pure(fp);
	header -> time = read64_pure(fp);
	header -> offset = read64_pure(fp);
	header -> type = (file_type) read32_pure(fp);
	return header;
}