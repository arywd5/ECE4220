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

	int i, bbroad = 1, soc, port, var, master = 0, votes = 0, flags[3];
	unsigned int length;
	char message[MSG_SIZE], myIP[NI_MAXHOST];
	struct ifaddrs *ifaddr, *ifa;	
	struct sockaddr_in server, from;
	char otherIP[2], otherVotes;
	srand(time(0));

	
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

	//broadcast and bind socket 
	if(bind(soc, (const struct sockaddr *)&from, sizeof(from)) < 0){
                printf("\nError binding socket..");
                return -1;
        }

	var = setsockopt(soc, SOL_SOCKET, SO_BROADCAST, &bbroad, sizeof(bbroad));
	if(var < 0){
		printf("\nError in setsockopt()");
		return -1;
	}

	//after socket is created enter an infinite loop to read in messages and send messages
	while(1){
		
		var = recvfrom(soc, message, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
		printf("\nMessage Recieved: %s", message);
		if(var < 0){
			printf("\nError Recieving Message..");
			return -1;
		}
		//WHOIS recieved 
		else if(strncmp(message, "WHOIS", 5) == 0){
			if(master == 1){
				sprintf(message, "Allie at %s if master", myIP);
				var = sendto(soc, message, strlen(message), 0, (struct sockaddr *)&from, length);			
			}
		}
		//VOTE recieved
		else if(strncmp(message, "VOTE", 4) == 0){
			votes = (rand()%10) + 1;		
			sprintf(message, "#%s %d", myIP, votes);
			var = sendto(soc, message, strlen(message), 0, (struct sockaddr *)&from, length);
			flags[2] = 1;	
		}
		//IP # of votes recieved 
		else if(strncmp(message, "# 128.206.19", (size_t)(12*sizeof(char)))){
			int getIP = 0, getVote = 0;
			for(i = 12; message[i] != '\0'; i++){ //start parsing at position 12 because we know its the same up until then 
				if(message[i] = '.'){
					getIP = 1;	
				}
				else if(message[i] = ' '){
					getIP = 0;
					getVote = 1;
				}
				else if(getIP > 0){		//if the get IP flag is set that means we found a . and the IP should follow it 
					otherIP[getIP - 1] = message[i];
					getIP++;
				}
				else if(getVote = 1){		//if get vote flag is on save this to our number of votes
					otherVotes = message[i];	
				}
			}
			//now that we have parsed the message and got the IP number and vote number we can compare ours 
			if(atoi(&otherVotes) == votes){
				//need to compare the IP's
				printf("\nComparing IPs...");
				
			}
			else if(atoi(&otherVotes) <= votes){
				//you are the master so send the message 
				master = 1;
				sprintf(message, "Allie at %s is master", myIP);
				var = sendto(soc, message, strlen(message), 0, (struct sockaddr *)&from, length);
			}
			else{ 	//you are not the master dont send any messages 
				master = 0; 	
				printf("\nYou are not the master..");
			}
		}
	}

	return 0;
}
