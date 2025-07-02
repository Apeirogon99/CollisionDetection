#pragma once

// ���� ����� �����ϴ� ����ü
struct Projection {
    float min;    // �ּҰ�
    float max;    // �ִ밪
};

struct SATResult {
    bool colliding;             // �浹 ����
    sf::Vector2f axis;          // �ּ� ħ�� ���̸� ���� ��
    float penetrationDepth;     // ħ�� ����
};

class SAT {
public:
    // �� �ٰ��� ���� �浹 �˻�
    static SATResult CheckCollision(const sf::Shape& InShapeLhs, const sf::Shape& InShapeRhs)
    {
        SATResult result;
        result.colliding = true;
        result.penetrationDepth = std::numeric_limits<float>::max();

        // ��� ���� �˻��ؾ� ��
        std::vector<sf::Vector2f> axes;

        // �ٰ��� A�� ��� ���� ������ �� �߰�
        for (size_t index = 0; index < InShapeLhs.getPointCount(); ++index)
        {
            if (InShapeLhs.getPoint(index).x == 0 && InShapeLhs.getPoint(index).y == 0)
            {
                continue;
            }
            axes.push_back(InShapeLhs.getPoint(index).perpendicular().normalized());
        }

        // �ٰ��� B�� ��� ���� ������ �� �߰�
        for (size_t index = 0; index < InShapeRhs.getPointCount(); ++index)
        {
            if (InShapeRhs.getPoint(index).x == 0 && InShapeRhs.getPoint(index).y == 0)
            {
                continue;
            }
            axes.push_back(InShapeRhs.getPoint(index).perpendicular().normalized());
        }

        // �� �࿡ ���� �˻�
        for (const auto& axis : axes) 
        {
            // �� �ٰ����� �࿡ ����
            Projection projA = projectPolygon(InShapeLhs, axis);
            Projection projB = projectPolygon(InShapeRhs, axis);

            // ������ ��ġ�� ������ �浹���� ����
            if (!overlaps(projA, projB)) 
            {
                result.colliding = false;
                return result;
            }

            // ��ħ�� ũ�� ��� (ħ�� ����)
            float overlap = getOverlap(projA, projB);

            // �ּ� ħ�� ���� ������Ʈ
            if (overlap < result.penetrationDepth) 
            {
                result.penetrationDepth = overlap;
                result.axis = axis;
            }
        }

        return result;
    }

private:
    // �ٰ����� �࿡ ����
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

        // ù ��° ���� �������� �ʱ�ȭ
        float min = InAxis.dot(vertices[0]);
        float max = min;

        // ������ ��� ���鿡 ���� �����ϰ� �ּ�/�ִ밪 ������Ʈ
        for (size_t i = 1; i < vertices.size(); i++) 
        {
            float proj = InAxis.dot(vertices[i]);

            if (proj < min) min = proj;
            if (proj > max) max = proj;
        }

        return { min, max };
    }

    // �� ������ ��ġ���� Ȯ��
    static bool overlaps(const Projection& projA, const Projection& projB) 
    {
        return !(projA.max < projB.min || projB.max < projA.min);
    }

    // �� ������ ��ġ�� ũ�� ���
    static float getOverlap(const Projection& projA, const Projection& projB) 
    {
        return std::min(projA.max, projB.max) - std::max(projA.min, projB.min);
    }
};
