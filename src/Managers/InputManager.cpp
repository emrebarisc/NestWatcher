#include "InputManager.h"

#include <ctime>

#include "Program.h"
#include "Managers/CameraManager.h"
#include "Managers/IOManager.h"

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
			fileName += timeString;
			std::cout << timeString << std::endl;

			/*ioManager->SaveFrameToPNG(
					cameraManager->GetFrameDataArray(),
					fileName,
					cameraManager->GetCameraWidth(),
					cameraManager->GetCameraHeight());*/
			break;
		}
		case Command::StartStream:
		{
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
