#pragma once

#include <string_view>

#include "support/objc3_ir_object_backend_kind.h"

namespace objc3c::support {

bool ParseIrObjectBackendToken(std::string_view value,
                               IrObjectBackendToken &backend);

}  // namespace objc3c::support
