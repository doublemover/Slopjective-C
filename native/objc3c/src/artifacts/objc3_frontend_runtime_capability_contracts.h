#pragma once

namespace objc3::artifacts::frontend {

inline constexpr const char *kArtifactRuntimeCapabilityReportingContractId =
    "objc3c.runtime.capability.reporting.v1";
inline constexpr const char *kArtifactRuntimeCapabilityReportingSchemaId =
    "objc3c-runtime-capability-report-v1";
inline constexpr const char *kArtifactRuntimeCapabilityReportingSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_capability_report";
inline constexpr const char *kArtifactRuntimeCapabilityReportingProfileModel =
    "core-profile-claimed-while-strict-profiles-and-optional-feature-gaps-remain-not-claimed-until-runtime-backed";
inline constexpr const char
    *kArtifactRuntimeCapabilityReportingOptionalFeatureModel =
        "unsupported-runtime-feature-ids-lower-into-not-claimed-public-optional-features";
inline constexpr const char *kArtifactRuntimeCapabilityReportingVersionModel =
    "deterministic-dev-version-surface-for-frontend-runtime-stdlib-and-module-format";
inline constexpr const char *kArtifactRuntimeCapabilityPublicSchemaId =
    "objc3-conformance-report/v1";
inline constexpr const char *kArtifactRuntimeCapabilityGeneratedAtReplayValue =
    "1970-01-01T00:00:00Z";
inline constexpr const char *kArtifactRuntimeCapabilityToolchainName =
    "objc3c";
inline constexpr const char *kArtifactRuntimeCapabilityToolchainVendor =
    "doublemover";
inline constexpr const char *kArtifactRuntimeCapabilityToolchainVersion =
    "0.0.0-dev";
inline constexpr const char *kArtifactRuntimeCapabilityTargetTriple =
    "x86_64-pc-windows-msvc";
inline constexpr const char *kArtifactRuntimeCapabilityLanguageFamily =
    "objective-c";
inline constexpr const char *kArtifactRuntimeCapabilityLanguageVersion = "3.0";
inline constexpr const char *kArtifactRuntimeCapabilitySpecRevision = "v1";
inline constexpr const char *kArtifactRuntimeCapabilityStrictnessMode =
    "permissive";
inline constexpr const char *kArtifactRuntimeCapabilityConcurrencyMode = "off";
inline constexpr const char *kArtifactRuntimeCapabilityModuleFormatVersion =
    "objc3c-runtime-metadata-v1";

}  // namespace objc3::artifacts::frontend
