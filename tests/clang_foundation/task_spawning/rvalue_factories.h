#ifndef OBJC3_RVALUE_FACTORIES_H
#define OBJC3_RVALUE_FACTORIES_H
#include <atomic>
#include <cassert>
#include <initializer_list>
#include <memory>
#include <thread>
namespace rvalue_factories {
struct Counters {std::atomic<unsigned> invoked{0};};
struct __attribute__((objc_unsafe_sendable)) Child {
  std::shared_ptr<int> owner;
  Counters *counts;
  Child(std::shared_ptr<int> value,Counters *counter):owner(std::move(value)),counts(counter) {}
  Child(const Child &)=delete;Child(Child &&)=default;
  objc3::task<int> operator()() & = delete;
  objc3::task<int> operator()() && {
    ++counts->invoked;
    assert(owner && *owner==42);
    co_return *owner;
  }
};
struct __attribute__((objc_unsafe_sendable)) Group {
  std::shared_ptr<int> owner;
  Counters *counts;
  bool fail;
  Group(std::shared_ptr<int> value,Counters *counter,bool error):owner(std::move(value)),counts(counter),fail(error) {}
  Group(const Group &)=delete;Group(Group &&)=default;
  objc3::task<int> operator()(objc3::task_group_handle) & = delete;
  objc3::task<int> operator()(objc3::task_group_handle group) && {
    ++counts->invoked;
    if(fail)throw 73;
    auto child=group.spawn(Child{owner,counts});
    co_return co_await child;
  }
};
struct __attribute__((objc_unsafe_sendable)) FrameChild {
  std::shared_ptr<int> owner;
  int operator()() & = delete;
  int operator()() && __objc3_async_frame {return *owner;}
};
struct __attribute__((objc_unsafe_sendable)) FrameGroup {
  std::shared_ptr<int> owner;
  int operator()(objc3::task_group_handle) & = delete;
  int operator()(objc3::task_group_handle) && __objc3_async_frame {return *owner;}
};
struct __attribute__((objc_unsafe_sendable)) Preferred {
  objc3::task<int> operator()() & {co_return 42;}
  objc3::task<int> operator()() && {co_return -1;}
};
struct __attribute__((objc_unsafe_sendable)) Throwing {
  std::shared_ptr<int> owner;
  objc3::task<int> operator()() & = delete;
  objc3::task<int> operator()() && {throw 73;}
};
static_assert(std::is_same_v<decltype(objc3::with_task_group(std::declval<Group>())),objc3::task<int>>);
static_assert(std::is_same_v<decltype(objc3_task_spawn(std::declval<Child>())),objc3::task_handle<int>>);
inline objc3::task<int> parent(std::shared_ptr<int> owner,Counters *counts) {
  auto group=objc3::task_group_handle::create();
  auto first=objc3_task_spawn(Child{owner,counts});
  auto second=objc3_task_spawn_in_group(group,Child{owner,counts});
  auto third=group.spawn(Child{owner,counts});
  auto preferred=objc3_task_spawn(Preferred{});
  auto framed=objc3_task_spawn_in_group<int>(group,FrameChild{owner});
  auto grouped_frame=group.spawn<int>(FrameChild{owner});
  group.close();
  int value=(co_await first)+(co_await second)+(co_await third)+(co_await preferred);
  co_await group;
  value+=(co_await framed)+(co_await grouped_frame);
  co_return value;
}
inline void contract(objc3_executor_t *source) {
  for(bool fail:{false,true}) {
    Counters counts;
    auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    auto task=objc3::with_task_group(Group{std::move(owner),&counts,fail}).start(source);
    assert(!weak.expired());uint32_t executed=0;
    assert(objc3_executor_pump(source,UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
    if(fail) {bool caught=false;try{(void)task.result();}catch(int value){caught=value==73;}assert(caught);}
    else assert(task.result()==42);
    task={};assert(weak.expired());assert(counts.invoked==(fail?1u:2u));
  }
  {
    Counters counts;auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    auto task=objc3::with_task_group(Group{std::move(owner),&counts,false}).start(source);
    task.cancel();uint32_t executed=0;
    assert(objc3_executor_pump(source,UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
    bool cancelled=false;try{(void)task.result();}catch(const objc3::task_cancelled &){cancelled=true;}
    assert(cancelled && counts.invoked==0);task={};assert(weak.expired());
  }
  {
    Counters counts;auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    auto task=parent(std::move(owner),&counts).start(source);uint32_t executed=0;
    assert(objc3_executor_pump(source,UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
    assert(task.result()==252 && counts.invoked==3);task={};assert(weak.expired());
  }
  {
    auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    auto task=objc3::with_task_group<int>(FrameGroup{std::move(owner)}).start(source);uint32_t executed=0;
    assert(objc3_executor_pump(source,UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
    assert(task.result()==42);task={};assert(weak.expired());
  }
  {
    auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    bool caught=false;try{(void)objc3_task_spawn(Throwing{std::move(owner)});}catch(int value){caught=value==73;}
    assert(caught && weak.expired());
  }
  {
    Counters counts;auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    auto child=objc3_task_detached(Child{std::move(owner),&counts});
    child.wait();assert(child.result()==42 && counts.invoked==1);
    child={};
    // Completion precedes retirement of the worker's active-frame lease.
    for(unsigned attempt=0;!weak.expired() && attempt!=1000000;++attempt)
      std::this_thread::yield();
    assert(weak.expired());
  }
  {
    auto owner=std::make_shared<int>(42);std::weak_ptr<int> weak=owner;
    auto child=objc3_task_detached<int>(FrameChild{std::move(owner)});
    child.wait();assert(child.result()==42);child={};
    for(unsigned attempt=0;!weak.expired() && attempt!=1000000;++attempt)
      std::this_thread::yield();
    assert(weak.expired());
  }
}
}
#endif
