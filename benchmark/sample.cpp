#include <benchmark/benchmark.h>

#include <string>

#include <sample/sample.h>

static void BM_sample_add(benchmark::State& state) {
    for (auto _ : state) {
        auto res = sample::add(std::string{"str1"}, std::string{"str2"});
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(BM_sample_add);
