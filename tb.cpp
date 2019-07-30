#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <future>
#include <stdlib.h>
#include <set>
#include "MemoryPool.hpp"


//TEST BENCH

class Fraction { 
    /* An Example type for testing*/
    int n; 
    int d;
public:
    Fraction(int n = 0, int d = 1):n(n), d(d) {}
};

template <class T>
class MemoryPool_Test {
    
    int mem_sz;
    int n_threads; 
    int n_thread_itr;
    bool VERBOSE;
    MemoryPool<T> m_pool;
    
    //Test Scenario #1
    std::vector<int> alloc_test() {
        /*1. Test to check if every thread has unique address allocated from Memory Pool
          2. Returns a vector of location(index in MemoryPool) for particular thread
          3. These vector from other threads are checked to make sure there is no duplicacy
          4. All this is checked by accessing return from the function
        */
        std::vector<int> ret;
        for(int i = 0; i < n_thread_itr; ++i) {
            usleep(rand()%100); //sleep so that all thread gets some chance to run in random manner
            T *p = m_pool.alloc();
            if (p) {
                size_t idx = m_pool.getPoolIndex(p);
                ret.push_back(idx);
            }
        }
        return ret;
    }

    //Test Scenario #2
    std::vector<std::set<size_t>> alloc_free_test() {
        /*1. Allocate and randomly free in Memory Pool
          2. Return two set using vector
          3. first set contain location (index in MemoryPool) which are still allocated
          4. Second set contain location (index in MemoryPool) which eventually got freed
          5. allocated location in each thread should unique
          6. Freed location should be in Memory Pool if it is not taken by other thread
          7. All this is checked by accessing return from the function
        */
        std::vector<std::set<size_t>> ret(2);
        for(int i = 0; i < n_thread_itr; ++i) {
            usleep(rand()%100); //sleep so that all thread gets chance to run in random manner
            T *p = m_pool.alloc();
            if (p) {
                ret[0].insert(m_pool.getPoolIndex(p));
            }
            if (rand()%3 == 0) {
                if (ret[0].size() > 0) {
                    int m = ret[0].size()/2;
                    auto it = ret[0].begin();
                    //TODO m
                    int idx = *it;
                    T* d = m_pool.getAddress(idx);
                    m_pool.free(d);
                    ret[1].insert(idx);
                    ret[0].erase(idx);
                    
                }
                 
            }
        }
        return ret;
    }

public:
    MemoryPool_Test(int mem_sz, int n_threads, int n_thread_itr, bool verbose = false):mem_sz(mem_sz), n_threads(n_threads), n_thread_itr(n_thread_itr), m_pool(mem_sz), VERBOSE(verbose) {
        //MemoryPool<T> m_pool(mem_sz);
    }
     

    void run() {
        /* 1. this function executes all test scenario 
           2. Testbench class takes configuration input via commmand line
        */
        try {

            //Test Scenario #1 - alloc_test
            {
                std::vector<std::future<std::vector<int>>> futures(n_threads);
                std::cout << "\n>>>Unique Allocation Test\n";
                for (decltype(futures)::size_type i = 0; i < n_threads; ++i) {
                    futures[i] = std::async(&MemoryPool_Test<T>::alloc_test, this);
                }
                bool pass = true;
                std::vector<bool> HASH(mem_sz, false); /*HASH make sure that a location belong to only a particular thread*/
	        for (decltype(futures)::size_type i = 0; i < n_threads; ++i) {
                    if (VERBOSE) std::cout << "\nThread #" << i <<"\nUsed Space Locations\n";
                    for (auto a : futures[i].get()) {
                        if (VERBOSE) std::cout << a << " ";
                        if (HASH[a]) {
                            std::cout << "\n>>>FAIL\n" ;
                            pass = false;
                            break;
                            
                        }
                        
                        HASH[a] = true;
                    }
                    if (!pass) break;
                    if (VERBOSE) std::cout << std::endl;
                }
                if (pass) std::cout << ">>>PASS\n";
                
            }
            

            if (VERBOSE) {
                std::cout << "\nRemaining indexes in MemoryPool " << m_pool.freeSZ() << std::endl;
                m_pool.printFreeLoc();
            }   
            
            m_pool.recycle();
            
            if (VERBOSE) {
                std::cout << "\nRemaining indexes in MemoryPool after recycle " << m_pool.freeSZ() << std::endl;
                m_pool.printFreeLoc();
            }
            //Test Scenario #2 - alloc_free_test
            {
                std::cout << "\n>>>Alloc Free Space Consistency Check\n";
                std::vector<std::future<std::vector<std::set<size_t>>>> futures(n_threads);
                for (decltype(futures)::size_type i = 0; i < n_threads; ++i) {
                    futures[i] = std::async(&MemoryPool_Test<T>::alloc_free_test, this);
                }
                bool pass = true;
                std::vector<bool> HASH(mem_sz, false); /*HASH make sure that a location belong to only a particular thread*/
                for (decltype(futures)::size_type i = 0; i < n_threads; ++i) {
                    if (VERBOSE) std::cout << "\nThread #" << i <<"\nUsed Space Locations\n";
                    std::vector<std::set<size_t>> res = futures[i].get();
                    for (auto a : res[0]) {
                        if (VERBOSE) std::cout << a << " ";
                        if (HASH[a]) {
                            std::cout << "\n>>>FAIL\n" ;
                            pass = false;
                            break;
                        }
                        HASH[a] = true;
                    }
                    if (!pass) break;
                    
                    if (VERBOSE) {
                        std::cout << "\nFreed Space Location\n";
                        for (auto a : res[1]) {
                            std::cout << a << " ";
                        }
                        std::cout<<std::endl;
                    }
                    
                    
                }
                if (pass) std::cout << ">>>PASS\n";

            }
            
            if (VERBOSE) {
                std::cout << "\nRemaining indexes in MemoryPool " << m_pool.freeSZ() << std::endl;
                m_pool.printFreeLoc();
            }   
            
           
        } catch (std::exception e) {
            std::cout << "Exception caught :: " << e.what() << std::endl;
        }
    }
     
};



int main(int argc, char** argv) {
    
    assert(argc >= 4);
    
    char* c_mem_sz = argv[1];
    char* c_n_threads = argv[2];
    char* c_n_thread_itr = argv[3];
    char* c_verbose = nullptr;
    int verbose = 0; //default - not verbose
    if (argc == 5) c_verbose = argv[4];
    if (c_verbose) verbose = atoi(c_verbose); // Any non zero value is considered as true

    std::cout << "\nMemory Pool Size :: " << c_mem_sz << " Number of threads :: " << c_n_threads << " Iteration in Thread :: " << c_n_thread_itr << " Verbose :: " << verbose << std::endl;
    
    /*construction of testbench for particular configuration*/
    MemoryPool_Test<Fraction> tb(atoi(c_mem_sz), atoi(c_n_threads), atoi(c_n_thread_itr), verbose);
    /*running test bench*/
    tb.run();
   
    return 0;
    
}
