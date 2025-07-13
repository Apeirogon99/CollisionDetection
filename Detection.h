#pragma once

class Detection
{
public:
	static bool AABB(const sf::FloatRect& a, const sf::FloatRect& b)
	{
		return !(a.position.x + a.size.x <= b.position.x ||
			b.position.x + b.size.x <= a.position.x ||
			a.position.y + a.size.y <= b.position.y ||
			b.position.y + b.size.y <= a.position.y);
	}

public:
    // 두 다각형 간의 충돌 검사
    static bool CheckCollision(Actor* InA, Actor* InB)
    {
		if (InA->GetShapeType() == EShapeType::Box && InB->GetShapeType() == EShapeType::Box)
		{
			if (Detection::AABB(InA->GetLocalBound(), InB->GetLocalBound()))
			{
				return true;
			}
		}
		else if (InA->GetShapeType() == EShapeType::Circle && InB->GetShapeType() == EShapeType::Circle)
		{
			const float dx = InB->GetLocation().x - InA->GetLocation().x;
			const float dy = InB->GetLocation().y - InA->GetLocation().y;
			const float distanceSquared = dx * dx + dy * dy;
			const float radiusSum = powf(InB->GetLocalRadius() + InA->GetLocalRadius(), 2);

			if (distanceSquared <= radiusSum)
			{
				return true;
			}
		}
		else if (InA->GetShapeType() == EShapeType::Circle && InB->GetShapeType() == EShapeType::Box)
		{
			const float& radius = InA->GetLocalRadius();
			BoxActor* boxActor = reinterpret_cast<BoxActor*>(InB);

			float closestX = std::clamp(InA->GetLocation().x, boxActor->GetPoint(0).x, boxActor->GetPoint(2).x);
			float closestY = std::clamp(InA->GetLocation().y, boxActor->GetPoint(0).y, boxActor->GetPoint(2).y);

			float distanceX = InA->GetLocation().x - closestX;
			float distanceY = InA->GetLocation().y - closestY;
			float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

			if (distanceSquared <= (radius * radius))
			{
				return true;
			}
		}
		else if (InA->GetShapeType() == EShapeType::Box && InB->GetShapeType() == EShapeType::Circle)
		{
			const float& radius = InB->GetLocalRadius();
			BoxActor* boxActor = reinterpret_cast<BoxActor*>(InA);

			float closestX = std::clamp(InB->GetLocation().x, boxActor->GetPoint(0).x, boxActor->GetPoint(2).x);
			float closestY = std::clamp(InB->GetLocation().y, boxActor->GetPoint(0).y, boxActor->GetPoint(2).y);

			float distanceX = InB->GetLocation().x - closestX;
			float distanceY = InB->GetLocation().y - closestY;
			float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

			if (distanceSquared <= (radius * radius))
			{
				return true;
			}
		}

		return false;
    }
};

