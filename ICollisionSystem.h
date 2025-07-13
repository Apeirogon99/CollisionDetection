#pragma once
#include "SAT.h"

class ICollisionSystem
{
public:
	virtual ~ICollisionSystem() {  }
	
public:
	virtual void Init() = 0;
	virtual void Destroy() = 0;

	virtual void Insert(Actor* Actor) = 0;
	virtual void Remove(Actor* Actor) = 0;
	virtual void Build() = 0;

	virtual std::vector<Actor*> Search(Attack& InAttack) = 0;
	virtual std::vector<Actor*> AllSearch() = 0;
	virtual void Draw(sf::VertexArray& OutVertexArray) = 0;
};

