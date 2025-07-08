#pragma once

enum class EWidgetType
{
	Event,
	Tick,
	Draw,

	ObjectCount,
	BuildTime,
	SearchTime,
};

class Widget
{
public:
	Widget();
	~Widget();

public:
	void Tick(float InDeltaTime);
	void Render(sf::RenderWindow* InWindow);

public:
	void SetFont(const std::string& InTTFPath);

	void UpdateEventTime(const double InTime);
	void UpdateTickTime(const double InTime);
	void UpdateDrawTime(const double InTime);

	void UpdateObjectCount(const double InCount);
	void UpdateBuildTime(const double InTime);
	void UpdateSearchTime(const double InTime);

private:
	sf::Font mFont;

	std::vector<sf::Text> mTexts;
};

