#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : menuButton(1850, 950, 64, 64, "menuButton.png", 0)
{
 
}

CMyGame::~CMyGame(void)
{
}

/************** UPDATE ***************/
void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();
	if (playState == PLAY) {
		map->Update(t, GetDeltaTime());
		if (map->IsGameOver()) {
			GameOver();
		}
		if (map->EndCutscne()) {
			SetLevel(1);
		}
		backgroundMusic.Pause();
		if (!map->IsPause()) {
			backgroundMusic.Resume();
		}
	}

}

/************** DRAW ***************/
void CMyGame::OnDraw(CGraphics* g)
{
	*g << font("PIXELOID.TTF");
	if (playState == PLAY) {
		map->Draw(g);
		menuButton.Draw(g);
	}
	else if (playState == MAIN) {
		for (auto s : mainMenu) {
			s->Draw(g);
		}
	}
	else {
		map->Draw(g);
		menuButton.Draw(g);
		for (auto s : endMenu) {
			s->Draw(g);
		}
	}
}


/************** INIT ***************/
void CMyGame::OnInitialize()
{
	// Main Menu
	mainMenu.push_back(new CSprite(GetWidth() / 2, GetHeight() / 2, 1920, 1080, "mainBack.png", GetTime()));
	mainMenu.push_back(new CSprite(1600, 200, 200, 100, "quitButton.png", GetTime())); // Exit
	mainMenu.push_back(new CSprite(500, 400, 200, 100, "level1Button.png", GetTime())); // Level 1
	mainMenu.push_back(new CSprite(800, 400, 200, 100, "level2Button.png", GetTime()));// Level 2
	mainMenu.push_back(new CSprite(1100, 400, 200, 100, "level3Button.png", GetTime())); // Level 3
	mainMenu.push_back(new CSprite(1400, 400, 200, 100, "level4Button.png", GetTime())); // Level 4
	mainMenu.push_back(new CSprite(950, 600, 400, 200, "playButton.png", GetTime())); // Play Button 

	// End Menu
	endMenu.push_back(new CSprite(GetWidth() / 2, GetHeight() / 2, 1920, 1080, "Win2.png", GetTime()));
	endMenu.back()->LoadImage("Win1.png");
	endMenu.back()->LoadImage("Win3.png");
	endMenu.back()->LoadImage("Win4.png");
	endMenu.back()->LoadImage("Lose.png");
	endMenu.push_back(new CSprite(700, 470, 200, 100, "menuButtonOther.png", GetTime()));
	endMenu.push_back(new CSprite(950, 470, 200, 100, "continueButton.png", GetTime()));
	endMenu.back()->LoadImage("restartButton.png");

	backgroundMusic.Play("music.wav", -1);
}

/************** TO CLEAN ***************/
void CMyGame::OnDisplayMenu()
{
	if (map) map->Clean();
	backgroundMusic.Resume();
	playState = MAIN;
}

void CMyGame::OnStartGame()
{
	
}

void CMyGame::OnStartLevel(Sint16 nLevel)
{
	float blockSize = 72;
	CVector offset;
	vector<vector<int>> intMap;
	vector<EntitySprite*> entities;
	switch (nLevel) {
	case 1:
		offset = CVector(72 / 2 + 72 * 3, 72 * 5);
		intMap = { {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
					{1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };

		entities = { new EntitySprite(1, 3, "Player", 16, GetTime(), blockSize, offset, 25, 10, { new Skill(2, 2, 1, 1, 0, "Spear") }, "Player"), new EntitySprite(9, 4, "Guard", 16, GetTime(), blockSize, offset, 10, 5, { new Skill(2, 1, 1, 1, 0, "Sword") }, "Enemy") };
		map = new SpriteMap(intMap, blockSize,offset, entities);
		playState = PLAY;
		endMenu[0]->SetImage("Win1.png", false);
		break;
	case 2:
		offset = CVector(72 / 2 + 72 * 3, 72 * 4);
		intMap = {  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
					{1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1},
					{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
					{1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 3, 1, 1, 1},
					{1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1},
					{1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1},
					{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };
		entities = { new EntitySprite(1, 4, "Player", 16, GetTime(), blockSize, offset, 25, 10, { new Skill(2, 2, 1, 1, 0, "Spear"),  new Skill(3, 3, 1, 1, 1, "Sword") }, "Player"), new EntitySprite(11, 3, "Mage Guard", 14, GetTime(), blockSize, offset, 15, 7, { new Skill(2, 3, 2, 2, 0, "Fireball") }, "Enemy") };
		map = new SpriteMap(intMap, blockSize, offset, entities);
		playState = PLAY;
		endMenu[0]->SetImage("Win2.png", false);
		break;
	case 3:
		offset = CVector(72 / 2 + 72 * 2, 72 * 4);
		intMap = {  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
					{1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1},
					{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0,  0, 0, 0, 1},
					{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
					{1, 0, 0, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 1, 1, 0, 1, 1},
					{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
					{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };
		entities = { new EntitySprite(1, 6, "Player", 16, GetTime(), blockSize, offset, 25, 10, { new Skill(2, 2, 1, 1, 0, "Spear"),  new Skill(3, 3, 1, 1, 1, "Sword"), new Skill(5, 5, 2, 3, 2, "Fireball") }, "Player"), new EntitySprite(14, 3, "Skeleton Shaman", 12, GetTime(), blockSize, offset, 15, 7, { new Skill(2, 1, 2, 1, 0, "Energy Sphere") }, "Enemy") };
		map = new SpriteMap(intMap, blockSize, offset, entities);
		playState = PLAY;
		endMenu[0]->SetImage("Win3.png", false);
		break;
	case 4:
		offset = CVector(72/2 + 72, 72 * 3 + 72 / 2);
		intMap = { {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
					{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 3, 3, 1, 1},
					{1, 0, 0, 1, 1, 3, 3, 1, 1, 3, 3, 3, 3, 1, 1, 0, 0, 0, 1, 1},
					{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
					{1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
					{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };

		entities = { new EntitySprite(1, 5, "Player", 16, GetTime(), blockSize, offset, 25, 10, { new Skill(5, 3, 1, 2, 0, "Energy Spear"),  new Skill(3, 3, 1, 1, 1, "Sword"), new Skill(5, 5, 2, 3, 2, "Fireball") }, "Player"), new EntitySprite(16, 3, "Skeleton Grandmaster", 7, GetTime(),  blockSize, offset, 25, 8, {  new Skill(5, 5, 1, 2, 0, "Energy Sword"), new Skill(2, 1, 3, 1, 1, "Energy Sphere") }, "Enemy") };
		map = new SpriteMap(intMap, blockSize, offset, entities);
		playState = PLAY;
		endMenu[0]->SetImage("Win4.png", false);
		break;
	case 5:
		// add map and player with the spear, Add CreateDummySpear() and in the function, it should create the dummy spear, and make the player walk to it and hide the spear in the UI. Then when it goes on top of it, show the spear in UI and make the player walk out to the right.
		// Maybe when it creates the spear, it should also create explision and bars.
		offset = CVector(72 / 2 + 72, 72 * 3 + 72 / 2);
		intMap = { {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
					{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
					{1, 3, 3, 0, 0, 3, 3, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 1},
					{1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
					{1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
					{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };
		entities = { new EntitySprite(3, 9, "Player", 16, GetTime(), blockSize, offset, 25, 10, { new Skill(2, 2, 1, 1, 0, "Spear") }, "Player") };
		map = new SpriteMap(intMap, blockSize, offset, entities); 
		map->ShowCutscene(GetTime());
		playState = PLAY;
		break;
	}
}

void CMyGame::OnGameOver()
{
	if (map->PlayerWon()) {
		playState = WIN;
		endMenu[2]->SetImage("continueButton.png", false);
	}
		
	else {
		endMenu[0]->SetImage("Lose.png", false);
		endMenu[2]->SetImage("restartButton.png", false);
		playState = LOSE;
	}
}

void CMyGame::OnTerminate()
{
}

/************** KEYBOARD EVENTS ***************/

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	//if (sym == SDLK_SPACE)
		//PauseGame();
	//if (sym == SDLK_F2)
		//NewGame();
	if (map) map->Input(sym);
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/************** MOUSE EVENTS ***************/

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	if (playState == PLAY) map->MouseMove(x, y);
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	switch (playState)
	{
	case MAIN:
		for (int i = 1; i < mainMenu.size(); i++) {
			if (mainMenu[i]->HitTest(x, y)) {
				if (i > 1) {
					SetLevel(i-1);
				}
				else if (i == 1) {
					StopGame();
				}
				buttonSound.Play("button.wav"); 
				buttonSound.Volume(0.5);
			}
		}
		break;
	case PLAY:
		map->MouseClick(x, y);
		if (menuButton.HitTest(x, y)) {
			NewGame();
			buttonSound.Play("button.wav");
		}
		break;
	case WIN:
		for (int i = 1; i < endMenu.size(); i++) {
			if (endMenu[i]->HitTest(x, y)) {
				if (i == 1) {
					NewGame();
				}
				else if (i == 2) {
					if (GetLevel() < 4)
						SetLevel(GetLevel() + 1);
				}
				buttonSound.Play("button.wav");
			}
		}
		break;
	case LOSE:
		for (int i = 1; i < endMenu.size(); i++) {
			if (endMenu[i]->HitTest(x, y)) {
				if (i == 1) {
					NewGame();
				}
				else if (i == 2) {
					StartGame();
				}
				buttonSound.Play("button.wav");
			}
		}
		break;
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
