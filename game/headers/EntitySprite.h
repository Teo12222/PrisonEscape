#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "SpriteBar.h"

class EntitySprite : public CSprite
{
public:
	EntitySprite(Sint16 x, Sint16 y, string pFileBitmap, int fontSize, Uint32 time, float blockSize, CVector offset, float maxHealth, float maxMana, vector<Skill*> skills, string type);
	enum PlayerStatus { IDLE, WALK, ATTACK};
	void SetAttack(int index);
	void ShowInformation();
	void HideInformation();

	void Update(long t);
	void Draw(CGraphics* g);
	Entity* GetEntity();
private:
	void PlayAnimation();

	float weapon;
	PlayerStatus newStatus;
	enum { UP, DOWN, RIGHT, LEFT} playerDirection;
	Entity entity;

	// Information
	CSprite back;
	CSpriteText name;
	CSprite model;
	SpriteBar hpBar;
	CSprite hpIcon;
	float maxHp;
	SpriteBar manaBar;
	CSprite manaIcon;
	float maxMp;
	vector<CSprite*> skillSprites;

	bool isShowing;
};

