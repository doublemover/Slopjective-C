from __future__ import annotations

from pathlib import Path

from hard_cutover_gate_contract_behavior import (
    assert_cli_summary_declares_owner_contract,
    assert_default_roots_cover_public_command_truth_surfaces,
    assert_pattern_groups_are_reported_as_owner_contracts,
    assert_policy_data_covers_closure_residue_classes,
    assert_report_declares_allowlist_free_contract,
    assert_report_declares_source_owned_contracts,
    assert_report_matches_schema_shape,
)
from hard_cutover_gate_pattern_ownership import (
    assert_implementation_residue_patterns_are_split_by_owner_modules,
    assert_public_claim_patterns_are_split_by_owner_modules,
)
from hard_cutover_gate_public_claim_behavior import (
    assert_backward_compatible_alias_claims_rejected,
    assert_legacy_compatibility_public_text_rejected,
    assert_projected_retired_behavior_claims_rejected,
    assert_public_compatibility_shim_support_claims_rejected,
    assert_public_migration_lane_support_claims_rejected,
)
from hard_cutover_gate_scanner_behavior import (
    assert_active_forbidden_pattern_fails,
    assert_canonical_config_registry_excluded,
    assert_compatibility_dispatch_symbol_fields_rejected,
    assert_compatibility_dispatch_wording_in_required_roots_rejected,
    assert_deterministic_arithmetic_wording_rejected,
    assert_dotted_runtime_shim_tokens_rejected,
    assert_fallback_behavior_wording_rejected,
    assert_fallback_only_wording_rejected,
    assert_implementation_compatibility_bridge_rejected,
    assert_implementation_fallback_handler_rejected,
    assert_implementation_legacy_support_rejected,
    assert_implementation_migration_lane_rejected,
    assert_legacy_language_profile_enum_values_rejected,
    assert_legacy_literal_diagnostics_switch_rejected,
    assert_negative_absence_assertions_allowed,
    assert_plain_compatibility_mode_wording_rejected,
    assert_retired_msgsend_symbol_rejected,
    assert_retired_tmp_allowlist_ignored,
    assert_runtime_docs_spec_and_test_surfaces_covered,
    assert_source_hygiene_violation_fixtures_excluded,
    assert_unresolved_dispatch_pseudo_success_rejected,
)
from hard_cutover_gate_workflow_behavior import (
    assert_direct_native_compile_wrapper_commands_rejected,
    assert_direct_runner_command_variables_rejected,
    assert_native_compile_wrapper_as_source_anchor_allowed,
    assert_packaged_runner_direct_commands_rejected,
    assert_retired_npm_workflow_aliases_rejected,
    assert_retired_package_alias_metadata_rejected,
    assert_retired_public_workflow_runner_path_rejected,
    assert_retired_workflow_registry_facade_rejected,
)


def test_public_claim_patterns_are_split_by_owner_modules() -> None:
    assert_public_claim_patterns_are_split_by_owner_modules()


def test_implementation_residue_patterns_are_split_by_owner_modules() -> None:
    assert_implementation_residue_patterns_are_split_by_owner_modules()


def test_hard_cutover_gate_fails_on_active_forbidden_pattern(tmp_path: Path) -> None:
    assert_active_forbidden_pattern_fails(tmp_path)


def test_hard_cutover_gate_rejects_dotted_runtime_shim_tokens(tmp_path: Path) -> None:
    assert_dotted_runtime_shim_tokens_rejected(tmp_path)


def test_hard_cutover_gate_rejects_retired_msgsend_symbol(tmp_path: Path) -> None:
    assert_retired_msgsend_symbol_rejected(tmp_path)


def test_hard_cutover_gate_rejects_unresolved_dispatch_pseudo_success(
    tmp_path: Path,
) -> None:
    assert_unresolved_dispatch_pseudo_success_rejected(tmp_path)


def test_hard_cutover_gate_rejects_compatibility_dispatch_symbol_fields(
    tmp_path: Path,
) -> None:
    assert_compatibility_dispatch_symbol_fields_rejected(tmp_path)


def test_hard_cutover_gate_rejects_compatibility_dispatch_wording_in_required_roots(
    tmp_path: Path,
) -> None:
    assert_compatibility_dispatch_wording_in_required_roots_rejected(tmp_path)


def test_hard_cutover_gate_covers_runtime_docs_spec_and_test_surfaces(
    tmp_path: Path,
) -> None:
    assert_runtime_docs_spec_and_test_surfaces_covered(tmp_path)


def test_hard_cutover_gate_allows_negative_absence_assertions(tmp_path: Path) -> None:
    assert_negative_absence_assertions_allowed(tmp_path)


def test_hard_cutover_gate_excludes_source_hygiene_violation_fixtures(
    tmp_path: Path,
) -> None:
    assert_source_hygiene_violation_fixtures_excluded(tmp_path)


def test_hard_cutover_gate_ignores_retired_tmp_allowlist(tmp_path: Path) -> None:
    assert_retired_tmp_allowlist_ignored(tmp_path)


def test_hard_cutover_gate_rejects_legacy_language_profile_enum_values(
    tmp_path: Path,
) -> None:
    assert_legacy_language_profile_enum_values_rejected(tmp_path)


def test_hard_cutover_gate_rejects_plain_compatibility_mode_wording(
    tmp_path: Path,
) -> None:
    assert_plain_compatibility_mode_wording_rejected(tmp_path)


def test_hard_cutover_gate_rejects_implementation_compatibility_bridge(
    tmp_path: Path,
) -> None:
    assert_implementation_compatibility_bridge_rejected(tmp_path)


def test_hard_cutover_gate_rejects_implementation_fallback_handler(
    tmp_path: Path,
) -> None:
    assert_implementation_fallback_handler_rejected(tmp_path)


def test_hard_cutover_gate_rejects_implementation_migration_lane(
    tmp_path: Path,
) -> None:
    assert_implementation_migration_lane_rejected(tmp_path)


def test_hard_cutover_gate_rejects_implementation_legacy_support(
    tmp_path: Path,
) -> None:
    assert_implementation_legacy_support_rejected(tmp_path)


def test_hard_cutover_gate_rejects_retired_public_workflow_runner_path(
    tmp_path: Path,
) -> None:
    assert_retired_public_workflow_runner_path_rejected(tmp_path)


def test_hard_cutover_gate_rejects_retired_npm_workflow_aliases(
    tmp_path: Path,
) -> None:
    assert_retired_npm_workflow_aliases_rejected(tmp_path)


def test_hard_cutover_gate_rejects_direct_native_compile_wrapper_commands(
    tmp_path: Path,
) -> None:
    assert_direct_native_compile_wrapper_commands_rejected(tmp_path)


def test_hard_cutover_gate_allows_native_compile_wrapper_as_source_anchor(
    tmp_path: Path,
) -> None:
    assert_native_compile_wrapper_as_source_anchor_allowed(tmp_path)


def test_hard_cutover_gate_rejects_retired_package_alias_metadata(
    tmp_path: Path,
) -> None:
    assert_retired_package_alias_metadata_rejected(tmp_path)


def test_hard_cutover_gate_rejects_direct_runner_command_variables(
    tmp_path: Path,
) -> None:
    assert_direct_runner_command_variables_rejected(tmp_path)


def test_hard_cutover_gate_rejects_packaged_runner_direct_commands(
    tmp_path: Path,
) -> None:
    assert_packaged_runner_direct_commands_rejected(tmp_path)


def test_hard_cutover_gate_rejects_retired_workflow_registry_facade(
    tmp_path: Path,
) -> None:
    assert_retired_workflow_registry_facade_rejected(tmp_path)


def test_hard_cutover_default_roots_cover_public_command_truth_surfaces() -> None:
    assert_default_roots_cover_public_command_truth_surfaces()


def test_hard_cutover_policy_data_covers_closure_residue_classes(
    tmp_path: Path,
) -> None:
    assert_policy_data_covers_closure_residue_classes(tmp_path)


def test_hard_cutover_pattern_groups_are_reported_as_owner_contracts(
    tmp_path: Path,
) -> None:
    assert_pattern_groups_are_reported_as_owner_contracts(tmp_path)


def test_hard_cutover_report_declares_allowlist_free_contract(
    tmp_path: Path,
) -> None:
    assert_report_declares_allowlist_free_contract(tmp_path)


def test_hard_cutover_report_declares_source_owned_contracts(
    tmp_path: Path,
) -> None:
    assert_report_declares_source_owned_contracts(tmp_path)


def test_hard_cutover_gate_rejects_legacy_literal_diagnostics_switch(
    tmp_path: Path,
) -> None:
    assert_legacy_literal_diagnostics_switch_rejected(tmp_path)


def test_hard_cutover_gate_rejects_fallback_behavior_wording(
    tmp_path: Path,
) -> None:
    assert_fallback_behavior_wording_rejected(tmp_path)


def test_hard_cutover_gate_rejects_fallback_only_wording(tmp_path: Path) -> None:
    assert_fallback_only_wording_rejected(tmp_path)


def test_hard_cutover_gate_rejects_deterministic_arithmetic_wording(
    tmp_path: Path,
) -> None:
    assert_deterministic_arithmetic_wording_rejected(tmp_path)


def test_hard_cutover_gate_rejects_projected_retired_behavior_claims(
    tmp_path: Path,
) -> None:
    assert_projected_retired_behavior_claims_rejected(tmp_path)


def test_hard_cutover_gate_rejects_backward_compatible_alias_claims(
    tmp_path: Path,
) -> None:
    assert_backward_compatible_alias_claims_rejected(tmp_path)


def test_hard_cutover_gate_rejects_public_compatibility_shim_support_claims(
    tmp_path: Path,
) -> None:
    assert_public_compatibility_shim_support_claims_rejected(tmp_path)


def test_hard_cutover_gate_rejects_public_migration_lane_support_claims(
    tmp_path: Path,
) -> None:
    assert_public_migration_lane_support_claims_rejected(tmp_path)


def test_hard_cutover_gate_rejects_legacy_compatibility_public_text(
    tmp_path: Path,
) -> None:
    assert_legacy_compatibility_public_text_rejected(tmp_path)


def test_hard_cutover_report_matches_schema_shape(tmp_path: Path) -> None:
    assert_report_matches_schema_shape(tmp_path)


def test_hard_cutover_gate_cli_summary_declares_owner_contract(
    tmp_path: Path,
) -> None:
    assert_cli_summary_declares_owner_contract(tmp_path)


def test_hard_cutover_gate_excludes_canonical_config_registry(tmp_path: Path) -> None:
    assert_canonical_config_registry_excluded(tmp_path)
