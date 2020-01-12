// C++ implementation of Dmitry Vyukov's non-intrusive lock free unbound MPSC queue
// http://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue
// https://github.com/mstump/queues/blob/master/include/mpsc-queue.hpp

#pragma once

#include <atomic>

template<typename T>
class MPSCQueue {
  public:
    MPSCQueue() :
        _head(reinterpret_cast<BufferNode*>(new BufferNodeAligned)),
        _tail(_head.load(std::memory_order_relaxed)) {
        BufferNode* front = _head.load(std::memory_order_relaxed);
        front->next.store(nullptr, std::memory_order_relaxed);
    }

    MPSCQueue(const MPSCQueue&) = delete;
    MPSCQueue(MPSCQueue&&) = delete;
    MPSCQueue& operator=(const MPSCQueue&) = delete;
    MPSCQueue& operator=(MPSCQueue&&) = delete;

    ~MPSCQueue() {
        T output;
        while (this->can_dequeue()) { this->dequeue(output); }
        BufferNode* front = _head.load(std::memory_order_relaxed);
        delete front;
    }

    void enqueue(T&& input) {
        BufferNode* node = reinterpret_cast<BufferNode*>(new BufferNodeAligned);
        node->data = std::move(input);
        node->next.store(nullptr, std::memory_order_relaxed);

        BufferNode* prev_head = _head.exchange(node, std::memory_order_acq_rel);
        prev_head->next.store(node, std::memory_order_release);
    }

    void enqueue(const T& input) {
        BufferNode* node = reinterpret_cast<BufferNode*>(new BufferNodeAligned);
        node->data = input;
        node->next.store(nullptr, std::memory_order_relaxed);

        BufferNode* prev_head = _head.exchange(node, std::memory_order_acq_rel);
        prev_head->next.store(node, std::memory_order_release);
    }

    bool can_dequeue() {
        BufferNode* tail = _tail.load(std::memory_order_relaxed);
        BufferNode* next = tail->next.load(std::memory_order_acquire);

        if (next == nullptr) {
            return false;
        }

        return true;
    }

    void dequeue(T& output)
    {
        BufferNode* tail = _tail.load(std::memory_order_relaxed);
        BufferNode* next = tail->next.load(std::memory_order_acquire);

        output = std::move(next->data);
        _tail.store(next, std::memory_order_release);
        delete tail;
    }

  private:
    struct BufferNode {
        T data;
        std::atomic<BufferNode*> next;
    };

    typedef typename std::aligned_storage<sizeof(BufferNode), std::alignment_of<BufferNode>::value>::type BufferNodeAligned;

    std::atomic<BufferNode*> _head;
    std::atomic<BufferNode*> _tail;
};
