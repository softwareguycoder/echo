#ifndef __SOCKETDEMOUTILS_H__
#define __SOCKETDEMOUTILS_H__

#define _GNU_SOURCE

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define OK              0		// The server completed successfully
#define ERROR           -1		// The server encountered an error

#define FALSE           0
#define TRUE            1

#define RECV_BLOCK_SIZE	1
#define RECV_FLAGS	0
#define BACKLOG_SIZE	128		// Max number of client connections

/**
 * \brief Checks the integer value supplied to ensure it's a valid user port
 * number and not reserved for a different service.
 * \param port Variable containing the value to be validated.
 * \returns Zero if the 'port' parameter is not in the range [1024, 49151]
 * (inclusive); nonzero otherwise.
 */
int isUserPortValid(int port);

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
int isValidHostnameOrIp(const char *hostnameOrIP, struct hostent **he);

/**
 * \brief Frees the memory at the address specified.
 * \param ppBuffer Address of a pointer which points to memory
 * allocated with the '*alloc' functions (malloc, calloc, realloc).
 * \remarks Remember to cast the address of the pointer being passed
 * to this function to void**
 */
void free_buffer(void **ppBuffer);

/**
 *  \brief Reports the error message specified as well as the error from
 *  the system.  Closes the socket file descriptor provided in order to
 *   free operating system resources.  Exits the program with the ERROR exit
 *   code.
 *  \param sockFd Socket file descriptor to be closed after the error
 *  has been reported.
 *  \param msg Additional error text to be echoed to the console.
 **/
void error_and_close(int sockFd, const char *msg);

/**
 *  \brief Reports the error message specified as well as the error from
 *  the system. Exits the program with the ERROR exit code.
 *  \param msg Additional error text to be echoed to the console.
 **/
void error(const char* msg);

/**
 *  \brief Creates a new socket endpoint for communicating with a remote
 *  host over TCP/IP.
 *  \returns Socket file descriptor which provides a handle to the newly-
 *  created socket endpoint.
 *  \remarks If an error occurs, prints the error to the console and forces
 *  the program to exit with the ERROR exit code.
 */
int SocketDemoUtils_createTcpSocket();

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
void SocketDemoUtils_populateServerAddrInfo(const char *port, struct sockaddr_in *addr);

/**
 *  \brief Binds a server socket to the address and port specified by the 'addr'
 *   parameter.
 *  \param sockFd Socket file descriptor that references the socket to be bound.
 *  \param addr Pointer to a sockaddr_in structure that specifies the host and port
 *  to which the socket endpoint should be bound.
*/
int SocketDemoUtils_bind(int sockFd, struct sockaddr_in *addr);

/**
 * \brief Sets up a TCP or UDP server socket to listen on a port and IP address
 * to which it has been bound previously with the SocketDemoUtils_bind function.
 * \param sockFd Socket file descriptor.
 * \returns ERROR if the socket file descriptor passed in sockFd does not represent
 * a valid, open socket and sets errno to EBADF.  Otherwise, returns the result of
 * calling listen on the socket file descriptor passed with a backlog size of
 * BACKLOG_SIZE (128 by default).  Zero is returned if the operation was successful.
 */
int SocketDemoUtils_listen(int sockFd);

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
int SocketDemoUtils_accept(int sockFd, struct sockaddr_in *addr);

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
int SocketDemoUtils_recv(int sockFd, char **buf);

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
int SocketDemoUtils_send(int sockFd, const char *buf);

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
int SocketDemoUtils_connect(int sockFd, const char *hostnameOrIp, int port);

#endif //__SOCKETDEMOUTILS_H__
