#pragma once
class Arrow
{
public:
	Arrow();
	~Arrow();

private:
	//Arrow(const Arrow&) = delete;
	//Arrow(Arrow&&) noexcept = delete;

	//Arrow& operator=(const Arrow&) = delete;
	//Arrow& operator=(Arrow&&) noexcept = delete;

public:
	void SetStartLocation(const sf::Vector2i& InLocation);
	void SetEndLocation(const sf::Vector2i& InLocation);
	void SetColor(const sf::Color& InColor);

	void Render(sf::VertexArray& InVertex);

public:
	sf::Vector2f GetStartPoint() const { return mStart.position; }
	sf::Vector2f GetEndPoint() const { return mEnd.position; }
	sf::Vector2f GetForward() const { return (mEnd.position - mStart.position).normalized(); }

private:
	sf::Vertex mStart;
	sf::Vertex mEnd;
};

