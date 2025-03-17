#pragma once

class CameraManager;
class IOManager;

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

		CameraManager* cameraManager_;
		IOManager* ioManager_;

		unsigned char pendingClose_ : 1;
};
