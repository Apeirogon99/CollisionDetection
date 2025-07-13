#include "pch.h"
#include "Array.h"
#include <set>
#include <map>

Array::Array() : mActors()
{
}

void Array::Init()
{
}

void Array::Destroy()
{
	mActors.clear();
}

void Array::Insert(Actor* Actor)
{
	mActors.emplace_back(Actor);
}

void Array::Remove(Actor* Actor)
{
	for (size_t index = 0; index < mActors.size(); ++index)
	{
		if (mActors[index] == Actor)
		{
			mActors[index] = mActors.back();
			mActors.pop_back();
			break;
		}
	}
}

void Array::Build()
{

}

std::vector<Actor*> Array::Search(Attack& InAttack)
{
	std::vector<Actor*> overlap;
	for (Actor* actor : mActors)
	{

#if USE_SAT
		SATResult result = SAT::CheckCollision(*actor->GetShape(), *InAttack.GetShape());
		if (result.colliding)
		{
			actor->EnterOverlap();
		}
#else
		if (Detection::CheckCollision(actor, &InAttack))
		{
			overlap.push_back(actor);
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

			if (actor == otherActor) continue;

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
				overlap.push_back(actor);
				actor->EnterOverlap();
			}
#endif
		}
	}
	return overlap;
}
