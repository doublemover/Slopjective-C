#pragma once

#include <filesystem>
#include <string>

std::filesystem::path BuildManifestArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);
std::filesystem::path BuildRuntimeMetadataBinaryArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);
std::filesystem::path BuildRuntimeMetadataLinkerResponseArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);
std::filesystem::path BuildRuntimeMetadataDiscoveryArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);
std::filesystem::path BuildRuntimeAwareImportModuleArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);
std::filesystem::path BuildVersionedConformanceReportArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);
std::filesystem::path BuildConformancePublicationArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);
std::filesystem::path BuildRuntimeRegistrationManifestArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);
std::filesystem::path BuildRuntimeRegistrationDescriptorArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);
std::filesystem::path BuildCrossModuleRuntimeLinkPlanArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);
std::filesystem::path BuildCrossModuleRuntimeLinkerResponseArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);

void WriteManifestArtifact(const std::filesystem::path &out_dir,
                           const std::string &emit_prefix,
                           const std::string &manifest_json);
void WriteRuntimeMetadataBinaryArtifact(const std::filesystem::path &out_dir,
                                        const std::string &emit_prefix,
                                        const std::string &binary_payload);
void WriteRuntimeMetadataLinkerResponseArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &response_payload);
void WriteRuntimeMetadataDiscoveryArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &discovery_json);
void WriteRuntimeAwareImportModuleArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json);
void WriteVersionedConformanceReportArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json);
void WriteConformancePublicationArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json);
void WriteRuntimeRegistrationManifestArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &manifest_json);
void WriteRuntimeRegistrationDescriptorArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &descriptor_json);
void WriteCrossModuleRuntimeLinkPlanArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &plan_json);
void WriteCrossModuleRuntimeLinkerResponseArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &response_payload);
