#include "vtg.h"

#define MAX_BUF 1024

int main()
{
	while(1){
    	int fd;
    	char * myfifo = "/tmp/myfifo";     //von gewaehlter Speicherposition Wird Pipe aufgegriffen
    	char buf[MAX_BUF];     //Buffer

    	/* open, read, and display the message from the FIFO */
    	fd = open(myfifo, O_RDONLY);       //Pipe wird geoeffnet
    	read(fd, buf, MAX_BUF);        //Pipe wird ausgelesen
    	printf("%s\n", buf);       //Message(Grid) wird geprintet
    	close(fd);     //Pipe wird geschlossen
    	usleep(200000);        //Kuze wartezeit bevor das naechste mal ausgelesen wird
	}

    return 0;
}