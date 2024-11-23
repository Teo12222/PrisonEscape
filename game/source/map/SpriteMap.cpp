#include "stdafx.h"
#include "SpriteMap.h"

SpriteMap::SpriteMap(vector<vector<int>> map, float blockSize, CVector offset, vector<EntitySprite*> entities) : mapClass(map.size(), map[0].size()), 
selectedBlock(-blockSize, -blockSize, blockSize, blockSize, "groundTile2.png", 0), hoveringBlock(-blockSize, -blockSize, blockSize, blockSize, "groundTile1.png", 0),
nextButton(1750, 120, 200, 100, "nextButton.png", 0), pauseButton(1750, 950, 64, 64, "pauseButton.png", 0), pauseScreen(960, 540, 1920, 1080, "pauseMenu.png", 0), playerUI(&state)
{
	for (int i = 0; i < map.size(); i++) {
		spriteMap.push_back({});
		for (int j = 0; j < map[0].size(); j++) {
			if (map[i][j] == 0){
				spriteMap[i].push_back(new CSprite(offset.m_x + (blockSize / 2) + (blockSize * j), offset.m_y + (blockSize / 2) + (blockSize * i), blockSize, blockSize, "groundTile.png", 0));
				spriteMap[i][j]->LoadImage("groundTileSelected.png");
				spriteMap[i][j]->SetStatus(0);
				continue;
			}
			else if (map[i][j] == 3) {
				spriteMap[i].push_back(new CSprite(offset.m_x + (blockSize / 2) + (blockSize * j), offset.m_y + (blockSize / 2) + (blockSize * i), blockSize, blockSize, "barsTile.png", 0));
				spriteMap[i][j]->SetStatus(1);
				continue;
			}
			int nTile = 5;
			if (i > 0 && map[i - 1][j] == 0) nTile -= 3;
			if (i < map.size()-1 && map[i + 1][j] == 0) nTile += 3;
			if (j > 0 && map[i][j - 1] == 0) nTile--;
			if (j < map[0].size()-1 && map[i][j + 1] == 0) nTile++;
			if (nTile == 5 && j > 0 && i > 0 && map[i - 1][j - 1] == 0) nTile = 14;
			if (nTile == 5 && j < map[0].size()-1 && i > 0 && map[i - 1][j + 1] == 0) nTile = 13;
			if (nTile == 5 && j > 0 && i < map.size()-1 && map[i + 1][j - 1] == 0) nTile = 11;
			if (nTile == 5 && j < map[0].size()-1 && i < map.size() - 1 && map[i + 1][j + 1] == 0) nTile = 10;

			nTile--;
			spriteMap[i].push_back(new CSprite(offset.m_x + (blockSize / 2) + (blockSize * j), offset.m_y + (blockSize / 2) + (blockSize * i), blockSize, blockSize, new CGraphics("finalTiles.png", 3, 5, nTile % 3, nTile / 3), 0));
			spriteMap[i][j]->SetStatus(1);
		}
	}
	order = entities;
	playerUI.SetPlayer(order[0]->GetEntity());
	
	footsteps.Play("footstepsStone.wav", -1);
	footsteps.Volume(0.4);
	footsteps.Pause();

	nextButton.LoadImage("nextButton2.png");

	pauseButton.LoadImage("pauseButton2.png");
	UpdateMapClass();
}

void SpriteMap::Draw(CGraphics* g)
{
	for (auto spriteList : spriteMap) {
		for (auto sprite : spriteList) {
			sprite->Draw(g);
		}
	}
	hoveringBlock.Draw(g);
	if (state.state != WAIT)
		selectedBlock.Draw(g);

	for (auto obj : cutsceneObjects) {
		if (!obj->IsDeleted() && !obj->IsDead())
			obj->Draw(g);
	}

	if (proj != nullptr) proj->Draw(g);
	for (auto entity : order) {
		if (!entity->IsDeleted()) entity->Draw(g);
	}
	playerUI.Draw(g);
	pauseButton.Draw(g);
	nextButton.Draw(g);
	r = true;
	if (state.state == PAUSE) pauseScreen.Draw(g);
}

void SpriteMap::Update(long t, long dt)
{
	if (state.state == PAUSE || state.state == WIN || state.state == LOSE) return;
	if (state.state == MOVING ) {
		static CVector startPos(-1, -1);
		if (movingPositions.size() == 1) {
			if (movingPositions[0] != order[0]->GetEntity()->GetPos())
				mapClass.MoveEntity(movingPositions[0] - startPos);
			if (cutsceneObjects.size() == 0) playerUI.UpdateUI();
			startPos = CVector(-1, -1);
			order[0]->SetSpeed(0); 
			order[0]->SetPos(spriteMap[movingPositions[0].m_y][movingPositions[0].m_x]->GetPos());
			spriteMap[movingPositions[0].m_y][movingPositions[0].m_x]->SetImage("groundTile.png", false);
			movingPositions.clear();
			state.state = WAIT;
			playerUI.GoMain();
			NextEnemyMove();
			DeselectBlock();
			mapClass.CreateDijkstraMap();
			if (cutsceneObjects.size() != 0) {
				if (cutsceneObjects[0]->IsDeleted()) {
					state.state = CUTSCENE;
				}
				else {
					cutsceneObjects[0]->Delete();
					MoveTo(18, 5);
					order[0]->GetEntity()->Move(CVector(-20, 0), true);
					playerUI.ShowSkills(true);
				}
			}
			footsteps.Pause();
			return;
 		}
		footsteps.Resume();
		order[0]->SetNormalisedVelocity(movingPositions[1] - movingPositions[0]);
		order[0]->SetSpeed(150); 
		if (Distance(order[0]->GetPos(), spriteMap[movingPositions[0].m_y][movingPositions[0].m_x]->GetPos()) >= spriteMap[0][0]->GetW() * 3 / 4) {
			spriteMap[movingPositions[0].m_y][movingPositions[0].m_x]->SetImage("groundTile.png", false);
		}
		if (Distance(order[0]->GetPos(), spriteMap[movingPositions[0].m_y][movingPositions[0].m_x]->GetPos()) >= spriteMap[0][0]->GetW()) {
			if (startPos == CVector(-1, -1)) startPos = movingPositions[0];
			order[0]->SetPos(spriteMap[movingPositions[1].m_y][movingPositions[1].m_x]->GetPos());
			movingPositions.erase(movingPositions.begin());
		}
	}
	if (state.state == ATTACKING) {
		if (order[0]->GetEntity()->GetAvaialableSkills()[selectedWeapon]->range == 1) {
			order[0]->SetNormalisedVelocity(spriteMap[selectedBlock.GetHealth()][selectedBlock.GetStatus()]->GetPos() - order[0]->GetPos());
			order[0]->SetSpeed(100);
			if (Distance(order[0]->GetPos(), spriteMap[selectedBlock.GetHealth()][selectedBlock.GetStatus()]->GetPos()) <= spriteMap[0][0]->GetW() / 2) {
				order[0]->SetAttack(selectedWeapon);
				if (order[0]->GetCurrentAnimationFrame() == 7) {
					movingPositions = { CVector(selectedBlock.GetStatus(), selectedBlock.GetHealth()), order[0]->GetEntity()->GetPos() };
					if (Attack(selectedWeapon)) return;
					state.state = MOVING;
				}
			}
		}
		else {
			if (order[0]->GetCurrentAnimationFrame() == 7) {
				// create projectcile and throw it towards target;
				order[0]->SetPos(spriteMap[order[0]->GetEntity()->GetPos().m_y][order[0]->GetEntity()->GetPos().m_x]->GetPos());
				proj = new CSprite(order[0]->GetPos() + CVector(0, -16), (order[0]->GetEntity()->GetAvaialableSkills()[selectedWeapon]->name + "Sprite.png").data(), t);
				proj->SetNormalisedVelocity(spriteMap[selectedBlock.GetHealth()][selectedBlock.GetStatus()]->GetPos() - order[0]->GetPos());
				proj->SetSpeed(400);
			}
			if (proj != nullptr) {
				proj->Update(t);
				for (int i = 1; i < order.size(); i++) {
					// deal damage to enemy when hit
					if (order[i]->HitTest(proj)) {
						delete proj; 
						proj = nullptr;
						if (Attack(selectedWeapon)) return;
						playerUI.GoMain();
						state.state = WAIT;
						NextEnemyMove();
						break;
					}
				}
			}
			else {
				order[0]->SetNormalisedVelocity(spriteMap[selectedBlock.GetHealth()][selectedBlock.GetStatus()]->GetPos() - order[0]->GetPos());
				order[0]->SetSpeed(0.1);
				order[0]->SetAttack(selectedWeapon);
			}
			
		}
	}
	if (state.state == WAIT && enemyMove.size() != 0 && r) {
		// do move, and switch state
		// Set ATTACKING to attack set the correct values to selected block
		// Set MOVING to move, and run SetBlocksToTraverse()
		r = false;
		if (get<2>(enemyMove[0]) == -1) {
			CVector movePosition = order[0]->GetEntity()->GetPos() + CVector(get<0>(enemyMove[0]), get<1>(enemyMove[0]));
			/*for (int i = 0; i < spriteMap.size(); i++) {
				for (int j = 0; j < spriteMap[0].size(); j++) {
					cout << mapClass.GetDistanceAt(j, i) << " ";
				}
				cout << endl;
			}*/
			SetBlocksToTraverse(movePosition);
			reverse(movingPositions.begin(), movingPositions.end());
			state.state = MOVING;

		}
		else {
			state.state = ATTACKING;
			selectedWeapon = get<2>(enemyMove[0]);
			selectedBlock.SetStatus(get<0>(enemyMove[0]));
			selectedBlock.SetHealth(get<1>(enemyMove[0]));
		}
	}
	if (playerUI.IsWait()) {
		nextButton.SetStatus(0);
		nextButton.SetImage("nextButton2.png", false);
	}
	else {
		nextButton.SetStatus(1);
		nextButton.SetImage("nextButton.png", false);
	}
	selectedBlock.Update(t);
	hoveringBlock.Update(t);
	for (auto entity : order) {
		entity->Update(t); 
	}
	if (cutsceneObjects.size() != 0) cutsceneObjects[1]->Update(t);
}

void SpriteMap::MouseMove(int x, int y)
{
	if (state.state == PAUSE || state.state == WIN || state.state == LOSE) return;
	if (hoveringBlock.GetStatus() != -1) {
		for (int i = 0; i < spriteMap.size(); i++) {
			for (int j = 0; j < spriteMap[i].size(); j++) {
				if (spriteMap[i][j]->HitTest(x, y) && spriteMap[i][j]->GetStatus() == 0 && (j != hoveringBlock.GetStatus() || i != hoveringBlock.GetHealth())) {
					hoveringBlock.SetPos(spriteMap[i][j]->GetPos());
					hoveringBlock.SetStatus(j); // x
					hoveringBlock.SetHealth(i); // y
					for (auto entity : order) {
						entity->HideInformation();
						if (entity->GetPos() == spriteMap[i][j]->GetPos()) {
							if (state.state == MOVE || state.state == WAIT)
								entity->ShowInformation();
							
						}
					}
					if (state.state == MOVE) {
						for (auto pos : movingPositions) {
							spriteMap[pos.m_y][pos.m_x]->SetImage("groundTile.png",false);
						}
						movingPositions.clear();
						SetBlocksToTraverse(CVector(j, i));
						reverse(movingPositions.begin(), movingPositions.end());
						for (auto pos : movingPositions) {
							spriteMap[pos.m_y][pos.m_x]->SetImage("groundTileSelected.png", false);
						}
					}
					return;
				}
			}
		}
	}
	playerUI.MouseMove(x, y);
}

void SpriteMap::MouseClick(int x, int y)
{
	if (state.state == WIN || state.state == LOSE) return;
	if (pauseButton.HitTest(x, y)) {
		if (state.state == PAUSE) {
			state.state = static_cast<State>(pauseButton.GetStatus());
			pauseButton.SetImage("pauseButton.png", false);
			buttonSound.Play("button.wav");
		}
		else {
			pauseButton.SetImage("pauseButton2.png", false);
			pauseButton.SetStatus(state.state);
			state.state = PAUSE;
			buttonSound.Play("button.wav"); 
			footsteps.Pause();
			return;
		}
	}
	if (state.state == PAUSE) return;
	if ((state.state == MOVE || state.state == ATTACK) && hoveringBlock.GetStatus() != -1) {
		for (int i = 0; i < spriteMap.size(); i++) {
			for (int j = 0; j < spriteMap[i].size(); j++) {
				if (spriteMap[i][j]->HitTest(x, y) && spriteMap[i][j]->GetStatus() == 0 && (j != selectedBlock.GetStatus() || i != selectedBlock.GetHealth())) {
					
					bool attack = false;
					for (int index = 0; index < order.size(); index++) {
						if (order[index]->GetPos() == spriteMap[i][j]->GetPos()) {
							if (state.state == ATTACK && order[0]->GetEntity()->GetMana() >= order[0]->GetEntity()->GetAvaialableSkills()[selectedWeapon]->manaCost) {// and if this entity is in range 
								if (order[0]->GetPos() != order[index]->GetPos()) {
									for (CVector pos : weaponRangePositions) {
										if (CVector(j, i) == pos) {
											playerUI.CanAttack(true);
											attack = true;
											break;
										}
									}
								}
							}
							else return;
						}
					}
					if (state.state != ATTACK || attack) {
						if (state.state == MOVE) {
							if (order[0]->GetEntity()->GetMana() >= movingPositions.size() - 1) {
								playerUI.CanMove(true, movingPositions.size() - 1);
							}
							else return;
						}
						
						selectedBlock.SetPos(spriteMap[i][j]->GetPos());
						selectedBlock.SetStatus(j); // x
						selectedBlock.SetHealth(i); // y
						hoveringBlock.SetStatus(-1);
					}
					return;
				}
			}
		}
	}
	State oldState = state.state;
	playerUI.MouseClick(x, y);
	if (oldState == WAIT || oldState == ATTACK) {
		switch (state.state) {
		case ATTACKING1:
			selectedWeapon = 0;
			ShowWeaponRange(0);
			state.state = ATTACK;
			break;
		case ATTACKING2:
			selectedWeapon = 1;
			ShowWeaponRange(1);
			state.state = ATTACK;
			break;
		case ATTACKING3:
			selectedWeapon = 2;
			ShowWeaponRange(2);
			state.state = ATTACK;
			break;
		}
	}
	if (oldState == ATTACK && state.state == ATTACKING) {
		HideWeaponRange();
	}
	if (oldState != WAIT && state.state == WAIT) {
		HideWeaponRange();
		HideMovingPositions();
		DeselectBlock();
	}
	if (nextButton.GetStatus() == 1 && nextButton.HitTest(x, y)) {
		HideWeaponRange();
		HideMovingPositions();
		DeselectBlock();
		playerUI.CanAttack(false);
		playerUI.CanMove(false);
		buttonSound.Play("button.wav");
		NextTurn();
	}
}

void SpriteMap::Input(SDLKey sym)
{
	if ((state.state == MOVE || state.state == ATTACK) && sym == SDLK_ESCAPE) {
		hoveringBlock.SetStatus(0);
		selectedBlock.SetStatus(0);
		selectedBlock.SetPos(-selectedBlock.GetH(), -selectedBlock.GetW());
		playerUI.CanMove(false);
		playerUI.CanAttack(false);
	}
}


bool SpriteMap::EndCutscne()
{
	return state.state == CUTSCENE;
}

bool SpriteMap::IsGameOver()
{
	return state.state == WIN || state.state == LOSE;
}

bool SpriteMap::IsPause()
{
	return state.state == PAUSE;
}

bool SpriteMap::PlayerWon()
{
	return state.state == WIN;

}

void SpriteMap::ShowCutscene(long t)
{
	playerUI.ShowSkills(false);
	cutsceneObjects.push_back(new CSprite(1225, 355, 64, 64, "SpearSprite.png", 0));
	cutsceneObjects.push_back(new CSprite(400, 800, 300, 300, "", t));
	cutsceneObjects.back()->AddImage("explosion.png", 5, 5, 0, 0, 4, 4, CColor::Black());
	cutsceneObjects.back()->SetAnimation("explosion.png", 25);
	cutsceneObjects.back()->SetSize(300, 300);
	cutsceneObjects.back()->Die(900);
	MoveTo(15, 1);
}

void SpriteMap::Clean()
{
	footsteps.Pause();
	for (int i = 0; i < order.size(); i++) {
		delete order[i];
	}
	for (int i = 0; i < spriteMap.size(); i++) {
		for (int j = 0; j < spriteMap[i].size(); j++) {
			delete spriteMap[i][j];
		}
	}

}

void SpriteMap::MoveTo(int x, int y)
{
	SetBlocksToTraverse(CVector(x, y));
	reverse(movingPositions.begin(), movingPositions.end());
	state.state = MOVING;
}

void SpriteMap::UpdateMapClass()
{
	int** intMap = new int*[spriteMap.size()];
	for (int i = 0; i < spriteMap.size(); i++) {
		intMap[i] = new int[spriteMap[0].size()];
		for (int j = 0; j < spriteMap[0].size(); j++) {
			intMap[i][j] = spriteMap[i][j]->GetStatus();
		}
	}
	vector<Entity*> entities;
	for (auto e : order) {
		entities.push_back(e->GetEntity());
	}
	mapClass.UpdateMap(intMap, entities);


}

void SpriteMap::SetBlocksToTraverse(CVector dest)
{
	movingPositions.push_back(dest);
	if (mapClass.GetDistanceAt(dest.m_x, dest.m_y) == 0) {
		return;
	}
	if (mapClass.GetDistanceAt(dest.m_x, dest.m_y - 1) == mapClass.GetDistanceAt(dest.m_x, dest.m_y) - 1) {
		SetBlocksToTraverse(CVector(dest.m_x, dest.m_y - 1));
	}
	else if (mapClass.GetDistanceAt(dest.m_x, dest.m_y + 1) == mapClass.GetDistanceAt(dest.m_x, dest.m_y) - 1) {
		SetBlocksToTraverse(CVector(dest.m_x, dest.m_y + 1));
	}
	else if (mapClass.GetDistanceAt(dest.m_x - 1, dest.m_y) == mapClass.GetDistanceAt(dest.m_x, dest.m_y) - 1) {
		SetBlocksToTraverse(CVector(dest.m_x - 1, dest.m_y));
	}
	else if (mapClass.GetDistanceAt(dest.m_x + 1, dest.m_y) == mapClass.GetDistanceAt(dest.m_x, dest.m_y) - 1) {
		SetBlocksToTraverse(CVector(dest.m_x + 1, dest.m_y));
	}
}

void SpriteMap::HideMovingPositions()
{
	for (auto pos : movingPositions) {
		spriteMap[pos.m_y][pos.m_x]->SetImage("groundTile.png", false);
	}
	movingPositions.clear();
}

void SpriteMap::DeselectBlock()
{
	hoveringBlock.SetStatus(0);
	selectedBlock.SetStatus(0);
	selectedBlock.SetPos(-100, -100);
}

void SpriteMap::ShowWeaponRange(int index)
{
	HideWeaponRange();
	weaponRangePositions.clear();
	for (int i = 0; i < spriteMap.size(); i++) {
		for (int j = 0; j < spriteMap[i].size(); j++) {
			if (mapClass.GetDistanceAt(j, i) >= 0 && mapClass.GetDistanceAt(j, i) <= order[0]->GetEntity()->GetAvaialableSkills()[index]->range) {
				spriteMap[i][j]->SetImage("groundTileSelected.png", false);
				weaponRangePositions.push_back(CVector(j, i));
			}
		}
	}
	// Check if enemies are close and highlight ground
	for (EntitySprite* e : order) {
		if (e->GetEntity()->GetPos() != order[0]->GetEntity()->GetPos()) {
			int lowestDistance = 100;
			if (int i = mapClass.GetDistanceAt(e->GetEntity()->GetPos().m_x + 1, e->GetEntity()->GetPos().m_y); i >= 0 && i < lowestDistance) {
				lowestDistance = i;
			}
			if (int i = mapClass.GetDistanceAt(e->GetEntity()->GetPos().m_x - 1, e->GetEntity()->GetPos().m_y); i >= 0 && i < lowestDistance) {
				lowestDistance = i;
			}
			if (int i = mapClass.GetDistanceAt(e->GetEntity()->GetPos().m_x, e->GetEntity()->GetPos().m_y + 1); i >= 0 && i < lowestDistance) {
				lowestDistance = i;
			}
			if (int i = mapClass.GetDistanceAt(e->GetEntity()->GetPos().m_x, e->GetEntity()->GetPos().m_y - 1); i >= 0 && i < lowestDistance) {
				lowestDistance = i;
			}
			if (lowestDistance != 100 && lowestDistance + 1 <= order[0]->GetEntity()->GetAvaialableSkills()[index]->range) {
				spriteMap[e->GetEntity()->GetPos().m_y][e->GetEntity()->GetPos().m_x]->SetImage("groundTileSelected.png", false);
				weaponRangePositions.push_back(CVector(e->GetEntity()->GetPos().m_x, e->GetEntity()->GetPos().m_y));
			}
		}
	}
}

void SpriteMap::HideWeaponRange()
{
	for (auto pos : weaponRangePositions) {
		spriteMap[pos.m_y][pos.m_x]->SetImage("groundTile.png", false);
	}
}

bool SpriteMap::Attack(int index)
{
	// Use the weapon at index, run animation and deal damage
	Skill* s = order[0]->GetEntity()->UseAbility(index); 
	for (int i = 1; i < order.size(); i++) {
		if (order[i]->GetEntity()->GetPos() == CVector(selectedBlock.GetStatus(), selectedBlock.GetHealth())); {
			hitSound.Play("hit.wav");
			order[i]->GetEntity()->DealDamage(s->damage);
			if (order[i]->GetEntity()->GetHealth() <= 0) order[i]->Delete();
		}
	}
	playerUI.UpdateUI();
	DeselectBlock();
	return CheckForEnd();
}

void SpriteMap::NextTurn()
{
	if (!r) return;
	order.push_back(order[0]);
	order.erase(order.begin()); 
	if (order[0]->GetEntity()->GetType() == "Player") {
		for (auto e : order) {
			e->GetEntity()->ProcessNextRound();
		}
		playerUI.UpdateUI();
		UpdateMapClass();
	}
	else {
		UpdateMapClass();
		enemyMove = mapClass.Minimax(nullptr, 3)->GetMove();
		mapClass.CreateDijkstraMap();
		state.state = WAIT;
		r = false;
		playerUI.GoWait();
	}
}

void SpriteMap::NextEnemyMove()
{
	if (enemyMove.size() != 0) {
		enemyMove.erase(enemyMove.begin());
		playerUI.GoWait();
		if (enemyMove.size() == 0) {
			NextTurn();
			playerUI.GoMain();
		}
	}
}

bool SpriteMap::CheckForEnd()
{
	if (mapClass.IsOver()) {
		if (order[0]->GetEntity()->GetType() == "Player") {
			state.state = WIN;
		}
		else {
			state.state = LOSE;
		}
		return true;
	}
}

