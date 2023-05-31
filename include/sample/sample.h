#ifndef SAMPLE_SAMPLE_H
#define SAMPLE_SAMPLE_H

namespace sample {
    template <typename T>
    [[nodiscard]] inline constexpr T add(const T& a, const T& b) noexcept {
        return a + b;
    }
}

#endif // SAMPLE_SAMPLE_H
