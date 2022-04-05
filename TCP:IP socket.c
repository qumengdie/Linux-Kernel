#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//check the format of the argument, only digit char allowed
int checkDigit(char *str){
    for(int i = 0;i<strlen(str);i++){
        if(!isdigit(str[i]))
            return 0;
    }
    return 1;
}

int main(int argc,char *argv[]){
    int n;
    
    // validate the input parameter n
    if(argc != 2){
        printf("usage: ./lab6_1 <n> \n");
        return -1;
    }

    // validate and obtain the input
    if(!checkDigit(argv[1])){
        printf("integer parameters only!\n");
        return -1;
    }
    n = atoi(argv[1]);


	pid_t pid = fork();
	if (pid < 0) {
		printf("Fork failed!");
		return -1;
	} 
	else if (pid == 0){   //child process (producer, server)
        int c = 0, interval;
        float val;

        struct sockaddr_in server, client;
		int sfd, cfd, len;

		//create the server socket
		sfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sfd < 0) {
			perror("create");
			return -1;
		}
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(8080);
        bzero(&(server.sin_zero), 8);

		int enable = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
    		perror("setsockopt(SO_REUSEADDR)");

		//bind 
		if (bind(sfd, (struct sockaddr*) &server, sizeof(struct sockaddr)) != 0) {
			perror("bind");
			return -1;
		}

		// set the seed
		srand((unsigned) time(NULL));

		//sleep before listening and accepting connections
		int slt = rand() % 5 + 1;
		sleep(slt);

		//listen
		if(listen(sfd, 5) < 0) {
			perror("listen");
			return -1;
		}
		
		//accept connections
		cfd = accept(sfd, (struct sockaddr*)&client, &len);
		if (cfd < 0) {
			perror("accept");
			return -1;
		}

		while(c < n){
			val = pow(c * 0.5,2);
			
			// write to the socket
			write(cfd, &val, sizeof(float));

			// wait for a random interval of time (0 to 4.999 seconds)
			interval = rand() % 5000;
			usleep(interval * 1000);
			
			c++;
		}
        //close the socket
        close(sfd);
        close(cfd);
        exit(0);
	} 
	else {    //parent process (consumer, client)
        int c = 0;
        float val;

        struct sockaddr_in client;
		int cfd;

		//create the client socket
		cfd = socket(AF_INET, SOCK_STREAM, 0);
		if (cfd < 0) {
			perror("create");
			return -1;
		}
		client.sin_family = AF_INET;
		client.sin_port = htons(8080);
		client.sin_addr.s_addr = inet_addr("127.0.0.1");
		bzero(&(client.sin_zero), 8);

		//try to connect
		while (connect(cfd, (struct sockaddr*)&client, sizeof(struct sockaddr)) != 0) {
			usleep(100 * 1000);
		}
	
		while(c < n){
			c++;
			
			// read from the socket
			read(cfd, &val, sizeof(float));
			printf("%f ", val);
			fflush(stdout);
		}

		printf("\n");
		fflush(stdout);
		
        //wait the child to finish
        wait(NULL);

        close(cfd);

	}

	return 0;
}
