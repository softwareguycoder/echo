///////////////////////////////////////////////////////////////////////////////
// SocketDemoUtils.c: Definitions for the functions in the SocketDemoUtils.lib
// static library

#include "SocketDemoUtils.h"

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
 *  structure contianing information for the remote host.
 */
int canResolveServerAddress(const char *hostnameOrIP, struct hostent **he)
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
    fprintf(stderr, "%s", msg);
	perror(NULL);

    if (socket > 0)
    {
        close(socket);
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
        "Attempting to allocate new connection endpoint...\n");
    
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd <= 0)
    {
        error("Could not create endpoint for connecting to the remote host.\n");
    }    

    fprintf(stdout, "Created connection endpoint successfully.\n");

    return sockFd;
}

int SocketDemoUtils_populateAddrInfo(char *port, char *hostnameOrIp, struct sockaddr_in *addr)
{
    
}
int SocketDemoUtils_bind(int sockFd, struct sockaddr_in *addr);
int SocketDemoUtils_listen(int sockFd);
int SocketDemoUtils_accept(int sockFd, struct sockaddr_in *addr);
int SocketDemoUtils_recv(int sockFd, char **buf);
int SocketDemoUtils_send(int sockFd, const char *buf, int numBytes);
int SocketDemoUtils_getline(int sockFd, char **lineptr, int *total_read);
int SocketDemoUtils_connect(int sockFd, const char *hostnameOrIp, int port);

