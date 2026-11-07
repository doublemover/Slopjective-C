#ifndef NATIVE_BLOCK_VALUES_H
#define NATIVE_BLOCK_VALUES_H
namespace NativeBlockValues {
using Callback=int (^)(int);
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
}
#endif
