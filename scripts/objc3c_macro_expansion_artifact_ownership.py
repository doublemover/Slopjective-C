"""Validator for the public macro expansion artifact ownership surface."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import (
    JsonSchemaValidationError,
    load_json_object,
    validate_json_schema,
)
from objc3c_tooling.paths import ROOT, repo_rel


CONTRACT_ID = "objc3c.metaprogramming.macro.expansion.artifact.ownership.v1"
SUMMARY_CONTRACT_ID = (
    "objc3c.metaprogramming.macro.expansion.artifact.ownership.summary.v1"
)
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "metaprogramming_public_surface"
    / "macro_expansion_artifact_ownership_contract.json"
)
REPORT_PATH = ROOT / "tmp" / "reports" / "metaprogramming-artifact-ownership.json"


@dataclass(frozen=True)
class MacroExpansionArtifactOwnershipResult:
    payload: dict[str, Any]
    failures: list[str]

    @property
    def passed(self) -> bool:
        return not self.failures


def _as_list(value: object) -> list[object]:
    return value if isinstance(value, list) else []


def _as_dict(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _source_text(path_text: str, failures: list[str]) -> str:
    path = ROOT / path_text
    if not path.is_file():
        failures.append(f"source anchor missing: {path_text}")
        return ""
    return path.read_text(encoding="utf-8")


def _path_forbidden(path_text: str, prefixes: list[str]) -> bool:
    normalized = path_text.replace("\\", "/")
    return any(normalized.startswith(prefix) for prefix in prefixes)


def _display_contract_path(contract_path: Path) -> str:
    try:
        return repo_rel(contract_path)
    except ValueError:
        return str(contract_path)


def _validate_schema(
    contract: dict[str, Any],
    contract_path: Path,
    failures: list[str],
) -> None:
    schema_path = str(contract.get("schema_path", ""))
    try:
        schema = load_json_object(ROOT / schema_path) if schema_path else {}
        validate_json_schema(contract, schema, label=_display_contract_path(contract_path))
    except (JsonSchemaValidationError, RuntimeError) as exc:
        failures.append(f"{_display_contract_path(contract_path)}: {exc}")


def _validate_source_anchors(
    contract: dict[str, Any],
    forbidden_prefixes: list[str],
    failures: list[str],
) -> int:
    anchor_count = 0
    for anchor in _as_list(contract.get("source_anchors")):
        if not isinstance(anchor, dict):
            failures.append("source anchor is not an object")
            continue
        path_text = str(anchor.get("path", ""))
        anchor_count += 1
        if _path_forbidden(path_text, forbidden_prefixes):
            failures.append(f"source anchor uses forbidden path: {path_text}")
            continue
        text = _source_text(path_text, failures)
        for token in _as_list(anchor.get("tokens")):
            if not isinstance(token, str) or token not in text:
                failures.append(f"source anchor token missing from {path_text}: {token}")
    return anchor_count


def _validate_document_fields(
    contract: dict[str, Any],
    forbidden_prefixes: list[str],
    failures: list[str],
) -> dict[str, int]:
    host_cache_document = _as_dict(contract.get("host_cache_document"))
    document_builder = str(host_cache_document.get("document_builder", ""))
    publication_owner = str(host_cache_document.get("publication_owner_path", ""))
    artifact_builder = str(host_cache_document.get("artifact_builder", ""))
    input_contract_source = str(host_cache_document.get("input_contract_source", ""))

    for path_text in (
        document_builder,
        publication_owner,
        artifact_builder,
        input_contract_source,
    ):
        if _path_forbidden(path_text, forbidden_prefixes):
            failures.append(f"host cache ownership path uses forbidden path: {path_text}")

    document_text = _source_text(document_builder, failures)
    publication_text = _source_text(publication_owner, failures)
    artifact_text = _source_text(artifact_builder, failures)
    input_contract_text = _source_text(input_contract_source, failures)
    live_cache_assertion_text = _source_text(
        "scripts/objc3c_runtime_acceptance/domains/metaprogramming_live_cache_payload_assertions.py",
        failures,
    )

    required_fields = [str(field) for field in _as_list(host_cache_document.get("required_fields"))]
    checked_true_fields = [
        str(field) for field in _as_list(host_cache_document.get("checked_true_fields"))
    ]
    stable_fields = [
        str(field) for field in _as_list(host_cache_document.get("cache_hit_stable_fields"))
    ]

    for field in required_fields:
        if f'"{field}"' not in document_text:
            failures.append(f"host-cache artifact document does not emit field: {field}")
    for field in checked_true_fields:
        if f'BoolField("{field}", true)' not in document_text:
            failures.append(f"host-cache artifact document does not force true field: {field}")
    if 'BoolField("support_claim_authority", false)' not in document_text:
        failures.append("host-cache artifact document does not deny support claim authority")

    for field in stable_fields:
        if field not in live_cache_assertion_text:
            failures.append(f"host-cache replay assertions do not preserve stable field: {field}")

    if "TryBuildObjc3MetaprogrammingMacroHostProcessCacheArtifact" not in publication_text:
        failures.append("publication owner does not call the host-cache artifact builder")
    if "kHardCutoverContractFailure" not in publication_text:
        failures.append("publication owner does not fail closed on host-cache builder rejection")
    if "complete_cache_entry" not in artifact_text or "any_cache_artifact_present" not in artifact_text:
        failures.append("host-cache artifact builder does not guard partial cache entries")
    if "ValidateMetaprogrammingMacroHostProcessCacheArtifactInputs" not in input_contract_text:
        failures.append("host-cache input contract validator is not anchored")

    return {
        "required_field_count": len(required_fields),
        "checked_true_field_count": len(checked_true_fields),
        "cache_hit_stable_field_count": len(stable_fields),
    }


def _validate_fail_closed_cases(
    contract: dict[str, Any],
    forbidden_prefixes: list[str],
    failures: list[str],
) -> int:
    case_count = 0
    for case in _as_list(contract.get("fail_closed_cases")):
        if not isinstance(case, dict):
            failures.append("fail-closed case is not an object")
            continue
        case_count += 1
        source_path = str(case.get("source_path", ""))
        if _path_forbidden(source_path, forbidden_prefixes):
            failures.append(f"fail-closed case uses forbidden path: {source_path}")
            continue
        text = _source_text(source_path, failures)
        for token in _as_list(case.get("tokens")):
            if not isinstance(token, str) or token not in text:
                failures.append(
                    f"fail-closed token missing for {case.get('case_id')}: {token}"
                )
    return case_count


def validate_macro_expansion_artifact_ownership(
    contract_path: Path = CONTRACT_PATH,
) -> MacroExpansionArtifactOwnershipResult:
    contract = load_json_object(contract_path)
    failures: list[str] = []
    _validate_schema(contract, contract_path, failures)

    forbidden_prefixes = [
        str(prefix) for prefix in _as_list(contract.get("forbidden_path_prefixes"))
    ]

    if contract.get("contract_id") != CONTRACT_ID:
        failures.append("macro expansion artifact ownership contract_id drifted")
    if contract.get("issue_ref") != 8168:
        failures.append("macro expansion artifact ownership must map to issue #8168")
    if (
        contract.get("public_command")
        != "npm run objc3c -- validate-metaprogramming-conformance"
    ):
        failures.append("macro expansion artifact ownership public command drifted")

    boundary = _as_dict(contract.get("generated_artifact_boundary"))
    if boundary.get("support_claim_authority") is not False:
        failures.append("generated host-cache artifact must not be support claim authority")
    if boundary.get("generated_artifact_role") != "metaprogramming-host-cache-provenance-only":
        failures.append("generated host-cache artifact role drifted")
    if "docs/support" not in str(boundary.get("runtime_output_root_policy", "")):
        failures.append("generated host-cache artifact boundary must exclude docs/support")

    source_anchor_count = _validate_source_anchors(contract, forbidden_prefixes, failures)
    field_counts = _validate_document_fields(contract, forbidden_prefixes, failures)
    fail_closed_case_count = _validate_fail_closed_cases(
        contract, forbidden_prefixes, failures
    )

    payload: dict[str, Any] = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "source_contract_id": contract.get("contract_id"),
        "status": "PASS" if not failures else "FAIL",
        "issue_ref": contract.get("issue_ref"),
        "public_command": contract.get("public_command"),
        "schema_path": contract.get("schema_path"),
        "contract_path": _display_contract_path(contract_path),
        "generated_artifact_boundary": boundary,
        "source_anchor_count": source_anchor_count,
        "fail_closed_case_count": fail_closed_case_count,
        **field_counts,
        "non_claims": contract.get("non_claims", []),
        "failures": failures,
    }
    return MacroExpansionArtifactOwnershipResult(payload=payload, failures=failures)


__all__ = [
    "CONTRACT_ID",
    "CONTRACT_PATH",
    "REPORT_PATH",
    "SUMMARY_CONTRACT_ID",
    "MacroExpansionArtifactOwnershipResult",
    "validate_macro_expansion_artifact_ownership",
]
