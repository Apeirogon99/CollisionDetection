#pragma once
class Array : public ICollisionSystem
{
public:
	Array(std::vector<Actor*>& InActors);
	~Array() { }

private:
	Array(const Array&) = delete;
	Array(Array&&) = delete;

	Array& operator=(const Array&) = delete;
	Array& operator=(Array&&) = delete;


public:	//ISearch
	virtual void Init() override;
	virtual void Build(std::vector<Actor*>& InActors) override;
	virtual void Destroy() override;
	virtual std::vector<Actor*> Search(Attack& InAttack) override;
	virtual std::vector<Actor*> AllSearch() override;
	virtual void Draw(sf::RenderWindow* InWindow) override;
	virtual void Draw(sf::VertexArray& OutVertexArray) override;

private:
	std::vector<Actor*>& mActors;
};