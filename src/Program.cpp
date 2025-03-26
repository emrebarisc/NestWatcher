#include "Program.h"

#include "Managers/CameraManager.h"
#include "Managers/InputManager.h"
#include "Managers/IOManager.h"
#include "Managers/NetworkManager.h"

Program* Program::instance_ = nullptr;

Program::Program()
{
	if(instance_ == nullptr)
	{
		instance_ = this;
	}

	cameraManager_ = new CameraManager();
	inputManager_ = new InputManager();
	ioManager_ = new IOManager();
	networkManager_ = new NetworkManager();

	pendingClose_ = false;
}

Program::~Program()
{
	delete networkManager_;
	delete ioManager_;
	delete inputManager_;
	delete cameraManager_;
}

void Program::Init()
{
	cameraManager_->Init();
	inputManager_->Init();
	ioManager_->Init();
	networkManager_->Init();
}

void Program::Start()
{
	cameraManager_->Start();

	networkManager_->StartListeningCommandAddress();
	
	while(!pendingClose_);
}
