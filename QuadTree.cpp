#include "pch.h"
#include "QuadTree.h"

LQNode::LQNode(LQuadtree* tree, std::shared_ptr<LQNode> parent, const sf::Rect<float>& bounds, int depth)
    : _tree(tree)
    , _parent(parent ? parent->weak_from_this() : std::weak_ptr<LQNode>())
    , _bounds(bounds)
    , _depth(depth)
{
    _qbounds = bounds;
    _qbounds.size = _qbounds.size * _tree->_constantK;
}

void LQNode::InsertAtDepth(Actor* item, int targetDepth)
{
    if (_depth < targetDepth) // If current depth is shallower than target depth, go deeper
    {
        if (Split())
        {
            _children[static_cast<int>(TestRegion(item->GetLocalBound()))]->InsertAtDepth(item, targetDepth);
        }
    }
    else if (_depth == targetDepth) // If current depth equals target depth, add to list
    {
        _items.push_back(item);
    }
}

void LQNode::RemoveAtDepth(Actor* item, int targetDepth)
{
    if (_depth < targetDepth) // If current depth is shallower than target depth, go deeper
    {
        if (IsSplitted())
        {
            _children[static_cast<int>(TestRegion(item->GetLocalBound()))]->RemoveAtDepth(item, targetDepth);
        }
    }
    else if (_depth == targetDepth) // If current depth equals target depth, add to list
    {
        auto it = std::find(_items.begin(), _items.end(), item);
        if (it != _items.end())
        {
            _items.erase(it);
        }

        if (_items.size() == 0)
        {
            Merge();
        }
    }
}

void LQNode::Query(Actor* item, std::vector<std::shared_ptr<LQNode>>& possibleNodes)
{
    possibleNodes.push_back(shared_from_this());

    if (IsSplitted())
    {
        std::vector<std::shared_ptr<LQNode>> quads = GetQuads(item->GetLocalBound());
        for (const auto& quad : quads)
        {
            quad->Query(item, possibleNodes);
        }
    }
}

std::vector<std::shared_ptr<LQNode>> LQNode::GetQuads(const sf::Rect<float>& bounds)
{
    std::vector<std::shared_ptr<LQNode>> quads;

    for (const auto& child : _children)
    {
        if (child->_qbounds.findIntersection(bounds))
        {
            quads.push_back(child);
        }
    }

    return quads;
}

LQNodeIndex LQNode::TestRegion(const sf::Rect<float>& bounds)
{
    bool negX = bounds.getCenter().x <= _bounds.getCenter().x;
    bool negY = bounds.getCenter().y >= _bounds.getCenter().y;
    bool posX = bounds.getCenter().x > _bounds.getCenter().x;
    bool posY = bounds.getCenter().y < _bounds.getCenter().y;

    if (negX && posY)
    {
        return LQNodeIndex::UPPERLEFT;
    }

    if (posX && posY)
    {
        return LQNodeIndex::UPPERRIGHT;
    }

    if (posX && negY)
    {
        return LQNodeIndex::LOWERRIGHT;
    }

    if (negX && negY)
    {
        return LQNodeIndex::LOWERLEFT;
    }

    return LQNodeIndex::OUTOFAREA;
}

bool LQNode::Split()
{
    if (_depth == _tree->_maxDepth) // Maximum depth reached
    {
        return false;
    }

    if (!IsSplitted())
    {
        sf::Vector2f newBoundPostion = { 0.0f, 0.0f };
        sf::Vector2f newBoundsSize = _bounds.size * 0.5f;
        _children.reserve(4);
        sf::Vector2f centerOffset = newBoundsSize;

        static sf::Vector2f Dir[4] =
        {
            {-1.0f, -1.0f},
            {+0.0f, -1.0f},
            {+0.0f, +0.0f},
            {-1.0f, +0.0f}
        };

        for (size_t index = 0; index < 4; ++index)
        {
            newBoundPostion.x = _bounds.getCenter().x + (centerOffset.x * Dir[index].x);
            newBoundPostion.y = _bounds.getCenter().y + (centerOffset.y * Dir[index].y);
            _children.push_back(std::make_shared<LQNode>(
                _tree,
                shared_from_this(),
                sf::Rect<float>(newBoundPostion, newBoundsSize),
                _depth + 1
            ));
        }
    }

    return true;
}

void LQNode::Merge()
{
    if (_items.empty() && !IsSplitted() && !_parent.expired())
    {
        // Check if all sibling nodes are also empty
        auto parent = _parent.lock();
        bool allChildrenEmpty = true;

        size_t itemCount = 0;
        for (const auto& child : parent->_children)
        {
            itemCount += child->_items.size();
            if (itemCount > 4.0f || child->IsSplitted())
            {
                allChildrenEmpty = false;
                break;
            }
        }

        // If all children are empty, collapse the parent
        if (allChildrenEmpty)
        {
            parent->_children.clear();

            // Recursively check parent
            parent->Merge();
        }
    }
}

void LQNode::Clear()
{
    if (!_children.empty())
    {
        for (auto& node : _children)
        {
            node->Clear();
        }
    }

    _items.clear();
    _children.clear();
}

void LQNode::CollectAllNodes(std::unordered_map<int, std::vector<std::shared_ptr<LQNode>>>& allNodes)
{
    if (allNodes.find(_depth) == allNodes.end())
    {
        allNodes[_depth] = std::vector<std::shared_ptr<LQNode>>();
    }
    allNodes[_depth].push_back(shared_from_this());

    if (IsSplitted())
    {
        for (auto& node : _children)
        {
            node->CollectAllNodes(allNodes);
        }
    }
}

void LQNode::DrawBounds(sf::VertexArray& OutVertexArray)
{
    // In C++, we'd need some equivalent to Unity's Handles.Label
    // This could be implemented with a rendering system specific to your environment

    if (IsSplitted())
    {
        static sf::Vector2f Dir[4] =
        {
            {-1.0f, -1.0f},
            {+1.0f, -1.0f},
            {+1.0f, +1.0f},
            {-1.0f, +1.0f}
        };

        sf::Vector2f newBoundPostion = { 0.0f, 0.0f };
        sf::Vector2f newBoundsSize = _bounds.size * 0.5f;
        sf::Vector2f centerOffset = newBoundsSize;

        for (int32 point = 0; point < 4; ++point)
        {
            sf::Vertex v1;
            newBoundPostion.x = _bounds.getCenter().x + (centerOffset.x * Dir[point].x);
            newBoundPostion.y = _bounds.getCenter().y + (centerOffset.y * Dir[point].y);
            v1.position = newBoundPostion;
            v1.color = sf::Color::Yellow;

            sf::Vertex v2;
            newBoundPostion.x = _bounds.getCenter().x + (centerOffset.x * Dir[(point + 1) % 4].x);
            newBoundPostion.y = _bounds.getCenter().y + (centerOffset.y * Dir[(point + 1) % 4].y);
            v2.position = newBoundPostion;
            v2.color = sf::Color::Yellow;

            OutVertexArray.append(v1);
            OutVertexArray.append(v2);

        }

        for (const auto& node : _children)
        {
            node->DrawBounds(OutVertexArray);
        }
    }
    else
    {
        if (!_items.empty())
        {
            static sf::Vector2f Dir[4] =
            {
                {-1.0f, -1.0f},
                {+1.0f, -1.0f},
                {+1.0f, +1.0f},
                {-1.0f, +1.0f}
            };

            sf::Vector2f newBoundPostion = { 0.0f, 0.0f };
            sf::Vector2f newBoundsSize = _bounds.size * 0.5f;
            sf::Vector2f centerOffset = newBoundsSize;

            for (int32 point = 0; point < 4; ++point)
            {
                sf::Vertex v1;
                newBoundPostion.x = _bounds.getCenter().x + (centerOffset.x * Dir[point].x);
                newBoundPostion.y = _bounds.getCenter().y + (centerOffset.y * Dir[point].y);
                v1.position = newBoundPostion;
                v1.color = sf::Color::White;

                sf::Vertex v2;
                newBoundPostion.x = _bounds.getCenter().x + (centerOffset.x * Dir[(point + 1) % 4].x);
                newBoundPostion.y = _bounds.getCenter().y + (centerOffset.y * Dir[(point + 1) % 4].y);
                v2.position = newBoundPostion;
                v2.color = sf::Color::White;

                OutVertexArray.append(v1);
                OutVertexArray.append(v2);

            }

            //static sf::Vector2f Dir[4] =
            //{
            //    {-1.0f, -1.0f},
            //    {+1.0f, -1.0f},
            //    {+1.0f, +1.0f},
            //    {-1.0f, +1.0f}
            //};

            //for (int32 point = 0; point < 4; ++point)
            //{
            //    sf::Vertex v1;
            //    v1.position = _bounds.position + sf::Vector2f(_bounds.size.x * Dir[point].x, _bounds.size.y * Dir[point].y);
            //    v1.color = sf::Color::White;

            //    sf::Vertex v2;
            //    v2.position = _bounds.position + sf::Vector2f(_bounds.size.x * Dir[(point + 1) % 4].x, _bounds.size.y * Dir[(point + 1) % 4].y);
            //    v2.color = sf::Color::White;

            //    OutVertexArray.append(v1);
            //    OutVertexArray.append(v2);

            //}
        }
    }
}

bool LQNode::IsSplitted() const
{
    return !_children.empty();
}

LQuadtree::LQuadtree(const sf::Vector2f& size)
{
    sf::Vector2f start({ 50.0f, 50.0f });
    sf::Vector2f sizeVec({ size.x, size.y });
    _rootNode = std::make_shared<LQNode>(this, nullptr, sf::Rect<float>(start, sizeVec), 0);
}

int LQuadtree::GetTargetDepth(Actor* item)
{
    // Predict the depth for the object being inserted
    float width = _rootNode->_bounds.size.x;
    float maxHalfValue = std::max(item->GetLocalBound().size.x, item->GetLocalBound().size.y);
    int targetDepth = static_cast<int>(std::floor(std::log2(width / maxHalfValue)));

    if (targetDepth > _maxDepth)
    {
        targetDepth = _maxDepth;
    }

    return targetDepth;
}

void LQuadtree::Insert(Actor* insertItem)
{
    _rootNode->InsertAtDepth(insertItem, GetTargetDepth(insertItem));
}

void LQuadtree::Remove(Actor* removeItem)
{
    _rootNode->RemoveAtDepth(removeItem, GetTargetDepth(removeItem));
}

std::vector<Actor*> LQuadtree::Query(Actor* queryItem, std::vector<std::shared_ptr<LQNode>>& possibleNodes)
{
    _rootNode->Query(queryItem, possibleNodes);
    std::vector<Actor*> intersects;

    for (const auto& node : possibleNodes)
    {
        for (auto* item : node->_items)
        {
            if (item->GetLocalBound().findIntersection(queryItem->GetLocalBound()))
            {
                intersects.push_back(item);
            }
        }
    }

    return intersects;
}

void LQuadtree::Clear()
{
    _rootNode->Clear();
}

std::unordered_map<int, std::vector<std::shared_ptr<LQNode>>> LQuadtree::GetAllNodes()
{
    std::unordered_map<int, std::vector<std::shared_ptr<LQNode>>> allNodes;
    _rootNode->CollectAllNodes(allNodes);
    return allNodes;
}

void LQuadtree::DrawBounds(sf::VertexArray& OutVertexArray)
{
    _rootNode->DrawBounds(OutVertexArray);
}

QuadTree::QuadTree(const sf::Vector2f& InArea) : _totalArea(InArea)
{
    if (_tree == nullptr)
    {
        _tree = std::make_unique<LQuadtree>(InArea);
    }
    else
    {
        _tree->Clear();
    }
}

QuadTree::~QuadTree()
{
}

void QuadTree::Init()
{
}

void QuadTree::Build(std::vector<Actor*>& InActors)
{
    for (Actor* actor : InActors)
    {
        auto it = find(_insertObjects.begin(), _insertObjects.end(), actor);
        if (it == _insertObjects.end())
        {
            _insertObjects.push_back(actor);
        }
        else
        {
            
        }
    }

    for (Actor* actor : _insertObjects)
    {
        _tree->Insert(actor);
    }

    //for (auto nodes : _tree->GetAllNodes())
    //{
    //    cout << nodes.first << " : ";

    //    size_t sumItem = 0;
    //    for (std::shared_ptr<LQNode> node : nodes.second)
    //    {
    //        sumItem += node->_items.size();
    //    }

    //    cout << sumItem << endl;
    //}

}

void QuadTree::Destroy()
{
}

std::vector<Actor*> QuadTree::Search(Attack& InAttack)
{
    _possibleNodes.clear();

    std::vector<Actor*> overlap;
    for (Actor* actor : _tree->Query(&InAttack, _possibleNodes))
    {
        //SATResult result = SAT::CheckCollision(*InAttack.GetShape(), *actor->GetShape());
        //if (result.colliding)
        //{
        //    overlap.push_back(actor);
        //    actor->EnterOverlap();
        //}

        if (Detection::CheckCollision(&InAttack, actor))
        {
            overlap.push_back(actor);
            actor->EnterOverlap();
        }
    }
    return overlap;
}

std::vector<Actor*> QuadTree::AllSearch()
{
    std::vector<Actor*> overlap;
    for (size_t index = 0; index < _insertObjects.size(); ++index)
    {
        _possibleNodes.clear();

        Actor* actorA = _insertObjects[index];
        for (Actor* actorB : _tree->Query(actorA, _possibleNodes))
        {
            if (actorA == actorB) continue;

#if USE_SAT
            SATResult result = SAT::CheckCollision(*actorA->GetShape(), *actorB->GetShape());
            if (result.colliding)
            {
                overlap.push_back(actorB);
                actorB->EnterOverlap();
            }
#else
            if (Detection::CheckCollision(actorA, actorB))
            {
                overlap.push_back(actorB);
                actorB->EnterOverlap();
            }
#endif
        }
    }

    for (Actor* actor : _insertObjects)
    {
        _tree->Remove(actor);
    }

    return overlap;
}

void QuadTree::Draw(sf::RenderWindow* InWindow)
{
}

void QuadTree::Draw(sf::VertexArray& OutVertexArray)
{

    static sf::Vector2f Dir[4] =
    {
        {-1.0f, -1.0f},
        {+1.0f, -1.0f},
        {+1.0f, +1.0f},
        {-1.0f, +1.0f}
    };

    sf::Rect<float> _bounds({ 50.0f, 50.0f }, _totalArea);
    sf::Vector2f newBoundPostion = { 0.0f, 0.0f };
    sf::Vector2f newBoundsSize = _bounds.size * 0.5f;
    sf::Vector2f centerOffset = newBoundsSize;


    for (int32 point = 0; point < 4; ++point)
    {
        sf::Vertex v1;
        newBoundPostion.x = _bounds.getCenter().x + (centerOffset.x * Dir[point].x);
        newBoundPostion.y = _bounds.getCenter().y + (centerOffset.y * Dir[point].y);
        v1.position = newBoundPostion;
        v1.color = sf::Color::White;

        sf::Vertex v2;
        newBoundPostion.x = _bounds.getCenter().x + (centerOffset.x * Dir[(point + 1) % 4].x);
        newBoundPostion.y = _bounds.getCenter().y + (centerOffset.y * Dir[(point + 1) % 4].y);
        v2.position = newBoundPostion;
        v2.color = sf::Color::White;

        OutVertexArray.append(v1);
        OutVertexArray.append(v2);

    }

    if (_tree != nullptr)
    {
        _tree->DrawBounds(OutVertexArray);
    }
}
