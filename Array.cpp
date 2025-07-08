#include "pch.h"
#include "Array.h"
#include <set>
#include <map>

Array::Array(std::vector<Actor*>& InActors) : mActors(InActors)
{
}

void Array::Init()
{
}

void Array::Build(std::vector<Actor*>& InActors)
{
	
}

void Array::Destroy()
{
}

std::vector<Actor*> Array::Search(Attack& InAttack)
{
	std::vector<Actor*> overlap;
	for (Actor* actor : mActors)
	{
		const sf::Vector2f& location = actor->GetLocation();
		const float& radius = actor->GetLocalRadius();

		const EShapeType& type = actor->GetShapeType();

#if USE_SAT
		SATResult result = SAT::CheckCollision(*actor->GetShape(), *InAttack.GetShape());
		if (result.colliding)
		{
			actor->EnterOverlap();
		}
#else
		if (Detection::CheckCollision(actor, &InAttack))
		{
			actor->EnterOverlap();
		}
#endif
	}
	return overlap;
}

std::vector<Actor*> Array::AllSearch()
{
	std::vector<Actor*> overlap;
	for (size_t i = 0; i < mActors.size(); i++)
	{
		Actor* actor = mActors[i];
		for (size_t j = i + 1; j < mActors.size(); j++)
		{
			Actor* otherActor = mActors[j];

#if USE_SAT
			SATResult result = SAT::CheckCollision(*actor->GetShape(), *otherActor->GetShape());
			if (result.colliding)
			{
				actor->EnterOverlap();
				otherActor->EnterOverlap();
			}
#else
			if (Detection::CheckCollision(actor, otherActor))
			{
				actor->EnterOverlap();
				otherActor->EnterOverlap();
			}
#endif
		}
	}
	return overlap;
}

void Array::Draw(sf::RenderWindow* InWindow)
{
}

void Array::Draw(sf::VertexArray& OutVertexArray)
{
}
