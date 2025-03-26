#pragma once

#include "IManager.h"

#include "Command.h"

class InputManager : public IManager
{
public:
	InputManager();
	virtual void Init() override;

	void ExecuteCommand(const CommandMessage& commandMessage);

protected:

private:

};
