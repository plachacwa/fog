#include <vector>
#include <memory>

struct Node;
using uNode = std::unique_ptr<Node>;

struct Node {
	std::vector<uNode> children_;
	
	Node() {};
	
	template <class ...Args>
	inline void setChildren(Args&& ...args) {
		children_.reserve(sizeof...(args));
	   (children_.emplace_back(std::forward<Args>(args)), ...);
	};
};