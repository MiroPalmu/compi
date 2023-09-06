#include "onetoone.hpp"

#include <algorithm>
#include <vector>

int main() {
    using Real = double;
    using Buffer = std::vector<Real>;

    auto print_buffer = [](const auto& buff) {
        for (const auto x : buff) std::cout << x << " ";
        std::cout << std::endl;
    };

    auto send_and_sort_received = [&]() -> compi::OneToOne {
        auto buff = Buffer{ 1, 2, 3};

        std::cout << "Initially: ";
        print_buffer(buff);

        co_yield buff;

        buff = co_await compi::Message(3);

        std::cout << "Received reversed: ";
        print_buffer(buff);

        std::ranges::sort(buff);

        std::cout << "Sorted: ";
        print_buffer(buff);

    };

    auto reverse_received_and_send = [&]() -> compi::OneToOne {
        auto buff = co_await compi::Message(3);

        std::ranges::reverse(buff);

        co_yield buff;
    };

    MPI_Init(nullptr, nullptr);
    try {
        compi::connect_onetoone(0, 1, send_and_sort_received, reverse_received_and_send);
    } catch (std::exception& x) { std::cout << x.what(); };
    MPI_Finalize();

}
