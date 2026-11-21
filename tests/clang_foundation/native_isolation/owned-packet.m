#include "provider.h"
#include <assert.h>
#if !__has_feature(objc_arc)
#error "This ownership witness requires ARC aggregate storage"
#endif

typedef struct OwnedHopPacket { HopObject *object; int value; } OwnedHopPacket;

static OwnedHopPacket owned_packet_entry(OwnedHopPacket packet)
    __attribute__((nothrow, objc_executor(main)));
static OwnedHopPacket owned_packet_entry(OwnedHopPacket packet) {
  objc3_executor_t *destination=0;
  assert(objc3_executor_get_main(&destination)==OBJC3_SERVICE_OK);
  assert(objc3_executor_is_current(destination));
  objc3_executor_release(destination);
  assert([packet.object number]==42);
  packet.value+=2;
  return packet;
}

int hop_owned_packet_roundtrip(objc3_executor_t *caller) __objc3_async_frame {
  OwnedHopPacket source={[HopObject new],40};
  OwnedHopPacket result=__objc3_await owned_packet_entry(source);
  assert(objc3_executor_is_current(caller));
  source.object=0;
  assert([result.object number]==42);
  int value=result.value;
  result.object=0;
  return value;
}
