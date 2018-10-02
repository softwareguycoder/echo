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

#include "../lib/SocketDemoUtils.h"

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
int canResolveServerAddress(const char *hostnameOrIP, struct hostent** he)
{
    if (hostnameOrIP == NULL
        || hostnameOrIP[0] == '\0'
        || strlen(hostnameOrIP) == 0)
    {
        return FALSE;
    }

    if (he == NULL) 
    {
        // return FALSE if no storage location for the he pointer passed
        return FALSE;
    }

    fprintf(stdout, 
        "client: Resolving host name or IP address '%s'...\n", hostnameOrIP);

    if ( (*he = gethostbyname(hostnameOrIP) ) == NULL ) {
        fprintf(stderr, "client: Hostname or IP address resolution failed.\n");
        *he = NULL;
        return FALSE;
    }

    fprintf(stdout,
        "client: Hostname or IP address resolution succeeded.\n");

    return TRUE;
}

// This function gets a line of text from a socket,
// and stores it in location referenced by lineptr.
// Returns the total bytes read of the current line. total_read
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
	
    //char prevch = '\0';
	while(1) 
	{
		char ch;		// receive one char at a time
		bytes_read = recv(socket, &ch, RECV_BLOCK_SIZE, RECV_FLAGS);
		if (bytes_read < 0)// || prevch == '.') 
		{
			// Connection terminated, so stop the loop
			// but continue running the program (because we may need
			// to free up storage)
			if (bytes_read < 0)
				fprintf(stderr, 
					"server: Network error stopped us from receiving more text.");

            //prevch = ch;
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
            //prevch = ch;
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

int main(int argc, char* argv[])
{
    int client_socket = 0;                      // Client socket for connecting to the server.
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

    const char* hostnameOrIp = argv[1];         // address or host name of the remote server
    int port = atoi(argv[2]);                      // port number that server is listening on

    fprintf(stdout, 
        "client: Configured to connect to server at address '%s'.\n", hostnameOrIp);
    fprintf(stdout,
        "client: Configured to connect to server listening on port %d.\n", port);
    fprintf(stdout,
        "client: Attempting to allocate new connection endpoint...\n");
    
    client_socket = SocketDemoUtils_createTcpSocket();
    if (client_socket <= 0)
    {
        error("client: Could not create endpoint for connecting to the server.\n");
    }    

    fprintf(stdout, "client: Created connection endpoint successfully.\n");

    // Attempt to connect to the server.  The function below is guaranteed to close the socket
    // and forcibly terminate this program in the event of a network error, so we do not need   
    // to check the result.
    SocketDemoUtils_connect(client_socket, hostnameOrIp, port);

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
       words 'exit' or 'quit' at the '>' prompt, then exit this program.  Otherwise, 
       send whatever string(s) the user types at the '>' prompt to the server, 
       and then display the server's response, if any.
    */

    int total_read = 0;             // total reply bytes read from the server
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
        
        // Keep a running total of the total bytes entered
        total_entered += strlen(cur_line);

        // send the text just now entered by the user to the server
        if( send(client_socket , cur_line, strlen(cur_line) , 0) < 0)
        {
            error_and_close(client_socket, "client: Failed to send the data.\n");
            return ERROR;
        } 

        // If a period '.' has been sent to the server, this is the way the user
        // says they are done using the server, so stop here before trying to receive
        // a reply from the server.
        if (strcasecmp(cur_line, ".\n") == 0) break;   

        // Now, assume the server has sent a reply, and call the recv() function
        // to attempt to pull the text sent back by the server off of the data
        // stream.  Assume that the server just sends back one line at a time.
        char *reply_buffer = NULL;

        if (0 > get_line(client_socket, &reply_buffer, &total_read))
        {
            free_buffer((void**)&reply_buffer);
            error_and_close(client_socket, "client: Failed to receive the line of text back from the server.\n");
            return ERROR;            
        }
        else
        {
            // Print the line received from the server to the console with a
            // 'S: ' prefix in front of it.  We assume that the reply_buffer
            // contains the newline character.  Free the memory allocated for
            // the server reply.
            fprintf(stdout, "S: %s", reply_buffer);
        
            free_buffer((void**)&reply_buffer);
        }

        fprintf(stdout, "> ");
    }

    close(client_socket);
    
    fprintf(stdout, "S: <disconnected>\n");

    fprintf(stdout, "client: Exited normally with error code %d.\n", OK);
    
    return OK;
}
