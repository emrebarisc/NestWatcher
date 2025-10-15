#include "InputManager.h"

#include <ctime>

#include "Program.h"
#include "Managers/CameraManager.h"
#include "Managers/IOManager.h"
#include "Managers/NetworkManager.h"

InputManager::InputManager()
{
}

void InputManager::Init()
{

}

void InputManager::ExecuteCommand(const CommandMessage& commandMessage)
{
	Program* program = Program::GetInstance();
	CameraManager* cameraManager = program->GetCameraManager();
	IOManager* ioManager = program->GetIOManager();
	NetworkManager* networkManager = program->GetNetworkManager();

	switch(commandMessage.command)
	{
		case Command::None:
		{
			break;
		}
		case Command::Connect:
		{
			break;
		}
		case Command::Disconnect:
		{
			break;
		}
		case Command::StartCamera:
		{
			break;
		}
		case Command::StopCamera:
		{
			break;
		}
		case Command::SetResolution:
		{
			break;
		}
		case Command::SetFPS:
		{
			break;
		}
		case Command::SetPixelFormat:
		{
			break;
		}
		case Command::StartRecording:
		{
			break;
		}
		case Command::PauseRecording:
		{
			break;
		}
		case Command::StopRecording:
		{
			break;
		}
		case Command::TakeAPhotograph:
		{
			std::time_t time = std::time({});
			char timeString[std::size("yyyymmddhhmmss")];
			
			std::strftime(  std::data(timeString), std::size(timeString),
					"20%2y%2m%2d%2H%2M%2S", std::localtime(&time));

			std::string fileName = "Shot_";
			fileName += std::string(timeString) + ".jpg";

			ioManager->SaveFrameJPG(
					cameraManager->GetFrameDataArray(),
					fileName,
					cameraManager->GetCameraWidth(),
					cameraManager->GetCameraHeight(),
					cameraManager->GetColorDepth());

			break;
		}
		case Command::StartStream:
		{
			networkManager->TransmitCameraImage();
			break;
		}
		case Command::StopStream:
		{
			break;
		}
		case Command::Shutdown:
		{
			break;
		}
	}	
}
