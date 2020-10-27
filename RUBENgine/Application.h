#pragma once
namespace RUBENgine
{
	class Application
	{
	public:
		void Initialize(const char* windowTitle, const int windowWidth, const int windowHeight);
		void Cleanup();
		void Run();
	private:
		static const float m_SecPerFrame; //0.016 for 60 fps, 0.033 for 30 fps
	};
}