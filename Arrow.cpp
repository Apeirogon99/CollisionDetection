#include "pch.h"
#include "Arrow.h"

Arrow::Arrow()
{
	SetColor(sf::Color::Green);
}

Arrow::~Arrow()
{
}

void Arrow::SetStartLocation(const sf::Vector2i& InLocation)
{
	const float& x = static_cast<float>(InLocation.x);
	const float& y = static_cast<float>(InLocation.y);

	mStart.position = { x, y };
}

void Arrow::SetEndLocation(const sf::Vector2i& InLocation)
{
	const float& x = static_cast<float>(InLocation.x);
	const float& y = static_cast<float>(InLocation.y);

	mEnd.position = { x, y };
}

void Arrow::SetColor(const sf::Color& InColor)
{
	mStart.color = InColor;
	mEnd.color = InColor;
}

void Arrow::Render(sf::VertexArray& InVertex)
{
	InVertex.append(mStart);
	InVertex.append(mEnd);
}
