#pragma once

class Quadtree;

enum class QNodeIndex : int 
{
    UPPERLEFT = 0,
    UPPERRIGHT,
    LOWERLEFT,
    LOWERRIGHT,
    STRADDLING,
    OUTOFAREA
};

class QNode 
{
public:
    QNode(Quadtree* tree, QNode* parent, const sf::FloatRect& bounds, int depth);

    void Insert(Actor* item);
    QNodeIndex TestRegion(const sf::FloatRect& bounds);
    void Query(Actor* item, std::vector<QNode*>& possibleNodes);
    void Clear();
    void CollectAllNodes(std::unordered_map<int, std::vector<QNode*>>& allNodes);
    void DrawBounds(sf::VertexArray& OutVertexArray);

    std::vector<std::unique_ptr<QNode>> children;
    std::vector<Actor*> items;
    int depth;
    sf::FloatRect bounds;

private:
    std::vector<QNodeIndex> GetQuads(const sf::FloatRect& Rect);
    bool Split();
    bool IsSplitted() const;

    Quadtree* tree;
    QNode* parent;
};

class Quadtree 
{
public:
    Quadtree(const sf::Vector2f& size, int maxDepth);

    void Insert(Actor* insertItem);
    void Remove(Actor* removeItem);
    std::vector<Actor*> Query(Actor* queryItem);
    void Clear();
    std::unordered_map<int, std::vector<QNode*>> GetAllNodes();
    void DrawBounds(sf::VertexArray& OutVertexArray);

    int GetMaxDepth() const;

private:
    std::unique_ptr<QNode> rootNode;
    int maxDepth = 5;
};

class QuadtreeManager : public ICollisionSystem 
{
public:
    QuadtreeManager();
    ~QuadtreeManager();

    virtual void Init() override;
    virtual void Destroy() override;

    virtual void Insert(Actor* Actor) override;
    virtual void Remove(Actor* Actor) override;
    virtual void Build() override;

    virtual std::vector<Actor*> Search(Attack& InAttack) override;
    virtual std::vector<Actor*> AllSearch() override;

    virtual void Draw(sf::VertexArray& OutVertexArray) override;

private:
    sf::Vector2f totalArea;

    std::unique_ptr<Quadtree> tree;
    std::vector<Actor*> mActors;
};