#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		return -1;
	}
	int socket_object;
	struct sockaddr_in address;
	struct hostent *server;
	char answer[64], answer_char;
	char buffer[512];
	int i, received_bytes = 0;

	socket_object = socket(AF_INET, SOCK_STREAM, 0);

	server = gethostbyname(argv[1]);
	if (server == NULL) {
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(6565);
	bcopy((char *)server->h_addr, (char *)&address.sin_addr.s_addr, server->h_length);

	if (connect(socket_object, (struct sockaddr *)&address, sizeof(address)) < 0) {
        	return -1;
	}

	send(socket_object, argv[2], strlen(argv[2])+1, 0);
	recv(socket_object, answer, 64, 0);
	printf(answer);
	answer[5]='\0';
	if (!strcmp(answer, "error")) {
		close(socket_object);
		return -1;
	}

	scanf("%c", &answer_char);
	if ((answer_char != 'y') && (answer_char != 'Y')) {
		answer_char = 'n';
		send(socket_object, &answer_char, 1, 0);
		close(socket_object);
		return 0;
	}

	i = strlen(argv[2])-1;
	while ((i > 0) && (argv[2][i] != '/'))
		i--;
	if (i)
		strcpy(argv[2], argv[2]+i+1);

	FILE *fout = fopen(argv[2], "wb");
	if (!fout) {
		answer_char = 'n';
		send(socket_object, &answer_char, 1, 0);
		close(socket_object);
		return -1;
	}

	send(socket_object, &answer_char, 1, 0);
	received_bytes = recv(socket_object, buffer, 512, 0);
	while (received_bytes > 0) {
		fwrite(buffer, 1, received_bytes, fout);
		received_bytes = recv(socket_object, buffer, 512, 0);
	}
	close(socket_object);
	fclose(fout);
	return 0;
}
