#pragma once

#include <cstddef>
#include <string>

struct Objc3Program;

namespace objc3c::artifacts {

bool IsSuspiciousObjc3NativeIRTruthGap(
    const std::string &ir_text,
    const Objc3Program &program,
    std::size_t message_send_sites);

}  // namespace objc3c::artifacts
