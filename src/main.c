#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ssh_run.h"

#define ARG_SIZE 20
#define DEBUG

/**
 * Helper function to get strings with whitespaces from files
**/
void get_string_with_whitespace(FILE *fptr, char* config, int max_size){
	if(!fgets(config,max_size, fptr)){
		fprintf(stderr, "Bad config file\n");
		exit(-1);
	}
	// Remove trailing newline
	// https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
	config[strcspn(config, "\n")] = 0;	
}

int main(int argc, char** argv){
	// Check we are given file
	if(argc != 2){
		printf("Usage %s file\n", argv[0]);
		exit(-1);
	}

	if( access( argv[1], R_OK) != 0 ){
		printf("Error: %s not readble\n", argv[1]);
		exit(-1);
	}
	
	
	// Define arguements
	char host[ARG_SIZE];
	int port;
	char username[ARG_SIZE];
	char password[ARG_SIZE];
	char command[ARG_SIZE];
	int rc;
	
	FILE *fptr = fopen(argv[1], "r");
	fscanf(fptr, "%s\n", host);
	fscanf(fptr, "%d\n", &port);
	get_string_with_whitespace(fptr, username, 20);
	get_string_with_whitespace(fptr, password, 20);
	get_string_with_whitespace(fptr, command, 20);

	fclose(fptr);

	#ifdef DEBUG
	printf("Host: %s\n", host);
	printf("Port: %d\n", port);
	printf("Username: %s\n", username);
	printf("Password: %s\n", password);
	printf("Command: %s\n",  command);
	#endif
	
	// Create session
	ssh_session my_ssh_session = ssh_new();
	session_init(my_ssh_session, host, port, SSH_LOG_PROTOCOL);

	// Connect
	connect_session(my_ssh_session, host);

	// Get public key
	ssh_key key;
	get_public_key(my_ssh_session, &key);

	// Get public key hash
	unsigned char* hash = NULL;
	size_t hlen;
	get_key_hash(&key, &hash, &hlen);
	
	// Print hash
	ssh_print_hash( HASH_TYPE, hash, hlen);	

	// Authenticate
	authenticate_username_password(my_ssh_session, username, password);
	printf("Authentication: SUCCESS\n");

	// Create channel
	ssh_channel my_channel;
	create_channel(my_ssh_session, &my_channel);
	printf("Channel: SUCCESS\n");

	// Run command
	char buffer[BUFFER_SIZE];
	int nbytes = execute_ssh(my_ssh_session, my_channel, command, buffer);
	printf("%s", buffer);
	
	// Disconnect and free
	ssh_key_free(key);
	cleanup(my_ssh_session);
	printf("Done!\n");
}
