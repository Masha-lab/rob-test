#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>

uint64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

int main() {
    const size_t ARRAY_SIZE = 16 * 1024 * 1024;
    
    std::vector<int> data(ARRAY_SIZE);
    std::vector<int> indices(ARRAY_SIZE);
    
    std::cout << "Инициализация массивов...\n";
    
    std::mt19937 gen(42);
    for (size_t i = 0; i < ARRAY_SIZE; ++i) indices[i] = i;
    for (size_t i = ARRAY_SIZE - 1; i > 0; --i) {
        size_t j = gen() % (i + 1);
        std::swap(indices[i], indices[j]);
    }
    for (size_t i = 0; i < ARRAY_SIZE; ++i) data[i] = i;
    
    std::cout << "Начало тестирования...\n\n";
    std::cout << "Nops,Average_Cycles\n";
    
    int k = 0;
    const int MAX_NOPS = 500;
    const int STEP = 10;
    const int ITERATIONS = 100000;
    
    for (int nops = 0; nops <= MAX_NOPS; nops += STEP) {
        uint64_t start = rdtsc();
        
        for (int i = 0; i < ITERATIONS; ++i) {
            k = data[indices[k]];
            
            for (int j = 0; j < nops; ++j) {
                __asm__ __volatile__("nop");
            }
        }
        
        uint64_t end = rdtsc();
        double avg_cycles = (double)(end - start) / ITERATIONS;
        std::cout << nops << "," << std::fixed << std::setprecision(2) << avg_cycles << "\n";
    }
    
    std::cout << "\nТестирование завершено.\n";
    std::cout << "Final k = " << k << "\n";
    
    return 0;
}
