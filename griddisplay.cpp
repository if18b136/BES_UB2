#include "vtg.h"

#define MAX_BUF 1024

int main()
{
	while(1){
    	int fd;
    	char * myfifo = "/tmp/myfifo";
    	char buf[MAX_BUF];

    	/* open, read, and display the message from the FIFO */
    	fd = open(myfifo, O_RDONLY);
    	read(fd, buf, MAX_BUF);
    	printf("%s\n", buf);
    	close(fd);
    	usleep(200000);
	}

    return 0;
}