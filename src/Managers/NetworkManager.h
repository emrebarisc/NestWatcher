#pragma once

#include "Managers/IManager.h"

#include <string>

#include <netinet/in.h>

#define SERVER_IP "192.168.31.154"

class NetworkManager : public IManager
{
	public:
		NetworkManager();

		virtual void Init() override;
	protected:

	private:
		static constexpr int COMMAND_PORT{ 1000 };
		static constexpr int DATA_PORT{ 1001 };

		int commandSocket_{ 0 };
		int dataSocket_{ 0 };

		sockaddr_in commandServerAddress_{ 0 };
		sockaddr_in dataServerAddress_{ 0 };
};
