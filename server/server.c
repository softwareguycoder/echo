///////////////////////////////////////////////////////////////////////////////
// server.c - Echo server in C
// The server receives text a line at a time and echoes the text back to its
// client only AFTER an entire line has been received.
//
// AUTHOR: Brian Hart
// DATE: 20 Sep 2018
//
// Shout-out to <https://gist.githubusercontent.com/suyash/2488ff6996c98a8ee3a8
// 4fe3198a6f85/raw/ba06e891060b277867a6f9c7c2afa20da431ec91/server.c> and
// <http://www.linuxhowtos.org/C_C++/socket.htm> for 
// inspiration
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

/*
	TCP uses 2 types of sockets: the connection socket and the listen socket.
	The goal is to separate the connection phase from the data exchange phase.
*/

#define OK              0		// The server completed successfully
#define ERROR           -1		// The server encountered an error

#define MIN_NUM_ARGS	2		// The minimum # of cmd line args to pass
#define USAGE_STRING	"Usage: server <port_num>\n" 	// Usage string
#define BACKLOG_SIZE	128		// Max number of client connections

#define RECV_BLOCK_SIZE	1
#define RECV_FLAGS	0

void error(const char* msg) 
{
	perror(msg);
	exit(ERROR);
}

// This function gets a line of text from a socket,
// and stores it in location referenced by lineptr.
// Returns the total bytes read of THIS line. total_read
// points to an integer variable that will receive the 
// OVERALL total bytes read.
int get_line(int socket, char **lineptr, int *total_read) 
{
	int bytes_read = 0;
	int bytes_of_this_line = 0;
	
	if (lineptr == NULL
		|| socket <= 0
		|| total_read == NULL)
	{
		fprintf(stderr, "server: invalid input to getline.\n");	
		exit(ERROR);
	}

	if (*lineptr == NULL) 
	{
		// If we get a NULL for *lineptr, just allocate up some
		// brand-new storage of size RECV_BLOCK_SIZE plus an extra 
		// slot to hold the null-terminator
		*total_read = 0;
		*lineptr = (char*)calloc(RECV_BLOCK_SIZE + 1, sizeof(char));
	}
	
	while(1) 
	{
		char ch;		// receive one char at a time
		bytes_read = recv(socket, &ch, RECV_BLOCK_SIZE, RECV_FLAGS);
		if (bytes_read <= 0) 
		{
			// Connection terminated, so stop the loop
			// but continue running the program (because we may need
			// to free up storage)
			if (bytes_read < 0)
				fprintf(stderr, 
					"server: Network error stopped us from receiving more text.");
			break;
		}

		// If we are here, then stuff came over the wire
		// stick the character received, from ch, into the next 
		// storage element referenced by *lineptr + total_read
		// and then allocate some more memory to hold the 
		// next char and then the null terminator
		*(*lineptr + *total_read) = ch;

		// Tally the total bytes read overall
		*total_read += bytes_read;
		bytes_of_this_line += bytes_read;

		// If the newline ('\n') character was the char received,
		// then we're done, time to apply the null terminator
		if (ch == '\n')
		{
			break;
		}
		
		// re-allocate more memory and make sure to leave room 
		// for the null-terminator
		*lineptr = (char*)realloc(*lineptr, (*total_read + RECV_BLOCK_SIZE + 1)*sizeof(char));
	}

	// We are done receiving, cap the string off with a null terminator
	// after resizing the buffer to match the total bytes read + 1.  if
	// a connection error happened prior to reading even one byte, then
	// total_read will be zero and the call below will be equivalent to
	// free.  strlen(*lineptr) will then return zero, and this will be
	// how we can tell not to call free() again on *lineptr
	*lineptr = (char*)realloc(*lineptr, (*total_read + 1)*sizeof(char));

	if (total_read > 0)
	{
		*(*lineptr + *total_read) = '\0';	// cap the buffer off with the null-terminator
	}

	// Now the storage at address *lineptr should contain the entire 
	// line just received, plus the newline and the null-terminator, plus
	// any previously-received data
	
	return bytes_of_this_line;
}

int main(int argc, char *argv[]) 
{
	// Check the arguments.  If there is less than 2 arguments, then 
	// we should print a message to stderr telling the user what to 
	// pass on the command line and then quit
	if (argc < MIN_NUM_ARGS) 
	{
		fprintf(stderr, USAGE_STRING);		
		exit(ERROR);
	}

	// Assume that the first argument (argv[1]) is the port number 
	// that the user wants us to listen on 
	int port = atoi(argv[1]);	

	// Validate the port (should be in range 1024-49151 per IANA)
	if (port < 1024 || port > 49151)
	{
		fprintf(stderr, 
			"server: Port number must be in the range 1024-49151 inclusive.\n");
		exit(ERROR);
	}
	fprintf(stdout, "server: configured to listen on port %d\n", port);

	// Get a handle to a socket that will listen on the desired port at
	// our IP address
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) 
	    error("server: Failed to open listen socket.");

	fprintf(stdout, "server: new TCP socket created.\n");

	// Set socket options to allow the listening socket to be reused.
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, 
		sizeof(int)) < 0)
	    error("server: setsockopt(SO_REUSEADDR) failed.");

	fprintf(stdout, "server: socket configured to be reusable\n");

	// socket address used for the server
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	fprintf(stdout, "server: socket address configured\n");

	// Configure the server socket to listen on the desired port
	server_address.sin_port = htons(port);

	// Tell the operating system that we do not care which IP address this
	// server listens on (the server will be reachable at localhost, 127.0.0.1,
	// or the computer's public IP address if the firewall has permitted it)
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
		
	// Bind the server socket to associate it with this host as a server
	if (bind(server_socket, (struct sockaddr*)&server_address,
		sizeof(server_address)) < 0) 
	{
		error("server: Could not bind socket.\n");
	}

	fprintf(stdout, "server: socket bound to localhost on port %d.\n", port);

	if (listen(server_socket, BACKLOG_SIZE) < 0) 
	{
		error("server: Could not open socket for listening.\n");
	}

	fprintf(stdout, "server: now listening on port %d\n", port);

	// socket address used to store client address
	struct sockaddr_in client_address;
	int client_address_len = 0;

	int client_socket = -1;
	// run indefinitely
	while(1) 
	{

		fprintf(stdout, "server: waiting for client connection...\n");

		// We now call the accept function.  This function holds us up
		// until a new client connection comes in, whereupon it returns
		// a file descriptor that represents the socket on our side that
		// is connected to the client.
		if ((client_socket = 
			accept(server_socket, 
			(struct sockaddr*)&client_address,
			&client_address_len)) < 0) 
		{
			error("server: Could not open a socket to accept data.\n");
		}

		fprintf(stdout, "server: new client connected.  awaiting data...\n");

		// receive all the lines of text that the client wants to send.
		// put them all in a buffer.
		char* buf = NULL;
		int total = 0;		// total size of the message (all lines)

		// just call getline (above) over and over again until 
		// all the data has been read that the client wants to send.
		// Clients should figure out a way to determine when to stop	
		// sending input.
		while (0 < get_line(client_socket, &buf, &total)) ;

		fprintf(stdout, "server: %d bytes read.\n", total);

		if (buf != NULL
			&& strlen(buf) > 0)	// we got stuff from the client
		{
			fprintf(stdout, "%s\n", buf);
			
			// echo received content back
			send(client_socket, buf, strlen(buf), 0);
		}
		
		// disconnect from the client
		close(client_socket);

		// Let the 'outer' while loop start over again, waiting to accept new
		// client connections.  User must close the server 'by hand'
		// but we want the server to remain 'up' as long as possible,
		// just in case that more clients want to connect
	}

	close(server_socket);
	fprintf(stdout, "server: execution finished with no errors.\n");	

	return OK;
}
