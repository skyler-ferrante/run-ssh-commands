#ifndef SSH_RUN
#define SSH_RUN

#define LIBSSH_STATIC 1
#include<libssh/libssh.h>

#define HASH_TYPE SSH_PUBLICKEY_HASH_SHA256

/**
 * Max size of result output
 * Makes sure we don't run commands forever
**/
#define BUFFER_SIZE 1024

/**
 * Check if session is valid.
 * Set HOST, LOG_VERBOSITY, and PORT
 * Exit on fail
**/
void session_init(ssh_session session, char* host, int port, int verbosity);

/**
 * Setup channel
 * Exit on fail
**/
void connect_session(ssh_session session, char* host);

/**
 * Get server public key
**/
void get_public_key(ssh_session session, ssh_key* key);

/**
 * Get hash from public key
**/
void get_key_hash(ssh_key* key, unsigned char** hash, size_t* len);

/**
 * Authenticate through username/password
**/
void authenticate_username_password(ssh_session session, char* username, char* password);

/**
 * Create channel
**/
void create_channel(ssh_session session, ssh_channel* channel);

/**
 * Execute command with given channel
 * @return Number of bytes
**/
int execute_ssh(ssh_session session, ssh_channel channel, const char* command, char buffer[BUFFER_SIZE]);

/**
 * Cleanup ssh_session and free key
**/
void cleanup(ssh_session session);

#endif
