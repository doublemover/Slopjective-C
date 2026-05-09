#include "driver/objc3_driver_command_dispatch.h"

#include "driver/objc3_objectivec_path.h"
#include "driver/objc3_objc3_path.h"

int DispatchObjc3DriverCommand(const Objc3CliOptions &cli_options,
                               Objc3DriverInputKind input_kind) {
  if (input_kind == Objc3DriverInputKind::kObjc3Language) {
    return RunObjc3LanguagePath(cli_options);
  }
  return RunObjectiveCPath(cli_options);
}
