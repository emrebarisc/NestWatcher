#include "Program.h"

int main(void)
{
	Program* program = Program::GetInstance();
	program->Init();
	program->Start();

	while(true);

	return 0;
}
