///////////////////////////////////////////////////////////////////////////////
// client.c - Echo client in C
// This program allows the user to connect to an ECHO server residing on a
// IP address and port as supplied on the command line.  The user interface 
// of this program allows the user to type lines of text to be sent to the
// server.
//
// AUTHOR: Brian Hart
// DATE: 21 Sep 2018
//
// Shout out to https://gist.github.com/DnaBoss/a5e1ea07de5ef3525937 for 
// code that provided inspiration
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define OK              0		// The server completed successfully
#define ERROR           -1		// The server encountered an error

#define MIN_NUM_ARGS	3		// The minimum # of cmd line args to pass
#define USAGE_STRING	"Usage: client <server_addr> <port_num>\n" 	// Usage string

void error(const char* msg) 
{
	perror(msg);
	exit(ERROR);
}

int main(int argc, char* argv[])
{
    fprintf(stdout, "client: welcome to the client program\n");

	// Check the arguments.  If there is less than 3 arguments, then 
	// we should print a message to stderr telling the user what to 
	// pass on the command line and then quit
	if (argc < MIN_NUM_ARGS) 
	{
		fprintf(stderr, USAGE_STRING);		
		exit(ERROR);
	}

    const char* server_addr = argv[1];      // address of the remote server
    int port = atoi(argv[2]);               // port number of the server

    fprintf(stdout, 
        "client: configured to connect to server at address '%s'.\n", server_addr);

    fprintf(stdout,
        "client: configured to connect to server listening on port %d.\n", port);

	return OK;
}
