#include "pch.h"
#include "Attack.h"

Attack::Attack(CollisionConfig* InOuter, const float& InSize, const sf::Vector2f& InLocation) : BoxActor(0, InOuter, InSize, InLocation)
{
	mShape->setFillColor(sf::Color::Transparent);
	mShape->setOutlineColor(sf::Color::Magenta);
}

Attack::~Attack()
{
}

void Attack::SetAttackLocation(const sf::Vector2i& InLocation)
{ 
	const float& x = static_cast<float>(InLocation.x);
	const float& y = static_cast<float>(InLocation.y);

	mShape->setPosition({ x, y });
	mShape->setOrigin(mShape->getGeometricCenter());
}

void Attack::AddAttackRange(const float InRange)
{
	sf::RectangleShape* rectangle = reinterpret_cast<sf::RectangleShape*>(mShape);

	sf::Vector2f size = rectangle->getSize();
	mSize = size.x + InRange;

	if (mSize <= 10.0f)
	{
		mSize = 10.0f;
	}

	rectangle->setSize({ mSize, mSize });
	rectangle->setOrigin(rectangle->getGeometricCenter());
}

void Attack::SetAttackCircumscriber(sf::VertexArray& InVertex)
{
	const int numVertices = 30;
	for (int i = 0; i < numVertices; ++i)
	{
		float angle = i * 2 * 3.14159265359f / numVertices;
		float x = mShape->getPosition().x + GetLocalRadius() * cos(angle);
		float y = mShape->getPosition().y + GetLocalRadius() * sin(angle);

		sf::Vertex vertex;
		vertex.position = sf::Vector2f(x, y);
		vertex.color = mShape->getOutlineColor();
		InVertex.append(vertex);
	}
}
