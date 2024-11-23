#pragma once
#include "EntitySprite.h"
#include "AlgorithmMap.h"
#include "PlayerUI.h"

class SpriteMap
{
public:
	SpriteMap(vector<vector<int>> map, float blockSize, CVector offSet, vector<EntitySprite*> entities);
	void Draw(CGraphics* g);
	void Update(long t, long dt);
	void MouseMove(int x, int y);
	void MouseClick(int x, int y);
	void Input(SDLKey sym);

	bool EndCutscne();
	bool IsGameOver();
	bool IsPause();
	bool PlayerWon();

	void ShowCutscene(long t);

	void Clean();
private:
	void UpdateMapClass();
	void SetBlocksToTraverse(CVector dest);

	void HideMovingPositions();
	void DeselectBlock();

	void ShowWeaponRange(int index);
	void HideWeaponRange();
	bool Attack(int index);

	void NextTurn(); 
	void NextEnemyMove();
	bool CheckForEnd();
	
	void MoveTo(int x, int y);

	vector<vector<CSprite*>> spriteMap;
	CSprite selectedBlock;
	CSprite hoveringBlock;

	vector<CSprite*> cutsceneObjects;

	vector<CVector> movingPositions;
	vector<CVector> weaponRangePositions;
	float selectedWeapon;
	CSprite* proj;

	bool r;

	PlayState state;
	PlayerUI playerUI;
	CSprite nextButton;
	CSprite pauseButton;
	CSprite pauseScreen;

	CSoundPlayer footsteps;
	CSoundPlayer hitSound;
	CSoundPlayer buttonSound;

	vector<EntitySprite*> order;
	AlgorithmMap mapClass;
	vector<vector<tuple<int, int, int>>> enemyMoves;
	vector<tuple<int, int, int>> enemyMove;
};

