#!/usr/bin/env python3
"""Validate M259-D001 toolchain/runtime operations freeze anchors."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-d001-toolchain-and-runtime-operations-freeze-v1"
CONTRACT_ID = "objc3c-runnable-toolchain-runtime-operations-freeze/m259-d001-v1"
OPERATIONS_MODEL = "runnable-core-build-compile-smoke-replay-operations-boundary"
EVIDENCE_MODEL = "operations-freeze-docs-package-and-script-anchors-for-runnable-core"
FAILURE_MODEL = "fail-closed-on-unsupported-packaging-or-runtime-operations-claim-drift"
NEXT_ISSUE = "M259-D002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-D001" / "toolchain_and_runtime_operations_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_toolchain_and_runtime_operations_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_SCRIPT = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
C002_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-C002" / "object_and_ir_replay_proof_plus_metadata_inspection_summary.json"
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

    checks_total += 4
    checks_passed += ensure_snippets(
        EXPECTATIONS_DOC,
        (
            SnippetCheck("M259-D001-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M259-D001-DOC-02", "Add deterministic checker coverage over the supported host baseline:"),
            SnippetCheck("M259-D001-DOC-03", "D001 freezes the operations boundary only; `M259-D002` implements the workflow and packaging surface"),
            SnippetCheck("M259-D001-DOC-04", "The contract must explicitly hand off to `M259-D002`."),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        PACKET_DOC,
        (
            SnippetCheck("M259-D001-PKT-01", "Packet: `M259-D001`"),
            SnippetCheck("M259-D001-PKT-02", "Issue: `#7214`"),
            SnippetCheck("M259-D001-PKT-03", "Dependencies: none"),
            SnippetCheck("M259-D001-PKT-04", "## Next Issue"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        DOC_SOURCE,
        (
            SnippetCheck("M259-D001-SRC-01", "## M259 toolchain and runtime operations freeze (D001)"),
            SnippetCheck("M259-D001-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-D001-SRC-03", "`M259-D002`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        DOC_NATIVE,
        (
            SnippetCheck("M259-D001-NDOC-01", "## M259 toolchain and runtime operations freeze (D001)"),
            SnippetCheck("M259-D001-NDOC-02", "Windows x64 + `pwsh` + `python` + `node`/`npm` + MSVC/CMake/Ninja + LLVM `llc`/`llvm-readobj`"),
            SnippetCheck("M259-D001-NDOC-03", "`artifacts/lib/objc3_runtime.lib`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        LOWERING_SPEC,
        (
            SnippetCheck("M259-D001-SPC-01", "## M259 toolchain and runtime operations freeze (D001)"),
            SnippetCheck("M259-D001-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-D001-SPC-03", f"`{FAILURE_MODEL}`"),
            SnippetCheck("M259-D001-SPC-04", "`M259-D002`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        METADATA_SPEC,
        (
            SnippetCheck("M259-D001-META-01", "## M259 toolchain and package-output freeze anchors (D001)"),
            SnippetCheck("M259-D001-META-02", "`artifacts/bin/objc3c-native.exe`"),
            SnippetCheck("M259-D001-META-03", "`tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        SMOKE_SCRIPT,
        (
            SnippetCheck("M259-D001-SMOKE-01", "M259-D001 toolchain-runtime-operations anchor:"),
            SnippetCheck("M259-D001-SMOKE-02", "installer or cross-platform packaging claims remain outside this freeze"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        REPLAY_SCRIPT,
        (
            SnippetCheck("M259-D001-REPLAY-01", "M259-D001 toolchain-runtime-operations anchor:"),
            SnippetCheck("M259-D001-REPLAY-02", "workflow/package expansion remains deferred to M259-D002"),
        ),
        failures,
    )
    checks_total += 7
    checks_passed += ensure_snippets(
        PACKAGE_JSON,
        (
            SnippetCheck("M259-D001-PKG-01", '"check:objc3c:m259-d001-toolchain-and-runtime-operations-contract"'),
            SnippetCheck("M259-D001-PKG-02", '"test:tooling:m259-d001-toolchain-and-runtime-operations-contract"'),
            SnippetCheck("M259-D001-PKG-03", '"check:objc3c:m259-d001-lane-d-readiness"'),
            SnippetCheck("M259-D001-PKG-04", '"build:objc3c-native":'),
            SnippetCheck("M259-D001-PKG-05", '"compile:objc3c":'),
            SnippetCheck("M259-D001-PKG-06", '"test:objc3c:execution-smoke":'),
            SnippetCheck("M259-D001-PKG-07", '"test:objc3c:execution-replay-proof":'),
        ),
        failures,
    )

    c002_summary = load_json(C002_SUMMARY)
    checks_total += 5
    checks_passed += require(c002_summary.get("contract_id") == "objc3c-runnable-object-ir-replay-and-metadata-inspection/m259-c002-v1", display_path(C002_SUMMARY), "M259-D001-DEP-01", "M259-C002 contract drift", failures)
    checks_passed += require(c002_summary.get("ok") is True, display_path(C002_SUMMARY), "M259-D001-DEP-02", "M259-C002 summary must remain green", failures)
    checks_passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M259-D001-ART-01", "native executable must exist for the frozen operations boundary", failures)
    checks_passed += require(RUNTIME_LIB.exists(), display_path(RUNTIME_LIB), "M259-D001-ART-02", "runtime library must exist for the frozen operations boundary", failures)
    replay_summary = c002_summary.get("probe_details", {}).get("replay_summary")
    checks_passed += require(isinstance(replay_summary, str) and bool(replay_summary.strip()), display_path(C002_SUMMARY), "M259-D001-DEP-03", "M259-C002 replay proof summary path must remain published", failures)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "operations_model": OPERATIONS_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "dependency": {
            "M259-C002": {
                "summary": display_path(C002_SUMMARY),
                "contract_id": c002_summary.get("contract_id"),
                "ok": c002_summary.get("ok"),
            }
        },
        "artifacts": {
            "native_exe": display_path(NATIVE_EXE),
            "runtime_lib": display_path(RUNTIME_LIB),
        },
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
