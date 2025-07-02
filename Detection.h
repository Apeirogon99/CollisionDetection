#pragma once

class Detection
{
public:
    // 두 다각형 간의 충돌 검사
    static bool CheckCollision(Actor* InA, Actor* InB)
    {
		if (InA->GetShapeType() == EShapeType::Box && InB->GetShapeType() == EShapeType::Box)
		{
			sf::RectangleShape* AttackRectangle = reinterpret_cast<sf::RectangleShape*>(InB->GetShape());
			sf::RectangleShape* ActorRectangle = reinterpret_cast<sf::RectangleShape*>(InA->GetShape());

			auto find = AttackRectangle->getGlobalBounds().findIntersection(ActorRectangle->getGlobalBounds());
			if (find.has_value() == true)
			{
				return true;
			}
		}
		else if (InA->GetShapeType() == EShapeType::Circle && InB->GetShapeType() == EShapeType::Circle)
		{
			const float& distanceSquared = powf(InB->GetLocation().x - InA->GetLocation().x, 2) + powf(InB->GetLocation().y - InA->GetLocation().y, 2);
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

