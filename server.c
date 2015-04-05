#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>



void *send_function(void *argument)
{
	long int size_of_file = 0;
        char name_of_file[128], answer[128], ans;
        char buffer[512];
        int bytes_to_read = 0;
	int client_socket = *(int*)argument;

	recv(client_socket, name_of_file, 128, 0);
	FILE *input_file = fopen(name_of_file, "rb");
        if (input_file) {
        	fseek(input_file, 0, SEEK_END);
                size_of_file = ftell(input_file);
                snprintf(answer, 128, "Size of file %s is %d B, do you want to continue? [y/n] ", name_of_file, size_of_file);
        }
        else {
                send(client_socket, answer, 128, 0);
                return;
	}

        send(client_socket, answer, 128, 0);
        recv(client_socket, &ans, 1, 0);
        if (ans == 'n') {
                close(client_socket);
                return;
        }

        fseek(input_file, 0, SEEK_SET);
        bytes_to_read = fread(buffer, 1, 512, input_file);
	while (bytes_to_read) {
                send(client_socket, buffer, bytes_to_read, 0);
                bytes_to_read = fread(buffer, 1, 512, input_file);
        }
        close(client_socket);
        fclose(input_file);
}

int main()
{
	int client_socket,server_socket;
	struct sockaddr_in address;
	int addrlen;
	pthread_t thread;
	pid_t process_id;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_family = AF_INET;
	address.sin_port = htons(6565);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(server_socket, (struct sockaddr *)&address, sizeof(address));

	listen(server_socket, 5);

	while (1) {
		client_socket = accept(server_socket, 0, 0);
		if (client_socket){
			#ifdef SEND_FILE_USING_THREAD
			pthread_create(&thread, NULL, send_function, &client_socket);
			#else
			process_id=fork();
			switch(process_id){
				case 0:
					close(server_socket);
					send_function((void*)&client_socket);
				case -1:
					exit(1);
				default:
					close(server_socket);
			}
			#endif
		}
	}
	return 0;
}
