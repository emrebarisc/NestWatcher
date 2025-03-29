#include "Managers/NetworkManager.h"

#include <arpa/inet.h>

#include <iostream>
#include <thread>

#include "Program.h"
#include "Managers/InputManager.h"

NetworkManager::NetworkManager()
{

}

NetworkManager::~NetworkManager()
{
	if(0 <= commandSocket_)
	{
		close(commandSocket_);
	}
	
	if(0 <= dataSocket_)
	{
		close(dataSocket_);
	}

	delete listenerThread_;
}

void NetworkManager::Init()
{
	commandSocket_ = socket(AF_INET, SOCK_STREAM, 0);

	commandAddress_.sin_family = AF_INET;
	commandAddress_.sin_port = htons(COMMAND_PORT);
	commandAddress_.sin_addr.s_addr = INADDR_ANY;//inet_addr(SERVER_IP);
	
	int commandSocketBindResult = bind(commandSocket_, (const sockaddr*)&commandAddress_, sizeof(commandAddress_)); 
	if(commandSocketBindResult != 0)
	{
		std::cerr << "Failed to bind command socket with error: " << commandSocketBindResult  << std::endl;
		close(commandSocket_);
		return;
	}

	dataSocket_ = socket(AF_INET, SOCK_DGRAM, 0);

	dataAddress_.sin_family = AF_INET;
	dataAddress_.sin_port = htons(DATA_PORT);
	dataAddress_.sin_addr.s_addr = inet_addr(SERVER_IP);
	
	int dataSocketBindResult = bind(dataSocket_, (struct sockaddr*)&dataAddress_, sizeof(dataAddress_)); 
	if(dataSocketBindResult != 0)
	{
		std::cerr << "Failed to bind data socket with error: " << dataSocketBindResult  << std::endl;
		close(dataSocket_);
		return;
	}
}


void NetworkManager::StartListeningCommandAddress()
{
	if(listenerThread_)
	{
		std::cout << "Already started listening command socket!" << std::endl;
		return;	
	}

	try
	{
		listenerThread_ = new std::thread(&NetworkManager::StartListeningCommandAddressAsync, this);
	}
	catch(const std::exception& exception)
	{
		std::cerr << "Failed to start listener thread: " << exception.what() << std::endl;
		return;
	}
}

void NetworkManager::StartListeningCommandAddressAsync()
{
	if(int listenResult = listen(commandSocket_, 5) != 0)
	{
		std::cerr << "Listen failed with code " << listenResult << std::endl;
		return;
	}

	std::cout << "Command server started." << std::endl;

	CommandMessage commandMessage;

	Program* program = Program::GetInstance();
	InputManager* inputManager = program->GetInputManager();

	while(true)
	{
		sockaddr_in clientAddress;
		socklen_t clientAddressSize = sizeof(clientAddress);
		int clientSocket = accept(commandSocket_, (struct sockaddr*)&clientAddress, &clientAddressSize);
		if(clientSocket < 0)
		{
			std::cerr << "Accept failed." << std::endl;
			continue;
		}

		clientIP_ = clientAddress.sin_addr;

		std::cout << "Connection established with " << inet_ntoa(clientIP_) << std::endl;

		while(0 < recv(clientSocket, &commandMessage, sizeof(commandMessage), 0))
		{
			inputManager->ExecuteCommand(commandMessage);

			std::cout << "\tCommand: " << (int)commandMessage.command << "\tCommand message: " << commandMessage.commandMessage << std::endl;
		}

		close(clientSocket);
	}
}

