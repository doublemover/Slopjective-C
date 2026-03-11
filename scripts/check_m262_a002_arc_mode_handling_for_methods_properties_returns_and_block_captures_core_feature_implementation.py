#!/usr/bin/env python3
"""Deterministic checker for M262-A002 ARC mode handling."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m262-a002-arc-mode-handling-for-methods-properties-returns-and-block-captures-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-arc-mode-handling/m262-a002-v1"
SOURCE_MODEL = "ownership-qualified-method-property-return-and-block-capture-surfaces-are-runnable-under-explicit-arc-mode"
MODE_MODEL = "driver-admits-fobjc-arc-and-fno-objc-arc-and-threads-arc-mode-through-frontend-sema-and-ir"
FAIL_CLOSED_MODEL = "non-arc-mode-still-rejects-executable-ownership-qualified-method-and-function-signatures"
NON_GOAL_MODEL = "no-generalized-arc-cleanup-synthesis-no-implicit-nonarc-promotion-no-full-arc-automation-yet"
NEXT_ISSUE = "M262-B001"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
CLI_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m262_a002_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
ARC_MODE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_mode_handling_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "a002-arc-mode-handling"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-A002" / "arc_mode_handling_summary.json"
BOUNDARY_PREFIX = "; arc_mode_handling = "
NAMED_METADATA_LINE = "!objc3.objc_arc_mode_handling = !{!76}"


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
        SnippetCheck("M262-A002-EXP-01", "# M262 ARC Mode Handling For Methods, Properties, Returns, And Block Captures Core Feature Implementation Expectations (A002)"),
        SnippetCheck("M262-A002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M262-A002-EXP-03", "compiles cleanly under `-fobjc-arc`"),
        SnippetCheck("M262-A002-EXP-04", "The contract must explicitly hand off to `M262-B001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M262-A002-PKT-01", "# M262-A002 ARC Mode Handling For Methods, Properties, Returns, And Block Captures Core Feature Implementation Packet"),
        SnippetCheck("M262-A002-PKT-02", "Issue: `#7195`"),
        SnippetCheck("M262-A002-PKT-03", "Packet: `M262-A002`"),
        SnippetCheck("M262-A002-PKT-04", "`M262-B001` is the explicit next handoff after this implementation lands."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M262-A002-SRC-01", "## M262 ARC mode handling for methods, properties, returns, and block captures (M262-A002)"),
        SnippetCheck("M262-A002-SRC-02", "the native driver now accepts `-fobjc-arc` and `-fno-objc-arc`"),
        SnippetCheck("M262-A002-SRC-03", "`!objc3.objc_arc_mode_handling`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M262-A002-NDOC-01", "## M262 ARC mode handling for methods, properties, returns, and block captures (M262-A002)"),
        SnippetCheck("M262-A002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-A002-NDOC-03", "`M262-B001` is the next issue."),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M262-A002-SPC-01", "## M262 ARC mode handling for methods, properties, returns, and block captures (A002)"),
        SnippetCheck("M262-A002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-A002-SPC-03", f"`{MODE_MODEL}`"),
        SnippetCheck("M262-A002-SPC-04", "`M262-B001` is the next issue."),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M262-A002-ATTR-01", "### B.2.4 Explicit ARC mode handling for executable ownership surfaces (implementation note) {#b-2-4}"),
        SnippetCheck("M262-A002-ATTR-02", "the native driver now accepts `-fobjc-arc` and `-fno-objc-arc`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M262-A002-ARCH-01", "## M262 ARC Mode Handling For Methods, Properties, Returns, And Block Captures (A002)"),
        SnippetCheck("M262-A002-ARCH-02", "the native driver now accepts `-fobjc-arc` and `-fno-objc-arc`"),
        SnippetCheck("M262-A002-ARCH-03", "the next issue is `M262-B001`"),
    ),
    CLI_CPP: (
        SnippetCheck("M262-A002-CLI-01", "[-fobjc-arc] [-fno-objc-arc]"),
        SnippetCheck("M262-A002-CLI-02", 'if (flag == "-fobjc-arc") {'),
        SnippetCheck("M262-A002-CLI-03", 'if (flag == "-fno-objc-arc") {'),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M262-A002-PM-01", "M262-A002 ARC mode-handling anchor"),
        SnippetCheck("M262-A002-PM-02", "ARC mode now widens executable ownership-qualified"),
    ),
    SEMA_CPP: (
        SnippetCheck("M262-A002-SEMA-01", "M262-A002 ARC mode-handling core implementation anchor"),
        SnippetCheck("M262-A002-SEMA-02", "allow_arc_mode_owned_bindings"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M262-A002-LHDR-01", "kObjc3ArcModeHandlingContractId"),
        SnippetCheck("M262-A002-LHDR-02", "std::string Objc3ArcModeHandlingSummary(bool arc_mode_enabled);"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M262-A002-LCPP-01", "std::string Objc3ArcModeHandlingSummary(bool arc_mode_enabled)"),
        SnippetCheck("M262-A002-LCPP-02", ";arc_mode="),
        SnippetCheck("M262-A002-LCPP-03", ";next_issue=M262-B001"),
    ),
    IR_CPP: (
        SnippetCheck("M262-A002-IR-01", "M262-A002 ARC mode-handling core implementation anchor"),
        SnippetCheck("M262-A002-IR-02", "; arc_mode_handling = "),
        SnippetCheck("M262-A002-IR-03", "objc_arc_mode_handling"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M262-A002-PKG-01", '"check:objc3c:m262-a002-arc-mode-handling-methods-properties-returns-block-captures-contract": "python scripts/check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py"'),
        SnippetCheck("M262-A002-PKG-02", '"test:tooling:m262-a002-arc-mode-handling-methods-properties-returns-block-captures-contract": "python -m pytest tests/tooling/test_check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py -q"'),
        SnippetCheck("M262-A002-PKG-03", '"check:objc3c:m262-a002-lane-a-readiness": "python scripts/run_m262_a002_lane_a_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M262-A002-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M262-A002-RUN-02", "build:objc3c-native"),
        SnippetCheck("M262-A002-RUN-03", "test_check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M262-A002-TEST-01", "def test_m262_a002_checker_emits_summary() -> None:"),
        SnippetCheck("M262-A002-TEST-02", CONTRACT_ID),
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


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def compile_fixture(fixture: Path, out_dir: Path, extra_args: Sequence[str] = ()) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(NATIVE_EXE), str(fixture), *extra_args, "--out-dir", str(out_dir), "--emit-prefix", "module"]
    return run_process(command)


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}

    enabled_out_dir = PROBE_ROOT / "arc-enabled-positive"
    disabled_out_dir = PROBE_ROOT / "arc-disabled-positive"
    explicit_nonarc_out_dir = PROBE_ROOT / "arc-explicit-disabled-hello"

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M262-A002-DYN-01", "native binary is missing", failures)
    checks_total += require(ARC_MODE_FIXTURE.exists(), display_path(ARC_MODE_FIXTURE), "M262-A002-DYN-02", "arc mode positive fixture is missing", failures)
    checks_total += require(HELLO_FIXTURE.exists(), display_path(HELLO_FIXTURE), "M262-A002-DYN-03", "hello fixture is missing", failures)
    if failures:
        return checks_total, payload

    enabled_completed = compile_fixture(ARC_MODE_FIXTURE, enabled_out_dir, ["-fobjc-arc"])
    enabled_ir = enabled_out_dir / "module.ll"
    enabled_diag = enabled_out_dir / "module.diagnostics.json"
    enabled_manifest = enabled_out_dir / "module.manifest.json"
    enabled_object = enabled_out_dir / "module.obj"
    checks_total += require(enabled_completed.returncode == 0, display_path(enabled_out_dir), "M262-A002-DYN-04", f"arc-enabled compile failed: {enabled_completed.stdout}{enabled_completed.stderr}", failures)
    checks_total += require(enabled_ir.exists(), display_path(enabled_ir), "M262-A002-DYN-05", "arc-enabled IR is missing", failures)
    checks_total += require(enabled_diag.exists(), display_path(enabled_diag), "M262-A002-DYN-06", "arc-enabled diagnostics are missing", failures)
    checks_total += require(enabled_manifest.exists(), display_path(enabled_manifest), "M262-A002-DYN-07", "arc-enabled manifest is missing", failures)
    checks_total += require(enabled_object.exists(), display_path(enabled_object), "M262-A002-DYN-08", "arc-enabled object is missing", failures)
    if enabled_completed.returncode == 0 and enabled_ir.exists() and enabled_diag.exists() and enabled_manifest.exists():
        enabled_diag_payload = load_json(enabled_diag)
        enabled_ir_text = enabled_ir.read_text(encoding="utf-8")
        enabled_manifest_payload = load_json(enabled_manifest)
        checks_total += require(enabled_diag_payload.get("diagnostics") == [], display_path(enabled_diag), "M262-A002-DYN-09", "arc-enabled fixture must stay diagnostics-clean", failures)
        checks_total += require(BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in enabled_ir_text, display_path(enabled_ir), "M262-A002-DYN-10", "arc-enabled IR is missing the arc-mode-handling line", failures)
        checks_total += require(NAMED_METADATA_LINE in enabled_ir_text, display_path(enabled_ir), "M262-A002-DYN-11", "arc-enabled IR is missing the arc-mode-handling named metadata", failures)
        checks_total += require(enabled_manifest_payload.get("frontend", {}).get("arc_mode") == "enabled", display_path(enabled_manifest), "M262-A002-DYN-12", f"expected frontend.arc_mode=enabled, observed {enabled_manifest_payload.get('frontend', {}).get('arc_mode')!r}", failures)
        payload["arc_enabled_case"] = {
            "out_dir": display_path(enabled_out_dir),
            "boundary_present": BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in enabled_ir_text,
            "named_metadata_present": NAMED_METADATA_LINE in enabled_ir_text,
            "frontend_arc_mode": enabled_manifest_payload.get("frontend", {}).get("arc_mode"),
        }

    disabled_completed = compile_fixture(ARC_MODE_FIXTURE, disabled_out_dir)
    disabled_diag = disabled_out_dir / "module.diagnostics.json"
    checks_total += require(disabled_completed.returncode != 0, display_path(disabled_out_dir), "M262-A002-DYN-13", "non-ARC compile must fail closed", failures)
    checks_total += require(disabled_diag.exists(), display_path(disabled_diag), "M262-A002-DYN-14", "non-ARC diagnostics are missing", failures)
    disabled_codes: list[str] = []
    if disabled_diag.exists():
        disabled_diag_payload = load_json(disabled_diag)
        diagnostics = disabled_diag_payload.get("diagnostics", [])
        if isinstance(diagnostics, list):
            disabled_codes = [str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)]
        checks_total += require("O3S221" in disabled_codes, display_path(disabled_diag), "M262-A002-DYN-15", f"expected O3S221 unsupported-feature rejection, observed {disabled_codes}", failures)
        checks_total += require(
            any("ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode" in str(diag.get("message", "")) for diag in diagnostics if isinstance(diag, dict)),
            display_path(disabled_diag),
            "M262-A002-DYN-16",
            "expected ARC ownership qualifier unsupported-feature diagnostic message",
            failures,
        )
        payload["non_arc_case"] = {
            "out_dir": display_path(disabled_out_dir),
            "diagnostic_codes": disabled_codes,
        }

    explicit_nonarc_completed = compile_fixture(HELLO_FIXTURE, explicit_nonarc_out_dir, ["-fno-objc-arc"])
    explicit_nonarc_ir = explicit_nonarc_out_dir / "module.ll"
    explicit_nonarc_diag = explicit_nonarc_out_dir / "module.diagnostics.json"
    explicit_nonarc_manifest = explicit_nonarc_out_dir / "module.manifest.json"
    checks_total += require(explicit_nonarc_completed.returncode == 0, display_path(explicit_nonarc_out_dir), "M262-A002-DYN-17", f"explicit -fno-objc-arc compile failed: {explicit_nonarc_completed.stdout}{explicit_nonarc_completed.stderr}", failures)
    checks_total += require(explicit_nonarc_ir.exists(), display_path(explicit_nonarc_ir), "M262-A002-DYN-18", "explicit -fno-objc-arc IR is missing", failures)
    checks_total += require(explicit_nonarc_diag.exists(), display_path(explicit_nonarc_diag), "M262-A002-DYN-19", "explicit -fno-objc-arc diagnostics are missing", failures)
    checks_total += require(explicit_nonarc_manifest.exists(), display_path(explicit_nonarc_manifest), "M262-A002-DYN-20", "explicit -fno-objc-arc manifest is missing", failures)
    if explicit_nonarc_completed.returncode == 0 and explicit_nonarc_ir.exists() and explicit_nonarc_diag.exists() and explicit_nonarc_manifest.exists():
        explicit_nonarc_diag_payload = load_json(explicit_nonarc_diag)
        explicit_nonarc_ir_text = explicit_nonarc_ir.read_text(encoding="utf-8")
        explicit_nonarc_manifest_payload = load_json(explicit_nonarc_manifest)
        checks_total += require(explicit_nonarc_diag_payload.get("diagnostics") == [], display_path(explicit_nonarc_diag), "M262-A002-DYN-21", "explicit -fno-objc-arc hello fixture must stay diagnostics-clean", failures)
        checks_total += require(BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in explicit_nonarc_ir_text, display_path(explicit_nonarc_ir), "M262-A002-DYN-22", "explicit -fno-objc-arc IR is missing the arc-mode-handling line", failures)
        checks_total += require(NAMED_METADATA_LINE in explicit_nonarc_ir_text, display_path(explicit_nonarc_ir), "M262-A002-DYN-23", "explicit -fno-objc-arc IR is missing the arc-mode-handling named metadata", failures)
        checks_total += require(explicit_nonarc_manifest_payload.get("frontend", {}).get("arc_mode") == "disabled", display_path(explicit_nonarc_manifest), "M262-A002-DYN-24", f"expected frontend.arc_mode=disabled, observed {explicit_nonarc_manifest_payload.get('frontend', {}).get('arc_mode')!r}", failures)
        payload["explicit_non_arc_case"] = {
            "out_dir": display_path(explicit_nonarc_out_dir),
            "boundary_present": BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in explicit_nonarc_ir_text,
            "named_metadata_present": NAMED_METADATA_LINE in explicit_nonarc_ir_text,
            "frontend_arc_mode": explicit_nonarc_manifest_payload.get("frontend", {}).get("arc_mode"),
        }

    return checks_total, payload


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0

    for path, snippets in STATIC_SNIPPETS.items():
        static_total, static_failures = check_static_contract(path, snippets)
        checks_total += static_total
        failures.extend(static_failures)

    if args.skip_dynamic_probes:
        dynamic_payload: dict[str, Any] = {"skipped": True}
    else:
        dynamic_total, dynamic_payload = run_dynamic_probes(failures)
        checks_total += dynamic_total

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "source_model": SOURCE_MODEL,
        "mode_model": MODE_MODEL,
        "non_goal_model": NON_GOAL_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_case": dynamic_payload,
        "findings": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M262-A002 arc mode handling validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
