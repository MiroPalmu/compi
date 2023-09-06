#include <coroutine>
#include <exception>
#include <format>
#include <functional>
#include <iostream>
#include <optional>

#include "mpi.h"

namespace compi {

class OneToOne {
   public:
    class promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

   private:
    handle_type handle_;
    /**
     * Private constructor for promise to give the handel to OneToOne
     */
    OneToOne(handle_type handle) : handle_(handle) {}

   public:
    class promise_type {
       public:
        std::optional<int> connected_rank_opt = {};
        std::exception_ptr exception = nullptr;

        OneToOne get_return_object() {
            auto handle = handle_type::from_promise(*this);
            return OneToOne{handle};
        }
        // Initial suspend is used to connect stuff behind scenes
        std::suspend_always initial_suspend() { return {}; }
        // Maybe not needed:
        void return_void() {}
        void unhandled_exception() { exception = std::current_exception(); }
        std::suspend_always final_suspend() noexcept { return {}; }

        std::suspend_never yield_value(const std::vector<double> send_buffer) {
            if (!connected_rank_opt)
                throw std::runtime_error("OneToOne not connected");

            const auto err = MPI_Send(
                send_buffer.data(), static_cast<int>(send_buffer.size()),
                MPI_DOUBLE, connected_rank_opt.value(), 42, MPI_COMM_WORLD);

            if (err != MPI_SUCCESS) throw std::runtime_error("MPI_Send failed");

            return {};
        }
    };

    // Object is not copyable
    OneToOne(const OneToOne &) = delete;
    OneToOne &operator=(const OneToOne &) = delete;
    // but is movable
    OneToOne(OneToOne &&rhs) : handle_(rhs.handle_) { rhs.handle_ = nullptr; }
    OneToOne &operator=(OneToOne &&rhs) {
        if (handle_) handle_.destroy();
        handle_ = rhs.handle_;
        rhs.handle_ = nullptr;
        return *this;
    }

    ~OneToOne() {
        handle_.destroy();
    }

    bool resume() {
        if (!handle_.done()) handle_.resume();
        if (handle_.promise().exception)
            std::rethrow_exception(handle_.promise().exception);

        return !handle_.done();
    }

    void connect(const int rank) {
        handle_.promise().connected_rank_opt = rank;
    }

    friend class Message;  // so it can get the handle
};

/**
 * Initially implemented using only double but refactor should happend to
 * templetize this
 *
 * Also tag's has to be dealt with
 */
class Message {
    size_t size_;
    using handle_type = OneToOne::handle_type;
    int sender_rank_;

   public:
    Message(const size_t size) : size_(size) {}
    bool await_ready() { return false; }
    bool await_suspend(handle_type handle) {
        if (handle.promise().connected_rank_opt)
            sender_rank_ = handle.promise().connected_rank_opt.value();
        else
            throw std::runtime_error("OneToOne not connected in Message");
        return false;
    }
    std::vector<double> await_resume() {
        auto recv_buffer = std::vector<double>(size_);
        // Unchecked refactor land
        MPI_Recv(recv_buffer.data(), static_cast<int>(size_), MPI_DOUBLE,
                 sender_rank_, 42, MPI_COMM_WORLD, nullptr);

        return recv_buffer;
    }
};

// Following is refactor land after mppi

void connect_onetoone(const int rank1, const int rank2,
                      const std::function<OneToOne()> task1,
                      const std::function<OneToOne()> task2) {
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == rank1) {
        auto onetoone1 = task1();
        onetoone1.connect(rank2);
        onetoone1.resume();
    } else if (my_rank == rank2) {
        auto onetoone2 = task2();
        onetoone2.connect(rank1);
        onetoone2.resume();
    } else {
        throw std::runtime_error(
            std::format("Rank {} is not {} nor {}\n!", my_rank, rank1, rank2));
    }
}

}  // namespace compi
