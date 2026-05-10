#pragma once

#include <filesystem>
#include <string>

#include "pipeline/objc3_runtime_import_surface.h"
#include "pipeline/runtime_import_json_helpers.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulatePreservationEvidence(const RuntimeImportJsonValue::Object &root,
                                  Objc3ImportedRuntimeModuleSurface &surface,
                                  std::string &error);

bool PopulateFrontendClosureSummary(
    const RuntimeImportJsonValue::Object &root,
    Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    std::string &error);

bool PopulateImportedRuntimeLanguageEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedInteropEvidence(const RuntimeImportJsonValue::Object &root,
                                     Objc3ImportedRuntimeModuleSurface &surface,
                                     std::string &error);

bool PopulateImportedMetaprogrammingEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedDispatchEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedRuntimeArtifactEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedTypeSystemOptionalKeypathSurfaceEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedTypeSystemGenericContractEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedTypeSystemNullabilityContractEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedTypeSystemProtocolContractEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateSerializedRuntimeMetadataReuse(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool ParseImportedRuntimeModuleSurface(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PublishImportedRuntimeModuleSurfaceReadiness(
    const std::filesystem::path &path,
    const Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool LoadImportedRuntimeModulePackagingPeerArtifacts(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error);

bool PublishImportedRuntimeModulePackagingLinkPlanReadiness(
    const Objc3ImportedRuntimeModuleSurface &surface,
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error);

}  // namespace objc3c::pipeline::runtime_import_preservation
