#pragma once
#include "State.h"
#include "Entity.h"
#include "SpriteBar.h"

class PlayerUI
{
public:
	PlayerUI(PlayState* state);
	void SetPlayer(Entity* entity);
	void CanAttack(bool a);
	void CanMove(bool m, int amountToMove=0);
	void GoMain();
	void GoWait();
	bool IsWait();

	void ShowSkills(bool s);
	void UpdateUI();

	void Draw(CGraphics* g);
	void MouseMove(int x, int y);
	void MouseClick(int x, int y);
private:
	PlayState* playState;
	enum {MAIN, MOVE, ATTACK, WAIT} UIState;
	Entity* player;

	CSoundPlayer buttonSound;

	int manaToUse;
	vector<CSprite*> mainButtons;
	CSprite moveButton;
	CSprite bigManaIcon;

	CSprite attackButton;
	vector<CSprite*> attackButtons;
	CSpriteRect selectedAttack;
	CSprite backButton;

	CSprite back;
	CSprite hpIcon;
	SpriteBar hpBar;
	float maxHp;
	CSprite manaIcon;
	SpriteBar manaBar;
	float maxMana;

	vector<CSprite*> skillSprites;

	vector<CSprite*> queueSprites;

	bool showing;
};

