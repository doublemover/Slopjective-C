#include "cpp-data-module.mm"
CxxData c_data_echo(CxxData value) { ++value.marker; return value; }
CxxData cpp_data_echo(CxxData value) { ++value.marker; return value; }
int data_overload(int value) { return 100 + value; }
long data_overload(long value) { return 200 + value; }
DataMode data_mode(void) { return DataMode::Copy; }
