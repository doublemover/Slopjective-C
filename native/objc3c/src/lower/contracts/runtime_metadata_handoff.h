#pragma once

#include "ast/objc3_ast_declarations.h"
#include "lower/contracts/lowering_ownership_contracts.h"

#include <cstddef>
#include <cstdint>
#include <string>

inline constexpr const char *kObjc3RuntimeRegistrationPublicationContractId =
    "objc3c.runtime.registration.publication.owner.contract.v1";
inline constexpr const char *kObjc3RuntimeRegistrationDescriptorArtifactName =
    "module.runtime-registration-descriptor.json";
inline constexpr const char *kObjc3RuntimeRegistrationManifestArtifactName =
    "module.runtime-registration-manifest.json";
inline constexpr const char *kObjc3RuntimeRegistrationPublicationOwnerModel =
    kObjc3LoweringNoRetiredRouteOwnerModel;

struct Objc3RuntimeRegistrationPublicationContract {
  std::string module_name;
  std::string contract_id = kObjc3RuntimeRegistrationPublicationContractId;
  std::string descriptor_artifact =
      kObjc3RuntimeRegistrationDescriptorArtifactName;
  std::string manifest_artifact =
      kObjc3RuntimeRegistrationManifestArtifactName;
  std::string metadata_handoff_owner = kObjc3RuntimeMetadataHandoffOwner;
  std::string descriptor_lowering_owner =
      kObjc3RuntimeRegistrationDescriptorLoweringOwner;
  std::string constructor_root_publication_owner =
      kObjc3RuntimeConstructorRootPublicationOwner;
  std::string init_stub_publication_owner =
      kObjc3RuntimeInitStubPublicationOwner;
  std::string registration_table_publication_owner =
      kObjc3RuntimeRegistrationTablePublicationOwner;
  std::string owner_model = kObjc3RuntimeRegistrationPublicationOwnerModel;
  std::size_t registration_root_count = 0;
  std::size_t descriptor_family_count = 0;
  std::size_t table_pointer_field_count = 0;
  std::uint64_t table_abi_version = 0;
  bool descriptor_artifact_authoritative = false;
  bool manifest_artifact_authoritative = false;
  bool constructor_root_published = false;
  bool init_stub_published = false;
  bool registration_table_published = false;
  bool image_local_init_state_published = false;
  bool owner_split_explicit = false;
  bool deterministic = false;
  bool ready = false;
  std::string replay_key;
};

struct Objc3RuntimeMetadataLoweringHandoff {
  std::string module_name;
  std::size_t class_count = 0;
  std::size_t protocol_count = 0;
  std::size_t category_count = 0;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  std::size_t ivar_layout_count = 0;
  bool has_runtime_registration_roots = false;
  bool deterministic = false;
  Objc3RuntimeRegistrationPublicationContract registration_publication;
  std::string replay_key;
};

Objc3RuntimeRegistrationPublicationContract
Objc3BuildRuntimeRegistrationPublicationContract(
    const Objc3RuntimeMetadataLoweringHandoff &handoff);
bool Objc3RuntimeRegistrationPublicationContractIsReady(
    const Objc3RuntimeRegistrationPublicationContract &contract);
std::string Objc3RuntimeRegistrationPublicationContractReplayKey(
    const Objc3RuntimeRegistrationPublicationContract &contract);
Objc3RuntimeMetadataLoweringHandoff Objc3BuildRuntimeMetadataLoweringHandoff(
    const Objc3Program &program);
bool Objc3RuntimeMetadataLoweringHandoffIsReady(
    const Objc3RuntimeMetadataLoweringHandoff &handoff);
std::string Objc3RuntimeMetadataLoweringHandoffReplayKey(
    const Objc3RuntimeMetadataLoweringHandoff &handoff);
