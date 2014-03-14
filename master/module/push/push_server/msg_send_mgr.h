#pragma once


#include "../../include/push_interface.h"
#include <kthread/auto_lock.h>
#include <kthread/threadpool.h>
#include <core/timer_mgr.h>
#include <pattern/object_pool.h>
using namespace klib::kthread;
using namespace klib::pattern;

#include <third/sigslot.h>

#define  DEFAULT_RETRY_INTERVAL     (6)         ///< 单位为秒
#define  DEFAULT_MAX_RETRY_TIMES    (4)         ///< 最大重试次数

namespace logic
{

struct client_key
{
    client_key(ip_v4 addr, USHORT port_) : addr_(addr), port_(port_)  {  }
    client_key(){}

private:
    ip_v4  addr_;
    USHORT port_;

public:
    DEFINE_ACCESS_FUN_REF2(ip_v4, addr, addr_);
    DEFINE_ACCESS_FUN_REF2(USHORT, port, port_);

    bool operator < (const client_key& other) const 
    {
        if (addr_ < other.addr_) { return true; }
        else if (addr_ > other.addr_) { return false; }

        return port_ < other.port_;
    }

    UINT  HashKey() const {   return addr_;  }
};

class msg_confirm_info
{
public:
    msg_confirm_info() : msg_id_(0), last_send_time_(0), sended_times_(0) {}
    
private:
    UINT64              msg_id_;                 ///< 消息的编号
    UINT64              last_send_time_;         ///< 最后发送的时间
    UINT64              sended_times_;           ///< 发送的次数
    push_msg_ptr        user_msg_;               ///< 用户消息的指针

    DEFINE_ACCESS_FUN_REF2(UINT64, msg_id, msg_id_);
    DEFINE_ACCESS_FUN2(UINT64, last_send_time, last_send_time_);
    DEFINE_ACCESS_FUN_REF2(UINT64, sended_times, sended_times_);
    DEFINE_ACCESS_FUN_REF2(push_msg_ptr, user_msg, user_msg_);
};

class msg_send_mgr : 
    public singleton<msg_send_mgr>,
    public sigslot::has_slots<>
{
    msg_send_mgr(void);
public:
    ~msg_send_mgr(void);

    // 需要外界提供发送的接口
    sigslot::signal3<ip_v4, USHORT, push_msg_ptr> sign_on_send;

public:
    // 参数设置(最大发送次数，重试间隔)
    void set_max_retry_times(UINT32 uMaxRetryTimes);                                        ///< 设置最大重试次数
    void set_retry_send_interval(UINT32 uRetrySendInterval);                                ///< 设置重试间隔

    UINT32 get_max_retry_time() { return max_retry_times_; }                                ///< 获取最大重试次数
    UINT32 get_retry_send_interval() { return retry_send_interval; }                       ///< 获取重试间隔

    BOOL post_send_msg(DWORD addr_, USHORT port_, push_msg_ptr msg);                 ///< 投递发送消息
    BOOL post_send_msg(DWORD addr_, USHORT port_, std::vector<push_msg_ptr>& vecMsg);     ///< 一次投递多个消息
    void on_msg_confirm(DWORD addr_, USHORT port_, UINT64 uMsgID);                         ///< 消息反馈处理
    BOOL remove_confirm_msg(UINT64 uMsgID);                                                ///< 删除正在发送确认的消息

protected:
    bool check_resend_msg();                                                   ///< 检查并重试发送消息

private:
    typedef std::map<client_key, msg_confirm_info*>        MapClientConfirmMsgInfoType;
    typedef std::map<UINT64,  MapClientConfirmMsgInfoType>  MapMsgIDMsgConfirmType;
    MapMsgIDMsgConfirmType              confirm_msg_map_;                                ///< 保存确认消息列表
    mutex                               confirm_msg_mutex_;                              ///< 

    CObjectPool<msg_confirm_info, 10000, 4000>         confirm_msg_pool_;                               ///< 待确认消息内存池
    ThreadPool                          task_thread_pool_;                      
    timer_mgr                           timer_mgr_;

    UINT32                              max_retry_times_;                               ///< 最大重试发送次数
    UINT32                              retry_send_interval;                           ///< 重试发送的间隔
    LONGLONG                            whole_resend_times_;   
};

}