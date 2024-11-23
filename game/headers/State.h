#pragma once


enum State {
	WAIT,
	MOVE,
	MOVING,
	ATTACKING,
	ATTACKING1,
	ATTACKING2,
	ATTACKING3,
	ATTACK,
	PAUSE,
	WIN,
	LOSE,
	CUTSCENE
};

class PlayState {
public:
	State state;
};