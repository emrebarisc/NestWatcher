#include "Managers/NetworkManager.h"

#include <arpa/inet.h>
#include <netinet/in.h>

NetworkManager::NetworkManager()
{

}

void NetworkManager::Init()
{
	commandSocket_ = socket(AF_INET, SOCK_STREAM, 0);

	commandServerAddress_.sin_family = AF_INET;
	commandServerAddress_.sin_port = htons(COMMAND_PORT);
	commandServerAddress_.sin_addr.s_addr = inet_addr(SERVER_IP);

	bind(commandSocket_, (struct sockaddr*)&commandServerAddress_, sizeof(commandServerAddress_));

	dataServerAddress_.sin_family = AF_INET;
	dataServerAddress_.sin_port = htons(DATA_PORT);
	dataServerAddress_.sin_addr.s_addr = inet_addr(SERVER_IP);

	bind(dataSocket_, (struct sockaddr*)&dataServerAddress_, sizeof(dataServerAddress_));

}

