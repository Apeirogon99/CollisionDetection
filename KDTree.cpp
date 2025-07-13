#include "pch.h"
#include "KDTree.h"

KDTree::KDTree() : mRoot(nullptr), mActors()
{
}

KDTree::~KDTree()
{
	Destroy();
}

void KDTree::Init()
{
}

void KDTree::Destroy()
{
	if (mRoot)
	{
		DeleteNode(mRoot);
	}

	mActors.clear();
}

void KDTree::Insert(Actor* Actor)
{
	mActors.emplace_back(Actor);
}

void KDTree::Remove(Actor* Actor)
{
	for (size_t index = 0; index < mActors.size(); ++index)
	{
		if (mActors[index] == Actor)
		{
			mActors[index] = mActors.back();
			mActors.pop_back();
			break;
		}
	}
}

void KDTree::Build()
{
	//기존 정보 삭제 (지속적인 이동을 가지기 때문)
	if (mRoot)
	{
		DeleteNode(mRoot);
	}

	if (mActors.size() != 0)
	{
		mRoot = InsertNode(mActors, INIT_DEPTH);
	}
}

std::vector<Actor*> KDTree::Search(Attack& InAttack)
{
	vector<Actor*> overlap;
	if (mRoot == nullptr) return overlap;

	std::vector<KDNode*> possilbeNode;
	RangeSearch(InAttack.GetLocation(), InAttack.GetLocalRadius(), mRoot, possilbeNode, INIT_DEPTH);

	for (const auto& otherNode : possilbeNode)
	{
		Actor* otherActor = otherNode->GetActor();

#if USE_SAT
		SATResult result = SAT::CheckCollision(*A->GetShape(), *B->GetShape());
		if (result.colliding)
		{
			overlap.emplace_back(otherActor);
			otherActor->EnterOverlap();
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

std::vector<Actor*> KDTree::AllSearch()
{
	vector<Actor*> overlap;
	if (mRoot == nullptr) return overlap;

	for (auto iter = mActors.begin(); iter != mActors.end(); ++iter)
	{
		Actor* actor = *iter;
		std::vector<KDNode*> possilbeNode;
		RangeSearch(actor->GetLocation(), actor->GetLocalRadius(), mRoot, possilbeNode, INIT_DEPTH);

		for (const auto& otherNode : possilbeNode)
		{
			Actor* otherActor = otherNode->GetActor();
			if (actor == otherActor) continue;

#if USE_SAT
			SATResult result = SAT::CheckCollision(*A->GetShape(), *B->GetShape());
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

	return overlap;
}

void KDTree::Draw(sf::VertexArray& OutVertexArray)
{
	if (mRoot)
	{
		const float windowX = static_cast<float>(WINDOW_WIDTH);
		const float windowY = static_cast<float>(WINDOW_HEIGHT);

		DrawNode(OutVertexArray, mRoot, 0.0f, windowX, 0.0f, windowY);
	}
}

KDNode* KDTree::InsertNode(std::vector<Actor*> inActors, const uint16 inDepth)
{
	if (inActors.size() == 0) return nullptr;

	const uint16 depth = inDepth % KD_TREE::MAX_DEPTH;

	// 현재 차원 기준으로 정렬
	std::sort(inActors.begin(), inActors.end(),
		[depth](const Actor* lhs, const Actor* rhs)
		{
			const sf::Vector2f& lhsLocation = lhs->GetLocation();
			const sf::Vector2f& rhsLocation = rhs->GetLocation();
			return (KD_TREE::X == depth) ? (lhsLocation.x < rhsLocation.x) : (lhsLocation.y < rhsLocation.y);
		});

	// 중간 값 찾기
	size_t middleIndex = inActors.size() / 2;
	Actor* middleActor = inActors[middleIndex];
	KDNode* node = new KDNode(middleActor, depth);

	// 왼쪽 오른쪽 나누기
	std::vector<Actor*> leftActors(inActors.begin(), inActors.begin() + middleIndex);
	std::vector<Actor*> rightActors(inActors.begin() + middleIndex + 1, inActors.end());

	node->SetLeftNode(InsertNode(leftActors, depth + 1));
	node->SetRightNode(InsertNode(rightActors, depth + 1));

	return node;
}

void KDTree::DeleteNode(KDNode* inNode)
{
	if (inNode == nullptr) return;

	DeleteNode(inNode->GetLeftNode());
	DeleteNode(inNode->GetRightNode());

	delete inNode;
	inNode = nullptr;
}

void KDTree::RangeSearch(const sf::Vector2f& inFindLocation, const float& inRadius, KDNode* inNode, std::vector<KDNode*>& OutNodes, const uint16& inDepth)
{
	if (inNode == nullptr) return;

	Actor* actor = inNode->GetActor();
	const sf::Vector2f& location = actor->GetLocation();
	const float& actorRadius = actor->GetLocalRadius();

	const float distanceSquared = powf(inFindLocation.x - location.x, 2) + powf(inFindLocation.y - location.y, 2);
	const float radiusSum = powf(inRadius + actorRadius, 2);

	if (distanceSquared <= radiusSum) 
	{
		OutNodes.push_back(inNode);
	}

	// 현재 노드의 분할 차원 결정
	const uint16 currentDimension = inDepth % KD_TREE::MAX_DEPTH;

	if (currentDimension == KD_TREE::X) 
	{ // X축 분할
		if (inFindLocation.x - inRadius <= location.x) // 왼쪽 영역과 겹침
			RangeSearch(inFindLocation, inRadius, inNode->GetLeftNode(), OutNodes, inDepth + 1);

		if (inFindLocation.x + inRadius >= location.x) // 오른쪽 영역과 겹침
			RangeSearch(inFindLocation, inRadius, inNode->GetRightNode(), OutNodes, inDepth + 1);
	}
	else 
	{ // Y축 분할
		if (inFindLocation.y - inRadius <= location.y) // 아래쪽 영역과 겹침
			RangeSearch(inFindLocation, inRadius, inNode->GetLeftNode(), OutNodes, inDepth + 1);

		if (inFindLocation.y + inRadius >= location.y) // 위쪽 영역과 겹침
			RangeSearch(inFindLocation, inRadius, inNode->GetRightNode(), OutNodes, inDepth + 1);
	}
}

void KDTree::DrawNode(sf::VertexArray& OutVertexArray, KDNode* InNode, float xMin, float xMax, float yMin, float yMax)
{
	if (InNode == nullptr) return;

	const sf::Vector2f& location = InNode->GetActor()->GetLocation();
	const sf::Color color = InNode->GetActor()->IsOverlap() ? sf::Color::Yellow : sf::Color::White;
	const uint32 axis = InNode->GetDepth() % KD_TREE::MAX_DEPTH;

	if (axis == KD_TREE::X)
	{
		sf::Vertex v1; v1.position = sf::Vector2f(location.x, yMin); v1.color = color;
		sf::Vertex v2; v2.position = sf::Vector2f(location.x, yMax); v2.color = color;

		OutVertexArray.append(v1);
		OutVertexArray.append(v2);

		DrawNode(OutVertexArray, InNode->GetLeftNode(), xMin, location.x, yMin, yMax);
		DrawNode(OutVertexArray, InNode->GetRightNode(), location.x, xMax, yMin, yMax);
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