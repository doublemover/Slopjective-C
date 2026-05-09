#pragma once

#include "sema/objc3_semantic_type_helpers.h"

#include <string>
#include <vector>

std::string JoinStringVector(const std::vector<std::string> &items,
                             const std::string &separator);
