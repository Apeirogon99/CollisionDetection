#include "pch.h"
#include "Actor.h"

Actor::Actor(const int64& InActorId, CollisionConfig* InOuter, const EShapeType& InShapeType) : mActorId(InActorId), mOuter(InOuter), mShapeType(InShapeType), mShape(nullptr), mGoalLocation(), mIsOverlap(false)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(10, 50);
	mSpeed = static_cast<float>(dis(gen));
}

Actor::~Actor()
{
	if (mShape)
	{
		delete mShape;
	}

	mShape = nullptr;
}

void Actor::Tick(float InDeltaTime)
{
	LeaveOverlap();

	sf::Vector2f curLocation = this->GetLocation();

	const float& distance = sqrtf(powf(mGoalLocation.x - curLocation.x, 2) + powf(mGoalLocation.y - curLocation.y, 2));
	if (distance <= 1.0f)
	{
		SetNewGoal();
	}
	else
	{
		const float dx = mGoalLocation.x - curLocation.x;
		const float dy = mGoalLocation.y - curLocation.y;

		const float nx = curLocation.x + (dx / distance) * InDeltaTime * mSpeed;
		const float ny = curLocation.y + (dy / distance) * InDeltaTime * mSpeed;

		mShape->setPosition({ nx , ny });
	}
}

void Actor::Render(sf::RenderWindow* InWindow)
{
	InWindow->draw(*mShape, sf::RenderStates::Default);
}

void Actor::SetColor(const sf::Color& InColor)
{
	mShape->setOutlineColor(InColor);
}

void Actor::EnterOverlap()
{
	SetColor(sf::Color::Red);
	mIsOverlap = true;
}

void Actor::LeaveOverlap()
{
	SetColor(sf::Color::Green);
	mIsOverlap = false;
}

void Actor::SetNewGoal()
{
	if (mShape == nullptr) return;

	sf::Vector2f location = this->GetLocation();
	sf::Vector2f windowSize = mOuter->GetWindowSize();

	const float moveRange = 100.0f;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distX(-moveRange, moveRange);
	std::uniform_real_distribution<float> distY(-moveRange, moveRange);

	float newX = std::clamp(location.x + distX(gen), 0.0f, windowSize.x);
	float newY = std::clamp(location.y + distY(gen), 0.0f, windowSize.y);

	mGoalLocation = { newX, newY };
}

CircleActor::CircleActor(const int64& InActorId, CollisionConfig* InOuter, const float& InRadius, const sf::Vector2f& InLocation) : Actor(InActorId, InOuter, EShapeType::Circle), mRadius(InRadius)
{
	mShape = new sf::CircleShape();
	sf::CircleShape* circle = reinterpret_cast<sf::CircleShape*>(mShape);

	circle->setPosition(InLocation);
	circle->setRadius(mRadius);
	circle->setOrigin(circle->getGeometricCenter());
	circle->setFillColor(sf::Color::Black);
	circle->setOutlineColor(sf::Color::Green);
	circle->setOutlineThickness(-2.0f);

	SetNewGoal();
}

CircleActor::~CircleActor()
{
}

void CircleActor::VertexRender(sf::VertexArray& InVertex)
{
	const int numVertices = 10;
	for (int i = 0; i < numVertices; ++i)
	{
		float angle = i * 2 * 3.14159265359f / numVertices;
		float x = mShape->getPosition().x + mRadius * cos(angle);
		float y = mShape->getPosition().y + mRadius * sin(angle);

		sf::Vertex vertex;
		vertex.position = sf::Vector2f(x, y);
		vertex.color = mShape->getOutlineColor();
		InVertex.append(vertex);
	}

	/*static sf::Vector2f Dir[4] =
	{
		{-1.0f, -1.0f},
		{+1.0f, -1.0f},
		{+1.0f, +1.0f},
		{-1.0f, +1.0f}
	};

	sf::Rect<float> rect = GetLocalBound();
	for (int32 point = 0; point < 4; ++point)
	{
		sf::Vertex v1;
		v1.position = rect.position + sf::Vector2f(rect.size.x * Dir[point].x, rect.size.y * Dir[point].y);
		v1.color = mShape->getOutlineColor();

		sf::Vertex v2;
		v2.position = rect.position + sf::Vector2f(rect.size.x * Dir[(point + 1) % 4].x, rect.size.y * Dir[(point + 1) % 4].y);
		v2.color = mShape->getOutlineColor();

		InVertex.append(v1);
		InVertex.append(v2);
	}*/
}

sf::Rect<float> CircleActor::GetLocalBound()
{
	sf::CircleShape* circle = reinterpret_cast<sf::CircleShape*>(mShape);
	sf::Rect<float> rect = circle->getGlobalBounds();
	//rect.position = GetLocation();
	return rect;
}

BoxActor::BoxActor(const int64& InActorId, CollisionConfig* InOuter, const float& InSize, const sf::Vector2f& InLocation) : Actor(InActorId, InOuter, EShapeType::Box), mSize(InSize)
{
	mShape = new sf::RectangleShape();
	sf::RectangleShape* rectangle = reinterpret_cast<sf::RectangleShape*>(mShape);

	rectangle->setPosition(InLocation);
	rectangle->setSize({ InSize, InSize });
	rectangle->setOrigin(rectangle->getGeometricCenter());
	rectangle->setFillColor(sf::Color::Black);
	rectangle->setOutlineColor(sf::Color::Green);
	rectangle->setOutlineThickness(-2.0f);

	SetNewGoal();
}

BoxActor::~BoxActor()
{
}

void BoxActor::VertexRender(sf::VertexArray& InVertex)
{
	sf::RectangleShape* rectangle = reinterpret_cast<sf::RectangleShape*>(mShape);
	for (int32 point = 0; point < 4; ++point)
	{
		sf::Vertex v1;
		v1.position = rectangle->getPosition() + rectangle->getPoint(point);
		v1.color = mShape->getOutlineColor();

		sf::Vertex v2;
		v2.position = rectangle->getPosition() + rectangle->getPoint(point + 1);
		v2.color = mShape->getOutlineColor();

		InVertex.append(v1);
		InVertex.append(v2);
	}

	/*static sf::Vector2f Dir[4] =
	{
		{-1.0f, -1.0f},
		{+1.0f, -1.0f},
		{+1.0f, +1.0f},
		{-1.0f, +1.0f}
	};

	sf::Rect<float> rect = GetLocalBound();
	for (int32 point = 0; point < 4; ++point)
	{
		sf::Vertex v1;
		v1.position = rect.position + sf::Vector2f(rect.size.x * Dir[point].x, rect.size.y * Dir[point].y);
		v1.color = sf::Color::Magenta;

		sf::Vertex v2;
		v2.position = rect.position + sf::Vector2f(rect.size.x * Dir[(point + 1) % 4].x, rect.size.y * Dir[(point + 1) % 4].y);
		v2.color = sf::Color::Magenta;

		InVertex.append(v1);
		InVertex.append(v2);
	}*/
}

float BoxActor::GetLocalRadius() const
{
	return std::sqrtf(std::powf(mSize, 2) + std::powf(mSize, 2)) / 2.0f;
}

sf::Rect<float> BoxActor::GetLocalBound()
{
	sf::RectangleShape* rectangle = reinterpret_cast<sf::RectangleShape*>(mShape);
	sf::Rect<float> rect = rectangle->getGlobalBounds();
	//rect.position = GetLocation() - rect.size * 0.5f;
	return rect;
}

sf::Vector2f BoxActor::GetCenter() const
{
	sf::RectangleShape* rectangle = reinterpret_cast<sf::RectangleShape*>(mShape);
	sf::Vector2f min = rectangle->getPosition() + rectangle->getPoint(0);
	sf::Vector2f max = rectangle->getPosition() + rectangle->getPoint(3);

	return (min + max) / 2.0f;
}

sf::Vector2f BoxActor::GetPoint(const int32& InIndex) const
{
	sf::Vector2f point;

	sf::RectangleShape* rectangle = reinterpret_cast<sf::RectangleShape*>(mShape);
	sf::Vector2f center = rectangle->getGeometricCenter();

	if (InIndex == 0)		{ point.x = this->GetLocation().x - this->mSize / 2.0f;	point.y = this->GetLocation().y - this->mSize / 2.0f; }
	else if (InIndex == 1)	{ point.x = this->GetLocation().x + this->mSize / 2.0f;	point.y = this->GetLocation().y - this->mSize / 2.0f; }
	else if (InIndex == 2)	{ point.x = this->GetLocation().x + this->mSize / 2.0f;	point.y = this->GetLocation().y + this->mSize / 2.0f; }
	else if (InIndex == 3)	{ point.x = this->GetLocation().x - this->mSize / 2.0f;	point.y = this->GetLocation().y + this->mSize / 2.0f; }

	return point;
}
