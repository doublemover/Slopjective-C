#include "provider.h"
#include "objc3_task.hpp"
#include <assert.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#if __has_feature(objc_arc)
#define hop_explicit_storage_reset hop_arc_explicit_storage_reset
#define hop_explicit_storage_live hop_arc_explicit_storage_live
#define hop_explicit_storage_created hop_arc_explicit_storage_created
#define hop_explicit_storage_entries hop_arc_explicit_storage_entries
#define hop_explicit_storage_completed hop_arc_explicit_storage_completed
#define hop_explicit_storage_roundtrip hop_arc_explicit_storage_roundtrip
#define hop_explicit_storage_unstarted hop_arc_explicit_storage_unstarted
#endif
static int live_values,created,entries,completed;
extern "C" int hop_explicit_storage_live(void) {return __atomic_load_n(&live_values,__ATOMIC_ACQUIRE);}
extern "C" int hop_explicit_storage_created(void) {return __atomic_load_n(&created,__ATOMIC_ACQUIRE);}
extern "C" int hop_explicit_storage_entries(void) {return __atomic_load_n(&entries,__ATOMIC_ACQUIRE);}
extern "C" int hop_explicit_storage_completed(void) {return __atomic_load_n(&completed,__ATOMIC_ACQUIRE);}
extern "C" void hop_explicit_storage_reset(void) {
  assert(!hop_explicit_storage_live());
  __atomic_store_n(&created,0,__ATOMIC_RELEASE);
  __atomic_store_n(&entries,0,__ATOMIC_RELEASE);
  __atomic_store_n(&completed,0,__ATOMIC_RELEASE);
}
static void copied(HopObject *object) {
#if !__has_feature(objc_arc)
  objc_retain(object);
#endif
  __atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);
}
static void created_object(void) {
  __atomic_fetch_add(&created,1,__ATOMIC_RELEASE);
  __atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);
}
static void release_object(HopObject *object) {
#if !__has_feature(objc_arc)
  objc_release(object);
#endif
  __atomic_fetch_sub(&live_values,1,__ATOMIC_RELEASE);
}
// This union's explicit C++ contract always constructs its object arm. The
// unused integer arm ensures that generic field-wise ownership cannot work.
// A different active-arm policy belongs to its source constructors/destructor.
#if __has_feature(objc_arc)
namespace explicit_arc_storage {
#else
namespace explicit_mrc_storage {
#endif
union __attribute__((objc_unsafe_sendable)) OwnedUnion {
  HopObject *object;
  long integer;
  OwnedUnion():object([HopObject new]) {created_object();}
  OwnedUnion(const OwnedUnion &other):object(other.object) {copied(object);}
  OwnedUnion(OwnedUnion &&other) noexcept:object(other.object) {
    other.object=0;__atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);
  }
  ~OwnedUnion() noexcept {release_object(object);object=0;}
  int number() const {return [object number];}
};
struct __attribute__((objc_unsafe_sendable)) OwnedAggregate {
  HopObject *object;
  OwnedAggregate():object([HopObject new]) {created_object();}
  OwnedAggregate(const OwnedAggregate &other):object(other.object) {copied(object);}
  OwnedAggregate(OwnedAggregate &&other) noexcept:object(other.object) {
    other.object=0;__atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);
  }
  ~OwnedAggregate() noexcept {release_object(object);object=0;}
  int number() const {return [object number];}
};
}
#if __has_feature(objc_arc)
using namespace explicit_arc_storage;
#else
using namespace explicit_mrc_storage;
#endif
static void enter(void) {
  objc3_executor_t *main=nullptr;
  assert(objc3_executor_get_main(&main)==OBJC3_SERVICE_OK);
  assert(objc3_executor_is_current(main));objc3_executor_release(main);
  __atomic_fetch_add(&entries,1,__ATOMIC_RELEASE);
}
static void complete(void) {__atomic_fetch_add(&completed,1,__ATOMIC_RELEASE);}
static int pause(void) __objc3_async {__objc3_await objc3::task_yield;return 0;}
template<class T> static T sync_entry(T value) noexcept __attribute__((objc_executor(main)));
template<class T> static T sync_entry(T value) noexcept {enter();complete();return value;}
template<class T> static T frame_entry(T value) noexcept __objc3_async_frame __attribute__((objc_executor(main)));
template<class T> static T frame_entry(T value) noexcept __objc3_async_frame {
  enter();__objc3_await pause();complete();return value;
}
template<class T> static T task_entry(T value) noexcept __objc3_async __attribute__((objc_executor(main)));
template<class T> static T task_entry(T value) noexcept __objc3_async {
  enter();__objc3_await objc3::task_yield;complete();return value;
}
struct StaticEntries {
  template<class T> static T run(T value) noexcept __attribute__((objc_executor(main))) {
    enter();complete();return value;
  }
};
#if __has_feature(objc_arc)
namespace explicit_arc_storage {
#else
namespace explicit_mrc_storage {
#endif
template<class T> struct __attribute__((objc_unsafe_sendable)) MemberEntries {
  int cookie=42;
  MemberEntries(){__atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);}
  MemberEntries(const MemberEntries &)=delete;
  MemberEntries(MemberEntries &&)=delete;
  virtual ~MemberEntries() noexcept {cookie=-1;__atomic_fetch_sub(&live_values,1,__ATOMIC_RELEASE);}
  T sync(T value) const & noexcept __attribute__((objc_executor(main))) {
    assert(cookie==42);enter();complete();return value;
  }
  T frame(T value) const & noexcept __objc3_async_frame __attribute__((objc_executor(main))) {
    assert(cookie==42);enter();__objc3_await pause();assert(cookie==42);complete();return value;
  }
  T task(T value) && noexcept __objc3_async __attribute__((objc_executor(main))) {
    assert(cookie==42);enter();__objc3_await objc3::task_yield;assert(cookie==42);complete();return value;
  }
  T temporary(T value) && noexcept __attribute__((objc_executor(main))) {
    assert(cookie==42);enter();complete();return value;
  }
  virtual T virtual_entry(T value) noexcept __attribute__((objc_executor(main))) {
    assert(cookie==42);enter();complete();return value;
  }
  static T static_entry(T value) noexcept __attribute__((objc_executor(main))) {
    enter();complete();return value;
  }
private:
  T private_entry(T value) noexcept __attribute__((objc_executor(main))) {
    assert(cookie==42);enter();complete();return value;
  }
public:
  T private_path(T value) __objc3_async_frame {
    return __objc3_await private_entry(static_cast<T&&>(value));
  }
};
template<class T> struct __attribute__((objc_unsafe_sendable)) DerivedEntries : MemberEntries<T> {
  T virtual_entry(T value) noexcept override {
    assert(this->cookie==42);enter();complete();return value;
  }
};
}
template<class T> static MemberEntries<T> &evaluated_member_receiver(MemberEntries<T> &object,int *order) {
  assert(*order==0);*order=1;return object;
}
template<class T> static T &&evaluated_member_argument(T &value,int *order) {
  assert(*order==1);*order=2;return static_cast<T&&>(value);
}

#if __has_feature(objc_arc)
namespace explicit_arc_storage {
#else
namespace explicit_mrc_storage {
#endif
static int await_plain(int value) noexcept {return value;}
static int await_frame(int value) __objc3_async_frame {return value;}
static int await_task(int value) __objc3_async {__objc3_await objc3::task_yield;return value;}
template<class Work> static int await_callback(Work work) __objc3_async {return __objc3_await work(42);}
template<int Bias> static int await_value(int value) __objc3_async {return __objc3_await (value+Bias);}
static int dependent_await_helpers(void) __objc3_async {
  assert(__objc3_await await_callback(await_plain)==42);
  assert(__objc3_await await_callback(await_frame)==42);
  assert(__objc3_await await_callback(await_task)==42);
  assert(__objc3_await await_value<2>(40)==42);
  return 42;
}
}

template<class T,int ABI> static int typed_roundtrip(objc3_executor_t *destination) __objc3_async_frame {
  T source;
  if constexpr(ABI>=4) assert(__objc3_await dependent_await_helpers()==42);
  if constexpr(ABI==0) {
    T result=__objc3_await sync_entry(static_cast<T &&>(source));
    assert(objc3_executor_is_current(destination));return result.number();
  } else if constexpr(ABI==1) {
    T result=__objc3_await frame_entry(static_cast<T &&>(source));
    assert(objc3_executor_is_current(destination));return result.number();
  } else if constexpr(ABI==2) {
    T result=__objc3_await task_entry(static_cast<T &&>(source));
    assert(objc3_executor_is_current(destination));return result.number();
  } else if constexpr(ABI==3) {
    T result=__objc3_await StaticEntries::run(static_cast<T &&>(source));
    assert(objc3_executor_is_current(destination));return result.number();
  } else if constexpr(ABI==8) {
    T result=__objc3_await MemberEntries<T>().temporary(static_cast<T&&>(source));
    assert(objc3_executor_is_current(destination));return result.number();
  } else if constexpr(ABI==9 || ABI==10) {
    DerivedEntries<T> receiver;
    if constexpr(ABI==9) {
      MemberEntries<T> *base=&receiver;
      T result=__objc3_await base->virtual_entry(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    } else {
      T result=__objc3_await receiver.MemberEntries<T>::virtual_entry(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    }
  } else {
    MemberEntries<T> receiver;
    if constexpr(ABI==4) {
      T result=__objc3_await receiver.sync(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    } else if constexpr(ABI==5) {
      T result=__objc3_await receiver.frame(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    } else if constexpr(ABI==6) {
      T result=__objc3_await static_cast<MemberEntries<T>&&>(receiver).task(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    } else if constexpr(ABI==7) {
      T result=__objc3_await receiver.private_path(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    } else {
      int order=0;
      if constexpr(ABI==11) {
        T result=__objc3_await evaluated_member_receiver(receiver,&order).static_entry(evaluated_member_argument(source,&order));
        assert(order==2 && objc3_executor_is_current(destination));return result.number();
      } else {
        T result=__objc3_await evaluated_member_receiver(receiver,&order).sync(evaluated_member_argument(source,&order));
        assert(order==2 && objc3_executor_is_current(destination));return result.number();
      }
    }
  }
}
template<class T,int ABI> static int typed_task_roundtrip(objc3_executor_t *destination) __objc3_async {
  T source;
  if constexpr(ABI>=4) assert(__objc3_await dependent_await_helpers()==42);
  if constexpr(ABI==0) {
    T result=__objc3_await sync_entry(static_cast<T &&>(source));
    assert(objc3_executor_is_current(destination));return result.number();
  } else if constexpr(ABI==1) {
    T result=__objc3_await frame_entry(static_cast<T &&>(source));
    assert(objc3_executor_is_current(destination));return result.number();
  } else if constexpr(ABI==2) {
    T result=__objc3_await task_entry(static_cast<T &&>(source));
    assert(objc3_executor_is_current(destination));return result.number();
  } else if constexpr(ABI==3) {
    T result=__objc3_await StaticEntries::run(static_cast<T &&>(source));
    assert(objc3_executor_is_current(destination));return result.number();
  } else if constexpr(ABI==8) {
    T result=__objc3_await MemberEntries<T>().temporary(static_cast<T&&>(source));
    assert(objc3_executor_is_current(destination));return result.number();
  } else if constexpr(ABI==9 || ABI==10) {
    DerivedEntries<T> receiver;
    if constexpr(ABI==9) {
      MemberEntries<T> *base=&receiver;
      T result=__objc3_await base->virtual_entry(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    } else {
      T result=__objc3_await receiver.MemberEntries<T>::virtual_entry(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    }
  } else {
    MemberEntries<T> receiver;
    if constexpr(ABI==4) {
      T result=__objc3_await receiver.sync(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    } else if constexpr(ABI==5) {
      T result=__objc3_await receiver.frame(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    } else if constexpr(ABI==6) {
      T result=__objc3_await static_cast<MemberEntries<T>&&>(receiver).task(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    } else if constexpr(ABI==7) {
      T result=__objc3_await receiver.private_path(static_cast<T&&>(source));
      assert(objc3_executor_is_current(destination));return result.number();
    } else {
      int order=0;
      if constexpr(ABI==11) {
        T result=__objc3_await evaluated_member_receiver(receiver,&order).static_entry(evaluated_member_argument(source,&order));
        assert(order==2 && objc3_executor_is_current(destination));return result.number();
      } else {
        T result=__objc3_await evaluated_member_receiver(receiver,&order).sync(evaluated_member_argument(source,&order));
        assert(order==2 && objc3_executor_is_current(destination));return result.number();
      }
    }
  }
}
static int task_roundtrip(int kind,objc3_executor_t *destination) __objc3_async {
  switch(kind) {
  case 0:return __objc3_await typed_task_roundtrip<OwnedUnion,0>(destination);
  case 1:return __objc3_await typed_task_roundtrip<OwnedUnion,1>(destination);
  case 2:return __objc3_await typed_task_roundtrip<OwnedUnion,2>(destination);
  case 3:return __objc3_await typed_task_roundtrip<OwnedUnion,3>(destination);
  case 4:return __objc3_await typed_task_roundtrip<OwnedAggregate,0>(destination);
  case 5:return __objc3_await typed_task_roundtrip<OwnedAggregate,1>(destination);
  case 6:return __objc3_await typed_task_roundtrip<OwnedAggregate,2>(destination);
  case 7:return __objc3_await typed_task_roundtrip<OwnedAggregate,3>(destination);
  case 8:return __objc3_await typed_task_roundtrip<OwnedUnion,4>(destination);
  case 9:return __objc3_await typed_task_roundtrip<OwnedUnion,5>(destination);
  case 10:return __objc3_await typed_task_roundtrip<OwnedUnion,6>(destination);
  case 11:return __objc3_await typed_task_roundtrip<OwnedUnion,7>(destination);
  case 12:return __objc3_await typed_task_roundtrip<OwnedUnion,8>(destination);
  case 13:return __objc3_await typed_task_roundtrip<OwnedUnion,9>(destination);
  case 14:return __objc3_await typed_task_roundtrip<OwnedUnion,10>(destination);
  case 15:return __objc3_await typed_task_roundtrip<OwnedUnion,11>(destination);
  case 16:return __objc3_await typed_task_roundtrip<OwnedUnion,12>(destination);
  case 17:return __objc3_await typed_task_roundtrip<OwnedAggregate,4>(destination);
  case 18:return __objc3_await typed_task_roundtrip<OwnedAggregate,5>(destination);
  case 19:return __objc3_await typed_task_roundtrip<OwnedAggregate,6>(destination);
  case 20:return __objc3_await typed_task_roundtrip<OwnedAggregate,7>(destination);
  case 21:return __objc3_await typed_task_roundtrip<OwnedAggregate,8>(destination);
  case 22:return __objc3_await typed_task_roundtrip<OwnedAggregate,9>(destination);
  case 23:return __objc3_await typed_task_roundtrip<OwnedAggregate,10>(destination);
  case 24:return __objc3_await typed_task_roundtrip<OwnedAggregate,11>(destination);
  case 25:return __objc3_await typed_task_roundtrip<OwnedAggregate,12>(destination);
  default:assert(0);return 0;
  }
}
extern "C" int hop_explicit_storage_roundtrip(int kind,objc3_executor_t *destination,int task_caller) __objc3_async_frame {
  if(task_caller)return __objc3_await task_roundtrip(kind,destination);
  switch(kind) {
  case 0:return __objc3_await typed_roundtrip<OwnedUnion,0>(destination);
  case 1:return __objc3_await typed_roundtrip<OwnedUnion,1>(destination);
  case 2:return __objc3_await typed_roundtrip<OwnedUnion,2>(destination);
  case 3:return __objc3_await typed_roundtrip<OwnedUnion,3>(destination);
  case 4:return __objc3_await typed_roundtrip<OwnedAggregate,0>(destination);
  case 5:return __objc3_await typed_roundtrip<OwnedAggregate,1>(destination);
  case 6:return __objc3_await typed_roundtrip<OwnedAggregate,2>(destination);
  case 7:return __objc3_await typed_roundtrip<OwnedAggregate,3>(destination);
  case 8:return __objc3_await typed_roundtrip<OwnedUnion,4>(destination);
  case 9:return __objc3_await typed_roundtrip<OwnedUnion,5>(destination);
  case 10:return __objc3_await typed_roundtrip<OwnedUnion,6>(destination);
  case 11:return __objc3_await typed_roundtrip<OwnedUnion,7>(destination);
  case 12:return __objc3_await typed_roundtrip<OwnedUnion,8>(destination);
  case 13:return __objc3_await typed_roundtrip<OwnedUnion,9>(destination);
  case 14:return __objc3_await typed_roundtrip<OwnedUnion,10>(destination);
  case 15:return __objc3_await typed_roundtrip<OwnedUnion,11>(destination);
  case 16:return __objc3_await typed_roundtrip<OwnedUnion,12>(destination);
  case 17:return __objc3_await typed_roundtrip<OwnedAggregate,4>(destination);
  case 18:return __objc3_await typed_roundtrip<OwnedAggregate,5>(destination);
  case 19:return __objc3_await typed_roundtrip<OwnedAggregate,6>(destination);
  case 20:return __objc3_await typed_roundtrip<OwnedAggregate,7>(destination);
  case 21:return __objc3_await typed_roundtrip<OwnedAggregate,8>(destination);
  case 22:return __objc3_await typed_roundtrip<OwnedAggregate,9>(destination);
  case 23:return __objc3_await typed_roundtrip<OwnedAggregate,10>(destination);
  case 24:return __objc3_await typed_roundtrip<OwnedAggregate,11>(destination);
  case 25:return __objc3_await typed_roundtrip<OwnedAggregate,12>(destination);
  default:assert(0);return 0;
  }
}
template<class T> static int unused_frame(T value) __objc3_async_frame {assert(!"unused value frame entered");return value.number();}
template<class T> static int unused_task(T value) __objc3_async {assert(!"unused value task entered");return value.number();}
extern "C" objc3_async_frame hop_explicit_storage_unstarted(int aggregate,int task) {
  if(aggregate)return task?__objc3_async_to_frame<int>(unused_task(OwnedAggregate())):unused_frame(OwnedAggregate());
  return task?__objc3_async_to_frame<int>(unused_task(OwnedUnion())):unused_frame(OwnedUnion());
}
