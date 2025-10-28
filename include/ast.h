#pragma once
#include <vector>
#include <memory>
#include <string_view>

struct NodeT;
using uNode = std::unique_ptr<NodeT>;

struct NodeT {
	struct Type {
		std::string_view type;
		explicit Type(std::string_view t) : type(t) {}
	};

	std::vector<uNode> children_;
	void* info_ = nullptr;
	Type type_{"unknown"};

	NodeT() = default;
	explicit NodeT(std::string_view t) : type_(t) {}

	template<class... Args>
	explicit NodeT(Args&&... args) {
		children_.reserve(sizeof...(args));
		(children_.emplace_back(std::forward<Args>(args)), ...);
	}

	NodeT(const NodeT&) = delete;
	NodeT& operator=(const NodeT&) = delete;

	template<class T>
	inline void setInfo(T* ptr) noexcept { info_ = static_cast<void*>(ptr); }

	template<class T>
	inline T* getInfo() const noexcept { return static_cast<T*>(info_); }

	inline void* rawInfo() const noexcept { return info_; }

	virtual ~NodeT() = default;
};

struct Atom : NodeT {
	std::string_view value_;

	explicit Atom(std::string_view value, std::string_view type)
		: NodeT(type), value_(value) {}

	std::string_view value() const noexcept { return value_; }
};