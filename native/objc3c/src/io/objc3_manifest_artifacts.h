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
