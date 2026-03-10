#!/usr/bin/env python3
"""Validate M259-B001 runnable core compatibility guard."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-b001-runnable-core-compatibility-guard-v1"
CONTRACT_ID = "objc3c-runnable-core-compatibility-guard/m259-b001-v1"
GUARD_MODEL = "runnable-core-distinguishes-live-runtime-backed-core-from-source-only-or-fail-closed-advanced-surfaces"
EVIDENCE_MODEL = "a002-live-runnable-core-proof-plus-sema-compatibility-selection-and-unsupported-claim-boundary"
FAILURE_MODEL = "fail-closed-on-runnable-core-compatibility-guard-drift-or-overclaimed-advanced-surface-support"
NEXT_ISSUE = "M259-B002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-B001" / "runnable_core_compatibility_guard_summary.json"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-A002" / "canonical_runnable_sample_set_summary.json"
A002_CONTRACT_ID = "objc3c-canonical-runnable-sample-set/m259-a002-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_b001_runnable_core_compatibility_guard_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_b001_runnable_core_compatibility_guard_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
EXECUTION_SMOKE = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
EXECUTION_REPLAY = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m259_b001_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m259_b001_runnable_core_compatibility_guard.py"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M259-B001-DOC-EXP-01", "# M259 B001 Runnable Core Compatibility Guard Expectations"),
        SnippetCheck("M259-B001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M259-B001-DOC-EXP-03", "migration-assist legacy-literal diagnostics remain fail-closed `O3S216`"),
        SnippetCheck("M259-B001-DOC-EXP-04", "The contract must explicitly hand off to `M259-B002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M259-B001-DOC-PKT-01", "# M259-B001 Runnable Core Compatibility Guard Packet"),
        SnippetCheck("M259-B001-DOC-PKT-02", "Packet: `M259-B001`"),
        SnippetCheck("M259-B001-DOC-PKT-03", "- `M259-A002`"),
        SnippetCheck("M259-B001-DOC-PKT-04", "Next issue: `M259-B002`."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M259-B001-NDOC-01", "## Runnable core compatibility and migration guard (M259-B001)"),
        SnippetCheck("M259-B001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M259-B001-NDOC-03", "`O3S216`"),
        SnippetCheck("M259-B001-NDOC-04", "`M259-B002`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M259-B001-SPC-01", "## M259 runnable core compatibility guard (B001)"),
        SnippetCheck("M259-B001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M259-B001-SPC-03", f"`{GUARD_MODEL}`"),
        SnippetCheck("M259-B001-SPC-04", "`M259-B002`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M259-B001-META-01", "## M259 runnable core compatibility guard metadata anchors (B001)"),
        SnippetCheck("M259-B001-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M259-B001-META-03", "`tmp/reports/m259/M259-A002/canonical_runnable_sample_set_summary.json`"),
        SnippetCheck("M259-B001-META-04", "`tmp/reports/m259/M259-B001/runnable_core_compatibility_guard_summary.json`"),
    ),
    TOKEN_HEADER: (
        SnippetCheck("M259-B001-TOKEN-01", "M259-B001 runnable-core compatibility guard anchor"),
        SnippetCheck("M259-B001-TOKEN-02", "unsupported:strictness-selection"),
        SnippetCheck("M259-B001-TOKEN-03", "unsupported:strict-concurrency-selection"),
        SnippetCheck("M259-B001-TOKEN-04", "unsupported:throws"),
        SnippetCheck("M259-B001-TOKEN-05", "unsupported:async-await"),
        SnippetCheck("M259-B001-TOKEN-06", "unsupported:actors"),
        SnippetCheck("M259-B001-TOKEN-07", "unsupported:blocks"),
        SnippetCheck("M259-B001-TOKEN-08", "unsupported:arc"),
    ),
    SEMA_CONTRACT: (
        SnippetCheck("M259-B001-SEMA-01", "M259-B001 runnable-core compatibility guard anchor"),
        SnippetCheck("M259-B001-SEMA-02", "frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics"),
        SnippetCheck("M259-B001-SEMA-03", "live-compatibility-and-migration-selection-source-only-downgrade-unsupported-fail-closed"),
        SnippetCheck("M259-B001-SEMA-04", "source-only-claims-remain-recognized-but-never-promote-to-runnable"),
        SnippetCheck("M259-B001-SEMA-05", "strictness-strict-concurrency-and-feature-macro-claims-remain-fail-closed"),
    ),
    SEMA_PASS_MANAGER: (
        SnippetCheck("M259-B001-PASSMGR-01", "M259-B001 runnable-core compatibility guard anchor"),
        SnippetCheck("M259-B001-PASSMGR-02", "if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {"),
        SnippetCheck("M259-B001-PASSMGR-03", '"O3S216"'),
    ),
    SEMANTIC_PASSES: (
        SnippetCheck("M259-B001-SEMPASS-01", "M259-B001/M264-B001 semantic freeze anchor"),
        SnippetCheck("M259-B001-SEMPASS-02", "unsupported feature claim: '@autoreleasepool' is not yet runnable in Objective-C 3 native mode"),
        SnippetCheck("M259-B001-SEMPASS-03", "unsupported feature claim: block literals are not yet runnable in Objective-C 3 native mode"),
        SnippetCheck("M259-B001-SEMPASS-04", "unsupported feature claim: 'throws' is not yet runnable in Objective-C 3 native mode"),
        SnippetCheck("M259-B001-SEMPASS-05", "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode"),
    ),
    EXECUTION_SMOKE: (
        SnippetCheck("M259-B001-SMOKE-01", "M259-B001 runnable-core-compatibility-guard anchor:"),
        SnippetCheck("M259-B001-SMOKE-02", "advanced unsupported features must fail closed instead of counting as runnable smoke coverage"),
    ),
    EXECUTION_REPLAY: (
        SnippetCheck("M259-B001-REPLAY-01", "M259-B001 runnable-core-compatibility-guard anchor:"),
        SnippetCheck("M259-B001-REPLAY-02", "advanced unsupported features must fail closed instead of counting as replay-proof runnable coverage"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M259-B001-PKG-01", '"check:objc3c:m259-b001-runnable-core-compatibility-guard"'),
        SnippetCheck("M259-B001-PKG-02", '"test:tooling:m259-b001-runnable-core-compatibility-guard"'),
        SnippetCheck("M259-B001-PKG-03", '"check:objc3c:m259-b001-lane-b-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M259-B001-RUN-01", "M259-A002 + M259-B001"),
        SnippetCheck("M259-B001-RUN-02", "check_m259_b001_runnable_core_compatibility_guard.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M259-B001-TEST-01", "def test_checker_passes(tmp_path: Path) -> None:"),
        SnippetCheck("M259-B001-TEST-02", CONTRACT_ID),
    ),
}


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


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = read_text(path)
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def main(argv: Sequence[str]) -> int:
    del argv
    failures: list[Finding] = []
    checks_total = 0
    static_contracts: dict[str, dict[str, Any]] = {}

    for path, snippets in STATIC_SNIPPETS.items():
        total, findings = check_static_contract(path, snippets)
        checks_total += total
        static_contracts[display_path(path)] = {"checks": total, "ok": len(findings) == 0}
        failures.extend(findings)

    a002_summary = load_json(A002_SUMMARY)
    checks_total += 1
    if a002_summary.get("contract_id") != A002_CONTRACT_ID:
        failures.append(Finding(display_path(A002_SUMMARY), "M259-B001-A002-CONTRACT", "M259-A002 contract id drift"))
    checks_total += 1
    if a002_summary.get("ok") is not True:
        failures.append(Finding(display_path(A002_SUMMARY), "M259-B001-A002-OK", "M259-A002 summary must remain green"))

    checks_passed = checks_total - len(failures)
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "guard_model": GUARD_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "static_contracts": static_contracts,
        "dependency": {
            "M259-A002": {
                "summary": display_path(A002_SUMMARY),
                "contract_id": a002_summary.get("contract_id"),
                "ok": a002_summary.get("ok"),
            }
        },
        "guard_boundary": {
            "live_selection_surface": [
                "compatibility_mode",
                "migration_assist",
                "runnable_core_release_claims",
            ],
            "landed_fail_closed_diagnostics": [
                "O3S216",
                "unsupported feature claim: '@autoreleasepool' is not yet runnable in Objective-C 3 native mode",
                "unsupported feature claim: block literals are not yet runnable in Objective-C 3 native mode",
                "unsupported feature claim: 'throws' is not yet runnable in Objective-C 3 native mode",
                "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
            ],
            "non_runnable_advanced_claim_families": [
                "strictness-selection",
                "strict-concurrency-selection",
                "throws",
                "async-await",
                "actors",
                "blocks",
                "arc",
            ],
        },
    }

    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
