#include "pch.h"
#include "QuadTree.h"

QNode::QNode(Quadtree* tree, QNode* parent, const Bounds& bounds, int depth)
    : tree(tree), parent(parent), bounds(bounds), depth(depth) {
}

void QNode::Insert(Actor* item)
{
    sf::FloatRect rect = item->GetLocalBound();
    Bounds itemBounds;
    itemBounds.center = rect.position;
    itemBounds.size = rect.size;

    QNodeIndex result = TestRegion(itemBounds);

    if (result == QNodeIndex::STRADDLING) 
    {
        items.push_back(item); // 경계에 걸친 경우
    }
    else if (result != QNodeIndex::OUTOFAREA) 
    {
        if (Split()) 
        {
            children[static_cast<int>(result)]->Insert(item);
        }
        else 
        {
            items.push_back(item); // 더 이상 분할 불가
        }
    }
}

QNodeIndex QNode::TestRegion(const Bounds& bounds)
{
    std::vector<QNodeIndex> quads = GetQuads(bounds);

    if (quads.size() == 0) {
        return QNodeIndex::OUTOFAREA;
    }
    else if (quads.size() == 1) {
        return quads[0];
    }
    else {
        return QNodeIndex::STRADDLING;
    }
}

void QNode::Query(Actor* item, std::vector<QNode*>& possibleNodes) 
{
    possibleNodes.push_back(this);

    if (IsSplitted()) 
    {
        sf::FloatRect rect = item->GetLocalBound();

        Bounds bounds;
        bounds.center = rect.position;
        bounds.size = rect.size;

        std::vector<QNodeIndex> quads = GetQuads(bounds);
        for (auto index : quads) 
        {
            children[static_cast<int>(index)]->Query(item, possibleNodes);
        }
    }
}

std::vector<QNodeIndex> QNode::GetQuads(const Bounds& bounds) {
    std::vector<QNodeIndex> quads;

    sf::Vector2f nodeCenter = this->bounds.center;
    sf::Vector2f objMin = bounds.min();
    sf::Vector2f objMax = bounds.max();

    bool left   = objMin.x < nodeCenter.x;  
    bool right  = objMax.x > nodeCenter.x; 
    bool top    = objMin.y < nodeCenter.y; 
    bool bottom = objMax.y > nodeCenter.y;

    if (left && top) {
        quads.push_back(QNodeIndex::UPPERLEFT);     // 좌상단
    }

    if (right && top) {
        quads.push_back(QNodeIndex::UPPERRIGHT);    // 우상단
    }

    if (right && bottom) {
        quads.push_back(QNodeIndex::LOWERRIGHT);    // 우하단
    }

    if (left && bottom) {
        quads.push_back(QNodeIndex::LOWERLEFT);     // 좌하단
    }

    return quads;
}

bool QNode::Split() {
    if (depth == tree->GetMaxDepth()) {
        return false;
    }

    if (!IsSplitted()) 
    {
        sf::Vector2f newBoundsSize = bounds.size * 0.5f;
        sf::Vector2f offset = newBoundsSize * 0.5f;

        children.resize(4);

        // 화면 좌표계 기준으로 자식 노드들 배치
        // UPPERLEFT [0] - 좌상단 (center - offset.x, center - offset.y)
        children[0] = std::make_unique<QNode>(tree, this,
            Bounds(sf::Vector2f(bounds.center.x - offset.x, bounds.center.y - offset.y), newBoundsSize),
            depth + 1);

        // UPPERRIGHT [1] - 우상단 (center + offset.x, center - offset.y)
        children[1] = std::make_unique<QNode>(tree, this,
            Bounds(sf::Vector2f(bounds.center.x + offset.x, bounds.center.y - offset.y), newBoundsSize),
            depth + 1);

        // LOWERRIGHT [2] - 우하단 (center + offset.x, center + offset.y)
        children[2] = std::make_unique<QNode>(tree, this,
            Bounds(sf::Vector2f(bounds.center.x + offset.x, bounds.center.y + offset.y), newBoundsSize),
            depth + 1);

        // LOWERLEFT [3] - 좌하단 (center - offset.x, center + offset.y)
        children[3] = std::make_unique<QNode>(tree, this,
            Bounds(sf::Vector2f(bounds.center.x - offset.x, bounds.center.y + offset.y), newBoundsSize),
            depth + 1);
    }

    return true;
}

void QNode::Clear() 
{
    if (!children.empty()) 
    {
        for (auto& node : children) 
        {
            node->Clear();
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
            node->CollectAllNodes(allNodes);
        }
    }
}

void QNode::DrawBounds(sf::VertexArray& OutVertexArray) 
{
    // 시각화 코드는 사용하는 그래픽 라이브러리에 따라 구현
    // 예: OpenGL, DirectX, SFML 등

    sf::Vector2f min = bounds.min();
    sf::Vector2f max = bounds.max();

    OutVertexArray.append({ sf::Vector2f(min.x, min.y), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(max.x, min.y), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(max.x, min.y), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(max.x, max.y), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(max.x, max.y), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(min.x, max.y), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(min.x, max.y), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(min.x, min.y), sf::Color::White });

    if (IsSplitted()) 
    {
        for (auto& node : children) 
        {
            node->DrawBounds(OutVertexArray);
        }
    }
}

bool QNode::IsSplitted() const {
    return !children.empty() && children.size() > 0;
}

Quadtree::Quadtree(const sf::Vector2f& size) 
{
    sf::Vector2f center = size * 0.5f;
    rootNode = std::make_unique<QNode>(this, nullptr, Bounds(center, size), 0);
}

void Quadtree::Insert(Actor* insertItem) {
    rootNode->Insert(insertItem);
}

std::vector<Actor*> Quadtree::Query(Actor* queryItem, std::vector<QNode*>& possibleNodes) 
{
    rootNode->Query(queryItem, possibleNodes);

    std::vector<Actor*> intersects;
    for (auto node : possibleNodes) 
    {
        for (auto item : node->items) 
        {
            if (item->GetLocalBound().findIntersection(queryItem->GetLocalBound()))
            {
                intersects.push_back(item);
            }
        }
    }

    return intersects;
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

void Quadtree::DrawBounds(sf::VertexArray& OutVertexArray) {
    rootNode->DrawBounds(OutVertexArray);
}

QuadtreeManager::QuadtreeManager(sf::Vector2f TotalArea) : totalArea(TotalArea)
{
}

QuadtreeManager::~QuadtreeManager() {
}

void QuadtreeManager::Init() 
{
    if (tree == nullptr) 
    {
        tree = std::make_unique<Quadtree>(totalArea);
    }
    else 
    {
        tree->Clear();
    }
}

void QuadtreeManager::Build(std::vector<Actor*>& InActors)
{
    // 모든 Actor들을 쿼드트리에 삽입 (아직 안 되어있다면)
    insertObjects.clear();
    tree->Clear();

    insertObjects = InActors;
    iIndex = 0;
    for (size_t ix = 0; ix < insertObjects.size(); ++ix) 
    {
        if (iIndex >= insertObjects.size()) 
        {
            return;
        }
        tree->Insert(insertObjects[iIndex++]);
    }
}

void QuadtreeManager::Destroy()
{
}

std::vector<Actor*> QuadtreeManager::Search(Attack& InAttack)
{
    std::vector<QNode*> possibleNodes;
    std::vector<Actor*> candidateTargets = tree->Query(&InAttack, possibleNodes);

    for (Actor* target : candidateTargets) 
    {
        if (!target) continue;
 
        if (Detection::CheckCollision(target, &InAttack))
        {
            target->EnterOverlap();
        }
    }

    return std::vector<Actor*>();
}

std::vector<Actor*> QuadtreeManager::AllSearch()
{
    if (insertObjects.size() == 0)
    {
        return std::vector<Actor*>();
    }

    // 쿼드트리를 이용한 최적화된 충돌 검사
    std::vector<std::pair<Actor*, Actor*>> collisions;

    // 중복 검사 방지를 위한 집합
    std::unordered_set<std::string> checkedPairs;

    for (Actor* actor : insertObjects)
    {
        if (!actor) continue;

        // 현재 Actor와 교차할 가능성이 있는 노드들 찾기
        std::vector<QNode*> possibleNodes;
        std::vector<Actor*> nearbyActors = tree->Query(actor, possibleNodes);

        // 근처 Actor들과 충돌 검사
        for (Actor* otherActor : nearbyActors) 
        {
            if (!otherActor || actor == otherActor)
            {
                continue;
            }

            // 중복 검사 방지 (A-B와 B-A는 같은 검사)
            std::string pairKey = "";
            if (actor->GetActorId() < otherActor->GetActorId())
            {
                pairKey = std::to_string(actor->GetActorId()) + "-" + std::to_string(otherActor->GetActorId());
            }
            else
            {
                pairKey = std::to_string(otherActor->GetActorId()) + "-" + std::to_string(actor->GetActorId());
            }
            if (checkedPairs.find(pairKey) != checkedPairs.end()) {
                continue;
            }
            checkedPairs.insert(pairKey);

            if (Detection::CheckCollision(actor, otherActor))
            {
                actor->EnterOverlap();
                otherActor->EnterOverlap();
            }
        }
    }

    return std::vector<Actor*>();
}

void QuadtreeManager::Draw(sf::RenderWindow* InWindow)
{
}

void QuadtreeManager::Draw(sf::VertexArray& OutVertexArray)
{
    tree->DrawBounds(OutVertexArray);
}