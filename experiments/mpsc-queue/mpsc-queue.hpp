// C++ implementation of Dmitry Vyukov's non-intrusive lock free unbound MPSC queue
// http://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue
// https://github.com/mstump/queues/blob/master/include/mpsc-queue.hpp

#pragma once

#include <atomic>
template<typename T>
class mpsc_queue_t
{
public:

    mpsc_queue_t() :
        _head(reinterpret_cast<buffer_node_t*>(new buffer_node_aligned_t)),
        _tail(_head.load(std::memory_order_relaxed))
    {
        buffer_node_t* front = _head.load(std::memory_order_relaxed);
        front->next.store(nullptr, std::memory_order_relaxed);
    }

    ~mpsc_queue_t()
    {
        T output;
        while (this->can_dequeue()) { this->dequeue(output); }
        buffer_node_t* front = _head.load(std::memory_order_relaxed);
        delete front;
    }

    void enqueue(const T& input)
    {
        buffer_node_t* node = reinterpret_cast<buffer_node_t*>(new buffer_node_aligned_t);
        node->data = input;
        node->next.store(nullptr, std::memory_order_relaxed);

        buffer_node_t* prev_head = _head.exchange(node, std::memory_order_acq_rel);
        prev_head->next.store(node, std::memory_order_release);
    }

    bool can_dequeue() {
        buffer_node_t* tail = _tail.load(std::memory_order_relaxed);
        buffer_node_t* next = tail->next.load(std::memory_order_acquire);

        if (next == nullptr) {
            return false;
        }
        return true;
    }

    void
    dequeue(T& output)
    {
        buffer_node_t* tail = _tail.load(std::memory_order_relaxed);
        buffer_node_t* next = tail->next.load(std::memory_order_acquire);

        output = std::move(next->data);
        _tail.store(next, std::memory_order_release);
        delete tail;
    }


private:

    struct buffer_node_t
    {
        T                           data;
        std::atomic<buffer_node_t*> next;
    };

    typedef typename std::aligned_storage<sizeof(buffer_node_t), std::alignment_of<buffer_node_t>::value>::type buffer_node_aligned_t;

    std::atomic<buffer_node_t*> _head;
    std::atomic<buffer_node_t*> _tail;

    mpsc_queue_t(const mpsc_queue_t&) {}
    void operator=(const mpsc_queue_t&) {}
};
