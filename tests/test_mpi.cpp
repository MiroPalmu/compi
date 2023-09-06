#include <boost/ut.hpp> // import boost.ut;

#include "mpi.h"

void expect_mpi_success(const auto error_code) {
    boost::ut::expect(error_code == MPI_SUCCESS);
}

int main() {
  using namespace boost::ut;

  "Init"_test = [] {
    expect_mpi_success(MPI_Init(nullptr, nullptr));
    int rank;
    expect_mpi_success(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    expect(rank == 0_i);
    expect_mpi_success(MPI_Finalize());
  };
}
