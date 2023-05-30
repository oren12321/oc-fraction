#ifndef SAMPLELIB_SAMPLE_H
#define SAMPLELIB_SAMPLE_H

template <typename T>
[[nodiscard]] T add(const T& a, const T& b) noexcept {
    return a + b;
}

[[nodiscard]] int add(const int& a, const int& b) noexcept;

#endif // SAMPLELIB_SAMPLE_H
