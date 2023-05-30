#include <benchmark/benchmark.h>

#include <string>

#include <samplelib/sample.h>

static void BM_samplelib_add(benchmark::State& state) {
    for (auto _ : state) {
        auto res = add(std::string{"str1"}, std::string{"str2"});
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(BM_samplelib_add);
