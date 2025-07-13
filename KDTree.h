#pragma once

class KDNode
{
public:
	KDNode(Actor* Actor, uint32 Depth) : 
		mActor(Actor),
		mDepth(Depth),
		mLeftNode(nullptr), 
		mRightNode(nullptr) {}
	~KDNode() { Clear(); }

private:
	KDNode(const KDNode&) = delete;
	KDNode(KDNode&&) = delete;

	KDNode& operator=(const KDNode&) = delete;
	KDNode& operator=(KDNode&&) = delete;

public:
	bool IsValid()
	{
		return mActor != nullptr;
	}

	void Clear()
	{
		mActor = nullptr;
		mLeftNode = nullptr;
		mRightNode = nullptr;
		mDepth = 0;
	}

	void SetActor(Actor* inActor) { mActor = inActor; };
	void SetDepth(const uint32& InDepth) { mDepth = InDepth; }
	void SetLeftNode(KDNode* inLeftNode) { mLeftNode = inLeftNode; }
	void SetRightNode(KDNode* inRightNode) { mRightNode = inRightNode; }

public:
	Actor*			GetActor()		const { return mActor; }
	const uint32&	GetDepth()		const { return mDepth; }
	KDNode*			GetLeftNode()	const { return mLeftNode; }
	KDNode*			GetRightNode()	const { return mRightNode; }

private:
	Actor*	mActor;
	uint32	mDepth;

	KDNode* mLeftNode;
	KDNode* mRightNode;
};

class KDTree : public ICollisionSystem
{
public:
	KDTree();
	~KDTree();

private:
	KDTree(const KDTree&) = delete;
	KDTree(KDTree&&) noexcept = delete;

	KDTree& operator=(const KDTree&) = delete;
	KDTree& operator=(KDTree&&) noexcept = delete;

public: //ISearch
	virtual void Init() override;
	virtual void Destroy() override;

	virtual void Insert(Actor* Actor) override;
	virtual void Remove(Actor* Actor) override;
	virtual void Build() override;

	virtual std::vector<Actor*> Search(Attack& InAttack) override;
	virtual std::vector<Actor*> AllSearch() override;

	virtual void Draw(sf::VertexArray& OutVertexArray) override;

private:
	KDNode* InsertNode(std::vector<Actor*> inActors, const uint16 inDepth);
	void DeleteNode(KDNode* inNode);

	void RangeSearch(const sf::Vector2f& inFindLocation, const float& inRadius, KDNode* inNode, std::vector<KDNode*>& OutNodes, const uint16& inDepth);

	void DrawNode(sf::VertexArray& OutVertexArray, KDNode* InNode, float xMin, float xMax, float yMin, float yMax);

private:
	enum KD_TREE : uint16
	{
		INIT_DEPTH = 0,
		INCREASE_DEPTH = 1,
		MAX_DEPTH = 2,
		X = 0,
		Y = 1,
	};

private:
	KDNode* mRoot;
	std::vector<Actor*> mActors;
};

