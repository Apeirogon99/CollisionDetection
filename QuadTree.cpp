#include "pch.h"
#include "QuadTree.h"

QNode::QNode(Quadtree* tree, QNode* parent, const sf::FloatRect& bounds, int depth)
    : tree(tree), parent(parent), bounds(bounds), depth(depth)
{
}

void QNode::Insert(Actor* item)
{
    sf::FloatRect rect = item->GetLocalBound();
    QNodeIndex result = TestRegion(rect);

    if (result == QNodeIndex::STRADDLING) 
    {
        items.push_back(item);
    }
    else if (result != QNodeIndex::OUTOFAREA) 
    {
        if (!IsSplitted()) 
        {
            if (items.size() >= 10 && Split()) 
            {
                children[static_cast<int>(result)]->Insert(item);
            }
            else 
            {
                items.push_back(item);
            }
        }
        else 
        {
            children[static_cast<int>(result)]->Insert(item);
        }
    }
}

QNodeIndex QNode::TestRegion(const sf::FloatRect& bounds)
{
    std::vector<QNodeIndex> quads = GetQuads(bounds);

    if (quads.size() == 0) 
    {
        return QNodeIndex::OUTOFAREA;
    }
    else if (quads.size() == 1)
    {
        return quads[0];
    }
    else
    {
        return QNodeIndex::STRADDLING;
    }
}

void QNode::Query(Actor* item, std::vector<QNode*>& possibleNodes)
{
    possibleNodes.push_back(this);

    if (IsSplitted())
    {
        std::vector<QNodeIndex> quads = GetQuads(item->GetLocalBound());
        for (auto index : quads)
        {
            children[static_cast<int>(index)]->Query(item, possibleNodes);
        }
    }
}

std::vector<QNodeIndex> QNode::GetQuads(const sf::FloatRect& bounds)
{
    std::vector<QNodeIndex> quads;

    sf::Vector2f nodeCenter = this->bounds.getCenter();
    float centerX = nodeCenter.x;
    float centerY = nodeCenter.y;

    float objLeft = bounds.position.x;
    float objRight = bounds.position.x + bounds.size.x;
    float objTop = bounds.position.y;
    float objBottom = bounds.position.y + bounds.size.y;

    if (objLeft < centerX && objTop < centerY) {
        quads.push_back(QNodeIndex::UPPERLEFT);
    }

    if (objRight > centerX && objTop < centerY) {
        quads.push_back(QNodeIndex::UPPERRIGHT);
    }

    if (objLeft < centerX && objBottom > centerY) {
        quads.push_back(QNodeIndex::LOWERLEFT);
    }

    if (objRight > centerX && objBottom > centerY) {
        quads.push_back(QNodeIndex::LOWERRIGHT);
    }

    return quads;
}

bool QNode::Split()
{
    if (IsSplitted() || depth >= tree->GetMaxDepth())
    {
        return false;
    }

    sf::Vector2f childSize = bounds.size * 0.5f;
    children.resize(4);

    children[0] = std::make_unique<QNode>(tree, this,
        sf::FloatRect(bounds.position, childSize),
        depth + 1);

    children[1] = std::make_unique<QNode>(tree, this,
        sf::FloatRect(sf::Vector2f(bounds.position.x + childSize.x, bounds.position.y), childSize),
        depth + 1);

    children[2] = std::make_unique<QNode>(tree, this,
        sf::FloatRect(sf::Vector2f(bounds.position.x, bounds.position.y + childSize.y), childSize),
        depth + 1);

    children[3] = std::make_unique<QNode>(tree, this,
        sf::FloatRect(sf::Vector2f(bounds.position.x + childSize.x, bounds.position.y + childSize.y), childSize),
        depth + 1);

    return true;
}

void QNode::Clear()
{
    if (!children.empty())
    {
        for (auto& node : children)
        {
            if (node) {
                node->Clear();
            }
        }
    }

    items.clear();
    children.clear();
}

void QNode::CollectAllNodes(std::unordered_map<int, std::vector<QNode*>>& allNodes) {
    if (allNodes.find(depth) == allNodes.end()) {
        allNodes[depth] = std::vector<QNode*>();
    }
    allNodes[depth].push_back(this);

    if (IsSplitted()) {
        for (auto& node : children) {
            if (node) {
                node->CollectAllNodes(allNodes);
            }
        }
    }
}

void QNode::DrawBounds(sf::VertexArray& OutVertexArray)
{
    float left = bounds.position.x;
    float top = bounds.position.y;
    float right = bounds.position.x + bounds.size.x;
    float bottom = bounds.position.y + bounds.size.y;

    OutVertexArray.append({ sf::Vector2f(left, top), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(right, top), sf::Color::White });

    OutVertexArray.append({ sf::Vector2f(right, top), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(right, bottom), sf::Color::White });

    OutVertexArray.append({ sf::Vector2f(right, bottom), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(left, bottom), sf::Color::White });

    OutVertexArray.append({ sf::Vector2f(left, bottom), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(left, top), sf::Color::White });

    if (IsSplitted())
    {
        for (auto& child : children)
        {
            child->DrawBounds(OutVertexArray);
        }
    }
}

bool QNode::IsSplitted() const
{
    return !children.empty();
}

Quadtree::Quadtree(const sf::Vector2f& size, int maxDepth)
    : maxDepth(maxDepth)
{
    rootNode = std::make_unique<QNode>(this, nullptr, sf::FloatRect({ 0.0f, 0.0f }, size), 0);
}

void Quadtree::Insert(Actor* insertItem)
{
    if (insertItem) {
        rootNode->Insert(insertItem);
    }
}

void Quadtree::Remove(Actor* removeItem)
{

}

std::vector<Actor*> Quadtree::Query(Actor* queryItem)
{
    std::vector<QNode*> possibleNodes;
    rootNode->Query(queryItem, possibleNodes);

    std::vector<Actor*> actors;
    for (QNode* node : possibleNodes) 
    {
        for (auto& item : node->items) 
        {
            if (Detection::AABB(queryItem->GetLocalBound(), item->GetLocalBound()))
            {
                actors.push_back(item);
            }
        }
    }
    return actors;
}

void Quadtree::Clear()
{
    rootNode->Clear();
}

std::unordered_map<int, std::vector<QNode*>> Quadtree::GetAllNodes() {
    std::unordered_map<int, std::vector<QNode*>> allNodes;
    rootNode->CollectAllNodes(allNodes);
    return allNodes;
}

void Quadtree::DrawBounds(sf::VertexArray& OutVertexArray)
{
    rootNode->DrawBounds(OutVertexArray);
}

int Quadtree::GetMaxDepth() const {
    return maxDepth;
}

QuadtreeManager::QuadtreeManager() : totalArea({ WINDOW_WIDTH, WINDOW_HEIGHT })
{
}

QuadtreeManager::~QuadtreeManager() {
}

void QuadtreeManager::Init()
{
    if (tree == nullptr)
    {
        tree = std::make_unique<Quadtree>(totalArea, 8);
    }
    else
    {
        tree->Clear();
    }
}

void QuadtreeManager::Destroy()
{
    if (tree) {
        tree->Clear();
    }
    mActors.clear();
}

void QuadtreeManager::Insert(Actor* actor)
{
    if (actor) {
        mActors.emplace_back(actor);
    }
}

void QuadtreeManager::Remove(Actor* actor)
{
    if (!actor) return;

    for (size_t index = 0; index < mActors.size(); ++index)
    {
        if (mActors[index] == actor)
        {
            mActors[index] = mActors.back();
            mActors.pop_back();
            break;
        }
    }
}

void QuadtreeManager::Build()
{
    if (!tree) return;

    tree->Clear();

    for (Actor* actor : mActors)
    {
        if (actor) 
        {
            tree->Insert(actor);
        }
    }
}

std::vector<Actor*> QuadtreeManager::Search(Attack& InAttack)
{
    std::vector<Actor*> overlap;
    if (!tree) return overlap;

   std::vector<Actor*> possibleActors = tree->Query(&InAttack);

    for (Actor* otherActor : possibleActors)
    {

#if USE_SAT
        SATResult result = SAT::CheckCollision(*InAttack.GetShape(), *target->GetShape());
        if (result.colliding)
        {
            overlap.emplace_back(target);
            target->EnterOverlap();
        }
#else
        if (Detection::CheckCollision(&InAttack, otherActor))
        {
            overlap.emplace_back(otherActor);
            otherActor->EnterOverlap();
        }
#endif
        
    }

    return overlap;
}

std::vector<Actor*> QuadtreeManager::AllSearch()
{
    if (!tree) return {};

    std::vector<Actor*> overlap;

    for (Actor* actor : mActors)
    {
        if (!actor) continue;

        std::vector<Actor*> possibleActors = tree->Query(actor);
        for (Actor* otherActor : possibleActors)
        {

            if (otherActor && otherActor != actor)
            {
#if USE_SAT
                SATResult result = SAT::CheckCollision(*actor->GetShape(), *otherActor->GetShape());
                if (result.colliding)
                {
                    overlap.emplace_back(actor);
                    actor->EnterOverlap();
                }
#else
                if (Detection::CheckCollision(actor, otherActor))
                {
                    overlap.emplace_back(actor);
                    actor->EnterOverlap();
                }
#endif
            }
            
        }
    }

    return overlap;
}

void QuadtreeManager::Draw(sf::VertexArray& OutVertexArray)
{
    if (tree) 
    {
        tree->DrawBounds(OutVertexArray);
    
        
        for (auto node : tree->GetAllNodes())
        {
            for (auto qnode : node.second)
            {
                cout << "ID : " << node.first << "ITEM : " << qnode->items.size() << endl;
            }
        }
    }
}