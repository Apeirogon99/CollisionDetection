#include "pch.h"

int main(void)
{
    CollisionConfig* config = new CollisionConfig(10000, ESearchType::KD_TREE, EThreadMode::SINGLE);
    config->Run();

    delete config;
    config = nullptr;
    return 0;
}

//#include "Parry.h"
//#include "Arrow.h"
//
//int main(void)
//{
//    sf::RenderWindow* mWindow;
//    mWindow = new sf::RenderWindow(sf::VideoMode({ 800, 800 }), "Parry");
//    mWindow->setFramerateLimit(FRAME);
//	
//    std::vector<Arrow> arrows;
//    int32 arrowCount = -1;
//    Parry parry({800.0f / 2.0f, (800.0f / 2.0f) - 50.0f}, 100.0f, 270.0f);
//    
//    sf::CircleShape player(50.0f);
//    player.setFillColor(sf::Color(255, 255, 255, 100.0f));
//    player.setOutlineColor(sf::Color::White);
//    player.setOutlineThickness(-2.0f);
//    player.setOrigin({ 30.0f, 30.0f });
//    player.setPosition({ 800.0f / 2.0f, 800.0f / 2.0f });
//
//    sf::RectangleShape hitBox({ 100.0f, 50.0f });
//    hitBox.setFillColor(sf::Color(0, 0, 255, 100.0f));
//    hitBox.setOutlineColor(sf::Color::Blue);
//    hitBox.setOutlineThickness(-2.0f);
//    hitBox.setOrigin({ 50.0f, 25.0f });
//    hitBox.setPosition({ 800.0f / 2.0f, (800.0f / 2.0f) - 60.0f });
//
//    while (mWindow->isOpen())
//    {
//        //EVENT
//        bool pressed = false;
//        while (const std::optional event = mWindow->pollEvent())
//        {
//            if (event->is<sf::Event::Closed>())
//            {
//                mWindow->close();
//            }
//
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Escape))
//            {
//                mWindow->close();
//            }
//
//            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
//            {
//                if (arrowCount != -1)
//                {
//                    arrows[arrowCount].SetStartLocation(sf::Mouse::getPosition(*mWindow));
//                }
//            }
//
//            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
//            {
//                if (arrowCount != -1)
//                {
//                    arrows[arrowCount].SetEndLocation(sf::Mouse::getPosition(*mWindow));
//                }
//            }
//
//
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
//            {
//                if (pressed == false)
//                {
//                    pressed = true;
//                    arrowCount++;
//                    arrows.push_back(Arrow());
//                }
//            }
//        
//        }
//
//        for (int32 index = 0; index <= arrowCount; ++index)
//        {
//            parry.TryParry(arrows[index]);
//        }
//
//        //DRAW
//        mWindow->clear();
//        {
//            for (int32 index = 0; index <= arrowCount; ++index)
//            {
//                sf::VertexArray arrowVertexArray(sf::PrimitiveType::Lines);
//                arrows[index].Render(arrowVertexArray);
//                mWindow->draw(arrowVertexArray);
//            }
//
//            parry.Render(mWindow);
//            
//            mWindow->draw(player);
//            mWindow->draw(hitBox);
//        }
//        mWindow->display();
//    }
//	
//	return 0;
//}