#include "../include/addr_mgr.h"


//------------------------------------------------------------------------
addr_mgr::addr_mgr()
{
    this->m_desc[0] = 0;
}

void addr_mgr::set_desc(const char* desc)
{
    strncpy(this->m_desc, desc, _countof(this->m_desc));
}

const char* addr_mgr::get_desc()
{
    return this->m_desc;
}

bool addr_mgr::add_addr_info(void* p, 
                             size_t nsize,
                             const char* desc)
{
    auto_lock locker(m_auto_cs);
    auto itr = m_addr_infos.find(p);
    if (itr == m_addr_infos.end()) 
    {
        addr_info& info = m_addr_infos[p];
        info.nsize = nsize;
        if (desc)
            strncpy(info.desc, desc, _countof(info.desc));

        // update stats info
        m_stats_info.nalloc_count ++ ;
        m_stats_info.ncurr_size += nsize;

        if (m_stats_info.ncurr_size > m_stats_info.nmax_size) 
        {
            m_stats_info.nmax_size = m_stats_info.ncurr_size;
        }
        return true;
    }

    return false;
}

bool addr_mgr::del_addr_info(void* p, 
                             size_t& nsize,
                             const char* desc)
{
    auto_lock locker(m_auto_cs);

    auto itr = m_addr_infos.find(p);
    if (itr != m_addr_infos.end()) 
    {
        nsize = itr->second.nsize;
        m_addr_infos.erase(itr);

        m_stats_info.nfree_count ++;
        m_stats_info.ncurr_size -= nsize;

        return true;
    }

    return false;
}

void addr_mgr::for_each(const view_addr_callback& func)
{
    auto_lock locker(m_auto_cs);

    auto itr = m_addr_infos.begin();
    for ( ; itr != m_addr_infos.end(); ++ itr)
    {
        func (&itr->second);
    }
}