#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <cassert>
#include <errno.h> 
#include <fcntl.h>

#define MAXDATASIZE 1024

using namespace std;

class Yuri {
	public:
		//Yuri();
		void listen(int);
		
	private:
		char* nick, user;
		char* listen_host;
		int connected_dry_clients = 0;
		int client_id = 0;
		
		
		int channel_num, port, listen_port, timeout, client, sock, listen_sock,  listen_threads_ex[32];
		void new_client(string, string);
		//void hibernate(bool);
		//bool send_pong(int, char*);
		void sync(char*, int);
		void kill_host(int);
		struct thread_info {
			pthread_t thread;
			uint64_t thread_id;
			int thread_num;
			bool is_allowed_hibernation;
			char* server_host;
			int server_port;
			int sock;
			int client_id;
		};

		struct client_data {
			int client_id;
			string client_name;
			string client_host;
			int client_channel;
		};




		static void *recieve_on_thread(void *ctx)
		{
			
			struct thread_info *data;

			//cout << "Created Thread id: " << data->thread_id << ":" << (uint)data->thread_id << endl;

			data = (struct thread_info *) ctx; 
			cout << "New client added to Queue." << endl;
			cout << data->sock << endl;
			//cout << "Entering Thread id: " << data->thread_id << ":" << (uint)data->thread_id << endl;
			
			cout << "Listening on: " << data->server_host << ":" << data->server_port << endl;
			cout << data->sock << endl;
			int client_connected = 1;


			while(client_connected)
			{
				cout << "Max data: " << MAXDATASIZE << endl;
				char recv_buffer[MAXDATASIZE] = "";
				char * rc = recv_buffer;
				cout << "Starting recv:" << endl;
				int bytes_recv = recv(data->sock, recv_buffer, MAXDATASIZE - 1, 0);
				cout << "Finished recv:" << endl;
				if (bytes_recv > 0)
				{
					send(data->sock, "Hello\n", 7, 0);
					send_info(data->sock, "test\n");
					cout << data->client_id << ": Client bytes: " << rc << endl;
				}
				else
				{
					cout << "recv: Error " << strerror(errno) << endl;
					cout << "Client" << data->client_id << " has dropped." << endl;
					close(data->sock);
					break;
				} 
				
			}
			

		}

		bool salgo(char* search, char* piece)
		{
			int len = strlen(search), piecelen = strlen(piece);

			for(int i = 0; i < len; i++)
			{
				if(search[0] == piece[i])
				{
					bool found = true;
					for (int x = 1; x < piecelen; x++)
					{
						if(search[i+x] != piece[x])
						{
							found = false;
						}
					}
					if (found == true)
						return true;
				}
			}
			return false;
		}

		bool hndl(char *message)
		{

		}

		static bool send_info(int client_sock, char *message)
		{
			int len = strlen(message);
			int bytes_sent = send(client_sock, message, len, 0);
			if(bytes_sent == 0)
				return false;
			else
				return true;
		}

		bool send_pong(int client_sock, char *buffer)
		{
			char * search = "PING ";
			bool found = this->salgo(buffer, search);

			if(found)
			{
				char res[5];
				res[0] = 'P';
				res[1] = 'I';
				res[2] = 'N';
				res[3] = 'G';
				res[4] = ' ';
				char * ptr = res;

				if(send_info(client_sock,ptr))
				{
					cout << "PING PONG" << endl;
					return true;
				}
			
			}
			else
			{
				return false;
			}
		}
};