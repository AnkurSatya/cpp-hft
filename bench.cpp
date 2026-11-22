#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <sys/types.h>
#include <vector>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <numeric>
#include <random>
#include <optional>

using namespace std;

template<typename T>
constexpr size_t bytes_to_num_elements(size_t num_bytes) {
    static_assert(sizeof(T) > 0, "Element type must have a positive size.");
    if (num_bytes == 0) { return 0;}
    return num_bytes / sizeof(T);
}

template<typename T>
T sum_vector_inorder_access(const vector<T> &data) {
    T sum {};
    for (size_t i {0}; i  < data.size(); ++i){
        sum += data[i];
    }
    return sum;
}

template<typename T>
T sum_vector_random_access(const vector<T> &data, const vector<uint32_t> &indices){
    T sum{};
    for (auto i: indices) {
        sum += data[i];
    }
    return sum;
}

static void BM_vector_access_I32(benchmark::State& state, size_t num_bytes, optional<int32_t> seed = nullopt) {
    // Setup outside 'for (auto _ : state)' is not benchmarked.
    vector<int32_t> data (bytes_to_num_elements<int32_t>(num_bytes), 1);

    if (seed){
        vector<size_t> indices(data.size());
        iota(indices.begin(), indices.end(), 0);
        std::mt19937_64 rng(seed.value());
        shuffle(indices.begin(), indices.end(), rng);

        for (auto _ : state){
            int32_t total = sum_vector_inorder_access(data);
            benchmark::DoNotOptimize(total);
            benchmark::ClobberMemory();
        }
    }
    else{
        for (auto _ : state){
            int32_t total = sum_vector_inorder_access(data);
            benchmark::DoNotOptimize(total);
            benchmark::ClobberMemory();
        }
    }

    // Could be different from num_bytes because if num_bytes is not a multiple of sizeof(T), then
    // the vector 'data' size would not be num_bytes.
    const size_t bytes_per_iter = data.size() * sizeof(int32_t);
    const size_t total_bytes = static_cast<size_t>(state.iterations()) * bytes_per_iter ;// Sum of data across all iterations
    state.SetBytesProcessed(static_cast<ssize_t>(total_bytes));

    // Explicit byte counters printed in the output
    state.counters["Bytes/iter"] = static_cast<double>(bytes_per_iter);
    state.counters["TotalBytes"] = total_bytes;
}

// BENCHMARK(BM_vector_inorder_access_I32)->RangeMultiplier(8)->Range(16*1024, 128*1024*1024)->ArgName("data(KBs)");
BENCHMARK_CAPTURE(BM_vector_access_I32, inorder_16KBs, 16 * 1024);
BENCHMARK_CAPTURE(BM_vector_access_I32, inorder_256KBs, 256 * 1024);
BENCHMARK_CAPTURE(BM_vector_access_I32, inorder_2MBs, 2 * 1024 * 1024);
BENCHMARK_CAPTURE(BM_vector_access_I32, inorder_16MBs, 16 * 1024 * 1024);
BENCHMARK_CAPTURE(BM_vector_access_I32, inorder_128MBs, 128 * 1024 * 1024);

BENCHMARK_CAPTURE(BM_vector_access_I32, random_16KBs, 16 * 1024, 42);
BENCHMARK_CAPTURE(BM_vector_access_I32, random_256KBs, 256 * 1024, 42);
BENCHMARK_CAPTURE(BM_vector_access_I32, random_2MBs, 2 * 1024 * 1024, 42);
BENCHMARK_CAPTURE(BM_vector_access_I32, random_16MBs, 16 * 1024 * 1024, 42);
BENCHMARK_CAPTURE(BM_vector_access_I32, random_128MBs, 128 * 1024 * 1024, 42);
BENCHMARK_MAIN();
