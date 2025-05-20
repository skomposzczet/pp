#include <chrono>
#include <climits>
#include <fstream>
#include <functional>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <string>
#include <vector>

void send_msg_with_data(const int num_of_msg,
                        const std::vector<int>& data,
                        const int msg_data_size)
{
    for (int msg_id = 1; msg_id < num_of_msg; ++msg_id) {
        const auto msg_data(data.data() + ((msg_id - 1) * msg_data_size));
        MPI_Send(msg_data, msg_data_size, MPI_INT, msg_id, 0, MPI_COMM_WORLD);
    }
}

int maxf(const std::vector<int>& data)
{
    int max = INT_MIN;
    std::vector<int> maxt(omp_get_max_threads(), 0);
#pragma omp parallel for private(max)
    for (int val : data) {
        int id = omp_get_thread_num();
        if (val > max) {
            max = val;
            maxt[id] = max;
        }
    }
    for (int maxthr : maxt) {
        if (maxthr > max) {
            max = maxthr;
        }
    }
    return max;
}

void compare_with_received_max(const int num_of_msg, int& max)
{
    for (int msg_id = 1; msg_id < num_of_msg; ++msg_id) {
        MPI_Status status;
        int received_max{};

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
    int max{ maxf(std::vector<int>(
        data.begin() + ((num_of_msg - 1) * msg_data_size), data.end())) };
    compare_with_received_max(num_of_msg, max);
}

std::vector<int> receive_data(const int msg_data_size)
{
    std::vector<int> buffer(msg_data_size);
    MPI_Status status;

    MPI_Recv(buffer.data(),
             msg_data_size,
             MPI_INT,
             0,
             MPI_ANY_TAG,
             MPI_COMM_WORLD,
             &status);

    return buffer;
}

void execute_for_worker(const int msg_data_size)
{
    const auto data{ receive_data(msg_data_size) };
    int max{ maxf(data) };
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

void meas(std::function<void()> f, const int rank)
{
    if (rank != 0) {
        f();
        return;
    }

    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();
    f();
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       begin)
                     .count()
              << std::endl;
}

void read_values(const std::string& filename, std::vector<int>& data)
{
    std::ifstream infile(filename);

    int a;
    while (infile >> a) {
        data.push_back(a);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        exit(1);
    }
    int thr = std::stoi(argv[1]);
    int rank{}, size{};
    std::vector<int> data{};

    MPI_Init(&argc, &argv);
    omp_set_num_threads(thr);

    if (rank == 0)
        read_values("data/datafile0", data);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    auto fn = [&]() { find_max(rank, size, data); };
    meas(fn, rank);

    MPI_Finalize();
    return 0;
}
