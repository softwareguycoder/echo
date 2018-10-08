#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "stdafx.h"

#define OK              0		// The server completed successfully
#define ERROR           -1		// The server encountered an error

#define FALSE           0
#define TRUE            1

#define MIN_NUM_ARGS	3		// The minimum # of cmd line args to pass
#define MAX_LINE_LENGTH 255     // The maximum length of a line
#define USAGE_STRING	"Usage: client <host name or IP> <port_num>\n" 	// Usage string

#define RECV_BLOCK_SIZE	1
#define RECV_FLAGS	0

// Determines whether the host name or IP address provided by the user can be
// resolved by DNS.  Attempts to actually perform the resolution.  Returns zero
// if resolution has failed, or if the 'he' parameter does not contain a valid
// address, or 'hostnameOrIP' is blank. Returns nonzero if the host name or IP address
// can be resolved, and fills the location pointed to by 'he' with a pointer to a
// hostent structure containing the information necessary to connect to a
// remote server.
int canResolveServerAddress(const char *hostnameOrIP, struct hostent** he);

// This function gets a line of text from a socket,
// and stores it in location referenced by lineptr.
// Returns the total bytes read of the current line. total_read
// points to an integer variable that will receive the
// OVERALL total bytes read.
int get_line(int socket, char **lineptr, int *total_read);

#endif//__CLIENT_H__
