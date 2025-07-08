#include "pch.h"
#include "CollisionConfig.h"

CollisionConfig::CollisionConfig(const uint32& InMaxObject, const ESearchType& InSearchType, const EThreadMode& InThreadMode) : mIsDebug(false), mMaxObject(InMaxObject), mCurObject(0), mSearchType(InSearchType), mThreadMode(InThreadMode), mTaskTimer(), mAttack(this, 100.0f, sf::Vector2f())
{
    mSpawnCount = InMaxObject / 100;

	mWindow = new sf::RenderWindow(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "CollisionDetection");
	mWindow->setFramerateLimit(FRAME);

    switch (InSearchType)
    {
    case ESearchType::ARRAY:
        mCollisionSystem = new Array(mActors);
        break;
    case ESearchType::KD_TREE:
        mCollisionSystem = new KDTree(InMaxObject);
        break;
    case ESearchType::QUAD_TREE:
        mCollisionSystem = new QuadtreeManager({ WINDOW_WIDTH, WINDOW_HEIGHT });
        break;
    default:
        break;
    }

    mCollisionSystem->Init();

    mWidget = new Widget();
    mWidget->SetFont("DNFBitBitTTF.ttf");
}

CollisionConfig::~CollisionConfig()
{
    delete mCollisionSystem;
}

void CollisionConfig::Run()
{
    double searchTime[100] = { 0, };
    int64 searchTimeCount = 0;
    while (mWindow->isOpen())
    {

        float deltaTime = mClock.restart().asSeconds();

        //EVENT
        auto event = [&]()
            {
                while (const std::optional event = mWindow->pollEvent())
                {
                    if (event->is<sf::Event::Closed>())
                    {
                        Close();
                    }
                    
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Escape))
                    {
                        Close();
                    }

                    if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
                    {
                        if (keyReleased->scancode == sf::Keyboard::Scancode::Num1)
                        {
                            SpawnActor(mSpawnCount);
                        }

                        if (keyReleased->scancode == sf::Keyboard::Scancode::Num2)
                        {
                            DestroyActor(mSpawnCount);
                        }

                        if (keyReleased->scancode == sf::Keyboard::Scancode::Num3)
                        {
                            mIsDebug = !mIsDebug;
                        }
                    }

                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                    {
                        mAttack.SetAttackLocation(sf::Mouse::getPosition(*mWindow));
                    }
                }
            };
        mWidget->UpdateEventTime(mTaskTimer.MeasureTask(event));

        //TICK
        auto tick = [&]()
            {
                for (Actor* actor : mActors)
                {
                    actor->Tick(deltaTime);
                }
                mWidget->Tick(deltaTime);
            };
        mWidget->UpdateTickTime(mTaskTimer.MeasureTask(tick));

        auto build = [&]()
            {
                mCollisionSystem->Build(mActors);
            };
        mWidget->UpdateBuildTime(mTaskTimer.MeasureTask(build));

        auto search = [&]()
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                {
                    mCollisionSystem->Search(mAttack);
                }
                else
                {
                    mCollisionSystem->AllSearch();
                }
            };
        searchTime[searchTimeCount] = mTaskTimer.MeasureTask(search);
        searchTimeCount = (searchTimeCount + 1) % 100;

        double sum = 0;
        for (size_t i = 0; i < 100; ++i)
        {
            sum += searchTime[i];
        }
        mWidget->UpdateSearchTime(sum / 100);
       

        //DRAW
        auto draw = [&]()
            {
                mWindow->clear();

                sf::VertexArray debugVertexArray(sf::PrimitiveType::Lines);
                if (mIsDebug)
                {
                    mCollisionSystem->Draw(debugVertexArray);
                    mAttack.SetAttackCircumscriber(debugVertexArray);
                }
                mWindow->draw(debugVertexArray);

                sf::VertexArray actorVertexArray(sf::PrimitiveType::Lines);
                for (Actor* actor : mActors)
                {
                    actor->VertexRender(actorVertexArray);
                }
                mWindow->draw(actorVertexArray);

                mAttack.Render(mWindow);

                mWidget->Render(mWindow);

                mWindow->display();
            };
        mWidget->UpdateDrawTime(mTaskTimer.MeasureTask(draw));
    }
}

void CollisionConfig::Close()
{
    for (Actor* actor : mActors)
    {
        delete actor;
        actor = nullptr;
    }

    mWindow->close();

    exit(0);
}

void CollisionConfig::SpawnActor(const uint32& InAddObjectCount)
{

    if (InAddObjectCount + mCurObject > mMaxObject)
    {
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disx(50, WINDOW_WIDTH - 100);
    std::uniform_int_distribution<> disy(50, WINDOW_HEIGHT - 100);

    for (uint32 count = 0; count < InAddObjectCount / 2.0f; ++count)
    {
        const float size = 4.0f;
        const float x = static_cast<float>(disx(gen));
        const float y = static_cast<float>(disy(gen));

        Actor* actor = new CircleActor(mCurObject++, this, size, {x, y});
        mActors.push_back(actor);
    }

    for (uint32 count = 0; count < InAddObjectCount / 2.0f; ++count)
    {
        const float size = 8.0f;
        const float x = static_cast<float>(disx(gen));
        const float y = static_cast<float>(disy(gen));

        Actor* actor = new BoxActor(mCurObject++, this, size, { x, y });
        mActors.push_back(actor);
    }

    mWidget->UpdateObjectCount(mCurObject);
}

void CollisionConfig::DestroyActor(const uint32& InSubObjectCount)
{
    if (mCurObject - InSubObjectCount < 0)
    {
        return;
    }

    for (uint32 count = 0; count < InSubObjectCount; ++count)
    {
        Actor* actor = mActors[count];

        delete actor;
        actor = nullptr;
    }

    mActors.erase(mActors.begin(), mActors.begin() + InSubObjectCount);

    mCurObject -= InSubObjectCount;
    mWidget->UpdateObjectCount(mCurObject);
}
