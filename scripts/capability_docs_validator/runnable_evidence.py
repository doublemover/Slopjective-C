from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import resolve_repo_path

from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.manifest import (
    PUBLIC_OBJC3C_COMMAND_PREFIX,
    _manifest_support_claims,
)
from capability_docs_validator.support_links import _row_support_claims


def _require_path(label: str, raw_path: Any) -> str:
    if not isinstance(raw_path, str) or not raw_path:
        raise CapabilityDocsError(f"{label} must be a non-empty path")
    if raw_path.startswith("tmp/") or raw_path.startswith("tmp\\"):
        raise CapabilityDocsError(f"{label} cannot use tmp as source truth: {raw_path}")
    if not resolve_repo_path(raw_path).is_file():
        raise CapabilityDocsError(f"{label} is missing: {raw_path}")
    return raw_path


def _require_path_list(label: str, raw_paths: Any) -> list[str]:
    if not isinstance(raw_paths, list) or not raw_paths:
        raise CapabilityDocsError(f"{label} must be a non-empty list")
    paths = [_require_path(f"{label}[{index}]", raw_path) for index, raw_path in enumerate(raw_paths)]
    if len(paths) != len(set(paths)):
        raise CapabilityDocsError(f"{label} must not contain duplicate paths")
    return paths


def _support_claim_to_capability(rows: list[dict[str, Any]]) -> dict[str, str]:
    claims: dict[str, str] = {}
    for row in rows:
        capability_id = str(row["id"])
        if str(row["state"]) != "implemented":
            continue
        for claim in _row_support_claims(row):
            claims[claim] = capability_id
    return claims


def _validate_support_claim_runnable_evidence_catalog(
    rows: list[dict[str, Any]],
    manifest: dict[str, Any],
    catalog: dict[str, Any],
) -> None:
    if catalog.get("contract_id") != "objc3c.conformance.support_claim_runnable_evidence_catalog.v1":
        raise CapabilityDocsError("support claim runnable evidence catalog contract_id drifted")
    if catalog.get("schema_version") != 1:
        raise CapabilityDocsError("support claim runnable evidence catalog schema_version drifted")

    policy = catalog.get("policy")
    if not isinstance(policy, dict):
        raise CapabilityDocsError("support claim runnable evidence catalog policy is missing")
    if policy.get("tmp_source_truth_allowed") is not False:
        raise CapabilityDocsError("support claim runnable evidence catalog must forbid tmp source truth")
    generated_report = policy.get("generated_report_boundary")
    if not isinstance(generated_report, str) or not generated_report.startswith("tmp/"):
        raise CapabilityDocsError("support claim runnable evidence generated report must stay under tmp/")

    manifest_claims = _manifest_support_claims(manifest)
    claim_capabilities = _support_claim_to_capability(rows)
    raw_rows = catalog.get("rows")
    if not isinstance(raw_rows, list) or not raw_rows:
        raise CapabilityDocsError("support claim runnable evidence catalog rows must be non-empty")

    seen_claims: set[str] = set()
    for index, raw_row in enumerate(raw_rows):
        if not isinstance(raw_row, dict):
            raise CapabilityDocsError(f"support claim runnable evidence rows[{index}] must be an object")
        claim = raw_row.get("support_claim")
        if not isinstance(claim, str) or not claim:
            raise CapabilityDocsError(f"support claim runnable evidence rows[{index}].support_claim must be non-empty")
        if claim in seen_claims:
            raise CapabilityDocsError(f"duplicate support claim runnable evidence row: {claim}")
        seen_claims.add(claim)

        if claim not in manifest_claims:
            raise CapabilityDocsError(f"{claim} is missing from canonical manifest support_claims")
        capability_id = raw_row.get("capability_id")
        if capability_id != claim_capabilities.get(claim):
            raise CapabilityDocsError(
                f"{claim} runnable evidence capability_id must match implemented matrix row"
            )
        if raw_row.get("owner_phase") != manifest_claims[claim]["owner_phase"]:
            raise CapabilityDocsError(f"{claim} runnable evidence owner_phase drifted")

        command = raw_row.get("runnable_command")
        if (
            not isinstance(command, str)
            or not command.startswith(PUBLIC_OBJC3C_COMMAND_PREFIX)
            or command == PUBLIC_OBJC3C_COMMAND_PREFIX
        ):
            raise CapabilityDocsError(f"{claim} runnable_command must use the public objc3c bridge")

        _require_path(f"{claim} conformance_fixture", raw_row.get("conformance_fixture"))
        _require_path(f"{claim} traceability_fixture", raw_row.get("traceability_fixture"))
        positive_paths = _require_path_list(f"{claim} positive_evidence", raw_row.get("positive_evidence"))
        negative_paths = _require_path_list(f"{claim} negative_evidence", raw_row.get("negative_evidence"))

        manifest_fixture = manifest_claims[claim]["behavior_fixture"]
        if manifest_fixture not in positive_paths:
            raise CapabilityDocsError(
                f"{claim} positive_evidence must include canonical manifest behavior fixture {manifest_fixture}"
            )
        diagnostic_codes = raw_row.get("required_diagnostic_codes")
        if not isinstance(diagnostic_codes, list) or not all(
            isinstance(code, str) and code for code in diagnostic_codes
        ):
            raise CapabilityDocsError(f"{claim} required_diagnostic_codes must be non-empty strings")
        if not any(code.startswith("O3") for code in diagnostic_codes):
            raise CapabilityDocsError(f"{claim} required_diagnostic_codes must include stable O3 diagnostics")
        if not negative_paths:
            raise CapabilityDocsError(f"{claim} must carry negative or strict-error runnable evidence")


__all__ = ("_validate_support_claim_runnable_evidence_catalog",)
