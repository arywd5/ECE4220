//Allison Yaeger 
//14244528


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

//IP 128.206.19.255

#define MSG_SIZE 40


int main(int argc, char *argv[]){

	if(argc != 2){
		printf("\nincorrect usage, correct format is: \n ./a.out <port number>");
		return -1;
	}

	int soc, port, var, master = 0, votes = 0;
	unsigned int length;
	char message[MSG_SIZE], myIP[NI_MAXHOST];
	struct ifaddrs *ifaddr, *ifa;	
	struct sockaddr_in server, from;
	
	//get IP of our network
	if((getifaddrs(&ifaddr)) == -1){
		printf("\nError in getifaddrs");
		return -1;
	}
	for(ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next){
		if(ifa->ifa_addr == NULL)
			continue;

		var = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), myIP, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);		
	}	

	printf("\nMy IP is: %s", myIP);

	//create socket 
	if((soc = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("\nSocket error..");
		return -1;
	}

	from.sin_family = AF_INET;
 	from.sin_port = htons(atoi(argv[1]));
	inet_aton("128.206.19.255", &from.sin_addr);
	length  = sizeof(struct sockaddr_in);

	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));

	

	//after socket is created enter an infinite loop to read in messages and send messages
	while(1){
		
		var = recvfrom(soc, message, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
		
		//WHOIS recieved 
		if(strcmp(message, "WHOIS") == 0){
			if(master == 1){
				sprintf(message, "Allie at %s if master", myIP);
				var = sendto(soc, message, strlen(message), 0, (struct sockaddr *)&from, length);			
			}

		}
		//VOTE recieved
		if(strcmp(message, "VOTE") == 0){
			votes = (rand()%10) + 1;		
			sprintf(message, "%s %d", myIP, votes);
			var = sendto(soc, message, strlen(message), 0, (struct sockaddr *)&from, length);

		}
		//IP # of votes recieved 
		






	}

	return 0;
}
