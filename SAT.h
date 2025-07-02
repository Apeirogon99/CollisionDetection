#pragma once

// 투영 결과를 저장하는 구조체
struct Projection {
    float min;    // 최소값
    float max;    // 최대값
};

struct SATResult {
    bool colliding;             // 충돌 여부
    sf::Vector2f axis;          // 최소 침투 깊이를 갖는 축
    float penetrationDepth;     // 침투 깊이
};

class SAT {
public:
    // 두 다각형 간의 충돌 검사
    static SATResult CheckCollision(const sf::Shape& InShapeLhs, const sf::Shape& InShapeRhs)
    {
        SATResult result;
        result.colliding = true;
        result.penetrationDepth = std::numeric_limits<float>::max();

        // 모든 축을 검사해야 함
        std::vector<sf::Vector2f> axes;

        // 다각형 A의 모든 변에 수직인 축 추가
        for (size_t index = 0; index < InShapeLhs.getPointCount(); ++index)
        {
            if (InShapeLhs.getPoint(index).x == 0 && InShapeLhs.getPoint(index).y == 0)
            {
                continue;
            }
            axes.push_back(InShapeLhs.getPoint(index).perpendicular().normalized());
        }

        // 다각형 B의 모든 변에 수직인 축 추가
        for (size_t index = 0; index < InShapeRhs.getPointCount(); ++index)
        {
            if (InShapeRhs.getPoint(index).x == 0 && InShapeRhs.getPoint(index).y == 0)
            {
                continue;
            }
            axes.push_back(InShapeRhs.getPoint(index).perpendicular().normalized());
        }

        // 각 축에 대해 검사
        for (const auto& axis : axes) 
        {
            // 각 다각형을 축에 투영
            Projection projA = projectPolygon(InShapeLhs, axis);
            Projection projB = projectPolygon(InShapeRhs, axis);

            // 투영이 겹치지 않으면 충돌하지 않음
            if (!overlaps(projA, projB)) 
            {
                result.colliding = false;
                return result;
            }

            // 겹침의 크기 계산 (침투 깊이)
            float overlap = getOverlap(projA, projB);

            // 최소 침투 깊이 업데이트
            if (overlap < result.penetrationDepth) 
            {
                result.penetrationDepth = overlap;
                result.axis = axis;
            }
        }

        return result;
    }

private:
    // 다각형을 축에 투영
    static Projection projectPolygon(const sf::Shape& InShape, const sf::Vector2f& InAxis) 
    {
        std::vector<sf::Vector2f> vertices;
        for (size_t index = 0; index < InShape.getPointCount(); ++index)
        {
            if (InShape.getPoint(index).x == 0 && InShape.getPoint(index).y == 0)
            {
                continue;
            }
            vertices.push_back(InShape.getPosition() - InShape.getOrigin() + InShape.getPoint(index));
        }

        // 첫 번째 점의 투영으로 초기화
        float min = InAxis.dot(vertices[0]);
        float max = min;

        // 나머지 모든 점들에 대해 투영하고 최소/최대값 업데이트
        for (size_t i = 1; i < vertices.size(); i++) 
        {
            float proj = InAxis.dot(vertices[i]);

            if (proj < min) min = proj;
            if (proj > max) max = proj;
        }

        return { min, max };
    }

    // 두 투영이 겹치는지 확인
    static bool overlaps(const Projection& projA, const Projection& projB) 
    {
        return !(projA.max < projB.min || projB.max < projA.min);
    }

    // 두 투영이 겹치는 크기 계산
    static float getOverlap(const Projection& projA, const Projection& projB) 
    {
        return std::min(projA.max, projB.max) - std::max(projA.min, projB.min);
    }
};
