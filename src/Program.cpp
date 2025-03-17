#include "Program.h"

#include "Managers/CameraManager.h"
#include "Managers/IOManager.h"

Program* Program::instance_ = nullptr;

Program::Program()
{
	if(instance_ == nullptr)
	{
		instance_ = this;
	}

	cameraManager_ = new CameraManager();
	ioManager_ = new IOManager();

	pendingClose_ = false;
}

Program::~Program()
{
	delete ioManager_;
	delete cameraManager_;
}

void Program::Init()
{
	cameraManager_->Init();
	ioManager_->Init();
}

void Program::Start()
{
	cameraManager_->Start();
	while(!pendingClose_);
}
