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
#include <strings.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h> //inet_addr
#include <netdb.h>

#define OK              0		// The server completed successfully
#define ERROR           -1		// The server encountered an error

#define MIN_NUM_ARGS	3		// The minimum # of cmd line args to pass
#define MAX_LINE_LENGTH 255     // The maximum length of a line
#define USAGE_STRING	"Usage: client <hostname> <port_num>\n" 	// Usage string

void error_and_close(int socket, const char* msg) 
{     
    fprintf(stderr, "%s", msg);
	perror("client");

    if (socket > 0)
    {
        close(socket);
        fprintf(stdout, "S: <disconnected>\n");
        fprintf(stderr, "client: Exiting with error code %d.\n", ERROR);
    }
    
	exit(ERROR);
}

void error(const char* msg) 
{
    fprintf(stderr, "%s", msg);
	perror("client");
	exit(ERROR);
}

int main(int argc, char* argv[])
{
    int client_socket = 0;                      // Client socket for connecting to the server.
    struct hostent      *he;                    // Host entry
    char                **pp = NULL;            // Pointer to a pointer of char
    char                answer[INET_ADDRSTRLEN];// Answer when resolving host
    struct sockaddr_in  server_address;         // Structure for the server address and port
    char cur_line[MAX_LINE_LENGTH + 1];         // Buffer for the current line inputted by the user
                                                // for sending to the server
    
    fprintf(stdout, "client: Welcome to the client program\n");

    fprintf(stdout, "client: Checking arguments...\n");

	// Check the arguments.  If there is less than 3 arguments, then 
	// we should print a message to stderr telling the user what to 
	// pass on the command line and then quit
	if (argc < MIN_NUM_ARGS) 
	{
		fprintf(stderr, USAGE_STRING);		
		exit(ERROR);
	}

    const char* hostname = argv[1];         // address or hostname of the remote server
    int port = atoi(argv[2]);               // port number that server is listening on

    fprintf(stdout, 
        "client: Configured to connect to server at address '%s'.\n", hostname);
    fprintf(stdout,
        "client: Configured to connect to server listening on port %d.\n", port);

    fprintf(stdout, 
        "client: Resolving hostname '%s'...\n", hostname);

   if ( (he = gethostbyname(hostname) ) == NULL ) {
        error("client: Hostname resolution failed.\n");
    }

    fprintf(stdout,
        "client: Attempting to allocate new connection endpoint...\n");
    
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket <= 0)
    {
        error("client: Could not create endpoint for connecting to the server.\n");
    }    

    fprintf(stdout, "client: Created connection endpoint successfully.\n");

    fprintf(stdout,
        "client: Attempting to contact the server at '%s' on port %d...\n", hostname, port);

    /* copy the network address to sockaddr_in structure */
    memcpy(&server_address.sin_addr, he->h_addr_list[0], he->h_length);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);  
    
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
    {
        char buf[75];
        sprintf(buf, "client: The attempt to contact the server at '%s' on port %d failed.\n", hostname, port);
        error_and_close(client_socket, buf);
    }

    fprintf(stdout, "client: Connected to the server at '%s' on port %d.\n", hostname, port);

    /* Print some usage directions */
    fprintf(stdout,
        "\nType the message to send to the server at the '>' prompt, and then press ENTER.\n");
    fprintf(stdout,
        "The server's reply, if any, will be shown with a 'S:' prefix.\n");
    fprintf(stdout,
        "When you have nothing more to say, type a dot ('.') on a line by itself.\n");
    fprintf(stdout,
        "To exit, type 'exit' or 'quit' and then press ENTER.\n\n");

    /* Show a '>' prompt to the user.  If the user just presses ENTER at a 
       prompt, then just give the user a new prompt.  If the user enters the
       words 'exit' or 'quit', then exit this program.  Otherwise, send whatever
       string(s) the user types at the '>' prompt to the server, and then display
       the server's response, if any.
    */

    int total_entered = 0;          // total bytes typed by the user for the current message

    fprintf(stdout, "> ");

    while(NULL != fgets(cur_line, MAX_LINE_LENGTH, stdin))
    {
        if (strcasecmp(cur_line, "exit\n") == 0) break;
        if (strcasecmp(cur_line, "quit\n") == 0) break;

        if (strcasecmp(cur_line, "\n") == 0) 
        {
            fprintf(stdout, "> ");
            continue;
        }
        
        // send the text just now entered by the user to the server
        if( send(client_socket , cur_line, strlen(cur_line) , 0) < 0)
        {
            error_and_close(client_socket, "client: Failed to send the data.\n");
            return 1;
        }

        fprintf(stdout, "> ");
    }

    // TODO: Add code here to connect to the server

    // TODO: Add code here to provide a user interface for sending
    // data to the server.  Be sure to prompt the user to type a period ('.')
    // on a line by itself to designate that the user is done sending stuff.

    close(client_socket);
    
    fprintf(stdout, "S: <disconnected>\n");

    fprintf(stdout, "client: Exited normally with error code %d.\n", OK);
    
    return OK;
}
