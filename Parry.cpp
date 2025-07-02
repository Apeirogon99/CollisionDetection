#include "pch.h"
#include "Parry.h"
#include "Arrow.h"

const float PI = 3.14159265359f;
float degToRad(float degrees) 
{
    return degrees * PI / 180.0f;
}

sf::Vector2f getForwardVector(float rotationDegrees) 
{
    float rotationRadians = rotationDegrees * (PI / 180.0f);

    float forwardX = std::cos(rotationRadians);
    float forwardY = std::sin(rotationRadians);

    return sf::Vector2f(forwardX, forwardY).normalized();
}

ParryShape::ParryShape(sf::Vector2f InLocation, float InRadius, float InAngle) : mLocation(InLocation), mRadius(InRadius), mAngle(InAngle)
{
    const int32 POINT_COUNT = 30;

    this->setPointCount(POINT_COUNT + 2);
    this->setPoint(0, sf::Vector2f(0, 0));

    float startAngle = InAngle - 45;
    float endAngle = InAngle + 45;

    float angleStep = (endAngle - startAngle) / POINT_COUNT;
    for (int i = 0; i <= POINT_COUNT; ++i) 
    {
        float angle = degToRad(startAngle + i * angleStep);
        float x = InRadius * cos(angle);
        float y = InRadius * sin(angle);
        this->setPoint(i + 1, sf::Vector2f(x, y));
    }

    this->setPosition(InLocation);

    this->setFillColor(sf::Color(231, 76, 60, 100));
    this->setOutlineColor(sf::Color(192, 57, 43, 255));
    this->setOutlineThickness(-2.0f);
}

ParryShape::~ParryShape()
{
}


Parry::Parry(sf::Vector2f InLocation, float InRadius, float InAngle) : mShape(InLocation, InRadius, InAngle)
{
}

Parry::~Parry()
{
}

void Parry::TryParry(Arrow& InArrow)
{
    InArrow.SetColor(sf::Color::Green);
    if (InParry(InArrow) == false)
    {
        return;
    }

    sf::Vector2f player = getForwardVector(mShape.mAngle);
    sf::Vector2f attack = InArrow.GetForward();

    float dotProduct = player.x * attack.x + player.y * attack.y;
    float angleRadians = std::acos(std::clamp(dotProduct, -1.0f, 1.0f));
    float angleDegrees = angleRadians * 180.0f / 3.14159265359f;

    cout << angleDegrees << " >= " << 180.0f - 45.0f << endl;
    
    if (angleDegrees >= 180.0f - 45.0f)
    {
        InArrow.SetColor(sf::Color::Red);
    }
    else
    {
        InArrow.SetColor(sf::Color::Green);
    }
}

bool Parry::InParry(Arrow& InArrow)
{
    float dx = InArrow.GetEndPoint().x - mShape.mLocation.x;
    float dy = InArrow.GetEndPoint().y - mShape.mLocation.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    if (distance > mShape.mRadius)
    {
        return false;
    }

    //2차 각도 안에 있는 검사
    float angle = std::atan2(dy, dx);
    float angleDegrees = angle * 180.0f / PI;

    if (angleDegrees < 0)
        angleDegrees += 360.0f;

    float  startAngleDegrees = mShape.mAngle - 45.0f;
    float  endAngleDegrees = mShape.mAngle + 45.0f;
    if (startAngleDegrees > endAngleDegrees)
    {
        return (angleDegrees >= startAngleDegrees || angleDegrees <= endAngleDegrees);
    }
    else
    {
        return (angleDegrees >= startAngleDegrees && angleDegrees <= endAngleDegrees);
    }
}

void Parry::Render(sf::RenderWindow* InWindow)
{
    InWindow->draw(mShape);
}

