#include "stdafx.h"
#include "MyGame.h"


CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	//app.DebugMemoryLeaks();
	app.OpenWindow(1920, 1080, "Prison Escape");
	app.OpenFullScreen(1920, 1080, 32);
	//app.OpenConsole();
	app.SetClearColor(CColor::Black());
	app.Run(&game);
	return(0);
}
