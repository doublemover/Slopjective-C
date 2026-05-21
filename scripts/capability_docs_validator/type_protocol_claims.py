from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.support_links import _row_support_claims


@dataclass(frozen=True)
class TypeProtocolClaimRule:
    issue_ref: int
    capability_id: str
    support_claim: str
    owner_phase: str
    runnable_command: str
    required_positive_evidence: frozenset[str]
    required_negative_evidence: frozenset[str]
    required_diagnostic_codes: frozenset[str]
    required_source_or_owner_paths: frozenset[str]


PUBLIC_CONFORMANCE_COMMAND = "npm run objc3c -- validate-conformance-corpus"

TYPE_PROTOCOL_CLAIM_RULES = (
    TypeProtocolClaimRule(
        issue_ref=8160,
        capability_id="language.generics.variance-specialization",
        support_claim="objc3c.behavior.language.generics.variance-specialization",
        owner_phase="sema",
        runnable_command=PUBLIC_CONFORMANCE_COMMAND,
        required_positive_evidence=frozenset(
            {
                "tests/tooling/fixtures/native/type_semantic_generic_variance_positive.objc3",
                "tests/tooling/fixtures/native/type_semantic_nested_generic_positive.objc3",
                "tests/conformance/semantic/TYP-8013-12.json",
                "tests/conformance/semantic/TYP-8013-13.json",
            }
        ),
        required_negative_evidence=frozenset(
            {
                "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_invariant_assignment.objc3",
                "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_nested_generic_constraint_violation.objc3",
                "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_substitution_unknown_message.objc3",
            }
        ),
        required_diagnostic_codes=frozenset({"O3S206", "O3S216"}),
        required_source_or_owner_paths=frozenset(
            {
                "native/objc3c/src/ir/objc3_ir_type_model.cpp",
                "native/objc3c/src/sema/objc3_semantic_passes_generic_protocol_message_validation.inc",
            }
        ),
    ),
    TypeProtocolClaimRule(
        issue_ref=8160,
        capability_id="runtime.generics.cross-module-metadata",
        support_claim="objc3c.behavior.runtime.generics.cross-module-metadata",
        owner_phase="runtime",
        runnable_command=PUBLIC_CONFORMANCE_COMMAND,
        required_positive_evidence=frozenset(
            {
                "tests/conformance/semantic/TYP-8013-17.json",
                "tests/tooling/fixtures/objc3c/validation_generic_metadata_abi_contract/replay_run_1/module.manifest.json",
                "tests/tooling/fixtures/objc3c/validation_lightweight_generics_constraints_contract/replay_run_1/module.manifest.json",
            }
        ),
        required_negative_evidence=frozenset(
            {
                "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_constraint_violation.objc3",
                "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_invariant_assignment.objc3",
            }
        ),
        required_diagnostic_codes=frozenset({"O3S206"}),
        required_source_or_owner_paths=frozenset(
            {
                "native/objc3c/src/pipeline/runtime_import_type_system_preservation_generic.cpp",
                "native/objc3c/src/pipeline/objc3_runtime_import_surface.h",
            }
        ),
    ),
    TypeProtocolClaimRule(
        issue_ref=8164,
        capability_id="language.protocols.protocol-qualified-existential-value-flow",
        support_claim=(
            "objc3c.behavior.language.protocols."
            "protocol-qualified-existential-value-flow"
        ),
        owner_phase="sema",
        runnable_command=PUBLIC_CONFORMANCE_COMMAND,
        required_positive_evidence=frozenset(
            {
                "tests/tooling/fixtures/native/protocol_qualified_existential_value_flow.objc3",
                "tests/tooling/fixtures/native/protocol_composition_existential_value_flow.objc3",
                "tests/conformance/semantic/TYP-8013-01.json",
                "tests/conformance/semantic/TYP-8013-18.json",
                "tests/conformance/semantic/TYP-8013-25.json",
            }
        ),
        required_negative_evidence=frozenset(
            {
                "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_qualified_unknown_message.objc3",
                "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_associated_type_rejected.objc3",
                "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_dynamic_dispatch_rejected.objc3",
            }
        ),
        required_diagnostic_codes=frozenset(
            {"O3P100", "O3S206", "O3S216", "O3S218", "O3S314"}
        ),
        required_source_or_owner_paths=frozenset(
            {
                "native/objc3c/src/sema/objc3_semantic_passes_expression_statement_validation_message_send_protocol_qualified.inc",
                "native/objc3c/src/sema/objc3_semantic_protocol_composition_parser.cpp",
                "native/objc3c/src/runtime/classes/protocol_conformance.cpp",
                "native/objc3c/src/runtime/public/objc3_runtime_language_semantics.cpp",
                "native/objc3c/src/pipeline/runtime_import_type_system_preservation_protocol.cpp",
            }
        ),
    ),
    TypeProtocolClaimRule(
        issue_ref=8164,
        capability_id="language.protocols.existential-witness-model",
        support_claim="objc3c.behavior.language.protocols.existential-witness-model",
        owner_phase="runtime",
        runnable_command=PUBLIC_CONFORMANCE_COMMAND,
        required_positive_evidence=frozenset(
            {
                "tests/tooling/fixtures/native/execution/positive/id_protocol_qualifier_alias_signature.objc3",
                "tests/tooling/runtime/language_semantics_runtime_api_probe.cpp",
                "tests/tooling/fixtures/objc3c/language_semantics_runtime_api_contract.json",
            }
        ),
        required_negative_evidence=frozenset(
            {
                "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_associated_type_rejected.objc3",
                "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_dynamic_dispatch_rejected.objc3",
            }
        ),
        required_diagnostic_codes=frozenset({"O3P100", "O3S216", "O3S314"}),
        required_source_or_owner_paths=frozenset(
            {
                "native/objc3c/src/runtime/classes/protocol_conformance.h",
                "native/objc3c/src/runtime/classes/protocol_conformance.cpp",
                "native/objc3c/src/runtime/classes/protocol_conformance_snapshots.cpp",
                "native/objc3c/src/runtime/public/objc3_runtime_language_semantics.h",
                "native/objc3c/src/runtime/public/objc3_runtime_language_semantics.cpp",
            }
        ),
    ),
)


def _as_path_set(row: dict[str, Any], field: str) -> set[str]:
    raw_paths = row.get(field)
    if not isinstance(raw_paths, list):
        raise CapabilityDocsError(f"{row.get('support_claim', '<unknown>')} {field} must be a list")
    paths = {path for path in raw_paths if isinstance(path, str)}
    if len(paths) != len(raw_paths):
        raise CapabilityDocsError(f"{row.get('support_claim', '<unknown>')} {field} must contain paths")
    return paths


def _matrix_evidence_paths(row: dict[str, Any]) -> set[str]:
    raw_evidence = row.get("evidence")
    if not isinstance(raw_evidence, list):
        raise CapabilityDocsError(f"{row.get('id', '<unknown>')} evidence must be a list")
    paths: set[str] = set()
    for evidence in raw_evidence:
        if isinstance(evidence, dict) and isinstance(evidence.get("path"), str):
            paths.add(str(evidence["path"]))
    return paths


def _owner_paths(row: dict[str, Any]) -> set[str]:
    raw_paths = row.get("owner_modules", [])
    if not isinstance(raw_paths, list):
        raise CapabilityDocsError(f"{row.get('id', '<unknown>')} owner_modules must be a list")
    paths = {path for path in raw_paths if isinstance(path, str)}
    if len(paths) != len(raw_paths):
        raise CapabilityDocsError(f"{row.get('id', '<unknown>')} owner_modules must contain paths")
    return paths


def _require_subset(label: str, required: frozenset[str], actual: set[str]) -> None:
    missing = sorted(required - actual)
    if missing:
        raise CapabilityDocsError(f"{label} missing required evidence: " + ", ".join(missing))


def _require_boundary_text(rule: TypeProtocolClaimRule, catalog_row: dict[str, Any]) -> None:
    raw_requirements = catalog_row.get("source_truth_requirements")
    if not isinstance(raw_requirements, list) or not raw_requirements:
        raise CapabilityDocsError(
            f"{rule.support_claim} must declare source_truth_requirements"
        )
    if not all(isinstance(requirement, str) and requirement for requirement in raw_requirements):
        raise CapabilityDocsError(
            f"{rule.support_claim} source_truth_requirements must be non-empty strings"
        )
    boundary_text = " ".join(raw_requirements).lower()
    if "fail-closed" not in boundary_text and "fail closed" not in boundary_text:
        raise CapabilityDocsError(
            f"{rule.support_claim} source_truth_requirements must name fail-closed evidence"
        )
    if not any(
        phrase in boundary_text
        for phrase in ("does not claim", "not claim", "without claiming", "limited to")
    ):
        raise CapabilityDocsError(
            f"{rule.support_claim} source_truth_requirements must bound unclaimed behavior"
        )


def _validate_rule(
    rule: TypeProtocolClaimRule,
    matrix_row: dict[str, Any],
    catalog_row: dict[str, Any],
) -> None:
    if matrix_row.get("state") != "implemented":
        raise CapabilityDocsError(f"{rule.capability_id} must remain an implemented capability row")
    if rule.support_claim not in _row_support_claims(matrix_row):
        raise CapabilityDocsError(f"{rule.capability_id} must publish {rule.support_claim}")
    if catalog_row.get("capability_id") != rule.capability_id:
        raise CapabilityDocsError(f"{rule.support_claim} capability_id drifted")
    if catalog_row.get("owner_phase") != rule.owner_phase:
        raise CapabilityDocsError(f"{rule.support_claim} owner_phase drifted")
    if catalog_row.get("runnable_command") != rule.runnable_command:
        raise CapabilityDocsError(f"{rule.support_claim} runnable_command drifted")

    positive_paths = _as_path_set(catalog_row, "positive_evidence")
    negative_paths = _as_path_set(catalog_row, "negative_evidence")
    _require_subset(f"{rule.support_claim} positive_evidence", rule.required_positive_evidence, positive_paths)
    _require_subset(f"{rule.support_claim} negative_evidence", rule.required_negative_evidence, negative_paths)

    raw_codes = catalog_row.get("required_diagnostic_codes")
    if not isinstance(raw_codes, list):
        raise CapabilityDocsError(f"{rule.support_claim} required_diagnostic_codes must be a list")
    diagnostic_codes = {code for code in raw_codes if isinstance(code, str)}
    if len(diagnostic_codes) != len(raw_codes):
        raise CapabilityDocsError(f"{rule.support_claim} required_diagnostic_codes must contain strings")
    missing_codes = sorted(rule.required_diagnostic_codes - diagnostic_codes)
    if missing_codes:
        raise CapabilityDocsError(
            f"{rule.support_claim} missing required diagnostic codes: "
            + ", ".join(missing_codes)
        )

    source_or_owner_paths = positive_paths | _matrix_evidence_paths(matrix_row) | _owner_paths(matrix_row)
    _require_subset(
        f"{rule.support_claim} source/owner evidence",
        rule.required_source_or_owner_paths,
        source_or_owner_paths,
    )
    _require_boundary_text(rule, catalog_row)


def _validate_type_protocol_capability_rows(
    rows: list[dict[str, Any]],
    catalog: dict[str, Any],
) -> None:
    issue_refs = catalog.get("issue_refs")
    if not isinstance(issue_refs, list):
        raise CapabilityDocsError("support claim runnable evidence catalog issue_refs must be a list")

    matrix_rows = {str(row.get("id")): row for row in rows if isinstance(row, dict)}
    catalog_rows = {
        str(row.get("support_claim")): row
        for row in catalog.get("rows", [])
        if isinstance(row, dict)
    }
    for rule in TYPE_PROTOCOL_CLAIM_RULES:
        if rule.issue_ref not in issue_refs:
            raise CapabilityDocsError(f"{rule.support_claim} requires issue #{rule.issue_ref}")
        matrix_row = matrix_rows.get(rule.capability_id)
        if matrix_row is None:
            raise CapabilityDocsError(f"{rule.capability_id} is missing from capability matrix")
        catalog_row = catalog_rows.get(rule.support_claim)
        if catalog_row is None:
            raise CapabilityDocsError(f"{rule.support_claim} is missing from runnable evidence catalog")
        _validate_rule(rule, matrix_row, catalog_row)


__all__ = ("_validate_type_protocol_capability_rows",)
