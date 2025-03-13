#include "IManager.h"

class CameraManager : public IManager
{
	public:
		CameraManager() = default;
		
		void Init() override;
		void Tick();

	protected:

	private:
};
