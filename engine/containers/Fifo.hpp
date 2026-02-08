#pragma once

#include <array>
#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

/**
 * @brief R3 FIFO implementation
 *
 * Elements are pushed to the back of the Fifo and are popped from the front of the Fifo
 * A Fifo may be used across different threads so long as there is only one master and there are enough heads to account
 * for all the slaves
 *
 * @tparam T Datatype
 * @tparam Capacity Number of elems that may be held before wrap case
 * @tparam HeadCount The number of Fifo heads that may be used by slaves
 */
template <typename T, usize Capacity, usize HeadCount>
requires(Capacity > 0 && HeadCount > 0);
class R3_API Fifo {
public:
    using ValueType = T;

    Fifo() = default;

    /* no copy */
    Fifo(const Fifo&)            = delete;
    Fifo& operator=(const Fifo&) = delete;

    /* no move */
    Fifo(Fifo&&)            = delete;
    Fifo& operator=(Fifo&&) = delete;

    ~Fifo() = default;

    /**
     * @brief Get the number of elems in Fifo
     * @tparam N Which head
     * @return Delta between tail and head
     */
    template <usize N = 0>
    constexpr usize count() const {
        return wrap(m_tail + capacity() - head<N>());
    }

    /**
     * @brief Get the constexpr capacity of Fifo
     * @return Capacity
     */
    static constexpr usize capacity() { return Capacity; }

    /**
     * @brief Zero Fifo by setting every head and tail to 0
     * Be careful as this creates a race condition
     * @note this invalidates back() and front()
     */
    constexpr void zero() {
        m_head.fill(0);
        m_tail = 0;
    }

    /**
     * @brief Reset Fifo by setting every head equal to tail
     * @note this invalidates back() and front()
     */
    constexpr void reset() { m_head.fill(m_tail); }

    /**
     * @brief Get head index of Fifo
     * @tparam N Which head
     * @return Head index
     */
    template <usize N = 0>
    constexpr usize head() const {
        return std::get<N>(m_head);
    }

    /**
     * @brief Get tail index of Fifo
     * @return Tail index
     */
    constexpr usize tail() const { return m_tail; }

    /**
     * @brief Get T data pointer
     * @return T data pointer
     */
    constexpr T* data() { return m_data; }

    /**
     * @brief Get T const data pointer
     * @return T const data pointer
     */
    constexpr const T* data() const { return m_data; }

    /**
     * @brief Get pointer to start of elements
     * @tparam N Which head
     * @return Data + head
     */
    template <usize N = 0>
    constexpr T* begin() {
        return m_data + head<N>();
    }

    /**
     * @brief Get const pointer to start of elements
     * @tparam N Which head
     * @return Data + head
     */
    template <usize N = 0>
    constexpr const T* begin() const {
        return m_data + head<N>();
    }

    /**
     * @brief Get pointer to end of elements
     * @tparam N Which head
     * @return Data + tail
     */
    constexpr T* end() { return m_data + m_tail; }

    /**
     * @brief Get const pointer to end of elements
     * @tparam N Which head
     * @return Data + tail
     */
    constexpr const T* end() const { return m_data + m_tail; }

    /**
     * @brief Get the front element of the Fifo
     * @tparam N Which head
     * @return Data[head]
     */
    template <usize N = 0>
    constexpr T& front() {
        return m_data[head<N>()];
    }

    /**
     * @brief Get the front element of the Fifo
     * @tparam N Which head
     * @return Data[head]
     */
    template <usize N = 0>
    constexpr const T& front() const {
        return m_data[head<N>()];
    }

    /**
     * @brief Get the back element of the Fifo
     * @tparam N Which head
     * @return Data[tail-1] (simplified)
     */
    constexpr T& back() { return m_data[wrap(m_tail + capacity() - 1)]; }

    /**
     * @brief Get the back element of the Fifo
     * @tparam N Which head
     * @return Data[tail-1] (simplified)
     */
    constexpr const T& back() const { return m_data[wrap(m_tail + capacity() - 1)]; }

    /**
     * @brief Pop front by incrementing the head of the Fifo
     * @tparam N Which head
     * @return The element just popped
     */
    template <usize N = 0>
    constexpr T& pop() {
        T& data             = *begin<N>();
        std::get<N>(m_head) = wrap(head<N>() + 1);
        return data;
    }

    /**
     * @brief Push an element on Fifo
     * @param elem Element to push
     */
    constexpr void push(const T& elem) {
        *end() = elem;
        m_tail = wrap(m_tail + 1);
    }

    /**
     * @brief Emplace an element on Fifo
     * @param args Arguements to element constructor
     */
    template <typename... Args>
    constexpr void emplace(Args&&... args) {
        *end() = T(std::forward<Args>(args)...);
        m_tail = wrap(m_tail + 1);
    }

    /**
     * @brief Get whether Fifo has elements
     * @return head == tail
     */
    template <usize N = 0>
    constexpr bool empty() const {
        return head<N>() == m_tail;
    }

    /**
     * @brief Get the number of unused elements
     * @tparam N Which head
     * @return Capacity - count - 1 because head cannot equal tail
     */
    template <usize N = 0>
    constexpr usize unused() const {
        assert(capacity() != count<N>());
        return capacity() - count<N>() - 1;
    }

    /**
     * @brief Get whether the Fifo is full
     * @return unused == 0
     */
    template <usize N = 0>
    constexpr bool full() const {
        return unused<N>() == 0;
    }

private:
    /* speed up modulos when capacity is 2^n */
    static constexpr usize wrap(usize x) {
        if constexpr ((capacity() & (capacity() - 1)) == 0) {
            return x & (capacity() - 1);
        } else {
            return x % capacity();
        }
    }

private:
    std::array<usize, HeadCount> m_head = {};
    usize m_tail                        = 0;
    T m_data[Capacity]                  = {};
};

} // namespace R3