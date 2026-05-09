#include "driver/objc3_driver_cross_module_link_plan_local_feature_inputs.h"

void PopulateObjc3DriverCrossModuleRuntimeLinkPlanLocalFeatureInputs(
    Objc3CrossModuleRuntimeLinkPlanArtifactInputs &link_plan_inputs,
    const Objc3FrontendArtifactBundle &artifacts) {
  const auto local_block_ownership_summary =
      artifacts.runtime_block_ownership_artifact_preservation_summary;
  link_plan_inputs.local_block_ownership_block_literal_sites =
      local_block_ownership_summary.local_block_literal_sites;
  link_plan_inputs.local_block_ownership_invoke_trampoline_symbolized_sites =
      local_block_ownership_summary.local_invoke_trampoline_symbolized_sites;
  link_plan_inputs.local_block_ownership_copy_helper_required_sites =
      local_block_ownership_summary.local_copy_helper_required_sites;
  link_plan_inputs.local_block_ownership_dispose_helper_required_sites =
      local_block_ownership_summary.local_dispose_helper_required_sites;
  link_plan_inputs.local_block_ownership_copy_helper_symbolized_sites =
      local_block_ownership_summary.local_copy_helper_symbolized_sites;
  link_plan_inputs.local_block_ownership_dispose_helper_symbolized_sites =
      local_block_ownership_summary.local_dispose_helper_symbolized_sites;
  link_plan_inputs.local_block_ownership_escape_to_heap_sites =
      local_block_ownership_summary.local_escape_to_heap_sites;
  link_plan_inputs.local_block_ownership_byref_layout_symbolized_sites =
      local_block_ownership_summary.local_byref_layout_symbolized_sites;

  const auto local_storage_reflection_summary =
      artifacts.runtime_storage_reflection_artifact_preservation_summary;
  link_plan_inputs
      .local_storage_reflection_implementation_owned_property_entries =
      local_storage_reflection_summary.implementation_owned_property_entries;
  link_plan_inputs.local_storage_reflection_synthesized_accessor_owner_entries =
      local_storage_reflection_summary.synthesized_accessor_owner_entries;
  link_plan_inputs.local_storage_reflection_synthesized_getter_entries =
      local_storage_reflection_summary.synthesized_getter_entries;
  link_plan_inputs.local_storage_reflection_synthesized_setter_entries =
      local_storage_reflection_summary.synthesized_setter_entries;
  link_plan_inputs.local_storage_reflection_synthesized_accessor_entries =
      local_storage_reflection_summary.synthesized_accessor_entries;
  link_plan_inputs.local_storage_reflection_current_property_read_entries =
      local_storage_reflection_summary.current_property_read_entries;
  link_plan_inputs.local_storage_reflection_current_property_write_entries =
      local_storage_reflection_summary.current_property_write_entries;
  link_plan_inputs.local_storage_reflection_current_property_exchange_entries =
      local_storage_reflection_summary.current_property_exchange_entries;
  link_plan_inputs.local_storage_reflection_weak_current_property_load_entries =
      local_storage_reflection_summary.weak_current_property_load_entries;
  link_plan_inputs.local_storage_reflection_weak_current_property_store_entries =
      local_storage_reflection_summary.weak_current_property_store_entries;
  link_plan_inputs.local_storage_reflection_ivar_layout_entries =
      local_storage_reflection_summary.ivar_layout_entries;
  link_plan_inputs.local_storage_reflection_ivar_layout_owner_entries =
      local_storage_reflection_summary.ivar_layout_owner_entries;
}
