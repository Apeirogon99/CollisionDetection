#pragma once

/// <summary>
/// Ž�� Ÿ��
/// </summary>
enum class ESearchType
{
	ARRAY,		//Array���
	KD_TREE,	//KD-Tree���
	QUAD_TREE,	//Quad-Tree���
};

/// <summary>
/// ������ ���
/// </summary>
enum class EThreadMode
{
	SINGLE,		//���� �̱� ó��
	MULTI,		//���� ��Ƽ ó��
	MIXED,		//�̱۷� ó�������� �۾��� ��Ƽ ó��
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

