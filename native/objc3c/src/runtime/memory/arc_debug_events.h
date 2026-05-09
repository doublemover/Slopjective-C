#pragma once

namespace objc3c::runtime {

void RecordRuntimeArcRetainCall(int value);
void RecordRuntimeArcReleaseCall(int value);
void RecordRuntimeArcAutoreleaseCall(int value);
void RecordRuntimeArcAutoreleasePoolPushCall();
void RecordRuntimeArcAutoreleasePoolPopCall();

}  // namespace objc3c::runtime
