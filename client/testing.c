#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define OK          0

int main(int argc, char *argv[])
{
    char buffer[255];

    fprintf(stdout, "> ");
    char *result = fgets(buffer, 255, stdin);
    fprintf(stdout, "main: fgets result length = %d bytes\n", strlen(result));
    fprintf(stdout, "main: length of string buffer = %d bytes\n", 
        strlen(buffer));

    fprintf(stdout, "testing: %s\n", buffer);
    fprintf(stdout, "Exiting\n");

    return OK;
}
