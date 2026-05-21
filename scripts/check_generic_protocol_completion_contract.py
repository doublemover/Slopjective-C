#!/usr/bin/env python3
"""Validate public generic/protocol completion claims against checked-in source truth."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Iterable


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CONTRACT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "type_protocol"
    / "generic_protocol_completion_contract.json"
)
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
CONTRACT_ID = "objc3c.type_protocol.generic_protocol_completion_contract.v1"
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "


class ContractError(Exception):
    """Raised when the generic/protocol completion contract drifts."""


@dataclass(frozen=True)
class ValidationResult:
    claim_count: int
    source_anchor_count: int
    positive_evidence_count: int
    negative_evidence_count: int


def _read_json(path: Path) -> dict[str, Any]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise ContractError(f"{_repo_rel(path)} is not valid JSON: {exc}") from exc
    if not isinstance(payload, dict):
        raise ContractError(f"{_repo_rel(path)} must contain a JSON object")
    return payload


def _repo_rel(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def _require(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def _string_list(value: Any, label: str, failures: list[str], *, allow_empty: bool = False) -> list[str]:
    if not isinstance(value, list):
        failures.append(f"{label} must be a list")
        return []
    result = [item for item in value if isinstance(item, str)]
    if len(result) != len(value):
        failures.append(f"{label} must contain only strings")
    if not allow_empty and not result:
        failures.append(f"{label} must not be empty")
    return result


def _path_exists(path: str, label: str, failures: list[str]) -> None:
    normalized = path.replace("\\", "/")
    _require(not normalized.startswith(("tmp/", "temp/")), f"{label} may not use tmp/temp source truth: {path}", failures)
    _require(
        not normalized.startswith("docs/support/generated/"),
        f"{label} may not use generated support projections as source truth: {path}",
        failures,
    )
    _require((ROOT / normalized).is_file(), f"{label} missing checked-in path: {path}", failures)


def _row_support_claims(row: dict[str, Any]) -> set[str]:
    raw = row.get("support_claims", [])
    if not isinstance(raw, list):
        return set()
    return {str(item) for item in raw if isinstance(item, str)}


def _matrix_evidence_paths(row: dict[str, Any]) -> set[str]:
    raw = row.get("evidence", [])
    paths: set[str] = set()
    if isinstance(raw, list):
        for item in raw:
            if isinstance(item, dict) and isinstance(item.get("path"), str):
                paths.add(str(item["path"]))
    raw_owner = row.get("owner_modules", [])
    if isinstance(raw_owner, list):
        paths.update(str(item) for item in raw_owner if isinstance(item, str))
    return paths


def _claim_maps(
    manifest: dict[str, Any],
    catalog: dict[str, Any],
    matrix: dict[str, Any],
) -> tuple[dict[str, dict[str, Any]], dict[str, dict[str, Any]], dict[str, dict[str, Any]]]:
    manifest_claims = {
        str(row.get("claim_id")): row
        for row in manifest.get("support_claims", [])
        if isinstance(row, dict)
    }
    catalog_rows = {
        str(row.get("support_claim")): row
        for row in catalog.get("rows", [])
        if isinstance(row, dict)
    }
    matrix_rows = {
        str(row.get("id")): row
        for row in matrix.get("capabilities", [])
        if isinstance(row, dict)
    }
    return manifest_claims, catalog_rows, matrix_rows


def _require_subset(
    claim_id: str,
    field: str,
    expected: Iterable[str],
    actual: Iterable[str],
    failures: list[str],
) -> None:
    expected_set = set(expected)
    actual_set = set(actual)
    missing = sorted(expected_set - actual_set)
    if missing:
        failures.append(f"{claim_id} {field} missing: {', '.join(missing)}")


def _validate_source_anchors(claim: dict[str, Any], failures: list[str]) -> int:
    claim_id = str(claim.get("support_claim"))
    anchors = claim.get("source_anchors")
    if not isinstance(anchors, list) or not anchors:
        failures.append(f"{claim_id} must declare source_anchors")
        return 0

    anchor_count = 0
    for index, anchor in enumerate(anchors):
        label = f"{claim_id} source_anchors[{index}]"
        if not isinstance(anchor, dict):
            failures.append(f"{label} must be an object")
            continue
        path = anchor.get("path")
        if not isinstance(path, str) or not path:
            failures.append(f"{label}.path must be a non-empty string")
            continue
        _path_exists(path, f"{label}.path", failures)
        snippets = _string_list(anchor.get("snippets"), f"{label}.snippets", failures)
        source_text = (ROOT / path).read_text(encoding="utf-8", errors="replace") if (ROOT / path).is_file() else ""
        for snippet in snippets:
            if snippet not in source_text:
                failures.append(f"{label} missing source snippet {snippet!r} in {path}")
        anchor_count += 1
    return anchor_count


def _validate_claim(
    claim: dict[str, Any],
    manifest_claims: dict[str, dict[str, Any]],
    catalog_rows: dict[str, dict[str, Any]],
    matrix_rows: dict[str, dict[str, Any]],
    failures: list[str],
) -> tuple[int, int, int]:
    claim_id = str(claim.get("support_claim"))
    capability_id = str(claim.get("capability_id"))
    owner_phase = str(claim.get("owner_phase"))
    behavior_fixture = str(claim.get("behavior_fixture"))
    public_command = str(claim.get("public_command"))

    _require(claim_id.startswith("objc3c.behavior."), f"{claim_id} must be a public behavior claim", failures)
    _require(public_command.startswith(PUBLIC_COMMAND_PREFIX), f"{claim_id} must use public objc3c command", failures)

    manifest_claim = manifest_claims.get(claim_id)
    catalog_row = catalog_rows.get(claim_id)
    matrix_row = matrix_rows.get(capability_id)
    _require(manifest_claim is not None, f"{claim_id} missing from canonical manifest support_claims", failures)
    _require(catalog_row is not None, f"{claim_id} missing from runnable evidence catalog", failures)
    _require(matrix_row is not None, f"{capability_id} missing from capability matrix", failures)

    if manifest_claim is not None:
        _require(manifest_claim.get("owner_phase") == owner_phase, f"{claim_id} manifest owner_phase drifted", failures)
        _require(
            manifest_claim.get("behavior_fixture") == behavior_fixture,
            f"{claim_id} manifest behavior_fixture drifted",
            failures,
        )
        _require(
            manifest_claim.get("executable_command") == public_command,
            f"{claim_id} manifest executable_command drifted",
            failures,
        )

    expected_positive = _string_list(claim.get("required_positive_evidence"), f"{claim_id}.required_positive_evidence", failures)
    expected_negative = _string_list(claim.get("required_negative_evidence"), f"{claim_id}.required_negative_evidence", failures)
    expected_codes = _string_list(claim.get("required_diagnostic_codes"), f"{claim_id}.required_diagnostic_codes", failures)
    expected_phrases = _string_list(claim.get("required_source_truth_phrases"), f"{claim_id}.required_source_truth_phrases", failures)
    unsupported_boundaries = _string_list(claim.get("unsupported_boundaries"), f"{claim_id}.unsupported_boundaries", failures)

    for path in [behavior_fixture, *expected_positive, *expected_negative]:
        _path_exists(path, f"{claim_id} evidence", failures)

    if catalog_row is not None:
        _require(catalog_row.get("capability_id") == capability_id, f"{claim_id} catalog capability_id drifted", failures)
        _require(catalog_row.get("owner_phase") == owner_phase, f"{claim_id} catalog owner_phase drifted", failures)
        _require(catalog_row.get("runnable_command") == public_command, f"{claim_id} catalog runnable_command drifted", failures)
        _require_subset(claim_id, "positive_evidence", expected_positive, catalog_row.get("positive_evidence", []), failures)
        _require_subset(claim_id, "negative_evidence", expected_negative, catalog_row.get("negative_evidence", []), failures)
        _require_subset(claim_id, "required_diagnostic_codes", expected_codes, catalog_row.get("required_diagnostic_codes", []), failures)
        source_truth = catalog_row.get("source_truth_requirements", [])
        if not isinstance(source_truth, list) or not source_truth:
            failures.append(f"{claim_id} catalog source_truth_requirements must be non-empty")
            source_truth_text = ""
        else:
            source_truth_text = " ".join(str(item) for item in source_truth)
        lower_source_truth = source_truth_text.lower()
        _require(
            "fail-closed" in lower_source_truth or "fail closed" in lower_source_truth,
            f"{claim_id} source_truth_requirements must name fail-closed behavior",
            failures,
        )
        for phrase in expected_phrases:
            _require(
                phrase.lower() in lower_source_truth,
                f"{claim_id} source_truth_requirements missing phrase: {phrase}",
                failures,
            )
        for boundary in unsupported_boundaries:
            _require(
                boundary.lower() in lower_source_truth,
                f"{claim_id} source_truth_requirements missing unsupported boundary: {boundary}",
                failures,
            )

    if matrix_row is not None:
        _require(matrix_row.get("state") == "implemented", f"{capability_id} matrix row must remain implemented", failures)
        _require(claim_id in _row_support_claims(matrix_row), f"{capability_id} matrix row must publish {claim_id}", failures)
        _require(
            bool(_matrix_evidence_paths(matrix_row)),
            f"{capability_id} matrix row must carry owner or evidence paths",
            failures,
        )

    anchor_count = _validate_source_anchors(claim, failures)
    return anchor_count, len(set(expected_positive)), len(set(expected_negative))


def validate_contract(contract_path: Path = DEFAULT_CONTRACT) -> ValidationResult:
    contract = _read_json(contract_path)
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)
    matrix = _read_json(MATRIX_PATH)

    failures: list[str] = []
    _require(contract.get("contract_id") == CONTRACT_ID, "unexpected contract_id", failures)
    _require(contract.get("schema_version") == 1, "unexpected schema_version", failures)
    _require(set(contract.get("issue_refs", [])) == {8160, 8164}, "contract must bind issues 8160 and 8164", failures)
    _require(8160 in catalog.get("issue_refs", []), "catalog must include issue #8160", failures)
    _require(8164 in catalog.get("issue_refs", []), "catalog must include issue #8164", failures)

    policy = contract.get("policy")
    if not isinstance(policy, dict):
        failures.append("policy must be an object")
    else:
        _require(policy.get("tmp_source_truth_allowed") is False, "tmp source truth must be disabled", failures)
        _require(
            policy.get("generated_projection_source_truth_allowed") is False,
            "generated support projection source truth must be disabled",
            failures,
        )
        _require(policy.get("public_command_prefix") == PUBLIC_COMMAND_PREFIX, "public command prefix drifted", failures)

    claims = contract.get("claims")
    if not isinstance(claims, list) or not claims:
        failures.append("claims must be a non-empty list")
        claims = []

    manifest_claims, catalog_rows, matrix_rows = _claim_maps(manifest, catalog, matrix)
    seen_claims: set[str] = set()
    source_anchor_count = 0
    positive_evidence_count = 0
    negative_evidence_count = 0
    for claim in claims:
        if not isinstance(claim, dict):
            failures.append("each claim must be an object")
            continue
        claim_id = str(claim.get("support_claim"))
        if claim_id in seen_claims:
            failures.append(f"duplicate claim in contract: {claim_id}")
        seen_claims.add(claim_id)
        anchors, positives, negatives = _validate_claim(
            claim,
            manifest_claims,
            catalog_rows,
            matrix_rows,
            failures,
        )
        source_anchor_count += anchors
        positive_evidence_count += positives
        negative_evidence_count += negatives

    if failures:
        raise ContractError("\n".join(failures))
    return ValidationResult(
        claim_count=len(claims),
        source_anchor_count=source_anchor_count,
        positive_evidence_count=positive_evidence_count,
        negative_evidence_count=negative_evidence_count,
    )


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--contract", type=Path, default=DEFAULT_CONTRACT)
    args = parser.parse_args(argv)

    try:
        result = validate_contract(args.contract)
    except ContractError as exc:
        print("generic-protocol-completion-contract: FAIL", file=sys.stderr)
        for line in str(exc).splitlines():
            print(f"- {line}", file=sys.stderr)
        return 1

    print("generic-protocol-completion-contract: PASS")
    print(f"claims: {result.claim_count}")
    print(f"source_anchors: {result.source_anchor_count}")
    print(f"positive_evidence_paths: {result.positive_evidence_count}")
    print(f"negative_evidence_paths: {result.negative_evidence_count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
