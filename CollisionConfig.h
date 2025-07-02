#pragma once

/// <summary>
/// 탐색 타입
/// </summary>
enum class ESearchType
{
	ARRAY,		//Array기반
	KD_TREE,	//KD-Tree기반
	QUAD_TREE,	//Quad-Tree기반
};

/// <summary>
/// 쓰레드 모드
/// </summary>
enum class EThreadMode
{
	SINGLE,		//오직 싱글 처리
	MULTI,		//오직 멀티 처리
	MIXED,		//싱글로 처리되지만 작업은 멀티 처리
};

class CollisionConfig
{
public:
	CollisionConfig(const uint32& InMaxObject, const ESearchType& InSearchType, const EThreadMode& InThreadMode);
	~CollisionConfig();

public:
	void Run();
	void Close();

public:
	const sf::Vector2f GetWindowSize() const { return {static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)}; }

private:
	void SpawnActor(const uint32& InAddObjectCount);
	void DestroyActor(const uint32& InSubObjectCount);

private:
	bool mIsDebug;
	sf::RenderWindow* mWindow;
	
	sf::Clock mClock;
	TaskTimer mTaskTimer;

	Widget* mWidget;

	std::vector<Actor*> mActors;
	uint32 mMaxObject;
	uint32 mCurObject;
	uint32 mSpawnCount;

	Attack mAttack;

	ESearchType mSearchType;
	ICollisionSystem* mCollisionSystem;

	EThreadMode mThreadMode;
};

