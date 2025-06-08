#include <climits>
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <omp.h>
#include <cmath>

void make_useless_calculations(const int value, const unsigned power_of_uselessness){
    for (unsigned i{0}; i < power_of_uselessness; ++i){
        const auto val{static_cast<double>(value)};
        const auto sum{val + val};
        const auto sq{sqrt(val)};
        const auto prod{sq / (val != 0 ? val : 1.0)};
        const auto more_calculations{sum / sq / prod};
    }
}

int maxf(const std::vector<int>& data) {
    int max = INT_MIN;
    std::vector<int> maxt(omp_get_max_threads(), 0);
    // printf("%zu\n", maxt.size());
    #pragma omp parallel for private(max)
    for (int val: data)
    {
        make_useless_calculations(val, 200);
        int id = omp_get_thread_num();
        if (val > max) {
            max = val;
            maxt[id] = max;
        }
    }
    for (int maxthr: maxt) {
        if (maxthr > max) {
            max = maxthr;
        }
    }
    return max;
}

void read_values(const std::string& filename, std::vector<int>& data) {
    std::ifstream infile(filename);

    int a;
    while (infile >> a) {
        data.push_back(a);
    }
}

void meas(std::function<void()> f) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    f();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout<< std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;

}

int main(int argc, char** argv) {
    if (argc < 2) {
        exit(1);
    }
    int thr = std::stoi(argv[1]);
    omp_set_num_threads(thr);
    std::vector<int> data;
    read_values("data/datafile0", data);
    auto fn = [&](){return maxf(data);};
    // printf("%d\n", fn());
    meas(fn);
}
