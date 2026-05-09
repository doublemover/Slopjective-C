#include "driver/objc3_driver_shell_paths.h"

#include <string>

#include "diag/objc3_diag_utils.h"

Objc3DriverInputKind ClassifyObjc3DriverInput(
    const std::filesystem::path &input) {
  const std::string extension = ToLower(input.extension().string());
  return extension == ".objc3"
             ? Objc3DriverInputKind::kObjc3Language
             : Objc3DriverInputKind::kObjectiveCTranslationUnit;
}

bool NeedsObjc3DriverClangPath(Objc3DriverInputKind input_kind,
                               Objc3IrObjectBackend ir_object_backend) {
  return input_kind != Objc3DriverInputKind::kObjc3Language ||
         ir_object_backend == Objc3IrObjectBackend::kClang;
}

bool NeedsObjc3DriverLlcPath(Objc3DriverInputKind input_kind,
                             Objc3IrObjectBackend ir_object_backend) {
  return input_kind == Objc3DriverInputKind::kObjc3Language &&
         ir_object_backend == Objc3IrObjectBackend::kLLVMDirect;
}
