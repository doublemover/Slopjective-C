#ifndef OBJC3_TASK_TRANSFERS_H
#define OBJC3_TASK_TRANSFERS_H
#include <atomic>
#include <cassert>
#include <initializer_list>
#include <memory>
#include <thread>
namespace task_transfers {
struct Counts {std::atomic<unsigned> invoked{0},resumed{0};};
inline objc3::task<int> leaf(std::shared_ptr<int> owner,Counts *counts) {
  ++counts->resumed;co_await objc3::task_yield;co_return *owner;
}
struct __attribute__((objc_unsafe_sendable)) Child {
  objc3::task<int> value;Counts *counts;bool fail=false;
  objc3::task<int> &&operator()() & {
    ++counts->invoked;if(fail)throw 73;return std::move(value);
  }
};
struct Group {
  objc3::task<int> value;Counts *counts;bool fail=false;
  objc3::task<int> &&operator()(objc3::task_group_handle) & = delete;
  objc3::task<int> &&operator()(objc3::task_group_handle) && {
    ++counts->invoked;if(fail)throw 73;return std::move(value);
  }
};
struct Borrowed {objc3::task<int> &operator()();};
struct Constant {const objc3::task<int> &&operator()();};
struct BorrowedGroup {objc3::task<int> &operator()(objc3::task_group_handle);};
struct ConstantGroup {const objc3::task<int> &&operator()(objc3::task_group_handle);};
struct FrameReference {objc3_async_frame &&operator()(objc3::task_group_handle);};
template<class Work>concept CanSpawn=requires(Work work){objc3_task_spawn(std::move(work));};
template<class Work>concept CanGroup=requires(Work work){objc3::with_task_group<int>(std::move(work));};
static_assert(CanSpawn<Child> && !CanSpawn<Borrowed> && !CanSpawn<Constant>);
static_assert(CanGroup<Group> && !CanGroup<BorrowedGroup> && !CanGroup<ConstantGroup> && !CanGroup<FrameReference>);
static_assert(std::is_same_v<decltype(objc3::with_task_group(std::declval<Group>())),objc3::task<int>>);
inline objc3::task<int> parent(std::shared_ptr<int> owner,Counts *counts) {
  auto group=objc3::task_group_handle::create();
  auto first=objc3_task_spawn(Child{leaf(owner,counts),counts});
  auto second=objc3_task_spawn_in_group(group,Child{leaf(owner,counts),counts});
  auto third=group.spawn(Child{leaf(owner,counts),counts});
  group.close();
  int result=(co_await first)+(co_await second)+(co_await third);
  co_await group;co_return result;
}
inline void contract(objc3_executor_t *source) {
  {
    Counts counts;auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    auto task=parent(std::move(owner),&counts).start(source);uint32_t ran=0;
    assert(objc3_executor_pump(source,UINT32_MAX,&ran)==OBJC3_SERVICE_OK);
    assert(task.result()==126 && counts.invoked==3 && counts.resumed==3);
    task={};assert(weak.expired());
  }
  for(bool fail:{false,true}) {
    Counts counts;auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    auto task=objc3::with_task_group(Group{leaf(std::move(owner),&counts),&counts,fail}).start(source);
    uint32_t ran=0;assert(objc3_executor_pump(source,UINT32_MAX,&ran)==OBJC3_SERVICE_OK);
    if(fail){bool caught=false;try{(void)task.result();}catch(int value){caught=value==73;}assert(caught);}
    else assert(task.result()==42);
    assert(counts.invoked==1 && counts.resumed==(fail?0u:1u));task={};assert(weak.expired());
  }
  {
    Counts counts;auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    auto task=objc3::with_task_group(Group{leaf(std::move(owner),&counts),&counts}).start(source);
    task.cancel();uint32_t ran=0;assert(objc3_executor_pump(source,UINT32_MAX,&ran)==OBJC3_SERVICE_OK);
    bool cancelled=false;try{(void)task.result();}catch(const objc3::task_cancelled &){cancelled=true;}
    assert(cancelled && counts.invoked==0 && counts.resumed==0);task={};assert(weak.expired());
  }
  {
    Counts counts;auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    bool caught=false;try{(void)objc3_task_spawn(Child{leaf(std::move(owner),&counts),&counts,true});}catch(int value){caught=value==73;}
    assert(caught && weak.expired() && counts.invoked==1 && counts.resumed==0);
  }
  {
    Counts counts;auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    auto task=objc3_task_detached(Child{leaf(std::move(owner),&counts),&counts});
    task.wait();assert(task.result()==42 && counts.invoked==1 && counts.resumed==1);task={};
    for(unsigned attempt=0;!weak.expired() && attempt!=1000000;++attempt)std::this_thread::yield();
    assert(weak.expired());
  }
}
}
#endif
