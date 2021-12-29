#include "ssh_run.h"
#include <stdio.h>
#include <stdlib.h>

// Function comments are in .h file
// (Sorry!)

void session_init(ssh_session session, char* host, int port, int verbosity){
	if (session == NULL)
		exit(-1);
	// Set session options
	ssh_options_set(session, SSH_OPTIONS_HOST, host);
	ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
	ssh_options_set(session, SSH_OPTIONS_PORT, &port);
}

void connect_session(ssh_session session, char* host){
	// Create channel
	int rc = ssh_connect(session);
	if( rc != SSH_OK) {
		fprintf(stderr, "Error connecting to %s: %s\n",
				host,
				ssh_get_error(session)
		);
		exit(-1);
	}
}

void get_public_key(ssh_session session, ssh_key* key){
	// Get pubkey
	int rc = ssh_get_server_publickey( session, key); 
	if( rc != SSH_OK ){
		fprintf(stderr, "Error getting publickey\n");
		exit(-1);
	}
}

void get_key_hash(ssh_key* key, unsigned char** hash, size_t* len){
	int rc = ssh_get_publickey_hash( *key, HASH_TYPE, hash, len);

	if( rc != SSH_OK ){
		fprintf(stderr, "Error getting publickey hash\n");
		exit(-1);
	}
}

void authenticate_username_password(ssh_session session, char* username, char* password){
	int rc = ssh_userauth_password( session, username, password);

	if( rc != SSH_AUTH_SUCCESS){
		fprintf(stderr, "Error authenticating: %s\n",
				ssh_get_error(session)
		);
		cleanup(session);
		exit(-1);
	}
}

void create_channel(ssh_session session, ssh_channel* channel){
	*channel = ssh_channel_new(session);
	int rc = ssh_channel_open_session(*channel);
	if( rc != SSH_OK){
		fprintf(stderr, "Error creating channel\n");
		ssh_channel_free(*channel);	
		exit(-1);
	}
}

int execute_ssh(ssh_session session, ssh_channel channel, const char* command, char buffer[BUFFER_SIZE]){
	int rc = ssh_channel_request_exec(channel, command);
	if( rc != SSH_OK ){
		cleanup(session);
		exit(-1);
	}
	int nbytes = ssh_channel_read(channel, buffer, BUFFER_SIZE, 0);

	// Put sentinel at the end of result
	if(nbytes >= 0){
		buffer[nbytes] = '\0';
	}
	
	// Handle no result
	else{
		buffer[0] = '\0';
	}

	return nbytes;
}

void cleanup(ssh_session session){
	ssh_disconnect(session);
	ssh_free(session);
}
