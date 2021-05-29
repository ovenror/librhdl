#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <ostream>
#include <unordered_map>

namespace rhdl {

class ISingle;
class Entity;

class Simulator
{
public:
	Simulator();
	virtual ~Simulator();
	
	virtual bool get(const ISingle *iface) const = 0;
	virtual void set(const ISingle *iface, bool value = true);
	void resetInput();

	void step();

	virtual void processInput();
	virtual void internalStep() = 0;

	bool getInput(const ISingle *iface) const {return input_.at(iface);}

protected:
	using StateMap = std::unordered_map<const ISingle *, bool>;

	void init(const std::vector<const ISingle *> &ifaces);
	const StateMap &input() const {return input_;}
	virtual void toStream(std::ostream &os) const;

	std::vector<const ISingle *> debug_ifaces_;

private:
	friend std::ostream &operator<<(std::ostream &os, const Simulator &sim);

	StateMap input_;
};

std::ostream &operator<<(std::ostream &os, const Simulator &sim);

}

#endif // SIMULATOR_H
