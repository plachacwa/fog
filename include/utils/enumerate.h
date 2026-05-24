#include <iterator>
#include <utility>

template <typename Iter>
class EnumerateIterator {
    Iter it;
    std::size_t index = 0;
public:
    EnumerateIterator(Iter it, std::size_t index = 0) : it(it), index(index) {}

    auto operator*() const -> std::pair<std::size_t, decltype(*it)> {
        return {index, *it};
    }

    EnumerateIterator& operator++() {
        ++it;
        ++index;
        return *this;
    }

    bool operator!=(const EnumerateIterator& other) const {
        return it != other.it;
    }
};

template <typename Container>
class EnumerateWrapper {
    Container& container;
public:
    EnumerateWrapper(Container& c) : container(c) {}

    auto begin() { return EnumerateIterator{std::begin(container), 0}; }
    auto end()   { return EnumerateIterator{std::end(container), 0}; }
};

template <typename Container>
auto enumerate(Container& c) {
    return EnumerateWrapper<Container>(c);
}