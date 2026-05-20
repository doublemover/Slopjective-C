from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs

SUMMARY_FIELDS = [
    "optional_binding_sites",
    "optional_binding_clause_sites",
    "guard_binding_sites",
    "optional_send_sites",
    "nil_coalescing_sites",
    "optional_propagation_sites",
    "optional_flow_refinement_sites",
    "guard_binding_exit_enforcement_sites",
    "typed_keypath_literal_sites",
    "typed_keypath_self_root_sites",
    "typed_keypath_class_root_sites",
    "object_pointer_semantic_sites",
    "protocol_composition_semantic_sites",
    "generic_suffix_semantic_sites",
    "generic_erasure_semantic_sites",
    "nullability_suffix_semantic_sites",
    "nullability_semantic_sites",
    "canonical_type_entries",
    "canonical_object_type_entries",
    "canonical_protocol_qualified_entries",
    "canonical_generic_argument_entries",
    "canonical_nullable_entries",
    "canonical_nonnull_entries",
    "canonical_implicitly_unwrapped_entries",
    "canonical_null_resettable_entries",
    "canonical_unspecified_nullability_entries",
    "canonical_invalid_type_entries",
    "invalid_generic_suffix_semantic_sites",
    "invalid_nullability_suffix_semantic_sites",
    "invalid_protocol_composition_semantic_sites",
    "optional_binding_contract_violation_sites",
    "optional_send_contract_violation_sites",
    "optional_flow_contract_violation_sites",
    "typed_keypath_contract_violation_sites",
    "ready_for_lowering_and_runtime",
    "deterministic",
    "replay_key",
]


def render_markdown(summary: dict[str, Any]) -> str:
    lines = [
        "# Type Semantic Model Closure",
        "",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Issue: `{summary['issue']}`",
        f"- Positive fixture: `{summary['positive_fixture']}`",
        f"- Nested generic positive fixture: `{summary['nested_generic_positive_fixture']}`",
        f"- Generic variance positive fixture: `{summary['generic_variance_positive_fixture']}`",
        f"- Protocol generic positive fixture: `{summary['protocol_generic_positive_fixture']}`",
        f"- Cross-module generic provider fixture: `{summary['cross_module_generic_provider_fixture']}`",
        f"- Cross-module generic consumer fixture: `{summary['cross_module_generic_consumer_fixture']}`",
        f"- Cross-module protocol provider fixture: `{summary['cross_module_protocol_provider_fixture']}`",
        f"- Cross-module protocol consumer fixture: `{summary['cross_module_protocol_consumer_fixture']}`",
        f"- Negative fixture: `{summary['negative_fixture']}`",
        f"- Nullability negative fixture: `{summary['nullability_negative_fixture']}`",
        f"- Protocol method nullability negative fixture: `{summary['protocol_method_nullability_negative_fixture']}`",
        f"- Protocol property nullability negative fixture: `{summary['protocol_property_nullability_negative_fixture']}`",
        f"- Protocol optional/required conflict negative fixture: `{summary['protocol_optional_required_conflict_negative_fixture']}`",
        f"- Unknown protocol composition negative fixture: `{summary['unknown_protocol_composition_negative_fixture']}`",
        f"- Protocol-qualified unknown message negative fixture: `{summary['protocol_qualified_unknown_message_negative_fixture']}`",
        f"- Typed object receiver unknown message negative fixture: `{summary['typed_object_receiver_unknown_message_negative_fixture']}`",
        f"- Generic constraint violation negative fixture: `{summary['generic_constraint_violation_negative_fixture']}`",
        f"- Generic substitution unknown message negative fixture: `{summary['generic_substitution_unknown_message_negative_fixture']}`",
        f"- Nested generic constraint violation negative fixture: `{summary['nested_generic_constraint_violation_negative_fixture']}`",
        f"- Generic invariant assignment negative fixture: `{summary['generic_invariant_assignment_negative_fixture']}`",
        f"- Protocol generic unknown protocol negative fixture: `{summary['protocol_generic_unknown_protocol_negative_fixture']}`",
        "",
        "## Checks",
    ]
    for name, passed in summary["checks"].items():
        lines.append(f"- `{name}`: `{'PASS' if passed else 'FAIL'}`")
    lines.extend(["", "## Observed Positive Counts"])
    model = summary.get("type_semantic_model") or {}
    for field in summary["positive_minimum_counts"]:
        lines.append(f"- `{field}`: `{model.get(field)}`")
    lines.extend(["", "## Validation Commands"])
    for command in summary["validation_commands"]:
        lines.append(f"- `{command}`")
    lines.append("")
    return "\n".join(lines)


def expected_report_outputs(summary: dict[str, Any]) -> tuple[str, str]:
    return expected_json_report(summary), render_markdown(summary)


def write_outputs(summary: dict[str, Any], *, json_path: Path, markdown_path: Path) -> None:
    write_report_outputs(
        summary=summary,
        json_path=json_path,
        markdown_path=markdown_path,
        markdown=render_markdown(summary),
    )
