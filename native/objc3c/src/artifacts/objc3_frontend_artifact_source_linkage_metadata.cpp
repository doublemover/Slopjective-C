#include "artifacts/objc3_frontend_artifact_source_linkage_metadata.h"

namespace {

const char *ArtifactLanguageProfileName(Objc3FrontendLanguageProfile mode) {
  (void)mode;
  return "canonical";
}

const char *ArtifactArcModeName(Objc3FrontendArcMode mode) {
  return mode == Objc3FrontendArcMode::kEnabled ? "enabled" : "disabled";
}

}  // namespace

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendSourceLinkageMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3FrontendOptions &options,
    const Objc3VersionedConformanceReportLoweringSummary
        &versioned_conformance_report_lowering,
    const Objc3FrontendCanonicalLiteralRejectionCounts
        &canonical_literal_rejection_counts,
    const Objc3InterfaceImplementationSummary &interface_implementation_summary,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary) {
  ir_frontend_metadata.language_version = options.language_version;
  ir_frontend_metadata.language_profile =
      ArtifactLanguageProfileName(options.language_profile);
  ir_frontend_metadata.arc_mode = ArtifactArcModeName(options.arc_mode);
  ir_frontend_metadata.arc_mode_enabled =
      options.arc_mode == Objc3FrontendArcMode::kEnabled;
  ir_frontend_metadata.versioned_conformance_report_lowering_ready =
      IsReadyObjc3VersionedConformanceReportLoweringSummary(
          versioned_conformance_report_lowering);
  ir_frontend_metadata.versioned_conformance_report_lowering_replay_key =
      versioned_conformance_report_lowering.replay_key;
  ir_frontend_metadata.canonical_literal_yes_rejection_sites =
      canonical_literal_rejection_counts.yes_literal_sites;
  ir_frontend_metadata.canonical_literal_no_rejection_sites =
      canonical_literal_rejection_counts.no_literal_sites;
  ir_frontend_metadata.canonical_literal_null_rejection_sites =
      canonical_literal_rejection_counts.null_literal_sites;

  ir_frontend_metadata.declared_interfaces =
      interface_implementation_summary.declared_interfaces;
  ir_frontend_metadata.declared_implementations =
      interface_implementation_summary.declared_implementations;
  ir_frontend_metadata.resolved_interface_symbols =
      interface_implementation_summary.resolved_interfaces;
  ir_frontend_metadata.resolved_implementation_symbols =
      interface_implementation_summary.resolved_implementations;
  ir_frontend_metadata.interface_method_symbols =
      interface_implementation_summary.interface_method_symbols;
  ir_frontend_metadata.implementation_method_symbols =
      interface_implementation_summary.implementation_method_symbols;
  ir_frontend_metadata.linked_implementation_symbols =
      interface_implementation_summary.linked_implementation_symbols;

  ir_frontend_metadata.declared_protocols =
      protocol_category_summary.declared_protocols;
  ir_frontend_metadata.declared_categories =
      protocol_category_summary.declared_categories;
  ir_frontend_metadata.resolved_protocol_symbols =
      protocol_category_summary.resolved_protocol_symbols;
  ir_frontend_metadata.resolved_category_symbols =
      protocol_category_summary.resolved_category_symbols;
  ir_frontend_metadata.protocol_method_symbols =
      protocol_category_summary.protocol_method_symbols;
  ir_frontend_metadata.category_method_symbols =
      protocol_category_summary.category_method_symbols;
  ir_frontend_metadata.linked_category_symbols =
      protocol_category_summary.linked_category_symbols;

  ir_frontend_metadata.declared_class_interfaces =
      class_protocol_category_linking_summary.declared_class_interfaces;
  ir_frontend_metadata.declared_class_implementations =
      class_protocol_category_linking_summary.declared_class_implementations;
  ir_frontend_metadata.resolved_class_interfaces =
      class_protocol_category_linking_summary.resolved_class_interfaces;
  ir_frontend_metadata.resolved_class_implementations =
      class_protocol_category_linking_summary.resolved_class_implementations;
  ir_frontend_metadata.linked_class_method_symbols =
      class_protocol_category_linking_summary.linked_class_method_symbols;
  ir_frontend_metadata.linked_category_method_symbols =
      class_protocol_category_linking_summary.linked_category_method_symbols;
  ir_frontend_metadata.protocol_composition_sites =
      class_protocol_category_linking_summary.protocol_composition_sites;
  ir_frontend_metadata.protocol_composition_symbols =
      class_protocol_category_linking_summary.protocol_composition_symbols;
  ir_frontend_metadata.category_composition_sites =
      class_protocol_category_linking_summary.category_composition_sites;
  ir_frontend_metadata.category_composition_symbols =
      class_protocol_category_linking_summary.category_composition_symbols;
  ir_frontend_metadata.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;

  ir_frontend_metadata.selector_method_declaration_entries =
      selector_normalization_summary.method_declaration_entries;
  ir_frontend_metadata.selector_normalized_method_declarations =
      selector_normalization_summary.normalized_method_declarations;
  ir_frontend_metadata.selector_piece_entries =
      selector_normalization_summary.selector_piece_entries;
  ir_frontend_metadata.selector_piece_parameter_links =
      selector_normalization_summary.selector_piece_parameter_links;

  ir_frontend_metadata.property_declaration_entries =
      property_attribute_summary.property_declaration_entries;
  ir_frontend_metadata.property_attribute_entries =
      property_attribute_summary.property_attribute_entries;
  ir_frontend_metadata.property_attribute_value_entries =
      property_attribute_summary.property_attribute_value_entries;
  ir_frontend_metadata.property_accessor_modifier_entries =
      property_attribute_summary.property_accessor_modifier_entries;
  ir_frontend_metadata.property_getter_selector_entries =
      property_attribute_summary.property_getter_selector_entries;
  ir_frontend_metadata.property_setter_selector_entries =
      property_attribute_summary.property_setter_selector_entries;
}

}  // namespace objc3::artifacts::frontend
