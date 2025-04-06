#include "Managers/NetworkManager.h"

#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <thread>

#include "Program.h"
#include "Managers/CameraManager.h"
#include "Managers/InputManager.h"

#include "Network/Command.h"
#include "Network/ImageCommand.h"
#include "Network/ImageData.h"

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

		close(clientSocket);
	}
}


const int WIDTH = 1920;
const int HEIGHT = 1080;
const int CHANNELS = 3;
const int TILE_COLS = 16;
const int TILE_ROWS = 9;
const int TILE_WIDTH = WIDTH / TILE_COLS;
const int TILE_HEIGHT = HEIGHT / TILE_ROWS;
const int TOTAL_PIXELS = WIDTH * HEIGHT * CHANNELS;

unsigned char image[TOTAL_PIXELS];
void GenerateColorPaletteImage() {


    for (int row = 0; row < TILE_ROWS; ++row) {
        for (int col = 0; col < TILE_COLS; ++col) {
            // Color based on position in palette grid (rough HSV-like variation)
            float hue = (float)col / TILE_COLS;
            float brightness = 0.5f + 0.5f * ((float)row / TILE_ROWS);

            unsigned char r = (unsigned char)(255 * brightness * hue);
            unsigned char g = (unsigned char)(255 * brightness * (1.0f - hue));
            unsigned char b = (unsigned char)(255 * (1.0f - brightness));

            for (int y = row * TILE_HEIGHT; y < (row + 1) * TILE_HEIGHT; ++y) {
                for (int x = col * TILE_WIDTH; x < (col + 1) * TILE_WIDTH; ++x) {
                    int idx = (y * WIDTH + x) * CHANNELS;
                    image[idx]     = r;
                    image[idx + 1] = g;
                    image[idx + 2] = b;
                }
            }
        }
    }
}

void NetworkManager::TransmitCameraImageAsync()
{
	GenerateColorPaletteImage();

	std::cout << "NetworkManager::TransmitCameraImageAsync()" << std::endl;

/*	cameraImageCommandSenderSocket_ = socket(AF_INET, SOCK_STREAM, 0);

	cameraImageCommandSenderAddress_.sin_family = AF_INET;
	cameraImageCommandSenderAddress_.sin_port = htons(COMMAND_PORT);
	cameraImageCommandSenderAddress_.sin_addr.s_addr = clientIP_.s_addr;//inet_addr(SERVER_IP);
	
	int commandSocketBindResult = connect(cameraImageCommandSenderSocket_, (const sockaddr*)&cameraImageCommandSenderAddress_, sizeof(cameraImageCommandSenderAddress_)); 
	if(commandSocketBindResult != 0)
	{
		std::cerr << "Failed to connect camera image command  socket with error: " << commandSocketBindResult  << std::endl;
		close(cameraImageCommandSenderSocket_);
		return;
	}
*/
	CameraManager* cameraManager = Program::GetInstance()->GetCameraManager();
	if(!cameraManager)
	{
		return;
	}

	std::shared_ptr<uint8_t[]> currentFrame = cameraManager->GetFrameDataArray();

	int cameraWidth = cameraManager->GetCameraWidth();
	int transmittedDataSize = sizeof(ImageData::rowIndex) + cameraWidth;
	
	int sectionDataSize = cameraWidth;


	ImageData imageData;
	imageData.row = new uint8_t[cameraWidth];

	dataSocket_ = socket(AF_INET, SOCK_DGRAM, 0);
	dataAddress_.sin_family = AF_INET;
	dataAddress_.sin_port = htons(DATA_PORT);
	dataAddress_.sin_addr.s_addr = clientIP_.s_addr;

	while(true)
	{
		ImageCommandMessage imageCommandMessage;
		imageCommandMessage.command = ImageCommand::FrameTransmissionStarted;
		send(cameraImageCommandSenderSocket_, (char*)&imageCommandMessage, sizeof(imageCommandMessage), 0);
		
		std::cerr << "Sending UDP to " << inet_ntoa(dataAddress_.sin_addr) << ":" << ntohs(dataAddress_.sin_port) << std::endl;

		for(int rowIndex = 0; rowIndex < cameraManager->GetCameraHeight(); ++rowIndex)
		{
			imageData.rowIndex = rowIndex;

			for(int sectionIndex = 0; sectionIndex < 3; ++sectionIndex)
			{
				imageData.sectionIndex = sectionIndex;
				//uint8_t* currentFrameRaw = currentFrame.get();
				uint8_t* currentFrameRaw = image;
				
				std::memcpy(imageData.row, (const void*)(currentFrameRaw + sectionDataSize * (rowIndex + sectionIndex)), sectionDataSize);
				
				sendto(dataSocket_, (char*)&imageData, transmittedDataSize, 0, (sockaddr*)&dataAddress_, sizeof(dataAddress_));
			}
		}

		std::cout << "Sent " << sizeof(ImageData::rowIndex) + cameraWidth << " bytes of data" << std::endl;

		//imageCommandMessage.command = ImageCommand::FrameTransmissionEnded;
		//send(cameraImageCommandSenderSocket_, (char*)&imageCommandMessage, sizeof(imageCommandMessage), 0);
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
