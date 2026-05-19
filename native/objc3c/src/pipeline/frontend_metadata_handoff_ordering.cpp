#include "pipeline/frontend_metadata_handoff_ordering.h"

#include <tuple>

bool IsClassSourceRecordLess(const Objc3RuntimeMetadataClassSourceRecord &lhs,
                             const Objc3RuntimeMetadataClassSourceRecord &rhs) {
  return std::tie(lhs.name, lhs.record_kind, lhs.objc_final_declared,
                  lhs.objc_sealed_declared, lhs.line, lhs.column) <
         std::tie(rhs.name, rhs.record_kind, rhs.objc_final_declared,
                  rhs.objc_sealed_declared, rhs.line, rhs.column);
}

bool IsProtocolSourceRecordLess(
    const Objc3RuntimeMetadataProtocolSourceRecord &lhs,
    const Objc3RuntimeMetadataProtocolSourceRecord &rhs) {
  return std::tie(lhs.name, lhs.line, lhs.column) <
         std::tie(rhs.name, rhs.line, rhs.column);
}

bool IsCategorySourceRecordLess(
    const Objc3RuntimeMetadataCategorySourceRecord &lhs,
    const Objc3RuntimeMetadataCategorySourceRecord &rhs) {
  return std::tie(lhs.class_name, lhs.category_name, lhs.record_kind, lhs.line,
                  lhs.column) <
         std::tie(rhs.class_name, rhs.category_name, rhs.record_kind, rhs.line,
                  rhs.column);
}

bool IsPropertySourceRecordLess(
    const Objc3RuntimeMetadataPropertySourceRecord &lhs,
    const Objc3RuntimeMetadataPropertySourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name, lhs.line,
                  lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name, rhs.line,
                  rhs.column);
}

bool IsMethodSourceRecordLess(const Objc3RuntimeMetadataMethodSourceRecord &lhs,
                              const Objc3RuntimeMetadataMethodSourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.selector,
                  lhs.effective_direct_dispatch, lhs.objc_final_declared,
                  lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.selector,
                  rhs.effective_direct_dispatch, rhs.objc_final_declared,
                  rhs.line, rhs.column);
}

bool IsIvarSourceRecordLess(const Objc3RuntimeMetadataIvarSourceRecord &lhs,
                            const Objc3RuntimeMetadataIvarSourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name,
                  lhs.ivar_binding_symbol, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name,
                  rhs.ivar_binding_symbol, rhs.line, rhs.column);
}

bool IsExecutableMetadataInterfaceNodeLess(
    const Objc3ExecutableMetadataInterfaceGraphNode &lhs,
    const Objc3ExecutableMetadataInterfaceGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataImplementationNodeLess(
    const Objc3ExecutableMetadataImplementationGraphNode &lhs,
    const Objc3ExecutableMetadataImplementationGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataClassNodeLess(
    const Objc3ExecutableMetadataClassGraphNode &lhs,
    const Objc3ExecutableMetadataClassGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataMetaclassNodeLess(
    const Objc3ExecutableMetadataMetaclassGraphNode &lhs,
    const Objc3ExecutableMetadataMetaclassGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataProtocolNodeLess(
    const Objc3ExecutableMetadataProtocolGraphNode &lhs,
    const Objc3ExecutableMetadataProtocolGraphNode &rhs) {
  return std::tie(lhs.protocol_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.protocol_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataCategoryNodeLess(
    const Objc3ExecutableMetadataCategoryGraphNode &lhs,
    const Objc3ExecutableMetadataCategoryGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.category_name, lhs.owner_identity, lhs.line,
                  lhs.column) <
         std::tie(rhs.class_name, rhs.category_name, rhs.owner_identity, rhs.line,
                  rhs.column);
}

bool IsExecutableMetadataPropertyNodeLess(
    const Objc3ExecutableMetadataPropertyGraphNode &lhs,
    const Objc3ExecutableMetadataPropertyGraphNode &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name,
                  lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name,
                  rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataMethodNodeLess(
    const Objc3ExecutableMetadataMethodGraphNode &lhs,
    const Objc3ExecutableMetadataMethodGraphNode &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.selector,
                  lhs.is_class_method, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.selector,
                  rhs.is_class_method, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataIvarNodeLess(
    const Objc3ExecutableMetadataIvarGraphNode &lhs,
    const Objc3ExecutableMetadataIvarGraphNode &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name,
                  lhs.ivar_binding_symbol, lhs.owner_identity, lhs.line,
                  lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name,
                  rhs.ivar_binding_symbol, rhs.owner_identity, rhs.line,
                  rhs.column);
}

bool IsExecutableMetadataGraphEdgeLess(
    const Objc3ExecutableMetadataGraphEdge &lhs,
    const Objc3ExecutableMetadataGraphEdge &rhs) {
  return std::tie(lhs.edge_kind, lhs.source_owner_identity, lhs.target_owner_identity,
                  lhs.line, lhs.column) <
         std::tie(rhs.edge_kind, rhs.source_owner_identity, rhs.target_owner_identity,
                  rhs.line, rhs.column);
}
