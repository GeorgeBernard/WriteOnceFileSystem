#include <unistd.h>

const char* get_key_from_user() {
	int prompt_length = 41;
	char prompt[prompt_length+1] = "Please enter a key (size between 4-256): ";
    char* key =  getpass(prompt);
   	return key;
}