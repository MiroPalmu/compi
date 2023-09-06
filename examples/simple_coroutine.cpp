#include <coroutine>
#include <iostream>

class UserFacing {
  public:
    class promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    class promise_type {
      public:
        UserFacing get_return_object() {
            auto handle = handle_type::from_promise(*this);
            return UserFacing{handle};
        }
        std::suspend_always initial_suspend() { return {}; }
        void return_void() {}
        void unhandled_exception() {}
        std::suspend_always final_suspend() noexcept { return {}; }
    };

  private:
    handle_type handle;

    UserFacing(handle_type handle) : handle(handle) {}

    UserFacing(const UserFacing &) = delete;
    UserFacing &operator=(const UserFacing &) = delete;

  public:
    bool resume() {
        if (!handle.done())
            handle.resume();
        return !handle.done();
    }

    UserFacing(UserFacing &&rhs) : handle(rhs.handle) {
        rhs.handle = nullptr;
    }
    UserFacing &operator=(UserFacing &&rhs) {
        if (handle)
            handle.destroy();
        handle = rhs.handle;
        rhs.handle = nullptr;
        return *this;
    }
    ~UserFacing() {
        handle.destroy();
    }

    friend class SuspendOtherAwaiter;  // so it can get the handle
};

class TrivialAwaiter {
  public:
    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<>) {}
    void await_resume() {}
};

UserFacing demo_coroutine() {
    std::cout << "TrivialAwaiter:" << std::endl;
    co_await TrivialAwaiter{};
}


int main() {
    UserFacing demo_instance = demo_coroutine();
    demo_instance.resume();
}
