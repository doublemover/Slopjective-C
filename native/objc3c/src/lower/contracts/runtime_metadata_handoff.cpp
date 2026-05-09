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
  handoff.replay_key = Objc3RuntimeMetadataLoweringHandoffReplayKey(handoff);
  return handoff;
}

bool Objc3RuntimeMetadataLoweringHandoffIsReady(
    const Objc3RuntimeMetadataLoweringHandoff &handoff) {
  return handoff.deterministic && !handoff.module_name.empty() &&
         handoff.has_runtime_registration_roots;
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
      << ";deterministic=" << (handoff.deterministic ? "true" : "false");
  return out.str();
}
