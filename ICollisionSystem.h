#pragma once
#include "SAT.h"

class ICollisionSystem
{
public:
	virtual ~ICollisionSystem() {  }
	
public:
	virtual void Init() = 0;
	virtual void Build(std::vector<Actor*>& InActors) = 0;
	virtual void Destroy() = 0;
	virtual std::vector<Actor*> Search(Attack& InAttack) = 0;
	virtual std::vector<Actor*> AllSearch() = 0;
	virtual void Draw(sf::RenderWindow* InWindow) = 0;
	virtual void Draw(sf::VertexArray& OutVertexArray) = 0;
};

