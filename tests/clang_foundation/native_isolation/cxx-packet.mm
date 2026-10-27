#include "provider.h"
#include "objc3_task.hpp"
#include <assert.h>

extern "C" int hop_throwing_argument(void) { throw 77; }

static int live_packets, packet_entries, move_count, fail_move;
struct __attribute__((objc_unsafe_sendable)) MovePacket {
  int value;
  explicit MovePacket(int value):value(value) {
    __atomic_fetch_add(&live_packets,1,__ATOMIC_RELEASE);
  }
  MovePacket(const MovePacket &)=delete;
  MovePacket(MovePacket &&source):value(source.value) {
    int ordinal=__atomic_add_fetch(&move_count,1,__ATOMIC_ACQ_REL);
    if (ordinal==__atomic_load_n(&fail_move,__ATOMIC_ACQUIRE)) throw 77;
    source.value=0;
    __atomic_fetch_add(&live_packets,1,__ATOMIC_RELEASE);
  }
  ~MovePacket() noexcept {__atomic_fetch_sub(&live_packets,1,__ATOMIC_RELEASE);}
};

static int move_entry(MovePacket packet) noexcept
    __attribute__((objc_executor(main)));
static int move_entry(MovePacket packet) noexcept {
  objc3_executor_t *destination=nullptr;
  assert(objc3_executor_get_main(&destination)==OBJC3_SERVICE_OK);
  assert(objc3_executor_is_current(destination));
  objc3_executor_release(destination);
  __atomic_fetch_add(&packet_entries,1,__ATOMIC_RELEASE);
  return packet.value;
}

extern "C" int hop_cxx_packet_roundtrip(objc3_executor_t *caller)
    __objc3_async_frame {
  MovePacket source(42);
  int result=__objc3_await move_entry(static_cast<MovePacket &&>(source));
  assert(source.value==0 && objc3_executor_is_current(caller));
  return result;
}
extern "C" int hop_cxx_packet_live(void) {
  return __atomic_load_n(&live_packets,__ATOMIC_ACQUIRE);
}
extern "C" int hop_cxx_packet_entries(void) {
  return __atomic_load_n(&packet_entries,__ATOMIC_ACQUIRE);
}
extern "C" void hop_cxx_packet_fail_move(int ordinal) {
  assert(hop_cxx_packet_live()==0);
  __atomic_store_n(&move_count,0,__ATOMIC_RELEASE);
  __atomic_store_n(&fail_move,ordinal,__ATOMIC_RELEASE);
}

static int live_values;
struct __attribute__((objc_unsafe_sendable)) CopyPacket {
  int value;
  explicit CopyPacket(int value):value(value) {
    __atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);
  }
  CopyPacket(const CopyPacket &source):value(source.value) {
    __atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);
  }
  CopyPacket(CopyPacket &&source) noexcept:value(source.value) {
    source.value=0;
    __atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);
  }
  ~CopyPacket() noexcept {__atomic_fetch_sub(&live_values,1,__ATOMIC_RELEASE);}
};
static CopyPacket value_entry(CopyPacket value) noexcept
    __attribute__((objc_executor(main)));
static CopyPacket value_entry(CopyPacket value) noexcept {
  value.value+=2;
  return value;
}
extern "C" int hop_cxx_value_roundtrip(objc3_executor_t *caller)
    __objc3_async_frame {
  CopyPacket source(40);
  CopyPacket result=__objc3_await value_entry(static_cast<CopyPacket &&>(source));
  assert(source.value==0 && objc3_executor_is_current(caller));
  return result.value;
}
extern "C" int hop_cxx_value_live(void) {
  return __atomic_load_n(&live_values,__ATOMIC_ACQUIRE);
}

@interface HopCounter (CxxValues)
- (CopyPacket)copyPacket:(CopyPacket)value __attribute__((nothrow));
- (int)movePacket:(MovePacket)value __attribute__((nothrow));
@end
@implementation HopCounter (CxxValues)
- (CopyPacket)copyPacket:(CopyPacket)value {
  value.value+=2;
  return value;
}
- (int)movePacket:(MovePacket)value {
  __atomic_fetch_add(&packet_entries,1,__ATOMIC_RELEASE);
  return value.value;
}
@end
extern "C" int hop_cxx_actor_roundtrip(HopCounter *counter,
                                       objc3_executor_t *caller)
    __objc3_async_frame {
  CopyPacket source(40);
  CopyPacket result=__objc3_await [counter copyPacket:static_cast<CopyPacket &&>(source)];
  assert(source.value==0 && objc3_executor_is_current(caller));
  MovePacket moved(42);
  int value=__objc3_await [counter movePacket:static_cast<MovePacket &&>(moved)];
  assert(moved.value==0 && objc3_executor_is_current(caller));
  return result.value+value-42;
}
