#pragma once

class KDNode
{
public:
	KDNode(const uint32& inNodeNumber) : mActor(nullptr), mNodeNumber(inNodeNumber), mDepth(0), mParentNode(nullptr), mLeftNode(nullptr), mRightNode(nullptr) {}
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
		mParentNode = nullptr;
		mLeftNode = nullptr;
		mRightNode = nullptr;
		mDepth = 0;
		mDirty = false;
	}

	void InitNode(Actor* inActor)
	{
		this->SetActor(inActor);
		this->SetDepth(0);
		this->SetDirty(false);
		this->SetParentNode(nullptr);
		this->SetLeftNode(nullptr);
		this->SetRightNode(nullptr);
	}

	void SetActor(Actor* inActor) { mActor = inActor; };
	void SetDepth(const uint32& InDepth) { mDepth = InDepth; }
	void SetDirty(const bool& InDirty) { mDirty = InDirty; }
	void SetParentNode(KDNode* InParentNode) { mParentNode = InParentNode; }
	void SetLeftNode(KDNode* inLeftNode) { mLeftNode = inLeftNode; }
	void SetRightNode(KDNode* inRightNode) { mRightNode = inRightNode; }

public:
	Actor*			GetActor()		const { return mActor; }
	const uint32&	GetNodeNumber() const { return mNodeNumber; }
	const uint32&	GetDepth()		const { return mDepth; }
	const bool&		GetDirty()		const { return mDirty; }
	KDNode*			GetParentNode()	const { return mParentNode; }
	KDNode*			GetLeftNode()	const { return mLeftNode; }
	KDNode*			GetRightNode()	const { return mRightNode; }

private:
	Actor*	mActor;
	uint32	mNodeNumber;
	uint32	mDepth;

	bool	mDirty;

	KDNode* mParentNode;
	KDNode* mLeftNode;
	KDNode* mRightNode;
};

class KDTree : public ICollisionSystem
{
public:
	KDTree(const uint32& inPoolSize);
	~KDTree();

private:
	KDTree(const KDTree&) = delete;
	KDTree(KDTree&&) noexcept = delete;

	KDTree& operator=(const KDTree&) = delete;
	KDTree& operator=(KDTree&&) noexcept = delete;

public: //ISearch
	virtual void Init() override;
	virtual void Build(std::vector<Actor*>& InActors) override;
	virtual void Destroy() override;
	virtual std::vector<Actor*> Search(Attack& InAttack) override;
	virtual std::vector<Actor*> AllSearch() override;
	virtual void Draw(sf::RenderWindow* InWindow) override;
	virtual void Draw(sf::VertexArray& OutVertexArray) override;

private:
	void InsertNode(KDNode* inParentNode, KDNode* inNode, const uint16& inDepth);
	void InsertNode(KDNode* inParentNode, KDNode* InNode);

	void DeleteNode(KDNode* inNode);

	void SearchNode(const sf::Vector2f& inFindLocation, const float& inRadius, KDNode* inNode, std::vector<int64>& OutNodes, const uint16& inDepth);
	void SearchNode(const sf::Vector2f& inFindLocation, const float& inRadius, KDNode* inNode, std::vector<int64>& OutNodes);
	void DrawNode(sf::RenderWindow* InWindow, KDNode* InNode, float xMin, float xMax, float yMin, float yMax);
	void DrawNode(sf::VertexArray& OutVertexArray, KDNode* InNode, float xMin, float xMax, float yMin, float yMax);


private:
	KDNode* NextAvailableNode();
	bool CompareLocation(const sf::Vector2f& inParentLocation, const sf::Vector2f& inNodeLocation, const uint16& inAxis);

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
	std::vector<KDNode*>	mNodesPool;
	std::vector<KDNode*>	mUseNode;
	std::deque<uint16>		mAvailableNodeNumber;
};

