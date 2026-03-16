#pragma once
#include "Engine_Define.h"


namespace Client {
	class CMainGame
	{
	private:
		CMainGame();

	public:
		~CMainGame();

	public:
		HRESULT Initalize();
		HRESULT Update(float deltaTime);
		void	Render();
	public:
		static unique_ptr<CMainGame> Create();
	};
}
