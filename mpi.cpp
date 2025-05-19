#include <iostream>
#include <limits>
#include <mpi.h>
#include <vector>
#include <functional>
#include <fstream>
#include <chrono>

void send_msg_with_data(const int num_of_msg,
                        const std::vector<int>& data,
                        const int msg_data_size)
{
    for (int msg_id = 1; msg_id < num_of_msg; ++msg_id) {
        const auto msg_data(data.data() + ((msg_id - 1) * msg_data_size));
        MPI_Send(msg_data,
                 msg_data_size,
                 MPI_INT,
                 msg_id,
                 0,
                 MPI_COMM_WORLD);
    }
}

int get_max(const std::vector<int>& data)
{
    int max{ std::numeric_limits<int>::min() };
    for (const auto& value : data) {
        if (value > max)
            max = value;
    }

    return max;
}

void compare_with_received_max(const int num_of_msg, int& max)
{
    for (int msg_id = 1; msg_id < num_of_msg; ++msg_id) {
        MPI_Status status;
        int received_max {};

        MPI_Recv(&received_max,
                 1,
                 MPI_INT,
                 msg_id,
                 MPI_ANY_TAG,
                 MPI_COMM_WORLD,
                 &status);

        if (received_max > max)
            max = received_max;
    }
}

void execute_for_root(const int num_of_msg,
                      const std::vector<int>& data,
                      const int msg_data_size)
{
    send_msg_with_data(num_of_msg, data, msg_data_size);
    int max{ get_max(std::vector<int>(
        data.begin() + ((num_of_msg - 1) * msg_data_size), data.end())) };
    compare_with_received_max(num_of_msg, max);
}

std::vector<int> receive_data(const int msg_data_size)
{
    int* raw_data;
    MPI_Status status;

    MPI_Recv(raw_data,
             msg_data_size,
             MPI_INT,
             0,
             MPI_ANY_TAG,
             MPI_COMM_WORLD,
             &status);

    return std::vector<int>(raw_data, raw_data + msg_data_size);
}

void execute_for_worker(const int msg_data_size)
{
    const auto data{ receive_data(msg_data_size) };
    int max{ get_max(data) };
    MPI_Send(&max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

void find_max(const int rank,
              const int num_of_msg,
              const std::vector<int>& data)
{
    const auto msg_data_size{ data.size() / num_of_msg };
    if (rank == 0) {
        execute_for_root(num_of_msg, data, msg_data_size);
    } else {
        execute_for_worker(msg_data_size);
    }
}

void meas(std::function<void()> f, const int rank) {
    if (rank != 0)
    {
        f();
        return;
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    f();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout<< std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
}

void read_values(const std::string& filename, std::vector<int>& data) {
    std::ifstream infile(filename);

    int a;
    while (infile >> a) {
        data.push_back(a);
    }
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank{}, size{};
    std::vector<int> data{};

    if (rank == 0)
        read_values("data/datafile0", data);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    auto fn = [&](){find_max(rank, size, data);};
    meas(fn, rank);

    MPI_Finalize();
    return 0;
}
