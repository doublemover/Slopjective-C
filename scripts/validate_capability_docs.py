#!/usr/bin/env python3
"""Validate canonical capability docs and evidence links."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from typing import Any, Sequence

from objc3c_shared.json_io import (
    JsonSchemaValidationError,
    load_json_object,
    validate_json_schema,
)
from objc3c_tooling.paths import ROOT, display_path, resolve_repo_path

MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
SCHEMA_PATH = ROOT / "docs" / "support" / "capability_matrix.schema.json"
MATRIX_DOC = ROOT / "docs" / "support" / "capability_matrix.md"
EVIDENCE_DOC = ROOT / "docs" / "support" / "evidence_map.md"
CANONICAL_MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
BEHAVIOR_MATRIX_COMMAND = "npm run objc3c -- test-behavior-matrix"
SUPPORT_CLAIM_RE = re.compile(r"\bobjc3c\.behavior\.[a-z0-9._-]+\b")


class CapabilityDocsError(RuntimeError):
    pass


def _require_matrix_shape(matrix: dict[str, Any]) -> list[dict[str, Any]]:
    capabilities = matrix.get("capabilities")
    if not isinstance(capabilities, list):
        raise CapabilityDocsError("capabilities must be a list")
    rows: list[dict[str, Any]] = []
    seen: set[str] = set()
    for index, row in enumerate(capabilities):
        if not isinstance(row, dict):
            raise CapabilityDocsError(f"capabilities[{index}] must be an object")
        capability_id = row.get("id")
        if not isinstance(capability_id, str):
            raise CapabilityDocsError(f"capabilities[{index}].id must be a string")
        if capability_id in seen:
            raise CapabilityDocsError(f"duplicate capability id: {capability_id}")
        seen.add(capability_id)
        rows.append(row)
    return rows


def _validate_evidence_rows(rows: list[dict[str, Any]]) -> None:
    for row in rows:
        capability_id = str(row["id"])
        state = str(row["state"])
        evidence = row["evidence"]
        if not isinstance(evidence, list) or not evidence:
            raise CapabilityDocsError(f"{capability_id} must have evidence")
        kinds = {str(item.get("kind")) for item in evidence if isinstance(item, dict)}
        if state == "implemented" and "test" not in kinds:
            raise CapabilityDocsError(f"{capability_id} implemented rows require test evidence")
        if state == "reserved" and kinds.isdisjoint({"diagnostic", "doc"}):
            raise CapabilityDocsError(f"{capability_id} reserved rows require diagnostic or doc evidence")
        for item in evidence:
            if not isinstance(item, dict):
                raise CapabilityDocsError(f"{capability_id} evidence entries must be objects")
            raw_path = item.get("path")
            if not isinstance(raw_path, str) or not raw_path:
                raise CapabilityDocsError(f"{capability_id} evidence path must be non-empty")
            path = resolve_repo_path(raw_path)
            if not path.exists():
                raise CapabilityDocsError(f"{capability_id} evidence path is missing: {raw_path}")


def _manifest_support_claims(manifest: dict[str, Any]) -> dict[str, dict[str, str]]:
    raw_fixtures = manifest.get("fixtures")
    if not isinstance(raw_fixtures, list):
        raise CapabilityDocsError("canonical manifest fixtures must be a list")
    fixture_paths: set[str] = set()
    for index, fixture in enumerate(raw_fixtures):
        if not isinstance(fixture, dict):
            raise CapabilityDocsError(f"canonical manifest fixtures[{index}] must be an object")
        raw_path = fixture.get("path")
        if not isinstance(raw_path, str) or not raw_path:
            raise CapabilityDocsError(f"canonical manifest fixtures[{index}].path must be non-empty")
        fixture_paths.add(raw_path)

    raw_claims = manifest.get("support_claims")
    if not isinstance(raw_claims, list) or not raw_claims:
        raise CapabilityDocsError("canonical manifest support_claims must be a non-empty list")

    claims: dict[str, dict[str, str]] = {}
    for index, claim in enumerate(raw_claims):
        if not isinstance(claim, dict):
            raise CapabilityDocsError(f"canonical manifest support_claims[{index}] must be an object")
        claim_id = claim.get("claim_id")
        owner_phase = claim.get("owner_phase")
        behavior_fixture = claim.get("behavior_fixture")
        executable_command = claim.get("executable_command")
        if not isinstance(claim_id, str) or not claim_id:
            raise CapabilityDocsError(f"canonical manifest support_claims[{index}].claim_id must be non-empty")
        if claim_id in claims:
            raise CapabilityDocsError(f"duplicate canonical manifest support claim: {claim_id}")
        if not isinstance(owner_phase, str) or not owner_phase:
            raise CapabilityDocsError(f"{claim_id} owner_phase must be non-empty")
        if not isinstance(behavior_fixture, str) or not behavior_fixture:
            raise CapabilityDocsError(f"{claim_id} behavior_fixture must be non-empty")
        if behavior_fixture not in fixture_paths:
            raise CapabilityDocsError(
                f"{claim_id} behavior fixture is not listed in canonical manifest fixtures: {behavior_fixture}"
            )
        if not resolve_repo_path(behavior_fixture).is_file():
            raise CapabilityDocsError(f"{claim_id} behavior fixture is missing: {behavior_fixture}")
        if executable_command != BEHAVIOR_MATRIX_COMMAND:
            raise CapabilityDocsError(
                f"{claim_id} must use executable command {BEHAVIOR_MATRIX_COMMAND!r}"
            )
        claims[claim_id] = {
            "claim_id": claim_id,
            "owner_phase": owner_phase,
            "behavior_fixture": behavior_fixture,
            "executable_command": executable_command,
        }
    return claims


def _row_support_claims(row: dict[str, Any]) -> list[str]:
    capability_id = str(row["id"])
    raw_claims = row.get("support_claims", [])
    if not isinstance(raw_claims, list):
        raise CapabilityDocsError(f"{capability_id} support_claims must be a list")
    claims: list[str] = []
    seen: set[str] = set()
    for index, raw_claim in enumerate(raw_claims):
        if not isinstance(raw_claim, str) or not raw_claim:
            raise CapabilityDocsError(f"{capability_id} support_claims[{index}] must be non-empty")
        if raw_claim in seen:
            raise CapabilityDocsError(f"{capability_id} duplicates support claim {raw_claim}")
        seen.add(raw_claim)
        claims.append(raw_claim)
    return claims


def _validate_support_claim_links(rows: list[dict[str, Any]], manifest: dict[str, Any]) -> None:
    manifest_claims = _manifest_support_claims(manifest)
    documented_by_claim: dict[str, str] = {}

    for row in rows:
        capability_id = str(row["id"])
        state = str(row["state"])
        support_claims = _row_support_claims(row)
        if state == "implemented" and not support_claims:
            raise CapabilityDocsError(f"{capability_id} implemented rows must link canonical support_claims")
        if state != "implemented" and support_claims:
            raise CapabilityDocsError(f"{capability_id} support_claims are only allowed on implemented rows")

        evidence = row["evidence"]
        test_evidence = {
            (str(item.get("path")), str(item.get("command")))
            for item in evidence
            if isinstance(item, dict) and item.get("kind") == "test"
        }
        for claim_id in support_claims:
            if claim_id not in manifest_claims:
                raise CapabilityDocsError(f"{capability_id} references unknown support claim: {claim_id}")
            previous_capability = documented_by_claim.get(claim_id)
            if previous_capability is not None:
                raise CapabilityDocsError(
                    f"support claim {claim_id} is linked by both {previous_capability} and {capability_id}"
                )
            claim = manifest_claims[claim_id]
            expected_evidence = (claim["behavior_fixture"], claim["executable_command"])
            if expected_evidence not in test_evidence:
                raise CapabilityDocsError(
                    f"{capability_id} support claim {claim_id} must include executable evidence "
                    f"{claim['behavior_fixture']} via {claim['executable_command']}"
                )
            documented_by_claim[claim_id] = capability_id

    missing_claims = sorted(set(manifest_claims) - set(documented_by_claim))
    if missing_claims:
        raise CapabilityDocsError(
            "canonical manifest support claims missing from capability matrix: "
            + ", ".join(missing_claims)
        )


def _validate_docs_reference_rows(rows: list[dict[str, Any]]) -> None:
    matrix_text = MATRIX_DOC.read_text(encoding="utf-8")
    evidence_text = EVIDENCE_DOC.read_text(encoding="utf-8")
    docs_text = f"{matrix_text}\n{evidence_text}"
    matrix_support_claims: set[str] = set()
    for row in rows:
        capability_id = str(row["id"])
        if capability_id not in evidence_text:
            raise CapabilityDocsError(f"evidence map missing capability id: {capability_id}")
        for claim_id in _row_support_claims(row):
            matrix_support_claims.add(claim_id)
            if claim_id not in matrix_text:
                raise CapabilityDocsError(f"capability matrix doc missing support claim: {claim_id}")
            if claim_id not in evidence_text:
                raise CapabilityDocsError(f"evidence map missing support claim: {claim_id}")
        for item in row["evidence"]:
            path = str(item["path"])
            if path not in evidence_text and path not in matrix_text:
                raise CapabilityDocsError(f"docs missing evidence path for {capability_id}: {path}")
            command = item.get("command")
            if isinstance(command, str) and command and command not in docs_text:
                raise CapabilityDocsError(f"docs missing evidence command for {capability_id}: {command}")

    undocumented_claims = sorted(set(SUPPORT_CLAIM_RE.findall(docs_text)) - matrix_support_claims)
    if undocumented_claims:
        raise CapabilityDocsError(
            "support docs mention claims not declared in capability_matrix.json: "
            + ", ".join(undocumented_claims)
        )


def validate() -> None:
    matrix = load_json_object(MATRIX_PATH)
    schema = load_json_object(SCHEMA_PATH)
    try:
        validate_json_schema(matrix, schema, label=display_path(MATRIX_PATH))
    except JsonSchemaValidationError as exc:
        raise CapabilityDocsError(str(exc)) from exc
    rows = _require_matrix_shape(matrix)
    _validate_evidence_rows(rows)
    _validate_support_claim_links(rows, load_json_object(CANONICAL_MANIFEST_PATH))
    _validate_docs_reference_rows(rows)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="validate capability docs")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    _ = build_parser().parse_args(argv)
    try:
        validate()
    except CapabilityDocsError as exc:
        print(f"capability docs validation error: {exc}", file=sys.stderr)
        return 1
    print("capability-docs: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
