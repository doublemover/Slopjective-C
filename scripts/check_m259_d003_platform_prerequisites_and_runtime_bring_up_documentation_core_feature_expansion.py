#!/usr/bin/env python3
"""Validate M259-D003 platform prerequisites and runtime bring-up docs."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-d003-platform-prerequisites-and-runtime-bringup-v1"
CONTRACT_ID = "objc3c-runnable-platform-prerequisites-runtime-bringup/m259-d003-v1"
BRINGUP_MODEL = "supported-windows-host-prereqs-and-package-root-runtime-bringup"
EVIDENCE_MODEL = "docs-and-script-anchors-for-prereq-and-runtime-bringup-truthfulness"
FAILURE_MODEL = "fail-closed-on-prerequisite-or-runtime-bringup-claim-drift"
NEXT_ISSUE = "M259-E001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-D003" / "platform_prerequisites_and_runtime_bring_up_documentation_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion_d003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_d003_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_SCRIPT = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
D002_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-D002" / "build_install_run_workflow_and_binary_packaging_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"


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
    parser.add_argument("--d002-summary", type=Path, default=D002_SUMMARY)
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
            SnippetCheck("M259-D003-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M259-D003-DOC-02", "Issue: `#7216`"),
            SnippetCheck("M259-D003-DOC-03", "`OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH`"),
            SnippetCheck("M259-D003-DOC-04", "`M259-E001`"),
            SnippetCheck("M259-D003-DOC-05", "Extend evidence coverage so documentation drift fails closed without requiring another full packaged runtime proof."),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M259-D003-PKT-01", "Packet: `M259-D003`"),
            SnippetCheck("M259-D003-PKT-02", "Issue: `#7216`"),
            SnippetCheck("M259-D003-PKT-03", "Dependencies: `M259-D002`"),
            SnippetCheck("M259-D003-PKT-04", "Validation stays issue-local and fail-closed; no new full packaged runtime proof is required here."),
            SnippetCheck("M259-D003-PKT-05", "`M259-E001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M259-D003-SRC-01", "## M259 platform prerequisites and runtime bring-up documentation (D003)"),
            SnippetCheck("M259-D003-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-D003-SRC-03", "LLVM `clang`, `clang++`, `llc`, `llvm-readobj`, `llvm-lib`"),
            SnippetCheck("M259-D003-SRC-04", "`OBJC3C_NATIVE_EXECUTION_LLC_PATH`"),
            SnippetCheck("M259-D003-SRC-05", "package-root execution assumes the repo-relative staged layout preserved by `M259-D002`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M259-D003-NDOC-01", "## M259 platform prerequisites and runtime bring-up documentation (D003)"),
            SnippetCheck("M259-D003-NDOC-02", "Windows x64"),
            SnippetCheck("M259-D003-NDOC-03", "`OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH`"),
            SnippetCheck("M259-D003-NDOC-04", "`scripts/check_objc3c_execution_replay_proof.ps1`"),
            SnippetCheck("M259-D003-NDOC-05", "release-gate closure remains deferred to `M259-E001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M259-D003-SPC-01", "## M259 platform prerequisites and runtime bring-up documentation (D003)"),
            SnippetCheck("M259-D003-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-D003-SPC-03", f"`{BRINGUP_MODEL}`"),
            SnippetCheck("M259-D003-SPC-04", f"`{FAILURE_MODEL}`"),
            SnippetCheck("M259-D003-SPC-05", "`M259-E001`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M259-D003-META-01", "## M259 platform prerequisite and bring-up metadata anchors (D003)"),
            SnippetCheck("M259-D003-META-02", "`OBJC3C_NATIVE_EXECUTION_CLANG_PATH`"),
            SnippetCheck("M259-D003-META-03", "`OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH`"),
            SnippetCheck("M259-D003-META-04", "`M259-E001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.smoke_script,
        (
            SnippetCheck("M259-D003-SMOKE-01", "M259-D003 platform-bringup anchor:"),
            SnippetCheck("M259-D003-SMOKE-02", "`OBJC3C_NATIVE_EXECUTABLE`"),
            SnippetCheck("M259-D003-SMOKE-03", "`OBJC3C_NATIVE_EXECUTION_CLANG_PATH`"),
            SnippetCheck("M259-D003-SMOKE-04", "`OBJC3C_NATIVE_EXECUTION_LLC_PATH`"),
            SnippetCheck("M259-D003-SMOKE-05", "`OBJC3C_NATIVE_EXECUTION_RUN_ID`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.replay_script,
        (
            SnippetCheck("M259-D003-REPLAY-01", "M259-D003 platform-bringup anchor:"),
            SnippetCheck("M259-D003-REPLAY-02", "`OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH`"),
            SnippetCheck("M259-D003-REPLAY-03", "`OBJC3C_NATIVE_EXECUTION_RUN_ID`"),
            SnippetCheck("M259-D003-REPLAY-04", "supported Windows host baseline"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M259-D003-PKG-01", '"package:objc3c-native:runnable-toolchain":'),
            SnippetCheck("M259-D003-PKG-02", '"check:objc3c:m259-d003-platform-prerequisites-and-runtime-bring-up-documentation":'),
            SnippetCheck("M259-D003-PKG-03", '"test:tooling:m259-d003-platform-prerequisites-and-runtime-bring-up-documentation":'),
            SnippetCheck("M259-D003-PKG-04", '"check:objc3c:m259-d003-lane-d-readiness": "python scripts/run_m259_d003_lane_d_readiness.py"'),
            SnippetCheck("M259-D003-PKG-05", '"test:objc3c:execution-replay-proof":'),
        ),
        failures,
    )

    d002_summary = load_json(args.d002_summary)
    checks_total += 5
    checks_passed += require(d002_summary.get("contract_id") == "objc3c-runnable-build-install-run-package/m259-d002-v1", display_path(args.d002_summary), "M259-D003-DEP-01", "M259-D002 contract drift", failures)
    checks_passed += require(d002_summary.get("ok") is True, display_path(args.d002_summary), "M259-D003-DEP-02", "M259-D002 summary must remain green", failures)
    checks_passed += require(d002_summary.get("next_issue") == "M259-D003", display_path(args.d002_summary), "M259-D003-DEP-03", "M259-D002 next-issue handoff drift", failures)
    checks_passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M259-D003-ART-01", "native executable must exist for documented bring-up", failures)
    checks_passed += require(RUNTIME_LIB.exists(), display_path(RUNTIME_LIB), "M259-D003-ART-02", "runtime library must exist for documented bring-up", failures)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "bringup_model": BRINGUP_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "dependency": {
            "M259-D002": {
                "summary": display_path(args.d002_summary),
                "contract_id": d002_summary.get("contract_id"),
                "ok": d002_summary.get("ok"),
                "dynamic_probes_executed": d002_summary.get("dynamic_probes_executed"),
            }
        },
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
