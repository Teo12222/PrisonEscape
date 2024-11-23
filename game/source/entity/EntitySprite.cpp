#include "stdafx.h"
#include "EntitySprite.h"

EntitySprite::EntitySprite(Sint16 x, Sint16 y, string pFileBitmap, int fontSize, Uint32 time, float blockSize, CVector offset, float maxHealth, float maxMana, vector<Skill*> skills, string type)
	: CSprite(offset.m_x + (blockSize / 2) + (x * blockSize), offset.m_y + (blockSize / 2) + (y * blockSize), 0, 0, time),
	entity(maxHealth, maxMana, skills, CVector(x, y), type), // CVector(x, y) needs to be translated to grid coordinates, 
	back(0, 0, 170, 150, "informationBack.png", time), name(0, 60, "Pixeloid.TTF", fontSize, pFileBitmap, CColor::White(), time),
	hpBar(-28, 35, 70, 10, CColor::Red(), time), manaBar(-28, 15, 70, 10, CColor::LightBlue(), time),
	hpIcon(-73, 35, 16, 16, "", time), manaIcon(-73, 15, 16, 16, "", time)
{
	LoadImage((pFileBitmap + "Walk.png").data(), "IdleRight", 9, 4, 0, 0, CColor::Black());
	LoadImage((pFileBitmap + "Walk.png").data(), "IdleLeft", 9, 4, 0, 2, CColor::Black());
	LoadImage((pFileBitmap + "Walk.png").data(), "IdleUp", 9, 4, 0, 3, CColor::Black());
	LoadImage((pFileBitmap + "Walk.png").data(), "IdleDown", 9, 4, 0, 1, CColor::Black());
	AddImage((pFileBitmap + "Walk.png").data(), "WalkRight", 9, 4, 0, 0, 9, 0, CColor::Black());
	AddImage((pFileBitmap + "Walk.png").data(), "WalkLeft", 9, 4, 0, 2, 9, 2, CColor::Black());
	AddImage((pFileBitmap + "Walk.png").data(), "WalkUp", 9, 4, 0, 3, 9, 3, CColor::Black());
	AddImage((pFileBitmap + "Walk.png").data(), "WalkDown", 9, 4, 0, 1, 9, 1, CColor::Black());

	for (int i = 0; i < skills.size(); i++) {
		AddImage((pFileBitmap + skills[i]->name + ".png").data(), ("Attack" +  to_string(i) + "Right").data(), 8, 4, 0, 0, 8, 0, CColor::Black());
		AddImage((pFileBitmap + skills[i]->name + ".png").data(), ("Attack" + to_string(i) + "Left").data(), 8, 4, 0, 2, 8, 2, CColor::Black());
		AddImage((pFileBitmap + skills[i]->name + ".png").data(), ("Attack" + to_string(i) + "Up").data(), 8, 4, 0, 3, 8, 3, CColor::Black());
		AddImage((pFileBitmap + skills[i]->name + ".png").data(), ("Attack" + to_string(i) + "Down").data(), 8, 4, 0, 1, 8, 1, CColor::Black());
	}

	newStatus = IDLE;
	SetStatus(WALK);
	if (pFileBitmap == "Player") playerDirection = RIGHT;
	else playerDirection = LEFT;

	model.LoadImage((pFileBitmap + "Walk.png").data(), "Model", 9, 4, 0, 1, CColor::Black());
	model.SetImage("Model");
	model.SetSize(model.GetSize()*3/4);
	model.SetPos(65, 30);
	hpBar.init();
	hpBar.SetColor(CColor(138, 48, 40));
	hpIcon.LoadImage("Icons.png", 3, 2, 0, 1);
	hpIcon.SetImage("Icons.png", false);
	manaBar.init();
	manaBar.SetColor(CColor(0, 111, 145));
	manaIcon.LoadImage("Icons.png", 3, 2, 2, 1);
	manaIcon.SetImage("Icons.png", false);
	maxHp = entity.GetHealth();
	maxMp = entity.GetMana();

	vector<Skill*> entitySkills = entity.GetAvaialableSkills();
	for (int i = 0; i < entitySkills.size(); i++) {
		skillSprites.push_back(new CSprite(-55 + 55 * i, -35, 48, 48, "", 0));
		skillSprites.back()->LoadImage((entitySkills[i]->name + ".png").data(), "orig");
		skillSprites.back()->LoadImage((entitySkills[i]->name + "_mono.png").data(), "mono");
		skillSprites.back()->SetImage("orig", false);
	}
}


void EntitySprite::SetAttack(int index)
{
	newStatus = ATTACK;
	weapon = index;
}

void EntitySprite::ShowInformation()
{
	CVector pos = back.GetPos();
	back.SetPos(GetPos());
	CVector change = back.GetPos() - pos;
	name.SetPos(name.GetPos() + change);
	model.SetPos(model.GetPos() + change);
	hpBar.SetPos(hpBar.GetPos() + change);
	manaBar.SetPos(manaBar.GetPos() + change);
	hpBar.SetProgress(entity.GetHealth() / maxHp * 100.f);
	manaBar.SetProgress(entity.GetMana() / maxMp * 100.f);
	hpIcon.SetPos(hpIcon.GetPos() + change);
	manaIcon.SetPos(manaIcon.GetPos() + change);
	vector<Skill*> skills = entity.GetAvaialableSkills();
	for (int i = 0; i < skills.size(); i++) {
		if (skills[i] != nullptr) {
			skillSprites[i]->SetImage("orig", false);
		}
		else {
			skillSprites[i]->SetImage("mono", false);
		}
		skillSprites[i]->SetPos(skillSprites[i]->GetPos() + change);
	}
	isShowing = true;
}

void EntitySprite::HideInformation() {
	isShowing = false;
}


void EntitySprite::Update(long t)
{
	CVector v = GetVelocity();
	if (v.m_x > 0.01f) {
		if (newStatus != ATTACK) newStatus = WALK;
		playerDirection = RIGHT;
	}
	else if (v.m_x < -0.01f) {
		if (newStatus != ATTACK) newStatus = WALK;
		playerDirection = LEFT;
	}
	else if (v.m_y > 0.01f) {
		if (newStatus != ATTACK) newStatus = WALK;
		playerDirection = UP;
	}
	else if (v.m_y < -0.01f) {
		if (newStatus != ATTACK) newStatus = WALK;
		playerDirection = DOWN;
	}
	else if (GetCurrentAnimationFrame() == 8) {
		newStatus = IDLE;
	}
	if (newStatus == ATTACK) {
		SetSpeed(0);
		if (GetCurrentAnimationFrame() == 7) {
			newStatus = IDLE;
		}
	}

	PlayAnimation();
	CSprite::Update(t);
}

void EntitySprite::Draw(CGraphics* g)
{
	CSprite::Draw(g); 
	if (isShowing) {
		back.Draw(g);
		name.Draw(g);
		model.Draw(g);
		hpBar.Draw(g);
		hpIcon.Draw(g);
		manaIcon.Draw(g);
		// Add health bar text
		*g << xy(hpBar.GetPos().m_x + 75, hpBar.GetPos().m_y-5) << font(12)  << entity.GetHealth() << "/" << maxHp;
		manaBar.Draw(g);
		// Add mana bar text
		*g << xy(manaBar.GetPos().m_x + 75, manaBar.GetPos().m_y-5) << entity.GetMana() << "/" << maxMp;
		for (CSprite* skill : skillSprites) {
			skill->Draw(g);
		}
	}
}

Entity* EntitySprite::GetEntity()
{
	return &entity;
}


void EntitySprite::PlayAnimation()
{
	if (newStatus != GetStatus() || playerDirection != GetHealth()) {
		SetStatus(newStatus);
		SetHealth(playerDirection);
	}
	else return;

	string alias = "";
	switch (playerDirection)
	{
	case RIGHT:
		alias = "Right";
		break;
	case LEFT:
		alias = "Left";
		break;
	case UP:
		alias = "Up";
		break;
	case DOWN:
		alias = "Down";
		break;
	}


	switch (newStatus)
	{
	case IDLE:
		alias = "Idle" + alias;
		SetImage(alias.data());
		break;
	case WALK:
		alias = "Walk" + alias;
		SetAnimation(alias.data(), 12);
		break;
	case ATTACK:
		alias = "Attack" + to_string(entity.GetAvaialableSkills()[weapon]->index) + alias;
		SetAnimation(alias.data(), 12);
		break;
	}

}
