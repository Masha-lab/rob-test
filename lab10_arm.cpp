#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <ctime>
#include <iomanip>

// На ARM нет rdtsc, используем clock_gettime
uint64_t get_ticks() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int main() {
    const size_t ARRAY_SIZE = 16 * 1024 * 1024; // 64 MB
    
    std::vector<int> data(ARRAY_SIZE);
    std::vector<int> indices(ARRAY_SIZE);
    
    std::cout << "Nops,Average_Nanoseconds\n";
    
    std::mt19937 gen(42);
    for (size_t i = 0; i < ARRAY_SIZE; ++i) indices[i] = i;
    for (size_t i = ARRAY_SIZE - 1; i > 0; --i) {
        size_t j = gen() % (i + 1);
        std::swap(indices[i], indices[j]);
    }
    for (size_t i = 0; i < ARRAY_SIZE; ++i) data[i] = i;
    
    int k = 0;
    const int MAX_NOPS = 500;
    const int STEP = 10;
    const int ITERATIONS = 100000;
    
    for (int nops = 0; nops <= MAX_NOPS; nops += STEP) {
        uint64_t start = get_ticks();
        
        for (int i = 0; i < ITERATIONS; ++i) {
            k = data[indices[k]];
            
            for (int j = 0; j < nops; ++j) {
                __asm__ __volatile__("nop");
            }
        }
        
        uint64_t end = get_ticks();
        double avg_ns = (double)(end - start) / ITERATIONS;
        std::cout << nops << "," << std::fixed << std::setprecision(2) << avg_ns << "\n";
    }
    
    std::cout << "Final k = " << k << "\n";
    return 0;
}
