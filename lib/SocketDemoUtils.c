///////////////////////////////////////////////////////////////////////////////
// SocketDemoUtils.c: Definitions for the functions in the SocketDemoUtils.lib
// static library

#include "SocketDemoUtils.h"

/**
 * \brief Checks the integer value supplied to ensure it's a valid user port
 * number and not reserved for a different service.
 * \param port Variable containing the value to be validated.
 * \returns Zero if the 'port' parameter is not in the range [1024, 49151]
 * (inclusive); nonzero otherwise.
 */
int isUserPortValid(int port)
{
    return port >= 1024 && port < 49151;
}

/**
 * \brief Attempts to resolve the hostname or IP address provided with
 * the Domain Name System (DNS) and reports success or failure.
 * \param hostnameOrIP The hostname or IP address of the remote computer
 * that is to be resolved with DNS.
 * \param Address of a storage location that is to be filled with a 
 *  hostent structure upon successful resolution of the hostname or 
 *  IP address provided.  
 * \returns Zero if resolution has failed; nonzero otherwise.
 * \remarks If this function returns nonzero, then the value of '*he'
 *  will be the address of a storage location containing a hostent
 *  structure containing information for the remote host.
 */
int isValidHostnameOrIp(const char *hostnameOrIP, struct hostent **he)
{
    if (hostnameOrIP == NULL
        || hostnameOrIP[0] == '\0'
        || strlen(hostnameOrIP) == 0)
    {
        return FALSE;
    }

    if (he == NULL) 
    {
        // return FALSE if no storage location for the 'he' pointer passed
        return FALSE;
    }

    fprintf(stdout, 
        "Resolving host name or IP address '%s'...\n", hostnameOrIP);

    if ( (*he = gethostbyname(hostnameOrIP) ) == NULL ) {
        fprintf(stderr, "Hostname or IP address resolution failed.\n");
        *he = NULL;
        return FALSE;
    }

    fprintf(stdout,
        "Hostname or IP address resolution succeeded.\n");

    return TRUE;
}

/**
 * \brief Frees the memory at the address specified.
 * \param ppBuffer Address of a pointer which points to memory
 * allocated with the '*alloc' functions (malloc, calloc, realloc).
 * \remarks Remember to cast the address of the pointer being passed 
 * to this function to void** 
 */
void free_buffer(void **ppBuffer)
{   
    if (ppBuffer == NULL || *ppBuffer == NULL)
        return;     // Nothing to do since there is no address referenced
    
    free(*ppBuffer);
    *ppBuffer = NULL;
}

/**
 *  \brief Reports the error message specified as well as the error from
 *  the system.  Closes the socket file descriptor provided in order to 
 *   free operating system resources.  Exits the program with the ERROR exit
 *   code.
 *  \param sockFd Socket file descriptor to be closed after the error
 *  has been reported.
 *  \param msg Additional error text to be echoed to the console.
 **/
void error_and_close(int sockFd, const char *msg)
{
    if (msg == NULL
        || strlen(msg) == 0
        || msg[0] == '\0')
    {
        perror(NULL);
        exit(ERROR);
        return;         // This return statement might not fire, but just in case.
    }

    fprintf(stderr, "%s", msg);
	perror(NULL);

    if (sockFd > 0)
    {
        close(sockFd);
        fprintf(stderr, "Exiting with error code %d.\n", ERROR);
    }
    
	exit(ERROR);
}

/**
 *  \brief Reports the error message specified as well as the error from
 *  the system. Exits the program with the ERROR exit code.
 *  \param msg Additional error text to be echoed to the console.
 **/
void error(const char* msg)
{
    if (msg == NULL
        || strlen(msg) == 0
        || msg[0] == '\0')
    {
        return;
    }

    fprintf(stderr, "%s", msg);
	perror(NULL);
	exit(ERROR);
}

/**
 *  \brief Creates a new socket endpoint for communicating with a remote
 *  host over TCP/IP.
 *  \returns Socket file descriptor which provides a handle to the newly-
 *  created socket endpoint. 
 *  \remarks If an error occurs, prints the error to the console and forces
 *  the program to exit with the ERROR exit code.
 */
int SocketDemoUtils_createTcpSocket()
{
    fprintf(stdout,
        "SocketDemoUtils_createTcpSocket: Allocating new TCP endpoint...\n");
    
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd <= 0)
    {
        error("SocketDemoUtils_createTcpSocket: Could not create endpoint.\n");
    }    

    fprintf(stdout, 
        "SocketDemoUtils_createTcpSocket: Endpoint created successfully.\n");

	// Set socket options to allow the listening socket to be reused.
	if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, 
		sizeof(int)) < 0)
    {
	    error("setsockopt(SO_REUSEADDR) failed.");
    }

	fprintf(stdout, "SocketDemoUtils_createTcpSocket: Endpoint configured to be reusable\n");

    return sockFd;
}

/**
 *  \brief Populates the port and address information for a server
 *  so the server knows the hostname/IP address and port of the computer 
 *  it is listening on.
 *  \param port String containing the port number to listen on.  Must be numeric.
 *  \param hostnameOrIp String containing the hostname or IP address of the server
 *  computer.  Can be NULL, in which case, htons(INADDR_ANY) will be set.  Use NULL
 *  for a sevrer, and a specific value for a client.
 *  \param addr Address of storage that will receive a filled-in sockaddr_in structure
 *  that defines the server endpoint.
 *  \remarks If invalid input is supplied or an error occurs, reports thse problem
 *  to the console and forces the program to die with the ERROR exit code.
 */
void SocketDemoUtils_populateServerAddrInfo(const char *port, struct sockaddr_in *addr)
{
    if (port == NULL
        || strlen(port) == 0
        || port[0] == '\0')
    {
        fprintf(stderr,
            "SocketDemoUtils_populateServerAddrInfo: String containing the port number is blank.\n");
        exit(ERROR);
    }

    if (addr == NULL)
    {
        fprintf(stderr,
            "SocketDemoUtils_populateServerAddrInfo: Missing pointer to a sockaddr_in structure.\n");
        exit(ERROR);        
    }

    // Get the port number from its string representation and then validate that it is in
    // the proper range
    int portnum = atoi(port);
    if (!isUserPortValid(portnum))
	{
		fprintf(stderr, 
			"SocketDemoUtils_populateServerAddrInfo: Port number must be in the range 1024-49151 inclusive.\n");
		exit(ERROR);
	}

    // Populate the fields of the sockaddr_in structure passed to us with the proper values.

    fprintf(stdout,
        "SocketDemoUtils_populateServerAddrInfo: Configuring server address and port...\n");

    addr->sin_family = AF_INET;
    addr->sin_port = htons(portnum);   
    addr->sin_addr.s_addr = htons(INADDR_ANY);

    fprintf(stdout,
        "SocketDemoUtils_populateServerAddrInfo: Server configured to listen on port %d.\n", portnum);
}

/**
 *  \brief Binds a server socket to the address and port specified by the 'addr'
 *   parameter.
 *  \param sockFd Socket file descriptor that references the socket to be bound.
 *  \param addr Pointer to a sockaddr_in structure that specifies the host and port
 *  to which the socket endpoint should be bound.
*/
int SocketDemoUtils_bind(int sockFd, struct sockaddr_in *addr)
{
    if (sockFd <= 0)
    {
        errno = EBADF;
        return ERROR;   // Invalid socket file descriptor
    }

    if (addr == NULL)
    {
        errno = EINVAL; // addr param required
        return ERROR;
    }

    return bind(sockFd, (struct sockaddr*)addr, sizeof(*addr));
}

/**
 * \brief Sets up a TCP or UDP server socket to listen on a port and IP address
 * to which it has been bound previously with the SocketDemoUtils_bind function.
 * \params sockFd Socket file descriptor.
 * \returns ERROR if the socket file descriptor passed in sockFd does not represent
 * a valid, open socket and sets errno to EBADF.  Otherwise, returns the result of
 * calling listen on the socket file descriptor passed with a backlog size of
 * BACKLOG_SIZE (128 by default).  Zero is returned if the operation was successful.
 */
int SocketDemoUtils_listen(int sockFd)
{
    if (sockFd <= 0)
    {
        errno = EBADF;
        return ERROR;   // Invalid socket file descriptor
    }

    return listen(sockFd, BACKLOG_SIZE);
}

/**
 * \brief Accepts an incoming connection on a socket and returns information about
 * the remote host.
 * \param sockFd Socket file descriptor on which to accept new incoming connections.
 * \param addr Reference to a sockaddr_in structure that receives information about
 * the IP address of the remote endpoint.
 * \returns Socket file descriptor representing the local endpoint of the new
 * incoming connection; or a negative number indicating that errno should be read
 * for the error description.
 * \remarks Returns ERROR if any of the following are true: (a) sets errno to EBADF
 * if sockFd is an invalid value (nonpositive) or (b) sets errno to EINVAL if addr
 * is NULL.  If the incoming connection is accepted successfully, this function also
 * calls fcntl on the new file descriptor to set the incoming socket connection to be
 * non-blocking.  This allows data to be read from recv buffer as it is still coming
 * in.  This function blocks the calling thread until an incoming connection has been
 * established.
 */
int SocketDemoUtils_accept(int sockFd, struct sockaddr_in *addr)
{
	socklen_t client_address_len;
    int result = ERROR;

    if (sockFd <= 0)
    {
        errno = EBADF;          // Bad file descriptor
        return result;
    }
    
    if (addr == NULL)
    {
        errno = EINVAL;         // Invalid value for addr parameter
        return result;
    }
    // We now call the accept function.  This function holds us up
    // until a new client connection comes in, whereupon it returns
    // a file descriptor that represents the socket on our side that
    // is connected to the client.
    if ((result = accept(sockFd, (struct sockaddr*)addr, &client_address_len)) < 0)
    {
        return result;
    }

    fprintf(stdout, 
        "SocketDemoUtils_accept: Configuring client endpoint to be non-blocking...\n");

    // Attempt to configure the client_socket to be non-blocking, this way
    // we can hopefully receive data as it is being sent until only getting
    // the data when the client closes the connection.
    if (fcntl(sockFd, F_SETFL, fcntl(sockFd, F_GETFL, 0) | O_NONBLOCK) < 0)
    {
        error_and_close(sockFd,
            "SocketDemoUtils_accept: Could not set the client endpoint to be non-blocking.\n");
    }

    fprintf(stdout, 
        "SocketDemoUtils_accept: Client endpoint configured to be non-blocking.\n");

    fprintf(stdout, 
        "SocketDemoUtils_accept: New client connected.\n");

    return result;
}

/** \brief Reads a line of data, terminated by the '\n' character, from a socket.
 *  \param sockFd Socket file descriptor from which to receive data.
 *  \param buf Reference to an address at which to allocate storage for the received data.
 *  \returns Total bytes read for the current line or a negative number otherwise.
 *  \remarks This function will forcibly terminate the calling program with an exit
 *  code of ERROR if the operation fails.  It is the responsibility of the caller to
 *  free the memory referenced by *buf.  The caller must always pass NULL for buf.  If
 *  valid storage is passed, this function will free the storage referenced by *buf and
 *  allocate brand-new storage for the incoming line.
 */
int SocketDemoUtils_recv(int sockFd, char **buf)
{
	int bytes_read = 0;
	int total_read = 0;  

	if (buf == NULL
		|| sockFd <= 0)
	{
		fprintf(stderr, "SocketDemoUtils_getline: Invalid input.\n");	
		exit(ERROR);
	}

    // Allocate up some brand-new storage of size RECV_BLOCK_SIZE 
    // plus an extra slot to hold the null-terminator.  Free any
    // storage already referenced by *buf.  If *buf happens to be
    // NULL already, a malloc is done.  Once the new memory has been
    // allocated, we then explicitly zero it out.
	total_read = 0;
	*buf = (char*)realloc(*buf, (RECV_BLOCK_SIZE + 1)*sizeof(char));
    explicit_bzero((void*)*buf, RECV_BLOCK_SIZE + 1);
	
    //char prevch = '\0';
	while(1) 
	{
		char ch;		// receive one char at a time
		bytes_read = recv(sockFd, &ch, RECV_BLOCK_SIZE, RECV_FLAGS);
		if (bytes_read < 0) 
		{
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;

            error("SocketDemoUtils_getline: Network error stopped us from receiving more text.");

            //prevch = ch;
			break;
		}

		// If we are here, then stuff came over the wire.
		// Stick the character received, from ch, into the next 
		// storage element referenced by *buf + total_read
		// and then allocate some more memory to hold the 
		// next char and then the null terminator
		*(*buf + total_read) = ch;

		// Tally the total bytes read overall
		total_read += bytes_read;

		// If the newline ('\n') character was the char received,
		// then we're done; it's time to apply the null terminator.
		if (ch == '\n')
		{
			break;
		}
		
		// re-allocate more memory and make sure to leave room 
		// for the null-terminator.
		*buf = (char*)realloc(*buf, (total_read + RECV_BLOCK_SIZE + 1)*sizeof(char));
	}

	// We are done receiving, cap the string off with a null terminator
	// after resizing the buffer to match the total bytes read + 1.  if
	// a connection error happened prior to reading even one byte, then
	// total_read will be zero and the call below will be equivalent to
	// free.  strlen(*buf) will then return zero, and this will be
	// how we can tell not to call free() again on *buf
	*buf = (char*)realloc(*buf, (total_read + 1)*sizeof(char));

	if (total_read > 0)
	{
		*(*buf + total_read) = '\0';	// cap the buffer off with the null-terminator
	}

	// Now the storage at address *buf should contain the entire 
	// line just received, plus the newline and the null-terminator, plus
	// any previously-received data
	
	return total_read;
}

/**
 *	\brief Sends data to the endpoint on the other end of the connection referenced
 *	by the connected socket.
 *	\param sockFd Socket file descriptor.  Must be a descriptor for a valid socket that
 *	is currently connected to a remote host.
 *	\param buf Address of a character array containing the bytes to be sent.
 *	\returns ERROR if the operation failed; number of bytes sent otherwise.
 *	If the ERROR value is returned, errno should be examined to determine the
 *  cause of the error.
 */
int SocketDemoUtils_send(int sockFd, const char *buf)
{
    if (sockFd <= 0)
    {
        errno = EBADF;
        return ERROR;
    }

    if (buf == NULL
        || strlen(buf) <= 0)
    {
        // Nothing to send
        return 0;
    }

    return (int)send(sockFd, buf, strlen(buf), 0);
}

/**
 * \brief Connects a socket to a remote host whose hostname or IP address and
 * port number is specified.
 * \param sockFd Socket file descriptor representing a socket that is not yet
 * connected to a remote endpoint.
 * \param hostnameOrIp String indicating the human-readable (in DNS) hostname
 * or the IP address of the remote host.
 * \param port Port number that the service on the remote host is listening on.
 * \returns Zero if successful; ERROR if an error occurred.  The errno
 * value should be examined if this happens.  In other cases, this function
 * forcibly terminates the calling program with the ERROR exit code.
 */
int SocketDemoUtils_connect(int sockFd, const char *hostnameOrIp, int port)
{  
    int result = ERROR;
    struct hostent      *he;                    // Host entry
    struct sockaddr_in  server_address;         // Structure for the server address and port
    
    if (sockFd <= 0)
    {
        fprintf(stderr,
            "SocketDemoUtils_connect: Attempted to connect to remote host with no endpoint.\n");
        exit(ERROR);
    }

    if (!isUserPortValid(port))
    {
		fprintf(stderr, 
			"SocketDemoUtils_populateServerAddrInfo: Port number must be in the range 1024-49151 inclusive.\n");
		exit(ERROR);
    }
    
    // First, try to resolve the host name or IP address passed to us, to ensure that
    // the host can even be found on the network in the first place.  Calling the function
    // below also has the added bonus of filling in a hostent structure for us if it succeeds.
    if (!isValidHostnameOrIp(hostnameOrIp, &he))
    {
        error_and_close(sockFd,
            "SocketDemoUtils_connect: Unable to validate/resolve hostname/IP address provided.");
    }

    fprintf(stdout,
        "SocketDemoUtils_connect: Attempting to contact the server at '%s' on nPort %d...\n", 
        hostnameOrIp, port);

    /* copy the network address to sockaddr_in structure */
    memcpy(&server_address.sin_addr, he->h_addr_list[0], he->h_length);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);  
    
    if ((result = connect(sockFd, (struct sockaddr*)&server_address, sizeof(server_address))) < 0)
    {
        char buf[100];
        sprintf(buf, "SocketDemoUtils_connect: The attempt to contact the server at '%s' on port %d failed.\n",
        	hostnameOrIp, port);
        error_and_close(sockFd, buf);
    }

    fprintf(stdout, 
        "SocketDemoUtils_connect: Connected to the server at '%s' on nPort %d.\n", hostnameOrIp, port);

    return result;
}

