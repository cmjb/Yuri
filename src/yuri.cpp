/*
	Yuri's subsystem

	Yuri can only take one user for mutiple connections, the goal in the future is to find some secure way of storing mutilple user 
	credentials without compromising the security of the users.

	Threads should only be spawned once users connect to service and a thread devided from a spawn 

*/
#include "include/yuri.h"





void Yuri::new_client(string client_name, string client_host )
{
	client_data * new_client;
	new_client->client_name = client_name;
	new_client->client_host = client_host;
	// TODO: Get number of clients and give new ID.
}

// NOTE: We let a thread wait for users, once user connects, get id and set into a handling thread for connections
void Yuri::listen(int arg_listen_port)
{
	char * temp_listen_host = "127.0.0.1"; // TODO: Add this as an overload with a non-requirement to set host, or set as config.
	pthread_t *listen_threads;
	int listen_threads_max = 32;
	listen_threads = new pthread_t[listen_threads_max];
	listen_host = (char *)temp_listen_host;
	listen_port = arg_listen_port;
	int listen_threads_num = 0;

	cout << "NOTICE: Yuri is set to recieve information from " << listen_host << ":" << listen_port << " without SSL." << endl;

	struct sockaddr client;
	socklen_t len = sizeof(struct sockaddr);

	struct sockaddr_in ip4addr;
	memset(&ip4addr, 0, sizeof(ip4addr));
	char lo[INET6_ADDRSTRLEN];
	ip4addr.sin_family = AF_INET;
	ip4addr.sin_port = htons(listen_port);
	ip4addr.sin_addr.s_addr = inet_addr("127.0.0.1");


	inet_pton(AF_INET, (char *)listen_host, lo);
	listen_sock = -1;
	try{
		if ((listen_sock = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
			throw errno;

	}
	catch(int error)
	{
		cout << "ER1:"<< error << "(" <<strerror(error) << ")" << endl;
		exit(1);
	}

	int on = 1;
	if(setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) == -1)
	{
		cout << "Failed." << endl;
		exit(1);
	}

	fcntl(listen_sock, F_SETFL, O_NONBLOCK);
	fcntl(listen_sock, F_SETFL, O_ASYNC);

	int bin;

	try
	{
		if (-1 == (bin = ::bind(listen_sock, (struct sockaddr*) &ip4addr, sizeof ip4addr)))
			throw errno;
	}
	catch(int error)
	{
		cout << "ER2:"<< error << "(" <<strerror(error) << ")" << endl;
		exit(1);
	}

	if(listen_sock < 0)
		cout << "SOCKET ERROR" << endl;

	bool hibernate = true;
	socklen_t slen = 300;
	int listen_conf = ::listen(listen_sock, 5);
	cout << "Waiting for users... Hibernating. Listen status: " << listen_conf << endl;

	struct sockaddr_in client_listen = { 0 };
	int accepted_sock = 0;
	unsigned int addrlen = sizeof(struct sockaddr_in);
	

	while((accepted_sock = accept(listen_sock, (struct sockaddr *)&client_listen, &addrlen )) > 0)
	{
		cout << "New client detected on host: " << accepted_sock << endl;
		//accepted_sock = ::accept(s, (struct sockaddr *)&client, &addrlen );
		
		/*
		char recv_buffer[4096] = "";
		int buff_limiter = 0;
		char * rc = recv_buffer;
		int bytes_recv = recv(accepted_sock, recv_buffer, 4096 - 1, 0);
		*/

		// Connected clients is the amount of clients that have connected and left.
		connected_dry_clients++;
		client_id = connected_dry_clients;
		cout << "Client id: " << client_id << endl;
		listen_threads_num++;
		struct thread_info tinfo[client_id];
		tinfo[client_id].server_host = (char *)listen_host;
		tinfo[client_id].server_port = listen_port;
		tinfo[client_id].sock = accepted_sock;
		tinfo[client_id].thread_id = 0; 
		tinfo[client_id].client_id = client_id;

		cout << "thread num " << listen_threads_num << endl;
		cout << "Sending data packet to thread socket: 4" << tinfo[client_id].sock << endl;
		int thd = 0;
		if((thd = pthread_create(&listen_threads[listen_threads_num], NULL, &this->recieve_on_thread, &tinfo[client_id] )) )
		{
			cout << "Client Error" << endl;
			perror("No thread.");
		}

		cout << "User Accepted." << endl;
		/*if (bytes_recv > 0)
		{
			send(accepted_sock, "Hello\n", 7, 0);
			cout << "Client bytes: " << rc << endl;
		}
		else
		{
			cout << "recv: Error " << strerror(errno) << endl;
			break;
		} 
		*/

	}

	cout << "test" << endl;
}


void Yuri::sync(char * host, int host_port)
{
	// Check SSL, inject module here.
	// TODO: Fix
	// port cannot be int, probably due to unix sockets
	// port needs to be handled, but hard coded for now.

	const char * port = "6667";
	bool startup = true;
	bool connection = false;
	struct addrinfo hints, *info;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int res, server_sock;
	if((res = getaddrinfo(host, port, &hints, &info)) != 0)
	{
		startup = false;
		cout << "ERROR: Failure to get address." << gai_strerror(res) << endl;

	}

	if((server_sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) == -1)
	{
		cout << "YURI-CLIENT: ERROR: Socket not avaliable." << endl;
	}

	if (::connect(server_sock, info->ai_addr, info->ai_addrlen) == -1)
	{
		cout << "YURI-CLIENT: ERROR: Client cannot connect." << endl;
		close(sock);
		
	}

	freeaddrinfo(info);

	int total_bytes;
	char buffer[MAXDATASIZE];

	connection = true;

	int uptime = 0;
	while (connection == true)
	{
		uptime++;
		switch(uptime)
		{
			case 3:
				this->send_info(server_sock, "test");
				this->send_info(server_sock, "test");
				break;
			case 4:
				this->send_info(server_sock, "JOIN #yurichannel\r\n");
			default:
				break;
		}

		total_bytes = recv(sock, buffer, MAXDATASIZE-1, 0);
		buffer[total_bytes] = '\0';
		cout << buffer;

		this->hndl(buffer);

		if(this->salgo(buffer, "PING"))
			send_pong(server_sock, buffer);

		if(total_bytes == 0)
		{
			cout << "Connection killed on channel" << endl;
			connection = false;
			break;
		}

		
	}


}


void Yuri::kill_host(int client_id)
{
//
}
