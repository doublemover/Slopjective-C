#pragma once

#include <iosfwd>
#include <string>

void WriteFrontendCApiRunnerPlaygroundReproPublicSurfaceJsonRows(
    std::ostream &out,
    const std::string &child_indent);
