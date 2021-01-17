// Server-Client structure recieved from https://www.geeksforgeeks.org/socket-programming-cc/?ref=lbp
// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <sstream>
#include <vector>
#include <pthread.h>
#include <iostream>
#include <netdb.h> 

using namespace std;

struct ParamPassStruct {
	char ipOctets[16],
		sMaskOctets[16];

	char network[16],
		broadcast[16],
		hostMin[16],
		hostMax[16];

	int hostNum;
};

struct PassToThread {
	u_short port;
	const char* address;
	ParamPassStruct *paramPass;
};

bool GetIpOctets(string, vector<int>&);
bool GetSubnetMaskOctets(string, vector<int>&);
void print(ParamPassStruct);
void* ChildThreadFunction(void*);


int main(int argc, char const* argv[])
{

	if (argc != 3)
	{
		printf("\n Client argument error \n");
		return -1;
	}

	vector <ParamPassStruct> allParameters;

	// Get IPs and Subnet Masks
	while (!cin.eof())																// Read each line of file
	{
		ParamPassStruct parameterPass;
		cin >> parameterPass.ipOctets >> parameterPass.sMaskOctets;
		allParameters.push_back(parameterPass);
	}

	const char* address = argv[1];
	u_short PORT = (unsigned short)strtoul(argv[2], NULL, 0);
	
	pthread_t tid[allParameters.size()];
	PassToThread passToThread[allParameters.size()];
	for (int i = 0; i < allParameters.size(); i++)
	{
		
		passToThread[i].address = address;

		passToThread[i].port = PORT;
		passToThread[i].paramPass = &allParameters[i];
		
		if (pthread_create(&tid[i], NULL, ChildThreadFunction, &passToThread[i]))		// pass one line of ip and sMask
		{
			cout << "Error creating thread./n";
			return 1;
		}
	}	

	for (int i = 0; i < allParameters.size(); i++)
		pthread_join(tid[i], NULL);

	
	for (int i = 0; i < allParameters.size(); i++) {
		print(allParameters[i]);
	}
	return 0;
}

/// <summary>
/// Prints given argStruct of addresses
/// </summary>
/// <param name="arguments">argStruct of addresses to be printed</param>
void print(ParamPassStruct parameterPass)
{
	printf("IP Address: %s\n", parameterPass.ipOctets);
	printf("Subnet: %s\n", parameterPass.sMaskOctets);
	printf("Network: %s\n", parameterPass.network);
	printf("Broadcast: %s\n", parameterPass.broadcast);
	printf("HostMin: %s\n", parameterPass.hostMin);
	printf("HostMax: %s\n", parameterPass.hostMax);
	printf("# Hosts: %i\n", parameterPass.hostNum);
	printf("\n");
}

void* ChildThreadFunction(void* passToThread) {
	PassToThread* recievedInThread = (PassToThread*)passToThread;
	ParamPassStruct* parameterPass = recievedInThread->paramPass;

	// Create socket
	int sock = 0;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return NULL;
	}
	struct sockaddr_in serv_addr;
	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(recievedInThread->port);

	 struct hostent *address = gethostbyname(recievedInThread->address);
	if (address == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return NULL;
	}
	//printf("IP: %s\n",parameterPass->ipOctets);
	send(sock, parameterPass, sizeof(ParamPassStruct), 0);
	int valread;
	valread = read(sock, parameterPass, sizeof(ParamPassStruct));
	return NULL;
}