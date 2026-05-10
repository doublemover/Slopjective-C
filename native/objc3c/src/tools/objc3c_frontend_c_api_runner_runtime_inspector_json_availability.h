#pragma once

#include <iosfwd>
#include <string>

void WriteFrontendCApiRunnerRuntimeInspectorAvailabilityReasonJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &availability_reason);
