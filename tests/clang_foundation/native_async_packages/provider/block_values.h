#ifndef NATIVE_BLOCK_VALUES_H
#define NATIVE_BLOCK_VALUES_H
namespace NativeBlockValues {
using Callback=int (^)(int);
Callback producer(int offset) __attribute__((ns_returns_retained));
Callback shared_producer(int offset);
void dispose(Callback value __attribute__((ns_consumed))) __attribute__((nothrow));
struct Slots {
  Callback values[2][2];
  Slots(Callback source,int flag)
      :values{{source,flag?source:^(int value){return value+2;}},{source,source}} {}
};
inline int exercise(int value) {
  int offset=2;
  Callback source=^(int input){return input+offset;};
  Slots slots(source,value==40);
  Callback local=(value+=0,slots.values[0][0]);
  slots.values[0][1]=(value+=0,local);
  slots.values[1][1]=value==40?local:slots.values[1][0];
  int answer=slots.values[1][1](value);
  slots.values[0][1]=(Callback)0;
  return answer;
}
inline int producer_exercise(int value) {
  Callback local=producer(2);
  Slots slots(local,1);
  slots.values[0][1]=producer(2);
  int answer=slots.values[0][1](value);
  dispose(slots.values[0][1]);slots.values[0][1]=(Callback)0;
  dispose(local);dispose(producer(2));
  return answer;
}
inline int shared_producer_exercise(int value) {
  Callback local=shared_producer(2);return local(value);
}
}
#endif
