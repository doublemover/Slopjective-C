#include "lower/contracts/runtime_metadata_handoff.h"

#include <sstream>

namespace {

void AddInterfaceMetadata(const Objc3InterfaceDecl &decl,
                          Objc3RuntimeMetadataLoweringHandoff &handoff) {
  if (decl.has_category) {
    ++handoff.category_count;
  } else {
    ++handoff.class_count;
  }
  handoff.property_count += decl.properties.size();
  handoff.method_count += decl.methods.size();
  for (const Objc3PropertyDecl &property : decl.properties) {
    if (property.executable_ivar_layout_valid ||
        !property.executable_ivar_layout_symbol.empty()) {
      ++handoff.ivar_layout_count;
    }
  }
}

void AddImplementationMetadata(
    const Objc3ImplementationDecl &decl,
    Objc3RuntimeMetadataLoweringHandoff &handoff) {
  if (decl.has_category) {
    ++handoff.category_count;
  }
  handoff.property_count += decl.properties.size();
  handoff.method_count += decl.methods.size();
  for (const Objc3PropertyDecl &property : decl.properties) {
    if (property.executable_ivar_layout_valid ||
        !property.executable_ivar_layout_symbol.empty()) {
      ++handoff.ivar_layout_count;
    }
  }
}

}  // namespace

Objc3RuntimeMetadataLoweringHandoff Objc3BuildRuntimeMetadataLoweringHandoff(
    const Objc3Program &program) {
  Objc3RuntimeMetadataLoweringHandoff handoff;
  handoff.module_name = program.module_name;
  handoff.protocol_count = program.protocols.size();
  for (const Objc3ProtocolDecl &protocol : program.protocols) {
    handoff.property_count += protocol.properties.size();
    handoff.method_count += protocol.methods.size();
  }
  for (const Objc3InterfaceDecl &interface_decl : program.interfaces) {
    AddInterfaceMetadata(interface_decl, handoff);
  }
  for (const Objc3ImplementationDecl &implementation_decl :
       program.implementations) {
    AddImplementationMetadata(implementation_decl, handoff);
  }
  handoff.has_runtime_registration_roots =
      handoff.class_count > 0 || handoff.protocol_count > 0 ||
      handoff.category_count > 0 || handoff.method_count > 0 ||
      handoff.property_count > 0 || handoff.ivar_layout_count > 0;
  handoff.deterministic = !handoff.module_name.empty();
  handoff.registration_publication =
      Objc3BuildRuntimeRegistrationPublicationContract(handoff);
  handoff.replay_key = Objc3RuntimeMetadataLoweringHandoffReplayKey(handoff);
  return handoff;
}

Objc3RuntimeRegistrationPublicationContract
Objc3BuildRuntimeRegistrationPublicationContract(
    const Objc3RuntimeMetadataLoweringHandoff &handoff) {
  Objc3RuntimeRegistrationPublicationContract contract;
  contract.module_name = handoff.module_name;
  contract.registration_root_count =
      handoff.class_count + handoff.protocol_count + handoff.category_count +
      handoff.method_count + handoff.property_count + handoff.ivar_layout_count;
  contract.descriptor_family_count =
      handoff.class_count + handoff.protocol_count + handoff.category_count +
      handoff.method_count + handoff.property_count + handoff.ivar_layout_count;
  contract.table_pointer_field_count = 12u;
  contract.table_abi_version = 2u;
  contract.descriptor_artifact_authoritative =
      handoff.has_runtime_registration_roots;
  contract.manifest_artifact_authoritative =
      handoff.has_runtime_registration_roots;
  contract.constructor_root_published = handoff.has_runtime_registration_roots;
  contract.init_stub_published = handoff.has_runtime_registration_roots;
  contract.registration_table_published = handoff.has_runtime_registration_roots;
  contract.image_local_init_state_published =
      handoff.has_runtime_registration_roots;
  contract.deterministic = handoff.deterministic;
  contract.owner_split_explicit =
      Objc3LoweringStrictOwnerModelIsReady(
          contract.metadata_handoff_owner,
          contract.owner_model,
          true,
          true) &&
      Objc3LoweringStrictOwnerModelIsReady(
          contract.descriptor_lowering_owner,
          contract.owner_model,
          true,
          true) &&
      Objc3LoweringStrictOwnerModelIsReady(
          contract.constructor_root_publication_owner,
          contract.owner_model,
          true,
          true) &&
      Objc3LoweringStrictOwnerModelIsReady(
          contract.init_stub_publication_owner,
          contract.owner_model,
          true,
          true) &&
      Objc3LoweringStrictOwnerModelIsReady(
          contract.registration_table_publication_owner,
          contract.owner_model,
          true,
          true);
  contract.ready = Objc3RuntimeRegistrationPublicationContractIsReady(contract);
  contract.replay_key =
      Objc3RuntimeRegistrationPublicationContractReplayKey(contract);
  return contract;
}

bool Objc3RuntimeRegistrationPublicationContractIsReady(
    const Objc3RuntimeRegistrationPublicationContract &contract) {
  return contract.deterministic && !contract.module_name.empty() &&
         contract.registration_root_count > 0u &&
         contract.descriptor_family_count > 0u &&
         contract.table_abi_version == 2u &&
         contract.table_pointer_field_count == 12u &&
         contract.descriptor_artifact_authoritative &&
         contract.manifest_artifact_authoritative &&
         contract.constructor_root_published &&
         contract.init_stub_published &&
         contract.registration_table_published &&
         contract.image_local_init_state_published &&
         contract.owner_split_explicit &&
         Objc3LoweringStrictOwnerModelIsReady(
             contract.metadata_handoff_owner,
             contract.owner_model,
             true,
             true) &&
         Objc3LoweringStrictOwnerModelIsReady(
             contract.descriptor_lowering_owner,
             contract.owner_model,
             true,
             true) &&
         Objc3LoweringStrictOwnerModelIsReady(
             contract.constructor_root_publication_owner,
             contract.owner_model,
             true,
             true) &&
         Objc3LoweringStrictOwnerModelIsReady(
             contract.init_stub_publication_owner,
             contract.owner_model,
             true,
             true) &&
         Objc3LoweringStrictOwnerModelIsReady(
             contract.registration_table_publication_owner,
             contract.owner_model,
             true,
             true);
}

bool Objc3RuntimeMetadataLoweringHandoffIsReady(
    const Objc3RuntimeMetadataLoweringHandoff &handoff) {
  return handoff.deterministic && !handoff.module_name.empty() &&
         handoff.has_runtime_registration_roots &&
         Objc3RuntimeRegistrationPublicationContractIsReady(
             handoff.registration_publication);
}

std::string Objc3RuntimeRegistrationPublicationContractReplayKey(
    const Objc3RuntimeRegistrationPublicationContract &contract) {
  std::ostringstream out;
  out << "ready="
      << (Objc3RuntimeRegistrationPublicationContractIsReady(contract) ? "true"
                                                                       : "false")
      << ";contract_id=" << contract.contract_id
      << ";module=" << contract.module_name
      << ";descriptor_artifact=" << contract.descriptor_artifact
      << ";manifest_artifact=" << contract.manifest_artifact
      << ";registration_roots=" << contract.registration_root_count
      << ";descriptor_families=" << contract.descriptor_family_count
      << ";table_abi_version=" << contract.table_abi_version
      << ";table_pointer_fields=" << contract.table_pointer_field_count
      << ";descriptor_authoritative="
      << (contract.descriptor_artifact_authoritative ? "true" : "false")
      << ";manifest_authoritative="
      << (contract.manifest_artifact_authoritative ? "true" : "false")
      << ";constructor_root_published="
      << (contract.constructor_root_published ? "true" : "false")
      << ";init_stub_published="
      << (contract.init_stub_published ? "true" : "false")
      << ";registration_table_published="
      << (contract.registration_table_published ? "true" : "false")
      << ";image_local_init_state_published="
      << (contract.image_local_init_state_published ? "true" : "false")
      << ";owner_split_explicit="
      << (contract.owner_split_explicit ? "true" : "false")
      << ";metadata_handoff_owner=" << contract.metadata_handoff_owner
      << ";descriptor_lowering_owner=" << contract.descriptor_lowering_owner
      << ";constructor_root_publication_owner="
      << contract.constructor_root_publication_owner
      << ";init_stub_publication_owner="
      << contract.init_stub_publication_owner
      << ";registration_table_publication_owner="
      << contract.registration_table_publication_owner
      << ";owner_model=" << contract.owner_model;
  return out.str();
}

std::string Objc3RuntimeMetadataLoweringHandoffReplayKey(
    const Objc3RuntimeMetadataLoweringHandoff &handoff) {
  std::ostringstream out;
  out << "module=" << handoff.module_name << ";classes=" << handoff.class_count
      << ";protocols=" << handoff.protocol_count
      << ";categories=" << handoff.category_count
      << ";properties=" << handoff.property_count
      << ";methods=" << handoff.method_count
      << ";ivar_layouts=" << handoff.ivar_layout_count
      << ";roots="
      << (handoff.has_runtime_registration_roots ? "true" : "false")
      << ";deterministic=" << (handoff.deterministic ? "true" : "false")
      << ";registration_publication={"
      << Objc3RuntimeRegistrationPublicationContractReplayKey(
             handoff.registration_publication)
      << "}";
  return out.str();
}
