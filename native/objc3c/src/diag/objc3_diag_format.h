#pragma once

#include <string>

std::string MakeDiag(unsigned line,
                     unsigned column,
                     const std::string &code,
                     const std::string &message);
