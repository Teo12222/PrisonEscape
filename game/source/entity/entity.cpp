#include "stdafx.h"
#include "Entity.h"


Entity::Entity(float maxHealth, float maxMana, vector<Skill*> skills, CVector initialPos, string t)
{
    health = maxHealth;
    manaPerRound = maxMana;
    mana = maxMana;
    availableSkills = skills;
    pos = initialPos;
    type = t;

    //Initialize skill queue TODO
    skillQueue = new vector<Skill*>[3];
    for (int i = 0; i < 3; i++) {
        skillQueue[i] = {};
    }
}

Entity::Entity(float health, float mana, float maxMana, vector<Skill*> skills, CVector pos, string t, vector<Skill*>* queue) :
    Entity(health, maxMana, skills, pos, t)
{
    this->mana = mana;
    for (int i = 0; i < 3; i++) {
        skillQueue[i] = queue[i];
    }
}

Entity::~Entity()
{
    /*for (Skill* s : availableSkills) {
        if (s->index >= 0 && s->index < 3)
            delete s;
    }*/
    availableSkills.clear();

   /* for (int i = 0; i < 3; i++) {
        for (int j = 0; j < skillQueue[i].size(); j++) {
            if (skillQueue[i][j]->index >= 0 && skillQueue[i][j]->index < 3)
                delete skillQueue[i][j];
        }
        skillQueue[i].clear();
    }*/
    delete[] skillQueue;
}

string Entity::GetType()
{
    return type;
}

float Entity::GetHealth()
{
    return health;
}

float Entity::GetMana()
{
    return mana;
}

CVector Entity::GetPos()
{
    return pos;
}

vector<Skill*> Entity::GetAvaialableSkills()
{
    return availableSkills;
}

vector<Skill*>* Entity::GetQueue()
{
    return skillQueue;
}

void Entity::Move(CVector amount, bool reAdd)
{
    pos += amount;
    if (reAdd) mana += (abs(amount.m_x) + abs(amount.m_y));
    else mana -= (abs(amount.m_x) + abs(amount.m_y));
}

Skill* Entity::UseAbility(int index)
{
    // Put ability used in the skill queue
    Skill* usedAbility = availableSkills[index];
    mana -= usedAbility->manaCost;
    skillQueue[availableSkills[index]->turnCooldown - 1].push_back(usedAbility);
    availableSkills[index] = nullptr;
    return usedAbility;
}

Skill* Entity::UnUseAbility(int index)
{
    for (int i = 0; i < 3; i++) {
        int skillFound = -1;
        for (int j = 0; j < skillQueue[i].size(); j++) {
            if (skillQueue[i][j]->index == index) {
                skillFound = j;
            }
        }
        if (skillFound != -1) {
            Skill* skill = skillQueue[i][skillFound];
            mana += skill->manaCost;
            availableSkills[index] = skill;
            skillQueue[i].erase(skillQueue[i].begin() + skillFound);
            return skill;
        }
    }
}

void Entity::ProcessNextRound()
{
    // Move the queue by one
    for (Skill* skill : skillQueue[0]) {
        availableSkills[skill->index] = skill;
    }
    skillQueue[0].clear();
    skillQueue[0] = skillQueue[1];
    skillQueue[1] = skillQueue[2];
    skillQueue[2].clear();
    mana = manaPerRound;
}

void Entity::DealDamage(float damage)
{
    health -= damage;
}

Entity* Entity::Copy()
{
    return new Entity(health, mana, manaPerRound, availableSkills, pos, type, skillQueue);
}
