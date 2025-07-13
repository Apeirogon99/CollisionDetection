#pragma once
class Attack : public BoxActor
{
public:
	Attack(CollisionConfig* InOuter, const float& InSize, const sf::Vector2f& InLocation);
	~Attack();

private:
	Attack(const Attack&) = delete;
	Attack(Attack&&) = delete;

	Attack& operator=(const Attack&) = delete;
	Attack& operator=(Attack&&) = delete;

public:
	void SetAttackLocation(const sf::Vector2i& InLocation);
	void AddAttackRange(const float InRange);
	void SetAttackCircumscriber(sf::VertexArray& InVertex);
};

