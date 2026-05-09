#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::artifacts {

bool IsSuspiciousObjc3NativeIRTruthGap(
    const std::string &ir_text,
    const Objc3Program &program,
    const Objc3MessageSendSelectorLoweringContract &message_send_contract);

}  // namespace objc3c::artifacts
