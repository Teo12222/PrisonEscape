#include "stdafx.h"
#include "AlgorithmMap.h"

AlgorithmMap::AlgorithmMap(int height, int width)
{
	map = new Block * [height];
	for (int i = 0; i < height; i++) {
		map[i] = new Block[width];
	}
	this->height = height;
	this->width = width;
}

AlgorithmMap::AlgorithmMap(int height, int width, Block** m, vector<Entity*> entities) : AlgorithmMap(height, width)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			map[i][j].SetBlock(m[i][j].GetBlock());
		}
	}
	//Create copy
	for (Entity* e : entities){
		order.push_back(e->Copy());
	}
}

set<vector<tuple<int, int, int>>>  AlgorithmMap::moves = set<vector<tuple<int, int, int>>>();

AlgorithmMap::~AlgorithmMap()
{
	for (int i = 0; i < height; i++) {
		delete[] map[i];
	}
	delete[] map;
	for (Entity* e : order) {
		delete e;
	}
	order.clear();
}

AlgorithmMap* AlgorithmMap::CopyMap()
{
	return new AlgorithmMap(height, width, map, order);
}

void AlgorithmMap::MakeMove(vector<tuple<int, int, int>> move)
{
	for (int i = 0; i < move.size(); i++) {
		// If the number in second index is not -1, it means that the entity wants to use a weapon. 
		if (get<2>(move[i]) != -1) {
			// Uses the skill and searches for the entity in the targeted position.
			Skill* skillUsed = order[0]->UseAbility(get<2>(move[i]));
			for (auto e : order) {
				if (e->GetPos() == CVector(get<0>(move[i]), get<1>(move[i]))) {
					// once it found it it deals damage
					e->DealDamage(skillUsed->damage);
				}
			}
		}
		//If the number in second index is -1, it means that the entity is going to move
		else if (get<2>(move[i]) == -1) {
			// Makes sure to remove the block from the previous place and moving it to the new one
			map[(int)order[0]->GetPos().m_y][(int)order[0]->GetPos().m_x].SetBlock(0);
			// And moves the entity by the amount specified in the move
			order[0]->Move(CVector(get<0>(move[i]), get<1>(move[i])));
			map[(int)order[0]->GetPos().m_y][(int)order[0]->GetPos().m_x].SetBlock(1);
		}
	}
	// Move the order to the next player
	order.push_back(order[0]);
	order.erase(order.begin());
	// If it is the player's turn it means that the round ended
	if (order[0]->GetType() == "Player") {
		for (auto e : order) {
			e->ProcessNextRound();
		}
	}
}

void AlgorithmMap::MakePartMove(tuple<int, int, int> m)
{
	if (get<2>(m) != -1) {
		Skill* skillUsed = order[0]->UseAbility(get<2>(m));
		for (auto e : order) {
			if (e->GetPos() == CVector(get<0>(m), get<1>(m))) {
				e->DealDamage(skillUsed->damage);
			}
		}
	}
	else if (get<2>(m) == -1) {
		map[(int)order[0]->GetPos().m_y][(int)order[0]->GetPos().m_x].SetBlock(0);
		order[0]->Move(CVector(get<0>(m), get<1>(m)));
		map[(int)order[0]->GetPos().m_y][(int)order[0]->GetPos().m_x].SetBlock(1);
	}
}

void AlgorithmMap::UnMakePartMove(tuple<int, int, int> m)
{
	if (get<2>(m) != -1) {
		Skill* skillUsed = order[0]->UnUseAbility(get<2>(m));
		for (Entity* e : order) {
			if (e->GetPos() == CVector(get<0>(m), get<1>(m))) {
				e->DealDamage(-skillUsed->damage);
			}
		}
	}
	else if (get<2>(m) == -1) {
		map[(int)order[0]->GetPos().m_y][(int)order[0]->GetPos().m_x].SetBlock(0);
		order[0]->Move(-CVector(get<0>(m),get<1>(m)), true);
		map[(int)order[0]->GetPos().m_y][(int)order[0]->GetPos().m_x].SetBlock(1);
	}
}

float AlgorithmMap::EvaluateMap()
{
	int playerHealth = 0;
	int enemyHealth = 0;
	for (Entity* entity : order) {
		if (entity->GetType() == "Player") {
			playerHealth = entity->GetHealth();
		}
		else {
			enemyHealth += entity->GetHealth();
		}
	}
	int eval = (enemyHealth / (order.size() - 1)) - playerHealth;
	return eval;
}

void AlgorithmMap::GetPossibleMoves(vector<tuple<int, int, int>> currentMove)
{	
	bool skillUsed = false;
	// Checks every skill in the entity
	for (auto skill : order[0]->GetAvaialableSkills()) {
		// If the skill is available
		if (skill != nullptr) {
			// It checks if the skill is in range of any other entity and if the current entity has enough mana to use it
			for (Entity* e : order) {
				if (order[0]->GetMana() >= skill->manaCost && e->GetPos() != order[0]->GetPos()) {
					CreateDijkstraMap();
					int lowestDistance = 100;
					if (int i = GetDistanceAt(e->GetPos().m_x + 1, e->GetPos().m_y); i >= 0 && i < lowestDistance) {
						lowestDistance = i;
					}
					if (int i = GetDistanceAt(e->GetPos().m_x - 1, e->GetPos().m_y); i >= 0 && i < lowestDistance) {
						lowestDistance = i;
					}
					if (int i = GetDistanceAt(e->GetPos().m_x, e->GetPos().m_y + 1); i >= 0 && i < lowestDistance) {
						lowestDistance = i;
					}
					if (int i = GetDistanceAt(e->GetPos().m_x, e->GetPos().m_y - 1); i >= 0 && i < lowestDistance) {
						lowestDistance = i;
					}
					if (lowestDistance != 100 && lowestDistance + 1 <= skill->range) {
						// If it is in range it adds using the move in the current move
						tuple<int, int, int> m((int)e->GetPos().m_x, (int)e->GetPos().m_y, skill->index);
						currentMove.push_back(m);
						// Adds this move to all the possible moves
						moves.insert(currentMove);
						MakePartMove(m);
						// Continues to check if any other moves can be done
						GetPossibleMoves(currentMove);
						// Once it finishes that it undos the move and removes the part move from the current move.
						UnMakePartMove(m);
						currentMove.erase(currentMove.begin() + currentMove.size() - 1);
						skillUsed = true;
						break;
					}
				}
			}
		}
	}
	// The program will then check the amount of mana, and if the entity previously moved, as the entity shouldnt move 2 times in a row
	if (!skillUsed && order[0]->GetMana() > 0 && (currentMove.size() == 0 || (currentMove.size() != 0 && get<2>(currentMove[currentMove.size() - 1]) != -1))) {
		// Then it checks around itself using the amount of mana
		for (int i = order[0]->GetPos().m_y - order[0]->GetMana(); i <= order[0]->GetPos().m_y + order[0]->GetMana(); i++) {
			for (int j = order[0]->GetPos().m_x - order[0]->GetMana(); j <= order[0]->GetPos().m_x + order[0]->GetMana(); j++) {
				CreateDijkstraMap();
				//int x = 0;
				/*if (IsEmptyInMap(j, i)){
					x = map[i][j].GetDistance();
					cout << j << "  " << i << "  " << x<< "   ";
				}
				else {
					cout << -1 << " " << -1 << " " << -1 << "   ";
				}*/
				/*for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						cout << map[y][x].GetDistance() << " ";
					}
					cout << endl;
				}*/
				// Then it checks if it can go there looking at the distance and mana
				if (IsEmptyInMap(j, i) && map[i][j].GetDistance() >= 0 && map[i][j].GetDistance() <= order[0]->GetMana()) {
					// If it can it is going to add the move
					tuple<int, int, int> m(j - order[0]->GetPos().m_x, i - order[0]->GetPos().m_y, -1); 
					// Then it proceeds just like how the skills move is added
					currentMove.push_back(m);
					moves.insert(currentMove);
					MakePartMove(m);
					GetPossibleMoves(currentMove);
					UnMakePartMove(m);
					currentMove.erase(currentMove.begin() + currentMove.size() - 1);
				}
			}
			//cout << endl;
		}
	}
}


EvalAndMove* AlgorithmMap::Minimax(AlgorithmMap* map, int depth, float alpha, float beta)
{
	if (map == nullptr) map = this;

	if (depth == 0 || map->IsOver()) {
		return new EvalAndMove(map->EvaluateMap(), {});
	}
	// Maximising player
	if (map->IsEnemyTurn())
	{
		float maxEval = -INFINITY;
		vector<tuple<int, int, int>> bestMove;
		moves.clear();
		map->GetPossibleMoves(); 
		set<vector<tuple<int, int, int>>> possibleMoves(moves);
		for (vector<tuple<int, int, int>> move : possibleMoves)
		{
			// Copies map and makes the selected move
			AlgorithmMap* mapCopy = map->CopyMap();
			mapCopy->MakeMove(move);
			// Then passes the new map to the next layer
			EvalAndMove* eval = Minimax(mapCopy, depth - 1, alpha, beta);
			maxEval = max(maxEval, eval->GetEval());
			alpha = max(alpha, eval->GetEval());
			if (maxEval == eval->GetEval()) {
				bestMove = move;
			}
			delete eval;
			delete mapCopy;
			if (beta <= alpha)
				break;
		}
		possibleMoves.clear();
		return new EvalAndMove(maxEval, bestMove);
	}
	// Minimising Player
	else
	{
		float minEval = INFINITY;
		vector<tuple<int, int, int>> bestMove;
		moves.clear();
		map->GetPossibleMoves();
		set<vector<tuple<int, int, int>>> possibleMoves(moves);
		for (vector<tuple<int, int, int>> move : possibleMoves)
		{
			AlgorithmMap* mapCopy = map->CopyMap();
			mapCopy->MakeMove(move);
			EvalAndMove* eval = Minimax(mapCopy, depth - 1, alpha, beta);
			minEval = min(minEval, eval->GetEval());
			beta = min(beta, eval->GetEval());
			if (minEval == eval->GetEval()) {
				bestMove = move;
			}
			delete eval;
			delete mapCopy;
			if (beta <= alpha) 
				break;
		}
		possibleMoves.clear();
;		return new EvalAndMove(minEval, bestMove);
	}
}

void AlgorithmMap::UpdateMap(int** m, vector<Entity*> entities)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			map[i][j].SetBlock(m[i][j]);
		}
	}

	order.clear();
	for (auto e : entities) {
		map[(int)e->GetPos().m_y][(int)e->GetPos().m_x].SetBlock(1);
		order.push_back(e);
	}
	CreateDijkstraMap();
	/*for (int i = height-1; i >= 0; i--) {
		for (int j = 0; j < width; j++) {
			if (map[i][j].GetDistance() != -1 && map[i][j].GetDistance() < 10) {
				cout << " " << map[i][j].GetDistance() << " ";
			}
			else cout << map[i][j].GetDistance() << " ";
		}
		cout << endl;
	}*/
}

int AlgorithmMap::GetDistanceAt(int x, int y)
{
	return map[y][x].GetDistance();
}

void AlgorithmMap::MoveEntity(CVector amount)
{
	map[(int)order[0]->GetPos().m_y][(int)order[0]->GetPos().m_x].SetBlock(0);
	order[0]->Move(amount);
	map[(int)order[0]->GetPos().m_y][(int)order[0]->GetPos().m_x].SetBlock(1);
}


void AlgorithmMap::CreateDijkstraMap()
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			map[i][j].SetDistance(-1);
		}
	}
	map[(int)order[0]->GetPos().m_y][(int)order[0]->GetPos().m_x].SetDistance(0);

	DijkstraMapStep(order[0]->GetPos().m_x, order[0]->GetPos().m_y);
}

void AlgorithmMap::DijkstraMapStep(int x, int y)
{
	int create[] = { 0, 0, 0, 0 };
	if (IsEmptyInMap(x + 1, y) && (map[y][x + 1].GetDistance() == -1 || map[y][x].GetDistance() + 1 < map[y][x + 1].GetDistance())) {
		map[y][x + 1].SetDistance(map[y][x].GetDistance() + 1);
		create[0] = 1;
	}
	if (IsEmptyInMap(x - 1, y) && (map[y][x - 1].GetDistance() == -1 || map[y][x].GetDistance() + 1 < map[y][x - 1].GetDistance())) {
		map[y][x - 1].SetDistance(map[y][x].GetDistance() + 1);
		create[1] = 1;
	}
	if (IsEmptyInMap(x, y + 1) && (map[y + 1][x].GetDistance() == -1 || map[y][x].GetDistance() + 1 < map[y + 1][x].GetDistance())) {
		map[y + 1][x].SetDistance(map[y][x].GetDistance() + 1);
		create[2] = 1;
	}
	if (IsEmptyInMap(x, y - 1) && (map[y - 1][x].GetDistance() == -1 || map[y][x].GetDistance() + 1 < map[y - 1][x].GetDistance())) {
		map[y - 1][x].SetDistance(map[y][x].GetDistance() + 1);
		create[3] = 1;
	}

	if (create[0] == 1) DijkstraMapStep(x + 1, y);
	if (create[1] == 1) DijkstraMapStep(x - 1, y);
	if (create[2] == 1)	DijkstraMapStep(x, y + 1);
	if (create[3] == 1)	DijkstraMapStep(x, y - 1);
}

bool AlgorithmMap::IsEmptyInMap(int x, int y)
{
	if (x >= 0 && x < width && y >= 0 && y < height) {
		if (map[y][x].GetBlock() == 0) {
			return true;
		}
	}
	return false;
}

bool AlgorithmMap::IsOver()
{
	int playerHealth = 0;
	int enemyHealth = 0;
	for (Entity* entity : order) {
		if (entity->GetType() == "Player") {
			playerHealth = entity->GetHealth();
		}
		else {
			enemyHealth += entity->GetHealth();
		}
	}
	return enemyHealth <= 0 || playerHealth <= 0;
}

bool AlgorithmMap::IsEnemyTurn()
{
	return order[0]->GetType() == "Enemy";
}
