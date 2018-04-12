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
	char otherIP[2], *otherVotes, WS[2];
	otherVotes = (char *)malloc(sizeof(char));
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
	WS[0] = myIP[11];			//Save work station number so we can use to compare later in the program 
	WS[1] = myIP[12];

	printf("\nMy IP is: %s", myIP);		//print IP to the user 

	//create socket 
	if((soc = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("\nSocket error..");
		return -1;
	}
	
	//set up sockaddr structure to use for sending and recieving messages
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
		memset(message,'\0', 40);			//clear message so we dont have any leftover numbers in it 
		var = recvfrom(soc, message, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
		printf("\nMessage Recieved: %s", message);	//print recieved messafge to the screen 
		if(var < 0){					//check that message was recieved properly 
			printf("\nError Recieving Message..");
			return -1;
		}
		//WHOIS recieved 
		else if(strncmp(message, "WHOIS", 5) == 0){
			if(master == 1){			//check if the master flag is set and if so send message 
				sprintf(message, "Allie at %s is master", myIP);
				var = sendto(soc, message, strlen(message), 0, (struct sockaddr *)&from, length);			
			}
		}
		//VOTE recieved
		else if(strncmp(message, "VOTE", 4) == 0){
			votes = (rand()%10) + 1;		//generate random vote number between 1-10	
			sprintf(message, "# %s %d", myIP, votes);	//then send vote 
			var = sendto(soc, message, strlen(message), 0, (struct sockaddr *)&from, length);
		}
		//IP # of votes recieved 
		else if(strncmp(message, "# 128.206.19", (size_t)(12*sizeof(char))) == 0){
			int getIP = 0, getVote = 0;	//flags to use when parsing the message string 
			for(i = 12; message[i - 2] != ' '; i++){ //start parsing at position 12 because we know its the same up until then 
				if(message[i] == '.'){		//if we read a . then next number will be the IP or workstation number 
					getIP = 1;		//so we set the flag 
				}
				else if(message[i] == ' '){	//if a space is read the next number will be the number of votes 
					getIP = 0;		//set getIP glad to zero 
					getVote = 1;		//and set getVote flag
				}
				else if(getIP > 0){		//if the get IP flag is set that means we found a . and the IP should follow it 
					otherIP[getIP - 1] = message[i];	//store next value in our otherIP variable 
					getIP++;
				}
				else if(getVote = 1){		//if get vote flag is on save this to our number of votes
					*otherVotes = message[i];	//if vote flag is set get number of votes 
					getVote = 0;
				}
			}
			//now that we have parsed the message and got the IP number and vote number we can compare ours 
			if(atoi(otherVotes) == votes){
				//need to compare the IP's
				printf("\nComparing IPs...");
				int theirs = atoi(otherIP), mine = atoi(WS);	//save both workstation numbers as integers 
				if(theirs == 0){							//check that this conversion occured properly
					theirs = atoi(&(otherIP[0]));			//if it didnt its probably a single digit workstation so only use first character in the array 
				}	
				if(theirs != 0){							//as long as thier workstation number is not zero we can compare them 
					if(mine > theirs){
						master = 1;							//set master flag if our IP is higher
						sprintf(message, "Allie at %s is master", myIP);	//create and send string 
						var = sendto(soc, message, strlen(message), 0, (struct sockaddr *)&from, length);
					}	
					else{									//if pur IP is not higher than we are not the master so set flag that way 
						master = 0;
					}
				}

			}
			else if(atoi(otherVotes) < votes){
				//you are the master so send the message 
				master = 1;
				sprintf(message, "Allie at %s is master", myIP);
				var = sendto(soc, message, strlen(message), 0, (struct sockaddr *)&from, length);
			}
			else{ 	//you are not the master dont send any messages 
				master = 0; 		
			}
		}
	}

	free(otherVotes);
	return 0;
}

