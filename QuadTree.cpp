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

    if (result == QNodeIndex::STRADDLING) {
        // 경계선에 걸친 객체는 현재 노드에 보관
        items.push_back(item);
    }
    else if (result != QNodeIndex::OUTOFAREA) {
        // 특정 사분면에 속하는 경우
        if (!IsSplitted()) {
            // 아직 분할되지 않은 경우: 아이템 수 확인 후 분할 결정
            if (items.size() >= 10 && Split()) {
                // 제한을 초과했고 분할 가능하면 분할 후 적절한 자식에게 위임
                children[static_cast<int>(result)]->Insert(item);
            }
            else {
                // 제한 내이거나 분할할 수 없으면 현재 노드에 보관
                items.push_back(item);
            }
        }
        else {
            // 이미 분할된 경우: 적절한 자식에게 위임
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

    // 수정: 현재 노드의 중심점을 기준으로 사분면 결정
    sf::Vector2f nodeCenter = this->bounds.getCenter();
    float centerX = nodeCenter.x;
    float centerY = nodeCenter.y;

    // 검사할 객체의 경계 계산
    float objLeft = bounds.position.x;
    float objRight = bounds.position.x + bounds.size.x;
    float objTop = bounds.position.y;
    float objBottom = bounds.position.y + bounds.size.y;

    // 각 사분면과의 교차 여부 확인
    // 경계선 상의 객체는 인접한 사분면들에 모두 포함될 수 있음

    // 좌상단 사분면 (UPPERLEFT)
    // 조건: 객체가 노드 중심의 왼쪽이면서 위쪽과 교차
    if (objLeft < centerX && objTop < centerY) {
        quads.push_back(QNodeIndex::UPPERLEFT);
    }

    // 우상단 사분면 (UPPERRIGHT)
    // 조건: 객체가 노드 중심의 오른쪽이면서 위쪽과 교차
    if (objRight > centerX && objTop < centerY) {
        quads.push_back(QNodeIndex::UPPERRIGHT);
    }

    // 좌하단 사분면 (LOWERLEFT)
    // 조건: 객체가 노드 중심의 왼쪽이면서 아래쪽과 교차
    if (objLeft < centerX && objBottom > centerY) {
        quads.push_back(QNodeIndex::LOWERLEFT);
    }

    // 우하단 사분면 (LOWERRIGHT)
    // 조건: 객체가 노드 중심의 오른쪽이면서 아래쪽과 교차
    if (objRight > centerX && objBottom > centerY) {
        quads.push_back(QNodeIndex::LOWERRIGHT);
    }

    return quads;
}

bool QNode::Split()
{
    // 이미 분할되어 있거나 최대 깊이에 도달한 경우 분할하지 않음
    if (IsSplitted() || depth >= tree->GetMaxDepth())
    {
        return false;
    }

    sf::Vector2f childSize = bounds.size * 0.5f;
    children.resize(4);

    // 화면 좌표계 기준으로 자식 노드들 배치
    // UPPERLEFT [0] - 좌상단 사분면
    children[0] = std::make_unique<QNode>(tree, this,
        sf::FloatRect(bounds.position, childSize),
        depth + 1);

    // UPPERRIGHT [1] - 우상단 사분면  
    children[1] = std::make_unique<QNode>(tree, this,
        sf::FloatRect(sf::Vector2f(bounds.position.x + childSize.x, bounds.position.y), childSize),
        depth + 1);

    // LOWERLEFT [2] - 좌하단 사분면
    children[2] = std::make_unique<QNode>(tree, this,
        sf::FloatRect(sf::Vector2f(bounds.position.x, bounds.position.y + childSize.y), childSize),
        depth + 1);

    // LOWERRIGHT [3] - 우하단 사분면
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

    // 사각형을 선으로 그리기 위해 8개의 점 추가
    // 상단 가로선
    OutVertexArray.append({ sf::Vector2f(left, top), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(right, top), sf::Color::White });

    // 우측 세로선
    OutVertexArray.append({ sf::Vector2f(right, top), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(right, bottom), sf::Color::White });

    // 하단 가로선
    OutVertexArray.append({ sf::Vector2f(right, bottom), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(left, bottom), sf::Color::White });

    // 좌측 세로선
    OutVertexArray.append({ sf::Vector2f(left, bottom), sf::Color::White });
    OutVertexArray.append({ sf::Vector2f(left, top), sf::Color::White });

    // 자식 노드들도 재귀적으로 그리기
    if (IsSplitted())
    {
        for (auto& child : children)
        {
            child->DrawBounds(OutVertexArray);
        }
    }
}

// 수정: 중복 검사 제거
bool QNode::IsSplitted() const
{
    return !children.empty();
}

// Quadtree 클래스 수정
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

// QuadtreeManager 클래스 수정
QuadtreeManager::QuadtreeManager() : totalArea({ WINDOW_WIDTH, WINDOW_HEIGHT })
{
}

QuadtreeManager::~QuadtreeManager() {
}

void QuadtreeManager::Init()
{
    if (tree == nullptr)
    {
        // 적절한 기본값 설정: 최대 깊이 8, 노드당 최대 아이템 수 10
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
                // 수정: 포인터 접근 방식 수정
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