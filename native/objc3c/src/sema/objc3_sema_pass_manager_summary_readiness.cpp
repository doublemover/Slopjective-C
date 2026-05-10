#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaCoreSemanticSummaryReadinessRecord
BuildObjc3SemaCoreSemanticSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaCoreSemanticSummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.interface_implementation_symbols_ready =
      surface.interface_implementation_summary.interface_method_symbols ==
          surface.interface_method_symbols_total &&
      surface.interface_implementation_summary.implementation_method_symbols ==
          surface.implementation_method_symbols_total &&
      surface.interface_implementation_summary.linked_implementation_symbols ==
          surface.linked_implementation_symbols_total;
  record.interface_implementation_handoff_ready =
      surface.interface_implementation_summary.deterministic &&
      surface.deterministic_interface_implementation_handoff;
  record.protocol_category_composition_symbols_ready =
      surface.protocol_category_composition_summary.protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.protocol_category_composition_summary.category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.protocol_category_composition_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites <=
          surface.protocol_category_composition_summary
              .total_composition_sites();
  record.protocol_category_composition_handoff_ready =
      surface.protocol_category_composition_summary.deterministic &&
      surface.deterministic_protocol_category_composition_handoff;
  record.class_protocol_category_linking_symbols_ready =
      surface.class_protocol_category_linking_summary.declared_interfaces ==
          surface.interface_implementation_summary.declared_interfaces &&
      surface.class_protocol_category_linking_summary.resolved_interfaces ==
          surface.interface_implementation_summary.resolved_interfaces &&
      surface.class_protocol_category_linking_summary.declared_implementations ==
          surface.interface_implementation_summary.declared_implementations &&
      surface.class_protocol_category_linking_summary.resolved_implementations ==
          surface.interface_implementation_summary.resolved_implementations &&
      surface.class_protocol_category_linking_summary.interface_method_symbols ==
          surface.interface_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .implementation_method_symbols ==
          surface.implementation_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .linked_implementation_symbols ==
          surface.linked_implementation_symbols_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.class_protocol_category_linking_summary.category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites <=
          surface.class_protocol_category_linking_summary
              .total_composition_sites();
  record.class_protocol_category_linking_handoff_ready =
      surface.class_protocol_category_linking_summary.deterministic &&
      surface.deterministic_class_protocol_category_linking_handoff;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.core_semantic_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.interface_implementation_symbols_ready &&
      record.interface_implementation_handoff_ready &&
      record.protocol_category_composition_symbols_ready &&
      record.protocol_category_composition_handoff_ready &&
      record.class_protocol_category_linking_symbols_ready &&
      record.class_protocol_category_linking_handoff_ready;
  return record;
}
