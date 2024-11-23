#include "stdafx.h"
#include "PlayerUI.h"

PlayerUI::PlayerUI(PlayState* state) :
	moveButton(1800, 450, 150, 75, "moveButton.png", 0), attackButton(1800, 450, 150, 75, "attackButton.png", 0),
	backButton(1725, 330, 80, 40, "backButton.png", 0), selectedAttack(-100, -100, 100, 100, CColor::Red(), 0),
	hpBar(280, 150, 400, 50, CColor::Red(), 0), manaBar(280, 80, 400, 50, CColor::Blue(), 0),
	hpIcon(40, 150, 48, 48, "", 0), manaIcon(40, 80, 48, 48, "", 0),
	back(960, 540, 1920, 1080, "backUI.png", 0), bigManaIcon(1820, 630, 128, 128, "", 0)
{
	mainButtons.push_back(new CSprite(1800, 750, 200, 100, "moveButton.png", 0));
	mainButtons.push_back(new CSprite(1800, 600, 200, 100, "attackButton.png", 0));

	moveButton.LoadImage("moveButton2.png");
	attackButton.LoadImage("attackButton2.png");
	bigManaIcon.LoadImage("Icons.png", 3, 2, 2, 1);
	bigManaIcon.SetImage("Icons.png", false);

	CanMove(false);
	CanAttack(false);

	queueSprites.push_back(new CSprite(1350, 120, 200, 100, "queueButton.png", 0));
	queueSprites.push_back(new CSprite(900, 290, 600, 150, "queueBack.png", 0));

	hpBar.init();
	hpBar.SetColor(CColor(138, 48, 40));
	hpIcon.LoadImage("Icons.png", 3, 2, 0, 1);
	hpIcon.SetImage("Icons.png", false);
	manaBar.init();
	manaBar.SetColor(CColor(0, 111, 145));
	manaIcon.LoadImage("Icons.png", 3, 2, 2, 1);
	manaIcon.SetImage("Icons.png", false);

	showing = true;
	playState = state;

}

void PlayerUI::SetPlayer(Entity* entity)
{
	player = entity;
	vector<Skill*> skills = player->GetAvaialableSkills();
	for (int i = 0; i < skills.size(); i++) {
		skillSprites.push_back(new CSprite(700 + 200 * i, 120, 128, 128, "", 0));
		skillSprites.back()->LoadImage((skills[i]->name + ".png").data(), "orig");
		skillSprites.back()->LoadImage((skills[i]->name + "_mono.png").data(), "mono");
		skillSprites.back()->SetImage("orig", false);
		skillSprites.push_back(new CSprite(700 + 200 * i, 315, 170, 250, "skillBack.png", 0));

		skillSprites.push_back(new CSpriteText(0, 105, "Pixeloid.TTF", 18, skills[i]->name, CColor::White(), 0));
		skillSprites.back()->SetPos(skillSprites.back()->GetPos() + skillSprites[i * 12 + 1]->GetPos());
		
		skillSprites.push_back(new CSprite(0, 50, 64, 64, (skills[i]->name + ".png").data(), 0)); 
		skillSprites.back()->SetPos(skillSprites.back()->GetPos() + skillSprites[i * 12 + 1]->GetPos());

		skillSprites.push_back(new CSprite(-40, -15, 32, 32, "", 0));
		skillSprites.back()->LoadImage("Icons.png", 3, 2, 1, 1);
		skillSprites.back()->SetImage("Icons.png", false);
		skillSprites.back()->SetPos(skillSprites.back()->GetPos() + skillSprites[i * 12 + 1]->GetPos());

		skillSprites.push_back(new CSpriteText(-40, -45, "Pixeloid.TTF", 16, to_string(skills[i]->damage), CColor::Black(), 0));
		skillSprites.back()->SetPos(skillSprites.back()->GetPos() + skillSprites[i * 12 + 1]->GetPos());

		skillSprites.push_back(new CSprite(40, -15, 32, 32, "", 0));
		skillSprites.back()->LoadImage("Icons.png", 3, 2, 2, 1);
		skillSprites.back()->SetImage("Icons.png", false);
		skillSprites.back()->SetPos(skillSprites.back()->GetPos() + skillSprites[i * 12 + 1]->GetPos());

		skillSprites.push_back(new CSpriteText(40, -45, "Pixeloid.TTF", 16, to_string(skills[i]->manaCost), CColor::Black(), 0));
		skillSprites.back()->SetPos(skillSprites.back()->GetPos() + skillSprites[i * 12 + 1]->GetPos());

		skillSprites.push_back(new CSprite(-40, -80, 32, 32, "", 0));
		skillSprites.back()->LoadImage("Icons.png", 3, 2, 1, 0);
		skillSprites.back()->SetImage("Icons.png", false);
		skillSprites.back()->SetPos(skillSprites.back()->GetPos() + skillSprites[i * 12 + 1]->GetPos());

		skillSprites.push_back(new CSpriteText(-40, -110, "Pixeloid.TTF", 16, to_string(skills[i]->turnCooldown), CColor::Black(), 0));
		skillSprites.back()->SetPos(skillSprites.back()->GetPos() + skillSprites[i * 12 + 1]->GetPos());

		skillSprites.push_back(new CSprite(40, -80, 32, 32, "", 0));
		skillSprites.back()->LoadImage("Icons.png", 3, 2, 2, 0);
		skillSprites.back()->SetImage("Icons.png", false);
		skillSprites.back()->SetPos(skillSprites.back()->GetPos() + skillSprites[i * 12 + 1]->GetPos());

		skillSprites.push_back(new CSpriteText(40, -110, "Pixeloid.TTF", 16, to_string(skills[i]->range), CColor::Black(), 0));
		skillSprites.back()->SetPos(skillSprites.back()->GetPos() + skillSprites[i * 12 + 1]->GetPos());

		attackButtons.push_back(new CSprite(1800, 800-120*i, 96, 96, "", 0));
		attackButtons.back()->LoadImage((player->GetAvaialableSkills()[i]->name + ".png").data(), "orig");
		attackButtons.back()->LoadImage((player->GetAvaialableSkills()[i]->name + "_mono.png").data(), "mono");
		attackButtons.back()->SetImage("orig", false);
	}
	maxHp = player->GetHealth();
	maxMana = player->GetMana();
}

void PlayerUI::CanAttack(bool a)
{
	if (a) {
		attackButton.SetStatus(1);
		attackButton.SetImage("attackButton.png", false);
		manaToUse = player->GetAvaialableSkills()[selectedAttack.GetStatus()]->manaCost;
	}
	else {
		attackButton.SetStatus(0);
		attackButton.SetImage("attackButton2.png", false);
		manaToUse = 0;
	}
}

void PlayerUI::CanMove(bool m, int amountToMove)
{
	if (m) {
		moveButton.SetStatus(1);
		moveButton.SetImage("moveButton.png", false);
		manaToUse = amountToMove;
	}
	else {
		moveButton.SetStatus(0);
		moveButton.SetImage("moveButton2.png", false);
		manaToUse = 0;
	}
}

void PlayerUI::GoMain()
{
	UIState = MAIN;
}

void PlayerUI::GoWait() {
	UIState = WAIT;
}

bool PlayerUI::IsWait()
{
	return UIState == WAIT;
}

void PlayerUI::ShowSkills(bool s)
{
	showing = s;
	UIState = WAIT;
}

void PlayerUI::UpdateUI()
{
	while (queueSprites.size() > 2) {
		queueSprites.erase(queueSprites.begin() + queueSprites.size()-1);
	}
	vector<Skill*>* queue = player->GetQueue();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < queue[i].size(); j++) {
			queueSprites.push_back(new CSprite(700 + 200 * i, 240 + 45 * j, 40, 40, (queue[i][j]->name + ".png").data(), 0));
		}
	}
	vector<Skill*> skills = player->GetAvaialableSkills();
	for (int i = 0; i < skills.size(); i++) {
		if (skills[i] != nullptr) {
			skillSprites[i*12]->SetImage("orig", false);
			attackButtons[i]->SetImage("orig", false);
			attackButtons[i]->SetStatus(0);
		}
	}
	hpBar.SetProgress(player->GetHealth() / maxHp * 100.f);
	manaBar.SetProgress(player->GetMana() / maxMana * 100.f);

}

void PlayerUI::Draw(CGraphics* g)
{
	hpBar.Draw(g);
	manaBar.Draw(g);
	back.Draw(g);
	hpIcon.Draw(g);
	manaIcon.Draw(g);
	*g << font(24);
	*g << xy(500, 138) << player->GetHealth() << "/" << maxHp;
	*g << xy(500, 68) << player->GetMana() << "/" << maxMana;
	if (showing) {
		for (int i = 0; i < skillSprites.size(); i += 12) {
			skillSprites[i]->Draw(g);
			if (skillSprites[i]->GetStatus() == 1) {
				for (int x = i + 1; x <= i + 11; x++) {
					skillSprites[x]->Draw(g);
				}
			}
		}
	}
	queueSprites[0]->Draw(g);
	if (queueSprites[0]->GetStatus() == 1) {
		for (int i = 1; i < queueSprites.size(); i++) {
			queueSprites[i]->Draw(g);
		}
	}
	switch (UIState) {
	case MAIN:
		for (auto but : mainButtons) 
			but->Draw(g);
		break;
	case MOVE:
		bigManaIcon.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(1710, 770) << "Mana used" << xy(1740, 740) << "to move:" <<  xy(1790, 600) << manaToUse;
		moveButton.Draw(g);
		backButton.Draw(g);
		break;
	case ATTACK:
		selectedAttack.Draw(g);
		attackButton.Draw(g);
		for (auto but : attackButtons) {
			but->Draw(g);
		}
		backButton.Draw(g);
		break;
	case WAIT:
		//Write text
		*g << font(30) << color(CColor::Black()) << xy(1710, 600) << "Wait that" << xy(1680, 570) << "move finishes";
		break;
	}
}

void PlayerUI::MouseMove(int x, int y)
{
	for (int i = 0; i < skillSprites.size(); i += 12) {
		skillSprites[i]->SetStatus(0);
		if (skillSprites[i]->HitTest(x, y)) {
			skillSprites[i]->SetStatus(1);
		}
	}
	queueSprites[0]->SetStatus(0);
	if (queueSprites[0]->HitTest(x, y)) {
		queueSprites[0]->SetStatus(1);
	}
}

void PlayerUI::MouseClick(int x, int y)
{
	switch (UIState) {
	case MAIN:
		if (mainButtons[0]->HitTest(x, y)) {
			playState->state = State::MOVE;
			UIState = MOVE;
			buttonSound.Play("button.wav");
		}
		else if (mainButtons[1]->HitTest(x, y)) {
			UIState = ATTACK;
			buttonSound.Play("button.wav");
		}
		break;
	case MOVE:
		if (moveButton.HitTest(x, y) && moveButton.GetStatus() == 1) {
			CanMove(false);
			playState->state = MOVING;
			UIState = WAIT;
			buttonSound.Play("button.wav");
		}
		if (backButton.HitTest(x, y)) {
			CanMove(false);
			playState->state = State::WAIT;
			UIState = MAIN;
			buttonSound.Play("button.wav");
		}
		break;
	case ATTACK:
		for (int i = 0; i < attackButtons.size(); i++) {
			if (attackButtons[i]->GetStatus() != 1 && attackButtons[i]->HitTest(x, y)) {
				switch (i) {
				case 0:
					playState->state = ATTACKING1;
					selectedAttack.SetStatus(0);
					selectedAttack.SetPos(attackButtons[i]->GetPos());
					break;
				case 1:
					playState->state = ATTACKING2;
					selectedAttack.SetStatus(1);
					selectedAttack.SetPos(attackButtons[i]->GetPos());
					break;
				case 2:
					playState->state = ATTACKING3;
					selectedAttack.SetStatus(2);
					selectedAttack.SetPos(attackButtons[i]->GetPos());
					break;
				}
				buttonSound.Play("button.wav");
			}
		}
		if (attackButton.HitTest(x, y) && attackButton.GetStatus() == 1) {
			CanAttack(false);
			playState->state = ATTACKING;
			UIState = WAIT;

			attackButtons[selectedAttack.GetStatus()]->SetStatus(1);
			attackButtons[selectedAttack.GetStatus()]->SetImage("mono", false);
			skillSprites[selectedAttack.GetStatus()*12]->SetImage("mono",false);

			selectedAttack.SetPos(-selectedAttack.GetSize());
			manaToUse = 0;
			buttonSound.Play("button.wav");
		}
		if (backButton.HitTest(x, y)) {
			CanAttack(false);
			playState->state = State::WAIT;
			UIState = MAIN; 
			selectedAttack.SetPos(-selectedAttack.GetSize());
			manaToUse = 0;
			buttonSound.Play("button.wav");
		}
		break;
	case WAIT:
		break;
	}
}
