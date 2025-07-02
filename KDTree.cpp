#include "pch.h"
#include "KDTree.h"

KDTree::KDTree(const uint32& inPoolSize)
{
	if (inPoolSize > UINT32_MAX)
	{
		wprintf(L"[KD-Tree] [Init] It's too big to make\n");
		return;
	}

	uint32 newPoolsize = inPoolSize;
	if (!((inPoolSize & (inPoolSize - 1)) == 0))
	{
		newPoolsize = static_cast<uint32>(pow(2, floor(log2(inPoolSize)) + 1));
	}

	for (uint32 index = 0; index < newPoolsize; ++index)
	{
		mNodesPool.push_back(new KDNode(index));
		mAvailableNodeNumber.push_back(index);
	}
}

KDTree::~KDTree()
{
	Destroy();
}

void KDTree::Init()
{
}

void KDTree::Build(std::vector<Actor*>& InActors)
{
	//기존 정보 삭제
	if (mUseNode.size() != 0)
	{
		DeleteNode(mUseNode[0]);
	}

	for (Actor* actor : InActors)
	{
		KDNode* nextNode = NextAvailableNode();
		if (nextNode == nullptr)
		{
			return;
		}
		nextNode->InitNode(actor);

		if (mUseNode.size() != 0)
		{
			InsertNode(mUseNode[0], nextNode, INIT_DEPTH);
		}
		mUseNode.push_back(nextNode);
	}
}

void KDTree::Destroy()
{
	for (KDNode* node : mNodesPool)
	{
		node->Clear();

		delete node;
		node = nullptr;
	}

	mNodesPool.clear();
	mUseNode.clear();
	mAvailableNodeNumber.clear();
}

std::vector<Actor*> KDTree::Search(Attack& InAttack)
{
	if (mUseNode.size() == 0)
	{
		return vector<Actor*>();
	}

	std::vector<int64> nodes;
	SearchNode(InAttack.GetLocation(), InAttack.GetLocalRadius(), mUseNode[0], nodes, INIT_DEPTH);

	vector<Actor*> overlap;
	for (int64 nodeId : nodes)
	{
		Actor* actor = mNodesPool[nodeId]->GetActor();
		//SATResult result = SAT::CheckCollision(*actor->GetShape(), *InAttack.GetShape());
		//if (result.colliding)
		//{
		//	overlap.push_back(actor);
		//	actor->EnterOverlap();
		//}

		if (Detection::CheckCollision(actor, &InAttack))
		{
			overlap.push_back(actor);
			actor->EnterOverlap();
		}
	}

	return overlap;
}

std::vector<Actor*> KDTree::AllSearch()
{
	if (mUseNode.size() == 0)
	{
		return vector<Actor*>();
	}

	vector<Actor*> overlap;
	for (KDNode* node : mUseNode)
	{
		Actor* A = node->GetActor();

		std::vector<int64> nodeIds;
		SearchNode(A->GetLocation(), A->GetLocalRadius(), mUseNode[0], nodeIds, INIT_DEPTH);

		if (nodeIds.size() <= 1)
		{
			continue;
		}

		for (const int64& id : nodeIds)
		{
			if (node->GetNodeNumber() == id) continue;
			Actor* B = mNodesPool[id]->GetActor();

#if USE_SAT
			SATResult result = SAT::CheckCollision(*A->GetShape(), *B->GetShape());
			if (result.colliding)
			{
				overlap.push_back(A);
				A->EnterOverlap();
			}
#else
			if (Detection::CheckCollision(A, B))
			{
				overlap.push_back(A);
				A->EnterOverlap();
			}
#endif
		}
	}


	return overlap;
}

void KDTree::Draw(sf::RenderWindow* InWindow)
{
	if (mUseNode.size() != 0)
	{
		sf::Vector2u windowSize = InWindow->getSize();
		const float windowX = static_cast<float>(WINDOW_WIDTH);
		const float windowY = static_cast<float>(WINDOW_HEIGHT);

		DrawNode(InWindow, mUseNode[0], 0.0f, windowX, 0.0f, windowY);
	}
}

void KDTree::Draw(sf::VertexArray& OutVertexArray)
{
	if (mUseNode.size() != 0)
	{
		const float windowX = static_cast<float>(WINDOW_WIDTH);
		const float windowY = static_cast<float>(WINDOW_HEIGHT);

		DrawNode(OutVertexArray, mUseNode[0], 0.0f, windowX, 0.0f, windowY);
	}
}

void KDTree::InsertNode(KDNode* inParentNode, KDNode* inNode, const uint16& inDepth)
{
	const uint32 nextAxis = (inDepth + KD_TREE::INCREASE_DEPTH) % KD_TREE::MAX_DEPTH;

	const sf::Vector2f& parentLocation = inParentNode->GetActor()->GetLocation();
	const sf::Vector2f& nodeLocation = inNode->GetActor()->GetLocation();

	bool result = CompareLocation(parentLocation, nodeLocation, inDepth);
	if (result)
	{
		if (inParentNode->GetLeftNode() == nullptr) { inParentNode->SetLeftNode(inNode); inNode->SetDepth(nextAxis); }
		else { InsertNode(inParentNode->GetLeftNode(), inNode, nextAxis); }
	}
	else
	{
		if (inParentNode->GetRightNode() == nullptr) { inParentNode->SetRightNode(inNode); inNode->SetDepth(nextAxis); }
		else { InsertNode(inParentNode->GetRightNode(), inNode, nextAxis); }
	}
}

void KDTree::InsertNode(KDNode* InParentNode, KDNode* InNode)
{
	std::deque<KDNode*> nodes;
	std::unordered_set<uint32> visit;

	InNode->SetParentNode(InParentNode);
	nodes.push_back(InNode);

	while (!nodes.empty())
	{
		KDNode* node = nodes.front(); nodes.pop_front();
		KDNode* parentNode = node->GetParentNode();

		if (node == nullptr || !node->IsValid()) continue;

		uint32 nodeId = node->GetNodeNumber();
		if (visit.find(nodeId) != visit.end()) continue;
		visit.insert(nodeId);

		const sf::Vector2f& parentLocation = parentNode->GetActor()->GetLocation();
		const sf::Vector2f& nodeLocation = node->GetActor()->GetLocation();
	
		uint32 curDepth = parentNode->GetDepth();
		uint32 nextDepth = (curDepth + KD_TREE::INCREASE_DEPTH) % KD_TREE::MAX_DEPTH;
		bool result = CompareLocation(parentLocation, nodeLocation, curDepth);
		if (result)
		{
			if (parentNode->GetLeftNode() == nullptr)
			{ 
				node->SetDepth(nextDepth);
				node->SetParentNode(parentNode);
				parentNode->SetLeftNode(node);
			}
			else 
			{ 
				if (parentNode->GetLeftNode() == node) continue;


				visit.erase(nodeId);
				node->SetParentNode(parentNode->GetLeftNode());
				nodes.push_back(node);
			}
		}
		else
		{
			if (parentNode->GetRightNode() == nullptr)
			{ 
				node->SetDepth(nextDepth);
				node->SetParentNode(parentNode);
				parentNode->SetRightNode(node);
			}
			else 
			{ 
				if (parentNode->GetRightNode() == node) continue;

				visit.erase(nodeId);
				node->SetParentNode(parentNode->GetRightNode());
				nodes.push_back(node);
			}
		}
	}
}

void KDTree::DeleteNode(KDNode* InNode)
{
	std::deque<KDNode*> nodes;
	nodes.push_back(InNode);

	while (!nodes.empty())
	{
		KDNode* node = nodes.front(); nodes.pop_front();

		if (node == nullptr || !node->IsValid()) continue;

		nodes.push_back(node->GetLeftNode());
		nodes.push_back(node->GetRightNode());

		mAvailableNodeNumber.push_back(node->GetNodeNumber());
	}

	for (KDNode* node : mUseNode)
	{
		node->Clear();
	}
	mUseNode.clear();

	//DeleteNode(inNode->GetLeftNode());
	//DeleteNode(inNode->GetRightNode());
}

void KDTree::SearchNode(const sf::Vector2f& inFindLocation, const float& inRadius, KDNode* InNode, std::vector<int64>& OutNodes, const uint16& inDepth)
{
	if (InNode == nullptr || !InNode->IsValid())
		return;
	InNode->SetDirty(true);

	Actor* actor = InNode->GetActor();
	const sf::Vector2f& location = actor->GetLocation();
	const float& radius = actor->GetLocalRadius();

	const float& distanceSquared = powf(inFindLocation.x - location.x, 2) + powf(inFindLocation.y - location.y, 2);
	const float radiusSum = powf(inRadius + radius, 2);
	if (distanceSquared <= radiusSum)
	{
		OutNodes.push_back(InNode->GetNodeNumber());
	}

	uint32 nextDepth = (inDepth + KD_TREE::INCREASE_DEPTH) % KD_TREE::MAX_DEPTH;
	if (inDepth == KD_TREE::X)
	{
		if (inFindLocation.x + inRadius > location.x) SearchNode(inFindLocation, inRadius, InNode->GetLeftNode(), OutNodes, nextDepth);
		if (inFindLocation.x - inRadius < location.x) SearchNode(inFindLocation, inRadius, InNode->GetRightNode(), OutNodes, nextDepth);
	}
	else //if (inDepth == KD_TREE::Y)
	{
		if (inFindLocation.y + inRadius > location.y) SearchNode(inFindLocation, inRadius, InNode->GetRightNode(), OutNodes, nextDepth);
		if (inFindLocation.y - inRadius < location.y) SearchNode(inFindLocation, inRadius, InNode->GetLeftNode(), OutNodes, nextDepth);
	}
}

void KDTree::SearchNode(const sf::Vector2f& inFindLocation, const float& inRadius, KDNode* InNode, std::vector<int64>& OutNodes)
{	
	std::deque<KDNode*> nodes;
	nodes.push_back(InNode);

	while (!nodes.empty())
	{
		KDNode* node = nodes.front(); nodes.pop_front();

		if (node == nullptr || !node->IsValid() || node->GetDirty()) continue;
		node->SetDirty(true);

		Actor* actor = node->GetActor();
		const sf::Vector2f& location = actor->GetLocation();
		const float& radius = actor->GetLocalRadius();
	
		const float& distanceSquared = powf(inFindLocation.x - location.x, 2) + powf(inFindLocation.y - location.y, 2);
		const float radiusSum = powf(inRadius + radius, 2);
		if (distanceSquared <= radiusSum)
		{
			OutNodes.push_back(node->GetNodeNumber());
		}

		//if (node->GetDepth() == KD_TREE::X)
		//{
		//	if (inFindLocation.x + inRadius > location.x) nodes.push_back(node->GetLeftNode());
		//	if (inFindLocation.x - inRadius < location.x) nodes.push_back(node->GetRightNode());
		//}
		//else //if (inDepth == KD_TREE::Y)
		//{
		//	if (inFindLocation.y + inRadius > location.y) nodes.push_back(node->GetRightNode());
		//	if (inFindLocation.y - inRadius < location.y) nodes.push_back(node->GetLeftNode());
		//}
	
		nodes.push_back(node->GetLeftNode());
		nodes.push_back(node->GetRightNode());
	}
}

void KDTree::DrawNode(sf::RenderWindow* InWindow, KDNode* InNode, float xMin, float xMax, float yMin, float yMax)
{
	if (InNode == nullptr) return;

	const sf::Vector2f& location = InNode->GetActor()->GetLocation();
	const uint32 axis = InNode->GetDepth() % KD_TREE::MAX_DEPTH;

	if (axis == KD_TREE::X)
	{
		sf::Vertex v1; v1.position = sf::Vector2f(location.x, yMin);
		sf::Vertex v2; v2.position = sf::Vector2f(location.x, yMax);
		sf::Vertex line[] =
		{
			v1,
			v2
		};

		InWindow->draw(line, 2, sf::PrimitiveType::Lines);

		DrawNode(InWindow, InNode->GetLeftNode(), location.x, xMax, yMin, yMax);
		DrawNode(InWindow, InNode->GetRightNode(), xMin, location.x, yMin, yMax);
	}
	else if (axis == KD_TREE::Y)
	{
		sf::Vertex v1; v1.position = sf::Vector2f(xMin, location.y); v1.color;
		sf::Vertex v2; v2.position = sf::Vector2f(xMax, location.y); v2.color;
		sf::Vertex line[] =
		{
			v1,
			v2
		};

		InWindow->draw(line, 2, sf::PrimitiveType::Lines);

		DrawNode(InWindow, InNode->GetLeftNode(), xMin, xMax, yMin, location.y);
		DrawNode(InWindow, InNode->GetRightNode(), xMin, xMax, location.y, yMax);
	}
}

void KDTree::DrawNode(sf::VertexArray& OutVertexArray, KDNode* InNode, float xMin, float xMax, float yMin, float yMax)
{
	if (InNode == nullptr) return;

	const sf::Vector2f& location = InNode->GetActor()->GetLocation();
	const sf::Color color = InNode->GetDirty() ? sf::Color::Yellow : sf::Color::White;
	const uint32 axis = InNode->GetDepth() % KD_TREE::MAX_DEPTH;

	if (axis == KD_TREE::X)
	{
		sf::Vertex v1; v1.position = sf::Vector2f(location.x, yMin); v1.color = color;
		sf::Vertex v2; v2.position = sf::Vector2f(location.x, yMax); v2.color = color;

		OutVertexArray.append(v1);
		OutVertexArray.append(v2);

		DrawNode(OutVertexArray, InNode->GetLeftNode(), location.x, xMax, yMin, yMax);
		DrawNode(OutVertexArray, InNode->GetRightNode(), xMin, location.x, yMin, yMax);
	}
	else if (axis == KD_TREE::Y)
	{
		sf::Vertex v1; v1.position = sf::Vector2f(xMin, location.y); v1.color = color;
		sf::Vertex v2; v2.position = sf::Vector2f(xMax, location.y); v2.color = color;

		OutVertexArray.append(v1);
		OutVertexArray.append(v2);

		DrawNode(OutVertexArray, InNode->GetLeftNode(), xMin, xMax, yMin, location.y);
		DrawNode(OutVertexArray, InNode->GetRightNode(), xMin, xMax, location.y, yMax);
	}
}

KDNode* KDTree::NextAvailableNode()
{
	if (0 == mAvailableNodeNumber.size())
	{
		wprintf(L"[KD-Tree] [NextAvailableNode] No nodes left\n");
		return nullptr;
	}

	const uint16 nextNumber = mAvailableNodeNumber.front();
	mAvailableNodeNumber.pop_front();

	return mNodesPool.at(nextNumber);
}

bool KDTree::CompareLocation(const sf::Vector2f& inParentLocation, const sf::Vector2f& inNodeLocation, const uint16& inAxis)
{
	if (inAxis == KD_TREE::X)
	{
		return inNodeLocation.x > inParentLocation.x;
	}
	else if (inAxis == KD_TREE::Y)
	{
		return inNodeLocation.y < inParentLocation.y;
	}

	return false;
}