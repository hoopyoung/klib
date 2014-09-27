#ifndef _klib_mem_mgr_h_
#define _klib_mem_mgr_h_



#include "../include/allocator.h"
#include "../include/lock.h"
#include "addr_mgr.h"

#include <vector>


// memory mgr
class mem_mgr
{
public:
    static mem_mgr* instance();

public:
    addr_mgr* create();
    bool free(addr_mgr* cur, void* ptr);

    simp_string stats();
    
protected:
    void print_info(simp_string& str, addr_info* );

protected:
    auto_cs      m_addr_cs;
    std::vector<addr_mgr*, MemAlloc<addr_mgr*> > m_mgr_arr;
    static mem_mgr* m_instance;
};




#endif //