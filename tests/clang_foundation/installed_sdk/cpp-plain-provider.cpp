#include "cpp-plain-module.cpp"
PlainData c_plain_echo(PlainData value) { ++value.marker; return value; }
PlainData cpp_plain_echo(PlainData value) { ++value.marker; return value; }
int plain_overload(int value) { return 100 + value; }
long plain_overload(long value) { return 200 + value; }
PlainMode plain_mode(void) { return PlainMode::Copy; }
