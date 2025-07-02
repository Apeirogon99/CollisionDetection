#pragma once

class ParryShape : public sf::ConvexShape
{
public:
	ParryShape(sf::Vector2f InLocation, float InRadius, float InAngle);
	~ParryShape();

private:
	ParryShape(const ParryShape&) = delete;
	ParryShape(ParryShape&&) noexcept = delete;

	ParryShape& operator=(const ParryShape&) = delete;
	ParryShape& operator=(ParryShape&&) noexcept = delete;

public:
	sf::Vector2f mLocation;
	float mRadius;
	float mAngle;
};

class Parry
{
public:
	Parry(sf::Vector2f InLocation, float InRadius, float InAngle);
	~Parry();

private:
	Parry(const Parry&) = delete;
	Parry(Parry&&) noexcept = delete;

	Parry& operator=(const Parry&) = delete;
	Parry& operator=(Parry&&) noexcept = delete;

public:
	void TryParry(class Arrow& InArrow);
	bool InParry(class Arrow& InArrow);
	void Render(sf::RenderWindow* InWindow);

private:
	ParryShape mShape;
};

