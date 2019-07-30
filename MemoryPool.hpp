#ifndef __MEMORYPOOL_HPP
#define __MEMORYPOOL_HPP

#include <mutex>
#include <vector>
#include <assert.h> 


class AvailableIndexes {
private:
    //std::mutex mtx;
    size_t sz;
    std::vector<size_t> indexes;
public:
    enum OP {
        PUSH,
        POP,
        END
    };
    void init () {
        indexes.resize(sz);
        for (size_t i = 0; i < sz; ++i) indexes[i] = i;
    }
    explicit AvailableIndexes(size_t sz):sz(sz),indexes(sz,0) { //KEEPING FIXED SIZE MEM POOL FOR NOW
        init();
    }

    size_t size() { //READ ONLY FUNCTION
        return indexes.size();
    }
    
    size_t back() { //READ ONLY FUNCTION
        return indexes.back();
    }

    void push_or_pop(OP op, int val = -1) { //CHANGES DATA IN VECTOR
        //std::lock_guard<std::mutex> l(mtx);
        if (op == PUSH) {
            assert(val != -1);
            indexes.push_back(val);
        } else if (op == POP) {
            indexes.pop_back();
        }
        
    }
    
    size_t freeSZ() {
        return indexes.size();
    }

    void printFreeLoc() {
        for (auto a : indexes) {
            std::cout << a << " ";
        }
        std::cout << std::endl;
    }
    
    void recycle() {
        init();
    }
   
    void clear() {
        indexes.clear();
    }

};

template <class T> 
class MemoryPool {

private:
    
    std::mutex mtx;
    
    size_t sz;             //FIXED CAPACITY MEM POOL FOR NOW
    std::vector<T> objs;   //HOLDS ENTIRE MEMORY
    AvailableIndexes aidx; //HOLDS AVAILABLE MEMORY LOCATION
    
public:
    
    explicit MemoryPool(size_t sz): sz(sz), objs(sz, T()), aidx(sz) { }
        
    T* alloc() {
        std::lock_guard<std::mutex> l(mtx);
        if (aidx.size() > 0) { 
            T* p = &objs[0] + aidx.back();
            aidx.push_or_pop(AvailableIndexes::POP);
            return p;
        } else {
            //std::cout << "MemoryPool limit exceeded..." << std::endl; 
            return nullptr;
        }
        
    }
    
    void free (T* p) {
        std::lock_guard<std::mutex> l(mtx);
        int index = p - &objs[0];
        if (index >= 0 && index < sz) {
            p->~T();
            *p = T(); // This re-initializes object to default values, 
            aidx.push_or_pop(AvailableIndexes::PUSH, index); 
        } else {
            // invalid pointer request  
            std::cout << " free: invalid pointer request " << std::endl;
        }
        
    }

    size_t getPoolIndex(T* p) {
        return p - &objs[0];
    }

    size_t freeSZ() {
        return aidx.freeSZ();
    }

    void printFreeLoc() {
        aidx.printFreeLoc();
    }
    
    T* getAddress(size_t idx) {
        return &objs[0]+idx;
    }
    
    void recycle() {
        for (size_t i = 0; i < sz; ++i) objs[i] = T();
        aidx.recycle();
        
    }

    void clear() {
        objs.clear();
        aidx.clear();
    }

};


#endif // __MEMORYPOOL_HPP

