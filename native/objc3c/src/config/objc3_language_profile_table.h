#pragma once

#include <span>

#include "config/objc3_language_profile.h"

namespace objc3c::config {

std::span<const LanguageProfileContract> LanguageProfileTable();

}  // namespace objc3c::config
