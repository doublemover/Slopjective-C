#pragma once

#include <filesystem>

#include "driver/objc3_cli_options.h"

enum class Objc3DriverInputKind {
  kObjc3Language,
  kObjectiveCTranslationUnit,
};

Objc3DriverInputKind ClassifyObjc3DriverInput(
    const std::filesystem::path &input);
bool NeedsObjc3DriverClangPath(Objc3DriverInputKind input_kind,
                               Objc3IrObjectBackend ir_object_backend);
bool NeedsObjc3DriverLlcPath(Objc3DriverInputKind input_kind,
                             Objc3IrObjectBackend ir_object_backend);
