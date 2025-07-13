#include "pch.h"
#include "Widget.h"

Widget::Widget()
{
    {
        sf::Text text = sf::Text(mFont, "Event Time", 20);
        text.setFillColor(sf::Color::White);
        text.setPosition({ WINDOW_WIDTH - 180, 10 });
        mTexts.push_back(text);
    }

    {
        sf::Text text = sf::Text(mFont, "Tick Time", 20);
        text.setFillColor(sf::Color::White);
        text.setPosition({ WINDOW_WIDTH - 180, 30 });
        mTexts.push_back(text);
    }

    {
        sf::Text text = sf::Text(mFont, "Draw Time", 20);
        text.setFillColor(sf::Color::White);
        text.setPosition({ WINDOW_WIDTH - 180, 50 });
        mTexts.push_back(text);
    }

    {
        sf::Text text = sf::Text(mFont, "Object", 20);
        text.setFillColor(sf::Color::White);
        text.setPosition({ 10, 10 });
        mTexts.push_back(text);
    }

    {
        sf::Text text = sf::Text(mFont, "Build Time", 20);
        text.setFillColor(sf::Color::White);
        text.setPosition({ 10, 30 });
        mTexts.push_back(text);
    }

    {
        sf::Text text = sf::Text(mFont, "Search Time", 20);
        text.setFillColor(sf::Color::White);
        text.setPosition({ 10, 50 });
        mTexts.push_back(text);
    }
}

Widget::~Widget()
{
}

void Widget::Tick(float InDeltaTime)
{
}

void Widget::Render(sf::RenderWindow* InWindow)
{
    for (sf::Text text : mTexts)
    {
        InWindow->draw(text);
    }
}

void Widget::SetFont(const std::string& InTTFPath)
{
    if (mFont.openFromFile(InTTFPath) == false)
    {
        std::cerr << "폰트를 로드할 수 없습니다!" << std::endl;
        return;
    }

    for (sf::Text text : mTexts)
    {
        text.setFont(mFont);
    }
}

void Widget::UpdateEventTime(const double InTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    stream << "Event Time: " << InTime << "ms";
    mTexts[static_cast<size_t>(EWidgetType::Event)].setString(stream.str());
}

void Widget::UpdateTickTime(const double InTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    stream << "Tick Time: " << InTime << "ms";
    mTexts[static_cast<size_t>(EWidgetType::Tick)].setString(stream.str());
}

void Widget::UpdateDrawTime(const double InTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    stream << "Draw Time: " << InTime << "ms";
    mTexts[static_cast<size_t>(EWidgetType::Draw)].setString(stream.str());
}

void Widget::UpdateObjectCount(const double InCount)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    stream << "Object Count: " << InCount;
    mTexts[static_cast<size_t>(EWidgetType::ObjectCount)].setString(stream.str());
}

void Widget::UpdateBuildTime(const double InTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    stream << "Build Time: " << InTime << "ms";
    mTexts[static_cast<size_t>(EWidgetType::BuildTime)].setString(stream.str());
}

void Widget::UpdateSearchTime(const double InTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    stream << "Search Time: " << InTime << "ms";
    mTexts[static_cast<size_t>(EWidgetType::SearchTime)].setString(stream.str());
}