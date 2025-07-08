#pragma once

struct Bounds {
    sf::Vector2f center;
    sf::Vector2f size;

    Bounds() : center(), size() {}
    Bounds(const  sf::Vector2f& center, const  sf::Vector2f& size) : center(center), size(size) {}

    sf::Vector2f min() const 
    {
        return  sf::Vector2f(center.x - size.x * 0.5f, center.y - size.y * 0.5f);
    }

    sf::Vector2f max() const 
    {
        return  sf::Vector2f(center.x + size.x * 0.5f, center.y + size.y * 0.5f);
    }

    sf::Vector2f extents() const {
        return size * 0.5f;
    }

    bool Intersects(const Bounds& other) const 
    {
        return !(min().x > other.max().x || max().x < other.min().x || min().y > other.max().y || max().y < other.min().y);
    }
};

class Quadtree;

enum class QNodeIndex : int 
{
    UPPERLEFT = 0,
    UPPERRIGHT,
    LOWERRIGHT,
    LOWERLEFT,
    STRADDLING,
    OUTOFAREA
};

class QNode 
{
public:
    QNode(Quadtree* tree, QNode* parent, const Bounds& bounds, int depth);

    void Insert(Actor* item);
    QNodeIndex TestRegion(const Bounds& bounds);
    void Query(Actor* item, std::vector<QNode*>& possibleNodes);
    void Clear();
    void CollectAllNodes(std::unordered_map<int, std::vector<QNode*>>& allNodes);
    void DrawBounds(sf::VertexArray& OutVertexArray);

    std::vector<std::unique_ptr<QNode>> children;
    std::vector<Actor*> items;
    int depth;
    Bounds bounds;

private:
    std::vector<QNodeIndex> GetQuads(const Bounds& Rect);
    bool Split();
    bool IsSplitted() const;

    Quadtree* tree;
    QNode* parent;
};

class Quadtree 
{
public:
    Quadtree(const sf::Vector2f& size);

    void Insert(Actor* insertItem);
    std::vector<Actor*> Query(Actor* queryItem, std::vector<QNode*>& possibleNodes);
    void Clear();
    std::unordered_map<int, std::vector<QNode*>> GetAllNodes();
    void DrawBounds(sf::VertexArray& OutVertexArray);

    int GetMaxDepth() const { return maxDepth; }

private:
    std::unique_ptr<QNode> rootNode;
    int maxDepth = 5;
};

class QuadtreeManager : public ICollisionSystem 
{
public:
    QuadtreeManager(sf::Vector2f TotalArea);
    ~QuadtreeManager();

    virtual void Init() override;
    virtual void Build(std::vector<Actor*>& InActors) override;
    virtual void Destroy() override;
    virtual std::vector<Actor*> Search(Attack& InAttack) override;
    virtual std::vector<Actor*> AllSearch() override;
    virtual void Draw(sf::RenderWindow* InWindow) override;
    virtual void Draw(sf::VertexArray& OutVertexArray) override;

private:
    sf::Vector2f totalArea;
    std::unique_ptr<Quadtree> tree;

    int iIndex = 0;
    std::vector<Actor*> insertObjects;
};