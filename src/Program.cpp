#include "Program.h"

#include "Managers/CameraManager.h"
#include "Managers/IOManager.h"

Program* instance_ = nullptr;

Program::Program()
{
	if(instance_ == nullptr)
	{
		instance_ = this;
	}

	cameraManager_ = new CameraManager();
	ioManager_ = new IOManager();
}

Program::~Program()
{

}
