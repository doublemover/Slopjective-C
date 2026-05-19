#pragma once

namespace objc3c::runtime {

struct RuntimeDispatchFrame;

void RecordRuntimeArcRetainCall(int value);
void RecordRuntimeArcReleaseCall(int value);
void RecordRuntimeArcAutoreleaseCall(int value);
void RecordRuntimeArcAutoreleasePoolPushCall();
void RecordRuntimeArcAutoreleasePoolPopCall();
void RecordRuntimeArcCurrentPropertyReadCall(
    const RuntimeDispatchFrame *frame);
void RecordRuntimeArcLastPropertyReadValue(int value);
void RecordRuntimeArcCurrentPropertyWriteCall(
    const RuntimeDispatchFrame *frame,
    int value);
void RecordRuntimeArcCurrentPropertyExchangeCall(
    const RuntimeDispatchFrame *frame,
    int value);
void RecordRuntimeArcLastPropertyExchangePreviousValue(int value);
void RecordRuntimeArcWeakCurrentPropertyLoadCall();
void RecordRuntimeArcLastWeakCurrentPropertyLoadedValue(int value);
void RecordRuntimeArcWeakCurrentPropertyStoreCall(int value);

}  // namespace objc3c::runtime
