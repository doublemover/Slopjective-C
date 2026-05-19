#pragma once

#include <cstddef>
#include <span>

#include "adr/objc3_adr_record.h"

namespace objc3c::adr {

std::span<const Objc3AdrRecord> Objc3FrontendAdrRecords();

const Objc3AdrRecord *FindObjc3FrontendAdrByNumber(uint16_t number);

const Objc3AdrRecord *FindObjc3FrontendAdrBySurface(
    std::string_view related_surface);

bool Objc3FrontendAdrRegistryIsWellFormed();

}  // namespace objc3c::adr
