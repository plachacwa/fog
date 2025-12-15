#pragma once
#include <vector>
#include <memory>
#include <string_view>
#include "infostructs.h"

struct NodeT;
using uNode = std::unique_ptr<NodeT>;

struct NodeT {
	std::vector<uNode> children_;
	void* info_ = nullptr;
	TypeInfo* type_;

	NodeT() = default;
	
	template<class... Args>
	explicit NodeT(Args&&... args) {
		children_.reserve(sizeof...(args));
		(children_.emplace_back(std::forward<Args>(args)), ...);
	};
	
	template<class... Args>
	explicit NodeT(TypeInfo* t, Args&&... args) : type_(t) {
		children_.reserve(sizeof...(args));
		(children_.emplace_back(std::forward<Args>(args)), ...);
	};


	NodeT(const NodeT&) = delete;
	NodeT& operator=(const NodeT&) = delete;
	

	template<class T>
	void setInfo(T* ptr) noexcept { info_ = static_cast<void*>(ptr); };

	template<class T>
	T* getInfo() const noexcept { return static_cast<T*>(info_); };


	virtual ~NodeT() = default;
};


struct Atom : NodeT {
	Atom(Literal* l, TypeInfo* t)
		: NodeT(t) {
		setInfo(l);
	};
};