#pragma once

#include <cstddef>
#include <iosfwd>

#include "io/objc3_process.h"

void EmitObjc3RuntimeRegistrationManifestHeaderJson(
    std::ostream &out,
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record,
    std::size_t runtime_metadata_binary_byte_count);
