#pragma once

#include <iosfwd>
#include <string>

void WriteFrontendCApiRunnerRuntimeInspectorRuntimeAbiJsonRows(
    std::ostream &out,
    const std::string &child_indent);
