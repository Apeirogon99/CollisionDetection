#pragma once
class Array : public ICollisionSystem
{
public:
	Array();
	~Array() { }

private:
	Array(const Array&) = delete;
	Array(Array&&) = delete;

	Array& operator=(const Array&) = delete;
	Array& operator=(Array&&) = delete;


public:	//ISearch
	virtual void Init() override;
	virtual void Destroy() override;

	virtual void Insert(Actor* Actor) override;
	virtual void Remove(Actor* Actor) override;
	virtual void Build() override;

	virtual std::vector<Actor*> Search(Attack& InAttack) override;
	virtual std::vector<Actor*> AllSearch() override;

	virtual void Draw(sf::VertexArray& OutVertexArray) override { /* DEBUG 내용 없음 */ }

private:
	std::vector<Actor*> mActors;
};