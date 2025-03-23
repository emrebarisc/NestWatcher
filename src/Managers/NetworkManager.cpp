#include "Managers/NetworkManager.h"

#include <arpa/inet.h>

#include <iostream>
#include <thread>

NetworkManager::NetworkManager()
{

}

NetworkManager::~NetworkManager()
{
	close(commandSocket_);
	close(dataSocket_);

	delete listenerThread_;
}

void NetworkManager::Init()
{
	commandSocket_ = socket(AF_INET, SOCK_STREAM, 0);

	commandAddress_.sin_family = AF_INET;
	commandAddress_.sin_port = htons(COMMAND_PORT);
	commandAddress_.sin_addr.s_addr = inet_addr(SERVER_IP);

	bind(commandSocket_, (struct sockaddr*)&commandAddress_, sizeof(commandAddress_));

	dataSocket_ = socket(AF_INET, SOCK_STREAM, 0);

	dataAddress_.sin_family = AF_INET;
	dataAddress_.sin_port = htons(DATA_PORT);
	dataAddress_.sin_addr.s_addr = inet_addr(SERVER_IP);

	bind(dataSocket_, (struct sockaddr*)&dataAddress_, sizeof(dataAddress_));

}


void NetworkManager::StartListeningCommandAddress()
{
	if(!listenerThread_)
	{
		std::cout << "Already started listening command socket!" << std::endl;
		return;	
	}

	listenerThread_ = new std::thread(&NetworkManager::StartListeningCommandAddressAsync, this);
}

void NetworkManager::StartListeningCommandAddressAsync()
{
	if(int listenResult = listen(commandSocket_, 5) != 0)
	{
		std::cerr << "Listen failed with code " << listenResult << std::endl;
	}

	sockaddr_in clientAddress;
	socklen_t clientAddressSize = sizeof(clientAddress);
	int clientSocket = accept(commandSocket_, (struct sockaddr*)&clientAddress, &clientAddressSize);
	if(clientSocket == -1)
	{
		std::cerr << "Accept failed." << std::endl;
	}

	clientIP_ = clientAddress.sin_addr;

	std::cout << "Connection established with " << inet_ntoa(clientIP_) << std::endl;
}

