#!/usr/bin/env python3
"""Fail-closed checker for M262-A001 ARC source surface and mode boundary."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m262-a001-arc-source-surface-mode-boundary-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-arc-source-mode-boundary-freeze/m262-a001-v1"
SOURCE_MODEL = (
    "ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode"
)
MODE_MODEL = (
    "native-driver-rejects-fobjc-arc-while-executable-ownership-qualified-functions-and-methods-stay-fail-closed"
)
FAIL_CLOSED_MODEL = "fail-closed-on-arc-source-mode-boundary-drift-before-arc-automation"
NON_GOAL_MODEL = (
    "no-fobjc-arc-cli-mode-no-fno-objc-arc-cli-mode-no-automatic-arc-cleanup-insertion-no-user-visible-arc-runtime-mode-split"
)
NEXT_ISSUE = "M262-A002"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SCAFFOLD_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_ownership_aware_lowering_behavior_scaffold.h"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m262_a001_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
OWNERSHIP_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_runtime_backed_object_ownership_attribute_surface_positive.objc3"
ARC_QUALIFIER_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m259_b002_unsupported_feature_claim_arc_ownership_qualifier.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "a001-arc-source-mode-boundary"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m262" / "M262-A001" / "arc_source_mode_boundary_summary.json"
BOUNDARY_PREFIX = "; arc_source_mode_boundary = "
NAMED_METADATA_LINE = "!objc3.objc_arc_source_mode_boundary = !{!75}"
FORWARD_COMPAT_ARC_BOUNDARY_PREFIX = "; arc_mode_handling = contract=objc3c-arc-mode-handling/m262-a002-v1"


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
        SnippetCheck("M262-A001-EXP-01", "# M262 ARC Source Surface And Mode Boundary Contract And Architecture Freeze Expectations (A001)"),
        SnippetCheck("M262-A001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M262-A001-EXP-03", "still fails closed with the driver `unknown arg`"),
        SnippetCheck("M262-A001-EXP-04", "The contract must explicitly hand off to `M262-A002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M262-A001-PKT-01", "# M262-A001 ARC Source Surface And Mode Boundary Contract And Architecture Freeze Packet"),
        SnippetCheck("M262-A001-PKT-02", "Issue: `#7194`"),
        SnippetCheck("M262-A001-PKT-03", "Packet: `M262-A001`"),
        SnippetCheck("M262-A001-PKT-04", "`M262-A002` is the explicit next handoff after this freeze closes."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M262-A001-SRC-01", "## M262 ARC source surface and mode boundary (M262-A001)"),
        SnippetCheck("M262-A001-SRC-02", "the native driver still rejects `-fobjc-arc`"),
        SnippetCheck("M262-A001-SRC-03", "`!objc3.objc_arc_source_mode_boundary`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M262-A001-NDOC-01", "## M262 ARC source surface and mode boundary (M262-A001)"),
        SnippetCheck("M262-A001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-A001-NDOC-03", "`M262-A002` is the next issue."),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M262-A001-SPC-01", "## M262 ARC source surface and mode boundary (A001)"),
        SnippetCheck("M262-A001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M262-A001-SPC-03", f"`{MODE_MODEL}`"),
        SnippetCheck("M262-A001-SPC-04", "`M262-A002` is the next issue."),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M262-A001-ATTR-01", "### B.2.3 ARC source-surface and current mode boundary (implementation note) {#b-2-3}"),
        SnippetCheck("M262-A001-ATTR-02", "the native driver does not yet expose a runnable `-fobjc-arc` mode"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M262-A001-ARCH-01", "## M262 ARC Source Surface And Mode Boundary (A001)"),
        SnippetCheck("M262-A001-ARCH-02", "the native driver still rejects `-fobjc-arc`"),
        SnippetCheck("M262-A001-ARCH-03", "the next issue is `M262-A002`"),
    ),
    SEMA_CPP: (
        SnippetCheck("M262-A001-SEMA-01", "ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode"),
        SnippetCheck("M262-A001-SEMA-02", "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode"),
    ),
    SEMA_PASS_MANAGER_CPP: (
        SnippetCheck("M262-A001-PM-01", "weak/unowned summaries, ARC fix-it summaries"),
        SnippetCheck("M262-A001-PM-02", "ARC mode now widens executable ownership-qualified"),
    ),
    SCAFFOLD_HEADER: (
        SnippetCheck("M262-A001-SCAF-01", "source-side inventory for ownership qualifiers, weak/unowned semantics"),
        SnippetCheck("M262-A001-SCAF-02", "ownership qualifiers, weak/unowned"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M262-A001-LHDR-01", "kObjc3ArcSourceModeBoundaryContractId"),
        SnippetCheck("M262-A001-LHDR-02", "std::string Objc3ArcSourceModeBoundarySummary();"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M262-A001-LCPP-01", "std::string Objc3ArcSourceModeBoundarySummary()"),
        SnippetCheck("M262-A001-LCPP-02", "rejects `-fobjc-arc`"),
        SnippetCheck("M262-A001-LCPP-03", ";next_issue=M262-A002"),
    ),
    IR_CPP: (
        SnippetCheck("M262-A001-IR-01", "M262-A001 ARC source-surface/mode-boundary anchor"),
        SnippetCheck("M262-A001-IR-02", "; arc_source_mode_boundary = "),
        SnippetCheck("M262-A001-IR-03", "objc_arc_source_mode_boundary"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M262-A001-PKG-01", '"check:objc3c:m262-a001-arc-source-surface-mode-boundary-contract": "python scripts/check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py"'),
        SnippetCheck("M262-A001-PKG-02", '"test:tooling:m262-a001-arc-source-surface-mode-boundary-contract": "python -m pytest tests/tooling/test_check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M262-A001-PKG-03", '"check:objc3c:m262-a001-lane-a-readiness": "python scripts/run_m262_a001_lane_a_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M262-A001-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M262-A001-RUN-02", "build:objc3c-native"),
        SnippetCheck("M262-A001-RUN-03", "test_check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M262-A001-TEST-01", "def test_m262_a001_checker_emits_summary() -> None:"),
        SnippetCheck("M262-A001-TEST-02", CONTRACT_ID),
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

    hello_out_dir = PROBE_ROOT / "hello"
    ownership_out_dir = PROBE_ROOT / "runtime-backed-ownership"
    qualifier_out_dir = PROBE_ROOT / "arc-qualifier-negative"
    mode_out_dir = PROBE_ROOT / "arc-mode-flag"

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M262-A001-DYN-01", "native binary is missing", failures)
    checks_total += require(HELLO_FIXTURE.exists(), display_path(HELLO_FIXTURE), "M262-A001-DYN-02", "hello fixture is missing", failures)
    checks_total += require(OWNERSHIP_FIXTURE.exists(), display_path(OWNERSHIP_FIXTURE), "M262-A001-DYN-03", "ownership baseline fixture is missing", failures)
    checks_total += require(ARC_QUALIFIER_NEGATIVE_FIXTURE.exists(), display_path(ARC_QUALIFIER_NEGATIVE_FIXTURE), "M262-A001-DYN-04", "arc qualifier negative fixture is missing", failures)
    if failures:
        return checks_total, payload

    hello_completed = compile_fixture(HELLO_FIXTURE, hello_out_dir)
    hello_ir = hello_out_dir / "module.ll"
    hello_diag = hello_out_dir / "module.diagnostics.json"
    checks_total += require(hello_completed.returncode == 0, display_path(hello_out_dir), "M262-A001-DYN-05", f"hello compile failed: {hello_completed.stdout}{hello_completed.stderr}", failures)
    checks_total += require(hello_ir.exists(), display_path(hello_ir), "M262-A001-DYN-06", "hello IR is missing", failures)
    checks_total += require(hello_diag.exists(), display_path(hello_diag), "M262-A001-DYN-07", "hello diagnostics are missing", failures)
    if hello_completed.returncode == 0 and hello_ir.exists() and hello_diag.exists():
        hello_diag_payload = load_json(hello_diag)
        hello_ir_text = hello_ir.read_text(encoding="utf-8")
        checks_total += require(hello_diag_payload.get("diagnostics") == [], display_path(hello_diag), "M262-A001-DYN-08", "hello fixture must stay diagnostics-clean", failures)
        checks_total += require(BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in hello_ir_text, display_path(hello_ir), "M262-A001-DYN-09", "hello IR is missing the arc source/mode boundary line", failures)
        checks_total += require(NAMED_METADATA_LINE in hello_ir_text, display_path(hello_ir), "M262-A001-DYN-10", "hello IR is missing the arc source/mode named metadata", failures)
        payload["hello_case"] = {
            "out_dir": display_path(hello_out_dir),
            "boundary_present": BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in hello_ir_text,
            "named_metadata_present": NAMED_METADATA_LINE in hello_ir_text,
        }

    ownership_completed = compile_fixture(OWNERSHIP_FIXTURE, ownership_out_dir)
    ownership_ir = ownership_out_dir / "module.ll"
    ownership_diag = ownership_out_dir / "module.diagnostics.json"
    checks_total += require(ownership_completed.returncode == 0, display_path(ownership_out_dir), "M262-A001-DYN-11", f"ownership baseline fixture compile failed: {ownership_completed.stdout}{ownership_completed.stderr}", failures)
    checks_total += require(ownership_ir.exists(), display_path(ownership_ir), "M262-A001-DYN-12", "ownership baseline IR is missing", failures)
    checks_total += require(ownership_diag.exists(), display_path(ownership_diag), "M262-A001-DYN-13", "ownership baseline diagnostics are missing", failures)
    if ownership_completed.returncode == 0 and ownership_ir.exists() and ownership_diag.exists():
        ownership_diag_payload = load_json(ownership_diag)
        ownership_ir_text = ownership_ir.read_text(encoding="utf-8")
        checks_total += require(ownership_diag_payload.get("diagnostics") == [], display_path(ownership_diag), "M262-A001-DYN-14", "ownership baseline fixture must stay diagnostics-clean", failures)
        checks_total += require(BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in ownership_ir_text, display_path(ownership_ir), "M262-A001-DYN-15", "ownership baseline IR is missing the arc source/mode boundary line", failures)
        payload["ownership_baseline_case"] = {
            "out_dir": display_path(ownership_out_dir),
            "boundary_present": BOUNDARY_PREFIX + f"contract={CONTRACT_ID}" in ownership_ir_text,
        }

    qualifier_completed = compile_fixture(ARC_QUALIFIER_NEGATIVE_FIXTURE, qualifier_out_dir)
    qualifier_diag = qualifier_out_dir / "module.diagnostics.json"
    checks_total += require(qualifier_completed.returncode != 0, display_path(qualifier_out_dir), "M262-A001-DYN-16", "arc qualifier negative fixture must still fail closed", failures)
    checks_total += require(qualifier_diag.exists(), display_path(qualifier_diag), "M262-A001-DYN-17", "arc qualifier diagnostics are missing", failures)
    qualifier_codes: list[str] = []
    if qualifier_diag.exists():
        qualifier_diag_payload = load_json(qualifier_diag)
        diagnostics = qualifier_diag_payload.get("diagnostics", [])
        if isinstance(diagnostics, list):
            qualifier_codes = [str(diag.get("code", "")) for diag in diagnostics if isinstance(diag, dict)]
        checks_total += require("O3S221" in qualifier_codes, display_path(qualifier_diag), "M262-A001-DYN-18", f"expected O3S221 unsupported-feature rejection, observed {qualifier_codes}", failures)
        checks_total += require(
            any("ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode" in str(diag.get("message", "")) for diag in diagnostics if isinstance(diag, dict)),
            display_path(qualifier_diag),
            "M262-A001-DYN-19",
            "expected ARC ownership qualifier unsupported-feature diagnostic message",
            failures,
        )
        payload["arc_qualifier_negative_case"] = {
            "out_dir": display_path(qualifier_out_dir),
            "diagnostic_codes": qualifier_codes,
        }

    mode_out_dir.mkdir(parents=True, exist_ok=True)
    mode_completed = run_process(
        [
            str(NATIVE_EXE),
            str(HELLO_FIXTURE),
            "-fobjc-arc",
            "--out-dir",
            str(mode_out_dir),
            "--emit-prefix",
            "module",
        ]
    )
    mode_output = (mode_completed.stdout or "") + (mode_completed.stderr or "")
    mode_ir = mode_out_dir / "module.ll"
    mode_manifest = mode_out_dir / "module.manifest.json"
    if mode_completed.returncode != 0:
        checks_total += require("unknown arg: -fobjc-arc" in mode_output, display_path(mode_out_dir), "M262-A001-DYN-20", f"expected unknown-arg rejection for -fobjc-arc, observed: {mode_output.strip()}", failures)
        payload["arc_mode_flag_case"] = {
            "out_dir": display_path(mode_out_dir),
            "returncode": mode_completed.returncode,
            "forward_compatible": False,
            "output": mode_output.strip(),
        }
    else:
        checks_total += require(mode_ir.exists(), display_path(mode_ir), "M262-A001-DYN-20", "forward-compatible -fobjc-arc compile is missing IR output", failures)
        checks_total += require(mode_manifest.exists(), display_path(mode_manifest), "M262-A001-DYN-21", "forward-compatible -fobjc-arc compile is missing manifest output", failures)
        mode_ir_text = mode_ir.read_text(encoding="utf-8") if mode_ir.exists() else ""
        mode_manifest_payload = load_json(mode_manifest) if mode_manifest.exists() else {}
        checks_total += require(FORWARD_COMPAT_ARC_BOUNDARY_PREFIX in mode_ir_text, display_path(mode_ir), "M262-A001-DYN-22", "forward-compatible -fobjc-arc compile is missing the M262-A002 arc-mode-handling boundary", failures)
        checks_total += require(mode_manifest_payload.get("frontend", {}).get("arc_mode") == "enabled", display_path(mode_manifest), "M262-A001-DYN-23", f"expected forward-compatible frontend.arc_mode=enabled, observed {mode_manifest_payload.get('frontend', {}).get('arc_mode')!r}", failures)
        payload["arc_mode_flag_case"] = {
            "out_dir": display_path(mode_out_dir),
            "returncode": mode_completed.returncode,
            "forward_compatible": True,
            "frontend_arc_mode": mode_manifest_payload.get("frontend", {}).get("arc_mode"),
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
    print(f"[ok] M262-A001 arc source/mode boundary validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
