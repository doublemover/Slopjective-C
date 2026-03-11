#!/usr/bin/env python3
"""Validate M259-E001 runnable object-model release gate freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-e001-runnable-object-model-release-gate-v1"
CONTRACT_ID = "objc3c-runnable-object-model-release-gate/m259-e001-v1"
GATE_MODEL = "lane-e-release-gate-over-a002-b002-c002-d003"
EVIDENCE_MODEL = "freeze-the-release-gate-over-canonical-sample-compatibility-replay-and-bringup"
FAILURE_MODEL = "fail-closed-on-release-gate-claim-drift"
NEXT_ISSUE = "M259-E002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-E001" / "runnable_object_model_release_gate_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_runnable_object_model_release_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_e001_runnable_object_model_release_gate_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_SCRIPT = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-A002" / "canonical_runnable_sample_set_summary.json"
B002_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-B002" / "fail_closed_unsupported_advanced_feature_diagnostics_summary.json"
C002_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-C002" / "object_and_ir_replay_proof_plus_metadata_inspection_summary.json"
D003_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-D003" / "platform_prerequisites_and_runtime_bring_up_documentation_summary.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--doc-source", type=Path, default=DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=DOC_NATIVE)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--smoke-script", type=Path, default=SMOKE_SCRIPT)
    parser.add_argument("--replay-script", type=Path, default=REPLAY_SCRIPT)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    return parser.parse_args(argv)


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    checks_total += 5
    checks_passed += ensure_snippets(
        args.expectations_doc,
        (
            SnippetCheck("M259-E001-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M259-E001-DOC-02", "Issue: `#7217`"),
            SnippetCheck("M259-E001-DOC-03", "`tmp/reports/m259/M259-D003/platform_prerequisites_and_runtime_bring_up_documentation_summary.json`"),
            SnippetCheck("M259-E001-DOC-04", "no block/ARC release claim yet"),
            SnippetCheck("M259-E001-DOC-05", "`M259-E002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M259-E001-PKT-01", "Packet: `M259-E001`"),
            SnippetCheck("M259-E001-PKT-02", "Issue: `#7217`"),
            SnippetCheck("M259-E001-PKT-03", "Dependencies: none"),
            SnippetCheck("M259-E001-PKT-04", "No full runnable conformance matrix claim yet."),
            SnippetCheck("M259-E001-PKT-05", "`M259-E002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M259-E001-SRC-01", "## M259 runnable object-model release gate (E001)"),
            SnippetCheck("M259-E001-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-E001-SRC-03", "`tmp/reports/m259/M259-A002/canonical_runnable_sample_set_summary.json`"),
            SnippetCheck("M259-E001-SRC-04", "no full runnable conformance matrix claim yet"),
            SnippetCheck("M259-E001-SRC-05", "`M259-E002` widens this into the full runnable object-model conformance matrix"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M259-E001-NDOC-01", "## M259 runnable object-model release gate (E001)"),
            SnippetCheck("M259-E001-NDOC-02", "`tmp/reports/m259/M259-C002/object_and_ir_replay_proof_plus_metadata_inspection_summary.json`"),
            SnippetCheck("M259-E001-NDOC-03", "no block/ARC release claim yet"),
            SnippetCheck("M259-E001-NDOC-04", "`M259-E002`"),
            SnippetCheck("M259-E001-NDOC-05", "`lane-e-release-gate-over-a002-b002-c002-d003`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M259-E001-SPC-01", "## M259 runnable object-model release gate (E001)"),
            SnippetCheck("M259-E001-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-E001-SPC-03", f"`{GATE_MODEL}`"),
            SnippetCheck("M259-E001-SPC-04", f"`{FAILURE_MODEL}`"),
            SnippetCheck("M259-E001-SPC-05", "`M259-E002`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M259-E001-META-01", "## M259 runnable object-model release-gate metadata anchors (E001)"),
            SnippetCheck("M259-E001-META-02", "`tmp/reports/m259/M259-B002/fail_closed_unsupported_advanced_feature_diagnostics_summary.json`"),
            SnippetCheck("M259-E001-META-03", "no full runnable conformance matrix claim yet"),
            SnippetCheck("M259-E001-META-04", "`M259-E002`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        args.smoke_script,
        (
            SnippetCheck("M259-E001-SMOKE-01", "M259-E001 release-gate-freeze anchor:"),
            SnippetCheck("M259-E001-SMOKE-02", "`M259-A002`, `M259-B002`, `M259-C002`,"),
            SnippetCheck("M259-E001-SMOKE-03", "`M259-E002`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        args.replay_script,
        (
            SnippetCheck("M259-E001-REPLAY-01", "M259-E001 release-gate-freeze anchor:"),
            SnippetCheck("M259-E001-REPLAY-02", "`M259-A002`, `M259-B002`,"),
            SnippetCheck("M259-E001-REPLAY-03", "`M259-E002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M259-E001-PKG-01", '"check:objc3c:m259-e001-runnable-object-model-release-gate":'),
            SnippetCheck("M259-E001-PKG-02", '"test:tooling:m259-e001-runnable-object-model-release-gate":'),
            SnippetCheck("M259-E001-PKG-03", '"check:objc3c:m259-e001-lane-e-readiness":'),
            SnippetCheck("M259-E001-PKG-04", '"check:objc3c:m259-e001-lane-e-readiness": "python scripts/run_m259_e001_lane_e_readiness.py"'),
            SnippetCheck("M259-E001-PKG-05", '"test:objc3c:execution-replay-proof":'),
        ),
        failures,
    )

    dependencies = {
        "M259-A002": load_json(A002_SUMMARY),
        "M259-B002": load_json(B002_SUMMARY),
        "M259-C002": load_json(C002_SUMMARY),
        "M259-D003": load_json(D003_SUMMARY),
    }
    expected_contracts = {
        "M259-A002": "objc3c-canonical-runnable-sample-set/m259-a002-v1",
        "M259-B002": "objc3c-runnable-core-unsupported-advanced-feature-diagnostics/m259-b002-v1",
        "M259-C002": "objc3c-runnable-object-ir-replay-and-metadata-inspection/m259-c002-v1",
        "M259-D003": "objc3c-runnable-platform-prerequisites-runtime-bringup/m259-d003-v1",
    }
    for name, path, expected in (
        ("M259-A002", A002_SUMMARY, expected_contracts["M259-A002"]),
        ("M259-B002", B002_SUMMARY, expected_contracts["M259-B002"]),
        ("M259-C002", C002_SUMMARY, expected_contracts["M259-C002"]),
        ("M259-D003", D003_SUMMARY, expected_contracts["M259-D003"]),
    ):
        payload = dependencies[name]
        checks_total += 2
        checks_passed += require(payload.get("contract_id") == expected, display_path(path), f"{name}-DEP-01", f"{name} contract drift", failures)
        checks_passed += require(payload.get("ok") is True, display_path(path), f"{name}-DEP-02", f"{name} summary must remain green", failures)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "gate_model": GATE_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "dependencies": {
            name: {
                "summary": display_path(path),
                "contract_id": dependencies[name].get("contract_id"),
                "ok": dependencies[name].get("ok"),
            }
            for name, path in (
                ("M259-A002", A002_SUMMARY),
                ("M259-B002", B002_SUMMARY),
                ("M259-C002", C002_SUMMARY),
                ("M259-D003", D003_SUMMARY),
            )
        },
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
