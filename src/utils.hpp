#pragma once
#include <queue>

namespace utils {
// simple std::deque wrapper
template <class T, uint64_t maxSize = 8> class CircularQueue {
  public:
    CircularQueue() { queue.resize(maxSize); };

    constexpr auto operator*() const { return queue; }
    constexpr auto operator->() const { return &queue; }
    const T& operator[](size_t index) const { return queue[index]; }
    const T& at(size_t index) const { return queue.at(index); }
    auto size() const { return queue.size(); }

    const T& push(const T& value) {
        while (queue.size() >= maxSize)
            queue.pop_back();
        return queue.emplace_front(value);
    }

    const T& push(T&& value) {
        while (queue.size() >= maxSize)
            queue.pop_back();
        return queue.emplace_front(value);
    }

  private:
    std::deque<T> queue;
};
} // namespace utils