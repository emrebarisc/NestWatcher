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

		static Program* instance_;

		CameraManager* cameraManager_;
		IOManager* ioManager_;
};
