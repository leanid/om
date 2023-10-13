# Just for fun try to find fastest strlen implementations

## here quick-bench implementations from https://quick-bench.com/q/XLvkFHIfETKFtuPmjrRKW1pHZro

It is not working with AVX implementation (we have to disable it, but store here for history)

```cpp
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>

#include <emmintrin.h>
#include <immintrin.h>

constexpr size_t g_max_len = 4000;

std::string generate_string(uintptr_t rnd)
{
    char letter = 'a' + rnd % 28;
    return std::string(g_max_len, letter);
}

size_t strlen_sse(const char* str) {
    const __m128i zero = _mm_setzero_si128();
    const char* char_ptr = str;
    while (reinterpret_cast<unsigned long long>(char_ptr) % 16 != 0) {
        if (*char_ptr == '\0') {
            return char_ptr - str;
        }
        ++char_ptr;
    }
    const __m128i* ptr = reinterpret_cast<const __m128i*>(char_ptr);
    while (!_mm_movemask_epi8(_mm_cmpeq_epi8(*ptr, zero))) {
        char_ptr += 16;
        ++ptr;
    }
    while (*char_ptr) {
        ++char_ptr;
    }
    return char_ptr - str;
}

size_t strlen_avx(const char* str) {
    const __m256i zero = _mm256_setzero_si256();
    const char* char_ptr = str;
    while (reinterpret_cast<unsigned long long>(char_ptr) % 32 != 0) {
        if (*char_ptr == '\0') {
            return char_ptr - str;
        }
        ++char_ptr;
    }
    const __m256i* ptr = reinterpret_cast<const __m256i*>(char_ptr);
    while (!_mm256_testz_si256(_mm256_cmpeq_epi8(*ptr, zero), zero)) {
        char_ptr += 32;
        ++ptr;
    }
    while (*char_ptr) {
        ++char_ptr;
    }
    return char_ptr - str;
}

size_t strlen_basic(const char* str)
{
    const char* p = str;
    while(*p)
    {
        p++;
    }
    return p - str;
}

template<typename T>
size_t strlen_template(const T* str)
{
    const T null_terminator{};
    const T* it_to_null = std::find(str, 
                                    str+g_max_len, 
                                    null_terminator);
    return it_to_null - str;
}


static void Strlen_sse(benchmark::State& state) {
  std::string str = generate_string(reinterpret_cast<uintptr_t>(&state));
  const char* str_ptr = str.data();
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    size_t result = strlen_sse(str_ptr);
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(result);
  }
}
// Register the function as a benchmark
BENCHMARK(Strlen_sse);

static void Strlen_avx(benchmark::State& state) {
  std::string str = generate_string(reinterpret_cast<uintptr_t>(&state));
  const char* str_ptr = str.data();
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    size_t result = strlen_avx(str_ptr);
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(result);
  }
}
// Register the function as a benchmark
BENCHMARK(Strlen_avx);

static void Strlen_basic(benchmark::State& state) {
  std::string str = generate_string(reinterpret_cast<uintptr_t>(&state));
  const char* str_ptr = str.data();
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    size_t result = strlen_basic(str_ptr);
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(result);
  }
}
// Register the function as a benchmark
BENCHMARK(Strlen_basic);

static void Strlen_template(benchmark::State& state) {
  std::string str = generate_string(reinterpret_cast<uintptr_t>(&state));
  const char* str_ptr = str.data();
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    size_t result = strlen_template(str_ptr);
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(result);
  }
}
// Register the function as a benchmark
BENCHMARK(Strlen_template);

static void Strlen_original(benchmark::State& state) {
  std::string str = generate_string(reinterpret_cast<uintptr_t>(&state));
  const char* str_ptr = str.data();
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    size_t result = strlen(str_ptr);
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(result);
  }
}
// Register the function as a benchmark
BENCHMARK(Strlen_original);

```
