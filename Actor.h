#pragma once

class CollisionConfig;

enum class EShapeType
{
	Box,
	Circle,
};

class Actor
{
public:
	Actor(const int64& InActorId, CollisionConfig* InOuter, const EShapeType& InShapeType);
	virtual ~Actor();

public:
	void Tick(float InDeltaTime);
	void Render(sf::RenderWindow* InWindow);
	virtual void VertexRender(sf::VertexArray& InVertex) {}
	void SetColor(const sf::Color& InColor);

	void EnterOverlap();
	void LeaveOverlap();

public:
	const int64& GetActorId() const { return mActorId; }
	const EShapeType& GetShapeType() const{ return mShapeType; }
	sf::Shape* GetShape() const { return mShape; }
	virtual float GetLocalRadius() const { return 0.0f; }
	virtual sf::Rect<float> GetLocalBound() { return sf::Rect<float>(); }
	const sf::Vector2f GetLocation() const { return mShape->getPosition(); }

protected:
	void SetNewGoal();

protected:
	int64 mActorId;
	CollisionConfig* mOuter;

	EShapeType mShapeType;
	sf::Shape* mShape;
	sf::Vector2f mGoalLocation;

	float mSpeed;
};

class CircleActor : public Actor
{
public:
	CircleActor(const int64& InActorId, CollisionConfig* InOuter, const float& InRadius, const sf::Vector2f& InLocation);
	virtual ~CircleActor();

	virtual void VertexRender(sf::VertexArray& InVertex) override;
	virtual float GetLocalRadius() const override { return mRadius; }
	virtual sf::Rect<float> GetLocalBound() override;

private:
	float mRadius;
};

class BoxActor : public Actor
{
public:
	BoxActor(const int64& InActorId, CollisionConfig* InOuter, const float& InSize, const sf::Vector2f& InLocation);
	virtual ~BoxActor();

public:
	virtual void VertexRender(sf::VertexArray& InVertex) override;
	virtual float GetLocalRadius() const override;
	virtual sf::Rect<float> GetLocalBound() override;

public:
	sf::Vector2f GetCenter() const;
	sf::Vector2f GetPoint(const int32& InIndex) const;

private:
	float mSize;
};