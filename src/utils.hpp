#pragma once
#include <queue>

namespace utils {
// simple std::deque wrapper
template <class T, uint64_t maxSize = 8> class CircularQueue {
  public:
    CircularQueue() = default;

    constexpr auto operator*() const { return queue; }
    constexpr auto operator->() const { return &queue; }
    auto operator[](size_t index) const { return queue[index]; }
    auto at(size_t index) const { return queue.at(index); }
    auto size() const { return queue.size(); }

    void push(const T& value) {
        while (queue.size() >= maxSize)
            queue.pop_back();
        queue.push_front(value);
    }

    void push(T&& value) {
        while (queue.size() >= maxSize)
            queue.pop_back();
        queue.push_front(value);
    }

  private:
    std::deque<T> queue;
};
} // namespace utils