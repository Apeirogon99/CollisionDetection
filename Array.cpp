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

		SATResult result = SAT::CheckCollision(*actor->GetShape(), *InAttack.GetShape());
		if (result.colliding)
		{
			overlap.push_back(actor);
			actor->EnterOverlap();
		}

		//if (Detection::CheckCollision(actor, &InAttack))
		//{
		//	overlap.push_back(actor);
		//	actor->EnterOverlap();
		//}
	}
	return overlap;
}

std::vector<Actor*> Array::AllSearch()
{
	std::vector<Actor*> overlap;
	for (size_t i = 0; i < mActors.size(); i++)
	{
		Actor* A = mActors[i];
		for (size_t j = i + 1; j < mActors.size(); j++)
		{
			Actor* B = mActors[j];
#if USE_SAT
			const float& distanceSquared = powf(A->GetLocation().x - B->GetLocation().x, 2) + powf(A->GetLocation().y - B->GetLocation().y, 2);
			const float radiusSum = powf(A->GetLocalRadius() + B->GetLocalRadius(), 2);
			if (distanceSquared <= radiusSum)
			{
				SATResult result = SAT::CheckCollision(*A->GetShape(), *B->GetShape());
				if (result.colliding)
				{
					A->EnterOverlap();
					B->EnterOverlap();
				}
			}
#else
			if (Detection::CheckCollision(A, B))
			{
				A->EnterOverlap();
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
