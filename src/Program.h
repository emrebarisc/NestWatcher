#pragma once

class CameraManager;
class InputManager;
class IOManager;
class NetworkManager;

class Program
{
	public:
		~Program();

		static Program* GetInstance()
		{
			if(instance_ == nullptr)
			{
				instance_ = new Program();
			}

			return instance_;
		}

		void Init();
		void Start();

		CameraManager* GetCameraManager() const
		{
			return cameraManager_;
		}
		
		InputManager* GetInputManager() const
		{
			return inputManager_;
		}

		IOManager* GetIOManager() const
		{
			return ioManager_;
		}

	protected:

	private:
		Program();
		Program(Program& rhs) = delete;
		Program operator=(Program& rhs) = delete;

		static Program* instance_;

		CameraManager* cameraManager_{ nullptr };
		InputManager* inputManager_{ nullptr };
		IOManager* ioManager_{ nullptr };
		NetworkManager* networkManager_{ nullptr };

		unsigned char pendingClose_ : 1;
};
