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

	void UpdateEventTime(const int64& InTime);
	void UpdateTickTime(const int64& InTime);
	void UpdateDrawTime(const int64& InTime);

	void UpdateObjectCount(const int64& InCount);
	void UpdateBuildTime(const int64& InTime);
	void UpdateSearchTime(const int64& InTime);

private:
	sf::Font mFont;

	std::vector<sf::Text> mTexts;
};

