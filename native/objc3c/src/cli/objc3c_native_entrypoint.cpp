#include "cli/objc3c_native_entrypoint.h"

#include "driver/objc3_driver_main.h"

int RunObjc3NativeCli(int argc, char **argv) {
  return RunObjc3DriverMain(argc, argv);
}
