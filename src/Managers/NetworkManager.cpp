#include "Managers/NetworkManager.h"

#include <arpa/inet.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

#include "Program.h"
#include "Managers/CameraManager.h"
#include "Managers/InputManager.h"

#include "Network/Command.h"
#include "Network/ImageCommand.h"
#include "Network/ImageData.h"

#include "IO/stb_image_write.h"

static void StbiWriteToVectorCallback(void *context, void *data, int size) 
{
    auto *vec = static_cast<std::vector<uint8_t>*>(context);
    const uint8_t *bytes = static_cast<const uint8_t *>(data);
    vec->insert(vec->end(), bytes, bytes + size);
}

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

	if(0 <= cameraImageCommandSenderSocket_)
	{
		close(cameraImageCommandSenderSocket_);
	}

	delete listenerThread_;
	delete imageTransmitterThread_;
}

void NetworkManager::Init(NetworkMode mode)
{
    if (mode == NetworkMode::AccessPoint)
    {
        std::cout << "Initializing Access Point Mode..." << std::endl;
        int result = system("nmcli dev wifi hotspot ifname wlan0 ssid NestWatcherAP password nestwatcher123");
        if (result != 0)
		{
            std::cerr << "Failed to start Access Point. Make sure NetworkManager is installed and managing wlan0." << std::endl;
        }
		else
		{
            std::cout << "Access Point started. Server IP is 10.42.0.1" << std::endl;
        }
    }
    else
    {
        std::cout << "Initializing Local Network Mode..." << std::endl;
        system("nmcli connection down Hotspot > /dev/null 2>&1");
    }

	commandSocket_ = socket(AF_INET, SOCK_STREAM, 0);

	commandAddress_.sin_family = AF_INET;
	commandAddress_.sin_port = htons(COMMAND_PORT);
	commandAddress_.sin_addr.s_addr = INADDR_ANY; // This correctly binds to BOTH local Wi-Fi and the new AP
	
	int commandSocketBindResult = bind(commandSocket_, (const sockaddr*)&commandAddress_, sizeof(commandAddress_)); 
	if(commandSocketBindResult != 0)
	{
		std::cerr << "Failed to bind command socket with error: " << commandSocketBindResult  << std::endl;
		close(commandSocket_);
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

void NetworkManager::TransmitCameraImage()
{
	if(imageTransmitterThread_)
	{
		std::cout << "Already started transmitting camera image!" << std::endl;
		return;	
	}

	try
	{
		imageTransmitterThread_ = new std::thread(&NetworkManager::TransmitCameraImageAsync, this);
	}
	catch(const std::exception& exception)
	{
		std::cerr << "Failed creating start transmitting camera image thread: " << exception.what() << std::endl;
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

		
		std::cout << "Connection closed with " << inet_ntoa(clientIP_) << std::endl;
	
		transmittingCameraImage_ = false;
		imageTransmitterThread_ = nullptr;

		close(clientSocket);
	}
}

void NetworkManager::TransmitCameraImageAsync()
{   
    CameraManager* cameraManager = Program::GetInstance()->GetCameraManager();
    if(!cameraManager) return;

    dataSocket_ = socket(AF_INET, SOCK_DGRAM, 0);
    dataAddress_.sin_family = AF_INET;
    dataAddress_.sin_port = htons(DATA_PORT);
    dataAddress_.sin_addr.s_addr = clientIP_.s_addr;

    std::cout << "Started sending camera image." << std::endl;
    transmittingCameraImage_ = true;

    uint32_t currentFrameId = 0;

    while(transmittingCameraImage_)
    {
        std::shared_ptr<uint8_t[]> currentFrame = cameraManager->GetFrameDataArray();
        if(!currentFrame)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        std::vector<uint8_t> jpegBuffer;
        jpegBuffer.reserve(250000);
        
        int cameraWidth = cameraManager->GetCameraWidth();
        int cameraHeight = cameraManager->GetCameraHeight();
        
        stbi_write_jpg_to_func(StbiWriteToVectorCallback, &jpegBuffer, cameraWidth, cameraHeight, 3, currentFrame.get(), 40);

        int totalChunks = (jpegBuffer.size() + 1399) / 1400; 
        ImageData chunk;
        chunk.frameId = ++currentFrameId;
        chunk.totalChunks = totalChunks;

        for (int i = 0; i < totalChunks && transmittingCameraImage_; ++i)
        {
            chunk.chunkIndex = i;
            chunk.dataSize = std::min(static_cast<int>(jpegBuffer.size()) - (i * 1400), 1400);
            std::memcpy(chunk.data, jpegBuffer.data() + (i * 1400), chunk.dataSize);

            int transmittedDataSize = sizeof(chunk.frameId) + sizeof(chunk.chunkIndex) + sizeof(chunk.totalChunks) + sizeof(chunk.dataSize) + chunk.dataSize;
            sendto(dataSocket_, &chunk, transmittedDataSize, 0, (sockaddr*)&dataAddress_, sizeof(dataAddress_));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(33)); 
    }
}

void NetworkManager::TransmitCameraImageLowQualityAsync()
{	
	CameraManager* cameraManager = Program::GetInstance()->GetCameraManager();
	if(!cameraManager)
	{
		return;
	}

	dataSocket_ = socket(AF_INET, SOCK_DGRAM, 0);
	dataAddress_.sin_family = AF_INET;
	dataAddress_.sin_port = htons(DATA_PORT);
	dataAddress_.sin_addr.s_addr = clientIP_.s_addr;

	std::cout << "Started sending low-quality camera image." << std::endl;

	transmittingCameraImage_ = true;
	uint32_t currentFrameId = 0;

	while(transmittingCameraImage_)
	{
		std::shared_ptr<uint8_t[]> currentFrame = cameraManager->GetFrameDataArray();

        if(!currentFrame)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        std::vector<uint8_t> jpegBuffer;
        jpegBuffer.reserve(50000);
        
        int cameraWidth = LOW_QUALITY_RESOLUTION_WIDTH;
        int cameraHeight = LOW_QUALITY_RESOLUTION_HEIGHT;
        int colorDepth = LOW_QUALITY_COLOR_DEPTH;

        stbi_write_jpg_to_func(StbiWriteToVectorCallback, &jpegBuffer, cameraWidth, cameraHeight, colorDepth, currentFrame.get(), 60);

        int totalChunks = (jpegBuffer.size() + 1023) / 1024;
        ImageData chunk;
        chunk.frameId = ++currentFrameId;
        chunk.totalChunks = totalChunks;

        for (int i = 0; i < totalChunks && transmittingCameraImage_; ++i)
        {
            chunk.chunkIndex = i;
            chunk.dataSize = std::min(static_cast<int>(jpegBuffer.size()) - (i * 1024), 1024);
            std::memcpy(chunk.data, jpegBuffer.data() + (i * 1024), chunk.dataSize);

            int transmittedDataSize = sizeof(chunk.frameId) + sizeof(chunk.chunkIndex) + sizeof(chunk.totalChunks) + sizeof(chunk.dataSize) + chunk.dataSize;
            sendto(dataSocket_, &chunk, transmittedDataSize, 0, (sockaddr*)&dataAddress_, sizeof(dataAddress_));
            
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }

		std::this_thread::sleep_for(std::chrono::milliseconds(33));
	}
}