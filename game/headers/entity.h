#pragma once
#include "Vector.h"


struct Skill
{
	//public:
	Skill(int skillDamage, int skillMana, int skillRange, int cooldown, int i, string name) {
		damage = skillDamage;
		manaCost = skillMana;
		range = skillRange;
		turnCooldown = cooldown;
		index = i;
		this->name = name;
	}
	int damage;
	int manaCost;
	int range;
	int turnCooldown;
	int index;
	string name;
};


class Entity
{
public:
	Entity(float maxHealth, float maxMana, vector<Skill*> skills, CVector initialPos, string t);
	Entity(float health, float mana, float maxMana, vector<Skill*> skills, CVector pos, string t, vector<Skill*>* queue);
	~Entity();

	// getters
	string GetType();
	float GetHealth();
	float GetMana();
	CVector GetPos();
	vector<Skill*> GetAvaialableSkills();
	vector<Skill*>* GetQueue();

	void Move(CVector amount, bool reAdd=false);
	// return the ability used, so it can be processed in Map
	Skill* UseAbility(int index);
	Skill* UnUseAbility(int index);
	void ProcessNextRound();
	void DealDamage(float damage);
	Entity* Copy();
protected:
	float health;
	float mana;
	float manaPerRound;
	vector<Skill*> availableSkills;
	vector<Skill*>*  skillQueue; // 2D array where skills are placed in here depending on their cooldown and are moved down every turn
	CVector pos;
	string type;
};