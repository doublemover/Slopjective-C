#pragma once

#include <filesystem>
#include <string>

#include "driver/objc3_cli_options.h"

enum class Objc3DriverInputKind {
  kObjc3Language,
  kObjectiveCTranslationUnit,
};

Objc3DriverInputKind ClassifyObjc3DriverInput(const std::filesystem::path &input);
bool NeedsObjc3DriverClangPath(Objc3DriverInputKind input_kind, Objc3IrObjectBackend ir_object_backend);
bool NeedsObjc3DriverLlcPath(Objc3DriverInputKind input_kind, Objc3IrObjectBackend ir_object_backend);
bool ValidateObjc3DriverShellInputs(const Objc3CliOptions &cli_options,
                                    Objc3DriverInputKind input_kind,
                                    std::string &error);
