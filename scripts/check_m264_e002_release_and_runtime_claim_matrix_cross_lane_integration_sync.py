#!/usr/bin/env python3
"""Fail-closed checker for M264-E002 release/runtime claim matrix closeout."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m264-e002-release-runtime-claim-matrix-closeout-v1"
CONTRACT_ID = "objc3c-release-runtime-claim-matrix/m264-e002-v1"
MATRIX_MODEL = "release-matrix-consumes-a002-b003-c002-d002-and-e001-without-widening-the-runnable-core-profile"
OPERATOR_MODEL = "native-cli-and-frontend-c-api-publication-surfaces-stay-core-json-only-and-fail-closed-for-unsupported-claims"
FAILURE_MODEL = "fail-closed-on-release-matrix-drift-profile-overclaim-format-widening-or-closeout-doc-mismatch"
NEXT_ISSUE = "M265-A001"

A002_CONTRACT_ID = "objc3c-feature-claim-strictness-truth-surface/m264-a002-v1"
B003_CONTRACT_ID = "objc3c-canonical-interface-and-feature-macro-truthfulness/m264-b003-v1"
C002_CONTRACT_ID = "objc3c-runtime-capability-reporting/m264-c002-v1"
D002_CONTRACT_ID = "objc3c-toolchain-conformance-claim-operations/m264-d002-v1"
E001_CONTRACT_ID = "objc3c-versioning-conformance-truth-gate/m264-e001-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_release_and_runtime_claim_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_e002_release_and_runtime_claim_matrix_cross_lane_integration_sync_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
PACKAGE_JSON = ROOT / "package.json"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
MATRIX_SCRIPT = ROOT / "scripts" / "publish_m264_release_runtime_claim_matrix.py"
MATRIX_JSON = ROOT / "tmp" / "reports" / "m264" / "M264-E002" / "release_runtime_claim_matrix.json"
MATRIX_MD = ROOT / "tmp" / "reports" / "m264" / "M264-E002" / "release_runtime_claim_matrix.md"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m264" / "M264-E002" / "release_runtime_claim_matrix_summary.json"

A002_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-A002" / "frontend_feature_claim_and_strictness_truthfulness_wiring_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-B003" / "canonical_interface_and_feature_macro_truthfulness_summary.json"
C002_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-C002" / "machine_readable_runtime_capability_reporting_summary.json"
D002_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-D002" / "cli_and_toolchain_conformance_claim_operations_summary.json"
E001_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-E001" / "versioning_and_conformance_truth_gate_summary.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    artifact = display_path(path)
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(artifact, snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(ROOT),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def validate_summary(
    name: str,
    path: Path,
    expected_contract_id: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("contract_id") == expected_contract_id, artifact, f"{name}-CID", "contract id drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{name}-CHECKS", "summary must preserve full passing coverage", failures)
    total += 1
    ok_value = payload.get("ok")
    passed += require(ok_value is True or ok_value is None, artifact, f"{name}-OK", "summary ok field drifted", failures)
    return total, passed, payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--matrix-json", type=Path, default=MATRIX_JSON)
    parser.add_argument("--matrix-md", type=Path, default=MATRIX_MD)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M264-E002-EXP-01", CONTRACT_ID),
            SnippetCheck("M264-E002-EXP-02", "tmp/reports/m264/M264-E002/release_runtime_claim_matrix.json"),
            SnippetCheck("M264-E002-EXP-03", "M265-A001"),
        ],
        PACKET_DOC: [
            SnippetCheck("M264-E002-PKT-01", "Issue: `#7243`"),
            SnippetCheck("M264-E002-PKT-02", "# M264-E002 Packet"),
            SnippetCheck("M264-E002-PKT-03", "- `M264-E001`"),
            SnippetCheck("M264-E002-PKT-04", "`M265-A001`"),
        ],
        DRIVER_CPP: [
            SnippetCheck("M264-E002-DRV-01", "M264-E002 release/runtime-claim-matrix anchor"),
        ],
        MANIFEST_CPP: [
            SnippetCheck("M264-E002-MAN-01", "M264-E002 release/runtime-claim-matrix anchor"),
        ],
        FRONTEND_ANCHOR_CPP: [
            SnippetCheck("M264-E002-FRONT-01", "M264-E002 release/runtime-claim-matrix anchor"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M264-E002-DOCSRC-01", "## M264 release/runtime claim matrix (E002)"),
            SnippetCheck("M264-E002-DOCSRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M264-E002-DOCSRC-03", "tmp/reports/m264/M264-E002/release_runtime_claim_matrix.json"),
            SnippetCheck("M264-E002-DOCSRC-04", "the next issue is `M265-A001`"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M264-E002-DOCNATIVE-01", "## M264 release/runtime claim matrix (E002)"),
            SnippetCheck("M264-E002-DOCNATIVE-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M264-E002-DOCNATIVE-03", "strict, strict-concurrency, and strict-system remain fail-closed and not"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M264-E002-SPEC-01", "## M264 release/runtime claim matrix (implementation note)"),
            SnippetCheck("M264-E002-SPEC-02", "JSON remains the only runnable emit/validate format"),
            SnippetCheck("M264-E002-SPEC-03", "`M265-A001` is next"),
        ],
        SPEC_DECISIONS: [
            SnippetCheck("M264-E002-DEC-01", "## D-025: The M264 closeout publishes one release/runtime claim matrix"),
            SnippetCheck("M264-E002-DEC-02", "profile claims for `strict`, `strict-concurrency`, or `strict-system`"),
            SnippetCheck("M264-E002-DEC-03", "release summary"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M264-E002-PKG-01", '"check:objc3c:m264-e002-release-runtime-claim-matrix"'),
            SnippetCheck("M264-E002-PKG-02", '"test:tooling:m264-e002-release-runtime-claim-matrix"'),
            SnippetCheck("M264-E002-PKG-03", '"check:objc3c:m264-e002-lane-e-readiness"'),
        ],
    }
    for path, snippets in static_snippets.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream: dict[str, dict[str, Any]] = {}
    for name, path, contract_id in (
        ("M264-A002", A002_SUMMARY, A002_CONTRACT_ID),
        ("M264-B003", B003_SUMMARY, B003_CONTRACT_ID),
        ("M264-C002", C002_SUMMARY, C002_CONTRACT_ID),
        ("M264-D002", D002_SUMMARY, D002_CONTRACT_ID),
        ("M264-E001", E001_SUMMARY, E001_CONTRACT_ID),
    ):
        total, passed, payload = validate_summary(name, path, contract_id, failures)
        checks_total += total
        checks_passed += passed
        upstream[name] = payload

    if not args.skip_dynamic_probes:
        published = run_command([
            sys.executable,
            str(MATRIX_SCRIPT),
            "--json-out",
            str(args.matrix_json),
            "--md-out",
            str(args.matrix_md),
        ])
        checks_total += 1
        checks_passed += require(
            published.returncode == 0,
            display_path(MATRIX_SCRIPT),
            "M264-E002-PUBLISH",
            f"matrix publisher failed: {published.stderr or published.stdout}",
            failures,
        )

    checks_total += 2
    matrix_json_exists = args.matrix_json.exists()
    matrix_md_exists = args.matrix_md.exists()
    if args.skip_dynamic_probes:
        checks_passed += require(True, display_path(args.matrix_json), "M264-E002-MATRIX-JSON", "static mode", failures)
        checks_passed += require(True, display_path(args.matrix_md), "M264-E002-MATRIX-MD", "static mode", failures)
    else:
        checks_passed += require(matrix_json_exists, display_path(args.matrix_json), "M264-E002-MATRIX-JSON", "matrix json missing", failures)
        checks_passed += require(matrix_md_exists, display_path(args.matrix_md), "M264-E002-MATRIX-MD", "matrix markdown missing", failures)

    matrix_payload: dict[str, Any] = {}
    if matrix_json_exists:
        matrix_payload = load_json(args.matrix_json)
        artifact = display_path(args.matrix_json)
        checks_total += 18
        checks_passed += require(matrix_payload.get("contract_id") == CONTRACT_ID, artifact, "M264-E002-MATRIX-01", "contract id drifted", failures)
        checks_passed += require(matrix_payload.get("ready") is True, artifact, "M264-E002-MATRIX-02", "matrix must report ready=true", failures)
        profiles = {entry.get("id"): entry for entry in matrix_payload.get("profiles", []) if isinstance(entry, dict)}
        checks_passed += require(profiles.get("core", {}).get("runtime_status") == "runnable", artifact, "M264-E002-MATRIX-03", "core profile must remain runnable", failures)
        checks_passed += require(profiles.get("strict", {}).get("selection_status") == "fail-closed", artifact, "M264-E002-MATRIX-04", "strict profile must remain fail-closed", failures)
        checks_passed += require(profiles.get("strict-concurrency", {}).get("selection_status") == "fail-closed", artifact, "M264-E002-MATRIX-05", "strict-concurrency must remain fail-closed", failures)
        checks_passed += require(profiles.get("strict-system", {}).get("selection_status") == "fail-closed", artifact, "M264-E002-MATRIX-06", "strict-system must remain fail-closed", failures)
        formats = {entry.get("format"): entry for entry in matrix_payload.get("operator_formats", []) if isinstance(entry, dict)}
        checks_passed += require(formats.get("json", {}).get("emit_status") == "supported", artifact, "M264-E002-MATRIX-07", "json emit must remain supported", failures)
        checks_passed += require(formats.get("json", {}).get("validate_status") == "supported", artifact, "M264-E002-MATRIX-08", "json validate must remain supported", failures)
        checks_passed += require(formats.get("yaml", {}).get("emit_status") == "fail-closed", artifact, "M264-E002-MATRIX-09", "yaml emit must remain fail-closed", failures)
        checks_passed += require(formats.get("yaml", {}).get("validate_status") == "fail-closed", artifact, "M264-E002-MATRIX-10", "yaml validate must remain fail-closed", failures)
        surfaces = {entry.get("surface"): entry for entry in matrix_payload.get("surfaces", []) if isinstance(entry, dict)}
        checks_passed += require(surfaces.get("native-cli", {}).get("validation") is True, artifact, "M264-E002-MATRIX-11", "native CLI must retain validation surface", failures)
        checks_passed += require(surfaces.get("frontend-c-api", {}).get("publication") is True, artifact, "M264-E002-MATRIX-12", "frontend C API must retain publication surface", failures)
        checks_passed += require(surfaces.get("frontend-c-api", {}).get("validation") is False, artifact, "M264-E002-MATRIX-13", "frontend C API must not claim validation surface", failures)
        optional_features = {entry.get("id"): entry.get("status") for entry in matrix_payload.get("optional_features", []) if isinstance(entry, dict)}
        checks_passed += require(optional_features.get("throws") == "not-claimed", artifact, "M264-E002-MATRIX-14", "throws must remain not-claimed", failures)
        checks_passed += require(optional_features.get("arc") == "not-claimed", artifact, "M264-E002-MATRIX-15", "arc must remain not-claimed at M264 closeout", failures)
        live_probes = matrix_payload.get("live_probes", {})
        checks_passed += require(live_probes.get("native_validation", {}).get("selected_profile") == "core", artifact, "M264-E002-MATRIX-16", "validation profile drifted", failures)
        checks_passed += require(live_probes.get("strict_profile_reject", {}).get("returncode", 0) != 0, artifact, "M264-E002-MATRIX-17", "strict-profile rejection must fail closed", failures)
        checks_passed += require(live_probes.get("yaml_emit_reject", {}).get("returncode", 0) != 0, artifact, "M264-E002-MATRIX-18", "yaml rejection must fail closed", failures)

    if matrix_md_exists:
        checks_total += 4
        checks_passed += ensure_snippets(
            args.matrix_md,
            [
                SnippetCheck("M264-E002-MD-01", "# M264 Release/Runtime Claim Matrix"),
                SnippetCheck("M264-E002-MD-02", "Claimed profile"),
                SnippetCheck("M264-E002-MD-03", "Strict / strict-concurrency / strict-system"),
                SnippetCheck("M264-E002-MD-04", "M265-A001"),
            ],
            failures,
        )

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "matrix_model": MATRIX_MODEL,
        "operator_model": OPERATOR_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "ok": not failures,
        "matrix_json": display_path(args.matrix_json),
        "matrix_md": display_path(args.matrix_md),
        "upstream_summaries": {
            key: {
                "contract_id": value.get("contract_id"),
                "summary_path": display_path(path),
            }
            for key, value, path in (
                ("M264-A002", upstream["M264-A002"], A002_SUMMARY),
                ("M264-B003", upstream["M264-B003"], B003_SUMMARY),
                ("M264-C002", upstream["M264-C002"], C002_SUMMARY),
                ("M264-D002", upstream["M264-D002"], D002_SUMMARY),
                ("M264-E001", upstream["M264-E001"], E001_SUMMARY),
            )
        },
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        json.dump(payload, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return 1
    print(f"[ok] M264-E002 release/runtime claim matrix checks passed ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
