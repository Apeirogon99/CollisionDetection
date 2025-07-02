#pragma once

class LQuadtree;

enum LQNodeIndex : int
{
    UPPERLEFT = 0,  // Completely contained in the upper left quadrant
    UPPERRIGHT,     // Completely contained in the upper right quadrant
    LOWERRIGHT,     // Completely contained in the lower right quadrant
    LOWERLEFT,      // Completely contained in the lower left quadrant
    STRADDLING,     // Straddling the boundary
    OUTOFAREA       // Out of area (input error)
};

class LQNode : public std::enable_shared_from_this<LQNode>
{
public:
    LQNode(LQuadtree* tree, std::shared_ptr<LQNode> parent, const sf::Rect<float>& bounds, int depth);

    std::vector<std::shared_ptr<LQNode>> _children;
    std::vector<Actor*> _items;
    int _depth = 0;

    LQuadtree* _tree;
    std::weak_ptr<LQNode> _parent;
    sf::Rect<float> _bounds;
    sf::Rect<float> _qbounds;

    /**
     * Insert an item at the specified depth
     */
    void InsertAtDepth(Actor* item, int targetDepth);

    void RemoveAtDepth(Actor* item, int targetDepth);

    /**
     * Query the node for items that potentially intersect with the query item
     */
    void Query(Actor* item, std::vector<std::shared_ptr<LQNode>>& possibleNodes);

    /**
     * Get quadrants that intersect with the given bounds
     */
    std::vector<std::shared_ptr<LQNode>> GetQuads(const sf::Rect<float>& bounds);

    /**
     * Test which region the bounds belong to
     */
    LQNodeIndex TestRegion(const sf::Rect<float>& bounds);

    /**
     * Split the node into four children
     */
    bool Split();

    void Merge();

    /**
     * Clear all items and children
     */
    void Clear();

    /**
     * Collect all nodes by depth
     */
    void CollectAllNodes(std::unordered_map<int, std::vector<std::shared_ptr<LQNode>>>& allNodes);

    /**
     * Draw the bounds of the node (for debugging)
     */
    void DrawBounds(sf::VertexArray& OutVertexArray);

    /**
     * Check if the node is split
     */
    bool IsSplitted() const;
};

class LQuadtree
{
public:
    std::shared_ptr<LQNode> _rootNode;
    int _maxDepth = 5;
    float _constantK = 4.0f;

    /**
     * Create root node
     * @param size Bounding volume size
     */
    LQuadtree(const sf::Vector2f& size);

    /**
     * Calculate the target depth for insertion
     */
    int GetTargetDepth(Actor* item);

    /**
     * Insert an item into the quadtree
     */
    void Insert(Actor* insertItem);

    void Remove(Actor* removeItem);

    /**
     * Query the quadtree for items that potentially intersect with the query item
     */
    std::vector<Actor*> Query(Actor* queryItem, std::vector<std::shared_ptr<LQNode>>& possibleNodes);

    /**
     * Clear all allocated nodes
     */
    void Clear();

    /**
     * Get all nodes by depth
     */
    std::unordered_map<int, std::vector<std::shared_ptr<LQNode>>> GetAllNodes();

    /**
     * Draw the bounds of the quadtree (for debugging)
     */
    void DrawBounds(sf::VertexArray& OutVertexArray);
};

class QuadTree : public ICollisionSystem
{
public:
	QuadTree(const sf::Vector2f& InArea);
	~QuadTree();

private:
	QuadTree(const QuadTree&) = delete;
	QuadTree(QuadTree&&) = delete;

	QuadTree& operator=(const QuadTree&) = delete;
	QuadTree& operator=(QuadTree&&) = delete;

public:	//ISearch
	virtual void Init() override;
	virtual void Build(std::vector<Actor*>& InActors) override;
	virtual void Destroy() override;
	virtual std::vector<Actor*> Search(Attack& InAttack) override;
	virtual std::vector<Actor*> AllSearch() override;
	virtual void Draw(sf::RenderWindow* InWindow) override;
	virtual void Draw(sf::VertexArray& OutVertexArray) override;

public:
    sf::Vector2f _totalArea = sf::Vector2f(64.0f, 64.0f);
    std::unique_ptr<LQuadtree> _tree = nullptr;

private:
    std::vector<Actor*> _insertObjects;
    std::vector<std::shared_ptr<LQNode>> _possibleNodes;
};
