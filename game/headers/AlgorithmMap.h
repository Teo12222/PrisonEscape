#pragma once
#include "Entity.h"
#include <vector>
#include <set>

class Block
{
public:
	void SetDistance(int dis) { distance = dis; }
	int GetDistance() { return distance; }
	void SetBlock(int i) { occupied = i; }
	int GetBlock() { return occupied; }
private:
	int occupied; //1 there is a wall or an entity, 0 is empty
	int distance = -1;
};


class EvalAndMove {
	vector<tuple<int, int, int>> move;
	float eval;
public:
	EvalAndMove(float e, vector<tuple<int, int, int>> m) {
		move = m;
		eval = e;
	}
	vector<tuple<int, int, int>> GetMove() { return move; }
	float GetEval() { return eval; }
};


class AlgorithmMap
{
public:
	AlgorithmMap(int height, int width);
	AlgorithmMap(int height, int width, Block** m, vector<Entity*> entities);
	~AlgorithmMap();

	EvalAndMove* Minimax(AlgorithmMap* map, int depth, float alpha=-INFINITY, float beta=INFINITY);

	void UpdateMap(int** m, vector<Entity*> entities);
	

	void CreateDijkstraMap();
	int GetDistanceAt(int x, int y);
	void MoveEntity(CVector amount);

	bool IsOver();
	
private:
	AlgorithmMap* CopyMap();

	static set<vector<tuple<int, int, int>>> moves;

	void MakeMove(vector<tuple<int, int, int>> move);
	void MakePartMove(tuple<int, int, int> m);
	void UnMakePartMove(tuple<int, int, int> m);
	float EvaluateMap();

	void GetPossibleMoves(vector<tuple<int, int, int>> currentMove = {});
	bool IsEnemyTurn();

	void DijkstraMapStep(int x, int y);
	bool IsEmptyInMap(int x, int y);


	Block** map;
	vector<Entity*> order;
	int height, width;
};
