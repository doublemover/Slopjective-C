#!/usr/bin/env python3
"""Validate M259-E003 runnable object-model closeout sign-off."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-e003-runnable-object-model-closeout-signoff-v1"
CONTRACT_ID = "objc3c-runnable-object-model-closeout-signoff/m259-e003-v1"
CLOSEOUT_MODEL = "runbook-plus-signoff-summary-over-all-m259-predecessor-summaries"
EVIDENCE_MODEL = "tracked-runbook-and-signoff-summary-with-predecessor-green-chain"
FAILURE_MODEL = "fail-closed-on-closeout-drift-or-missing-predecessor-signoff"
NEXT_ISSUE = "M260-A001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-E003" / "runnable_object_model_closeout_signoff_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_runnable_object_model_closeout_signoff_e003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_e003_runnable_object_model_closeout_signoff_packet.md"
RUNBOOK = ROOT / "docs" / "runbooks" / "m259_runnable_object_model_closeout.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_SCRIPT = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
PREDECESSOR_SUMMARIES: dict[str, tuple[Path, str]] = {
    "M259-A001": (
        ROOT / "tmp" / "reports" / "m259" / "M259-A001" / "runnable_sample_surface_contract_summary.json",
        "objc3c-runnable-sample-surface/m259-a001-v1",
    ),
    "M259-A002": (
        ROOT / "tmp" / "reports" / "m259" / "M259-A002" / "canonical_runnable_sample_set_summary.json",
        "objc3c-canonical-runnable-sample-set/m259-a002-v1",
    ),
    "M259-B001": (
        ROOT / "tmp" / "reports" / "m259" / "M259-B001" / "runnable_core_compatibility_guard_summary.json",
        "objc3c-runnable-core-compatibility-guard/m259-b001-v1",
    ),
    "M259-B002": (
        ROOT / "tmp" / "reports" / "m259" / "M259-B002" / "fail_closed_unsupported_advanced_feature_diagnostics_summary.json",
        "objc3c-runnable-core-unsupported-advanced-feature-diagnostics/m259-b002-v1",
    ),
    "M259-C001": (
        ROOT / "tmp" / "reports" / "m259" / "M259-C001" / "end_to_end_replay_and_inspection_summary.json",
        "objc3c-runnable-replay-and-inspection-evidence-freeze/m259-c001-v1",
    ),
    "M259-C002": (
        ROOT / "tmp" / "reports" / "m259" / "M259-C002" / "object_and_ir_replay_proof_plus_metadata_inspection_summary.json",
        "objc3c-runnable-object-ir-replay-and-metadata-inspection/m259-c002-v1",
    ),
    "M259-D001": (
        ROOT / "tmp" / "reports" / "m259" / "M259-D001" / "toolchain_and_runtime_operations_contract_summary.json",
        "objc3c-runnable-toolchain-runtime-operations-freeze/m259-d001-v1",
    ),
    "M259-D002": (
        ROOT / "tmp" / "reports" / "m259" / "M259-D002" / "build_install_run_workflow_and_binary_packaging_summary.json",
        "objc3c-runnable-build-install-run-package/m259-d002-v1",
    ),
    "M259-D003": (
        ROOT / "tmp" / "reports" / "m259" / "M259-D003" / "platform_prerequisites_and_runtime_bring_up_documentation_summary.json",
        "objc3c-runnable-platform-prerequisites-runtime-bringup/m259-d003-v1",
    ),
    "M259-E001": (
        ROOT / "tmp" / "reports" / "m259" / "M259-E001" / "runnable_object_model_release_gate_contract_summary.json",
        "objc3c-runnable-object-model-release-gate/m259-e001-v1",
    ),
    "M259-E002": (
        ROOT / "tmp" / "reports" / "m259" / "M259-E002" / "full_runnable_object_model_conformance_matrix_summary.json",
        "objc3c-runnable-object-model-conformance-matrix/m259-e002-v1",
    ),
}


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
    parser.add_argument("--runbook", type=Path, default=RUNBOOK)
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
            SnippetCheck("M259-E003-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M259-E003-DOC-02", "Issue: `#7219`"),
            SnippetCheck("M259-E003-DOC-03", "`docs/runbooks/m259_runnable_object_model_closeout.md`"),
            SnippetCheck("M259-E003-DOC-04", "`M259` closes the runnable object-model slice only"),
            SnippetCheck("M259-E003-DOC-05", "`M260-A001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M259-E003-PKT-01", "Packet: `M259-E003`"),
            SnippetCheck("M259-E003-PKT-02", "Issue: `#7219`"),
            SnippetCheck("M259-E003-PKT-03", "Dependencies: `M259-E002`"),
            SnippetCheck("M259-E003-PKT-04", "The closeout checker verifies every predecessor `M259` summary is present and green."),
            SnippetCheck("M259-E003-PKT-05", "`M260-A001`"),
        ),
        failures,
    )
    checks_total += 7
    checks_passed += ensure_snippets(
        args.runbook,
        (
            SnippetCheck("M259-E003-RBK-01", "# M259 Runnable Object-Model Closeout Runbook"),
            SnippetCheck("M259-E003-RBK-02", "`npm run build:objc3c-native`"),
            SnippetCheck("M259-E003-RBK-03", "`npm run package:objc3c-native:runnable-toolchain`"),
            SnippetCheck("M259-E003-RBK-04", "`pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m259/e003/canonical --emit-prefix module`"),
            SnippetCheck("M259-E003-RBK-05", "`pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1`"),
            SnippetCheck("M259-E003-RBK-06", "`spec/planning/compiler/m259/m259_e002_full_runnable_object_model_conformance_matrix.json`"),
            SnippetCheck("M259-E003-RBK-07", "`M260-A001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M259-E003-SRC-01", "## M259 runnable object-model closeout and sign-off (E003)"),
            SnippetCheck("M259-E003-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-E003-SRC-03", "`docs/runbooks/m259_runnable_object_model_closeout.md`"),
            SnippetCheck("M259-E003-SRC-04", "block/ARC work remains in `M260+`"),
            SnippetCheck("M259-E003-SRC-05", "`M260-A001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M259-E003-NDOC-01", "## M259 runnable object-model closeout and sign-off (E003)"),
            SnippetCheck("M259-E003-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-E003-NDOC-03", "`docs/runbooks/m259_runnable_object_model_closeout.md`"),
            SnippetCheck("M259-E003-NDOC-04", "`M259` closes the runnable object-model slice only"),
            SnippetCheck("M259-E003-NDOC-05", "`M260-A001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M259-E003-SPC-01", "## M259 runnable object-model closeout and sign-off (E003)"),
            SnippetCheck("M259-E003-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-E003-SPC-03", f"`{CLOSEOUT_MODEL}`"),
            SnippetCheck("M259-E003-SPC-04", f"`{FAILURE_MODEL}`"),
            SnippetCheck("M259-E003-SPC-05", "`M260-A001`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M259-E003-META-01", "## M259 runnable object-model closeout metadata anchors (E003)"),
            SnippetCheck("M259-E003-META-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-E003-META-03", "`tmp/reports/m259/M259-E002/`"),
            SnippetCheck("M259-E003-META-04", "`M260-A001`"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        args.smoke_script,
        (
            SnippetCheck("M259-E003-SMOKE-01", "M259-E003 closeout-signoff anchor:"),
            SnippetCheck("M259-E003-SMOKE-02", "final `M259` closeout runbook and sign-off summary."),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        args.replay_script,
        (
            SnippetCheck("M259-E003-REPLAY-01", "M259-E003 closeout-signoff anchor:"),
            SnippetCheck("M259-E003-REPLAY-02", "final `M259` closeout runbook and sign-off summary."),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M259-E003-PKG-01", '"check:objc3c:m259-e003-runnable-object-model-closeout-signoff":'),
            SnippetCheck("M259-E003-PKG-02", '"test:tooling:m259-e003-runnable-object-model-closeout-signoff":'),
            SnippetCheck("M259-E003-PKG-03", '"check:objc3c:m259-e003-lane-e-readiness":'),
            SnippetCheck("M259-E003-PKG-04", '"check:objc3c:m259-e003-lane-e-readiness": "python scripts/run_m259_e003_lane_e_readiness.py"'),
            SnippetCheck("M259-E003-PKG-05", '"package:objc3c-native:runnable-toolchain":'),
        ),
        failures,
    )

    dependency_summaries: dict[str, dict[str, Any]] = {}
    for name, (path, expected_contract) in PREDECESSOR_SUMMARIES.items():
        checks_total += 2
        payload = load_json(path)
        dependency_summaries[name] = payload
        checks_passed += require(
            payload.get("contract_id") == expected_contract,
            display_path(path),
            f"{name}-DEP-01",
            f"{name} contract drift",
            failures,
        )
        checks_passed += require(
            payload.get("ok") is True,
            display_path(path),
            f"{name}-DEP-02",
            f"{name} summary must remain green",
            failures,
        )

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "closeout_model": CLOSEOUT_MODEL,
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
                "contract_id": dependency_summaries[name].get("contract_id"),
                "ok": dependency_summaries[name].get("ok"),
            }
            for name, (path, _expected_contract) in PREDECESSOR_SUMMARIES.items()
        },
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
