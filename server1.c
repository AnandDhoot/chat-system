#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#define BACKLOG 10
int chkNumeric(char* str)
{
	int i=0;
	for(; i<strlen(str); i++)
	{
		if(str[i] < '0' || str[i] > '9')
			return 0;
	}
	return 1;
}
int stoi(char* str)
{
	int result = 0;
	int i=0;
	for(; i<strlen(str); i++)
	{
		result = result*10 + (str[i]-'0');
	}
	return result;
}
char* itos(int num)
{
	static char result[10];
	sprintf(result, "%d", num);
	return result;
}
char* getPortIP(int fd)
{
	struct sockaddr addr_client;
	socklen_t len;
	if(getpeername(fd, &addr_client, &len) == -1)
		printf("Error getting information about the client IP and Port\n");
	struct sockaddr_in *s = (struct sockaddr_in *) &addr_client;
	char ipstr[25];
	int port = ntohs(s->sin_port);
	inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

	static char result[35];
	strcpy(result, "");
	strcat(result, ipstr);
	strcat(result, ":");
	strcat(result, itos(port));
	return result;
}
int main(int argc, char* argv[])
{	
	if(argc == 2 && ((chkNumeric(argv[1])) == 0))
	{
		printf("Non-numeric argument provided\n");
		return 1;
	}
	else if(argc != 2)
	{
		printf("Incorrect number of arguments\n");
		return 1;
	}
	int port_num = stoi(argv[1]);

	// Getting the file descriptor
	int fd_listener;
	fd_listener = socket(PF_INET, SOCK_STREAM, 0);
	if(fd_listener == -1)
		printf("Error in getting the socket file descriptor\n");

	// Binding socket with port
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port_num);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);
	if((bind(fd_listener, (struct sockaddr*) &my_addr, sizeof(struct sockaddr))) == -1)
	{	
		printf("Error binding socket with port\n");
		return 2;
	}
	
	// Waiting for incoming connections
	if((listen(fd_listener, BACKLOG)) == -1)
		printf("Error listening\n");

	// Accepting the pending connection on the port port_num
	int fd_client;
	struct sockaddr_in addr_client;
	unsigned int sin_size;
	sin_size = sizeof(struct sockaddr_in);
	fd_client = accept(fd_listener, (struct sockaddr*) &addr_client, &sin_size);
	if(fd_client == -1)
		printf("Error accepting connections\n");
	close(fd_listener);

	while(1 == 1)
	{
		// Receiving the message
		char recv_buffer[3000];
		int len, bytes_recd;
		len = 3000;
		bytes_recd = recv(fd_client, recv_buffer, len, 0);
		if(bytes_recd == 0)
			break;

		// Constructing the message to be sent
		char msg[45];
		if(bytes_recd == 4 && (recv_buffer[0] == 'B' && recv_buffer[1] == 'y' && recv_buffer[2] == 'e' && recv_buffer[3] == '\n'))
			strcpy(msg, "Goodbye ");
		else
			strcpy(msg, "OK ");
		strcpy(recv_buffer, "");

		strcat(msg, getPortIP(fd_client));		
		// Sending the message
		int bytes_sent;
		len = strlen(msg);
		while(1 == 1)
		{
			bytes_sent = send(fd_client, msg, len, 0);
			if(bytes_sent == len)
				break;
			printf("%d Bytes sent out of %d. Resending.\n", bytes_sent, len);         
		}
	}

	// Closing connection
	close(fd_client);

	return 0;
}
