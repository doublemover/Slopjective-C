#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "artifacts/json/semantic_type_manifest_json.h"
#include "sema/objc3_sema_contract_semantic_type_metadata_records.h"

namespace objc3::artifacts::json {

std::string RenderSemanticCanonicalType(
    const Objc3SemanticCanonicalType &type);
std::string RenderSemanticCanonicalTypeArray(
    const std::vector<Objc3SemanticCanonicalType> &types);
void WriteSemanticCanonicalTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticCanonicalType &type);
void WriteSemanticMethodTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticMethodTypeMetadata &metadata);
void WriteSemanticPropertyTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticPropertyTypeMetadata &metadata);
void WriteSemanticFunctionTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticFunctionTypeMetadata &metadata);
void WriteSemanticInterfaceTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticInterfaceTypeMetadata &metadata);
void WriteSemanticImplementationTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticImplementationTypeMetadata &metadata);

}  // namespace objc3::artifacts::json
