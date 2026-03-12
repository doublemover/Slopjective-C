#!/usr/bin/env python3
"""Fail-closed contract checker for M262-C004 ARC/block autorelease-return lowering."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_ID = "objc3c-arc-block-autorelease-return-lowering/m262-c004-v1"
MODE = "m262-c004-arc-and-block-interaction-lowering-with-autorelease-return-conventions-core-feature-expansion-v1"
BOUNDARY_COMMENT_PREFIX = "; arc_block_autorelease_return_lowering = contract=objc3c-arc-block-autorelease-return-lowering/m262-c004-v1"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_arc_and_block_interaction_lowering_with_autorelease_return_conventions_core_feature_expansion_c004_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_c004_arc_and_block_interaction_lowering_with_autorelease_return_conventions_core_feature_expansion_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_NATIVE_DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_SYNTAX_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
DEFAULT_SCAFFOLD_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_ownership_aware_lowering_behavior_scaffold.h"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_AUTORELEASE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_autorelease_return_positive.objc3"
DEFAULT_BLOCK_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_block_autorelease_return_positive.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "c004-arc-block-autorelease-return"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m262/M262-C004/arc_block_autorelease_return_lowering_summary.json")


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M262-C004-DOC-EXP-01", "# M262 ARC And Block-Interaction Lowering With Autorelease-Return Conventions Core Feature Expansion Expectations (C004)"),
    SnippetCheck("M262-C004-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M262-C004-DOC-EXP-03", "`; arc_block_autorelease_return_lowering = ...`"),
    SnippetCheck("M262-C004-DOC-EXP-04", "`!objc3.objc_arc_block_autorelease_return_lowering = !{...}`"),
    SnippetCheck("M262-C004-DOC-EXP-05", "`tmp/reports/m262/M262-C004/arc_block_autorelease_return_lowering_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M262-C004-DOC-PKT-01", "# M262-C004 ARC And Block-Interaction Lowering With Autorelease-Return Conventions Core Feature Expansion Packet"),
    SnippetCheck("M262-C004-DOC-PKT-02", "Packet: `M262-C004`"),
    SnippetCheck("M262-C004-DOC-PKT-03", "Issue: `#7202`"),
    SnippetCheck("M262-C004-DOC-PKT-04", "tests/tooling/fixtures/native/m262_arc_block_autorelease_return_positive.objc3"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M262-C004-ARCH-01", "## M262 ARC And Block-Interaction Lowering With Autorelease-Return Conventions (C004)"),
    SnippetCheck("M262-C004-ARCH-02", "terminal branch cleanup that preserves sibling-branch ARC cleanup state"),
    SnippetCheck("M262-C004-ARCH-03", "`!objc3.objc_arc_block_autorelease_return_lowering`"),
)
NATIVE_DOC_SOURCE_SNIPPETS = (
    SnippetCheck("M262-C004-NSRC-01", "## M262 ARC and block-interaction lowering with autorelease-return conventions (M262-C004)"),
    SnippetCheck("M262-C004-NSRC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C004-NSRC-03", "`objc3_runtime_promote_block_i32`"),
    SnippetCheck("M262-C004-NSRC-04", "`M262-D001` is the next issue."),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M262-C004-NDOC-01", "## M262 ARC and block-interaction lowering with autorelease-return conventions (M262-C004)"),
    SnippetCheck("M262-C004-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C004-NDOC-03", "`!objc3.objc_arc_block_autorelease_return_lowering`"),
    SnippetCheck("M262-C004-NDOC-04", "`M262-D001` is the next issue."),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M262-C004-SPC-01", "## M262 ARC and block-interaction lowering with autorelease-return conventions (C004)"),
    SnippetCheck("M262-C004-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C004-SPC-03", "`M262-D001` is the next issue."),
)
SYNTAX_SPEC_SNIPPETS = (
    SnippetCheck("M262-C004-SYN-01", "`M262-C004` now covers the supported escaping-block plus autoreleasing-return"),
    SnippetCheck("M262-C004-SYN-02", "branch-local cleanup and ARC return conventions must compose without manual ownership management"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M262-C004-LHDR-01", "kObjc3ArcBlockAutoreleaseReturnLoweringContractId"),
    SnippetCheck("M262-C004-LHDR-02", "kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel"),
    SnippetCheck("M262-C004-LHDR-03", "kObjc3ArcBlockAutoreleaseReturnLoweringModel"),
    SnippetCheck("M262-C004-LHDR-04", "Objc3ArcBlockAutoreleaseReturnLoweringSummary()"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M262-C004-LCPP-01", "Objc3ArcBlockAutoreleaseReturnLoweringSummary()"),
    SnippetCheck("M262-C004-LCPP-02", "M262-C004 ARC/block autorelease-return lowering anchor"),
    SnippetCheck("M262-C004-LCPP-03", ";next_issue=M262-D001"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M262-C004-SEMA-01", "M262-C004 ARC/block autorelease-return lowering anchor:"),
)
SEMA_PM_SNIPPETS = (
    SnippetCheck("M262-C004-PM-01", "M262-C004 ARC/block autorelease-return implementation anchor:"),
)
SCAFFOLD_SNIPPETS = (
    SnippetCheck("M262-C004-SCAF-01", "M262-C004 ARC/block autorelease-return implementation anchor:"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M262-C004-IR-01", 'out << "; arc_block_autorelease_return_lowering = "'),
    SnippetCheck("M262-C004-IR-02", "M262-C004 ARC/block autorelease-return implementation anchor:"),
    SnippetCheck("M262-C004-IR-03", "!objc3.objc_arc_block_autorelease_return_lowering = !{!82}"),
    SnippetCheck("M262-C004-IR-04", "EmitPendingBlockDisposeTerminalCleanupToDepth"),
    SnippetCheck("M262-C004-IR-05", "EmitArcOwnedTerminalCleanupToDepth"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M262-C004-PKG-01", '"check:objc3c:m262-c004-arc-block-autorelease-return-contract": "python scripts/check_m262_c004_arc_and_block_interaction_lowering_with_autorelease_return_conventions_core_feature_expansion.py"'),
    SnippetCheck("M262-C004-PKG-02", '"test:tooling:m262-c004-arc-block-autorelease-return-contract": "python -m pytest tests/tooling/test_check_m262_c004_arc_and_block_interaction_lowering_with_autorelease_return_conventions_core_feature_expansion.py -q"'),
    SnippetCheck("M262-C004-PKG-03", '"check:objc3c:m262-c004-lane-c-readiness": "python scripts/run_m262_c004_lane_c_readiness.py"'),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--native-doc-source", type=Path, default=DEFAULT_NATIVE_DOC_SOURCE)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--syntax-spec", type=Path, default=DEFAULT_SYNTAX_SPEC)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--sema-pass-manager", type=Path, default=DEFAULT_SEMA_PASS_MANAGER)
    parser.add_argument("--scaffold-header", type=Path, default=DEFAULT_SCAFFOLD_HEADER)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--autorelease-fixture", type=Path, default=DEFAULT_AUTORELEASE_FIXTURE)
    parser.add_argument("--block-fixture", type=Path, default=DEFAULT_BLOCK_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def extract_function_body(ir_text: str, name: str, return_type: str) -> str:
    pattern = rf"define {re.escape(return_type)} @{re.escape(name)}\([^)]*\) \{{\n(?P<body>.*?)\n\}}"
    match = re.search(pattern, ir_text, flags=re.DOTALL)
    return match.group("body") if match else ""


def run_probe(*, args: argparse.Namespace, fixture: Path, out_leaf: str) -> tuple[subprocess.CompletedProcess[str], Path, Path]:
    probe_dir = args.probe_root.resolve() / out_leaf
    command = [
        str(args.native_exe.resolve()),
        str(fixture.resolve()),
        "--out-dir",
        str(probe_dir),
        "--emit-prefix",
        "module",
        "-fobjc-arc",
    ]
    result = run_command(command, ROOT)
    return result, probe_dir / "module.ll", probe_dir / "module.obj"


def require_boundary(ir_text: str, ir_path: Path, failures: list[Finding], prefix_check_id: str) -> tuple[int, int, str]:
    checks_total = checks_passed = 0
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    checks_total += 1
    checks_passed += require(bool(boundary_line), display_path(ir_path), f"{prefix_check_id}-BOUNDARY", "IR must publish the ARC/block autorelease-return boundary line", failures)
    checks_total += 1
    checks_passed += require("!objc3.objc_arc_block_autorelease_return_lowering = !{" in ir_text, display_path(ir_path), f"{prefix_check_id}-METADATA", "IR must publish ARC/block autorelease-return named metadata", failures)
    return checks_total, checks_passed, boundary_line


def validate_autorelease_case(*, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    result, ir_path, obj_path = run_probe(args=args, fixture=args.autorelease_fixture, out_leaf="autorelease-return")
    case: dict[str, Any] = {
        "case_id": "M262-C004-CASE-AUTORELEASE",
        "fixture": display_path(args.autorelease_fixture),
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }
    checks_total = checks_passed = 0
    checks_total += 1; checks_passed += require(result.returncode == 0, display_path(ir_path), "M262-C004-AUTO-STATUS", "autorelease-return probe must exit 0", failures)
    checks_total += 1; checks_passed += require(ir_path.exists(), display_path(ir_path), "M262-C004-AUTO-IR", "autorelease-return probe must emit module.ll", failures)
    checks_total += 1; checks_passed += require(obj_path.exists(), display_path(obj_path), "M262-C004-AUTO-OBJ", "autorelease-return probe must emit module.obj", failures)
    if not ir_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    extra_total, extra_passed, boundary_line = require_boundary(ir_text, ir_path, failures, "M262-C004-AUTO")
    checks_total += extra_total
    checks_passed += extra_passed
    case["boundary_line"] = boundary_line
    bounce_body = extract_function_body(ir_text, "bounce", "i32")
    case["bounce_body"] = bounce_body
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_autorelease_i32" in bounce_body, display_path(ir_path), "M262-C004-AUTO-AUTORELEASE", "bounce must lower autorelease on return", failures)
    checks_total += 1; checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M262-C004-AUTO-OBJ-SIZE", "autorelease-return object output must be non-empty", failures)
    return checks_total, checks_passed, case


def validate_block_case(*, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    result, ir_path, obj_path = run_probe(args=args, fixture=args.block_fixture, out_leaf="block-autorelease-return")
    case: dict[str, Any] = {
        "case_id": "M262-C004-CASE-BLOCK",
        "fixture": display_path(args.block_fixture),
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }
    checks_total = checks_passed = 0
    checks_total += 1; checks_passed += require(result.returncode == 0, display_path(ir_path), "M262-C004-BLOCK-STATUS", "block autorelease-return probe must exit 0", failures)
    checks_total += 1; checks_passed += require(ir_path.exists(), display_path(ir_path), "M262-C004-BLOCK-IR", "block autorelease-return probe must emit module.ll", failures)
    checks_total += 1; checks_passed += require(obj_path.exists(), display_path(obj_path), "M262-C004-BLOCK-OBJ", "block autorelease-return probe must emit module.obj", failures)
    if not ir_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    extra_total, extra_passed, boundary_line = require_boundary(ir_text, ir_path, failures, "M262-C004-BLOCK")
    checks_total += extra_total
    checks_passed += extra_passed
    case["boundary_line"] = boundary_line
    make_body = extract_function_body(ir_text, "make", "i32")
    case["make_body"] = make_body
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_promote_block_i32" in make_body, display_path(ir_path), "M262-C004-BLOCK-PROMOTE", "make must lower escaping block promotion through the runtime hook", failures)
    checks_total += 1; checks_passed += require(make_body.count("call i32 @objc3_runtime_autorelease_i32") >= 2, display_path(ir_path), "M262-C004-BLOCK-AUTORELEASE-COUNT", "make must autorelease on both return paths", failures)
    checks_total += 1; checks_passed += require(make_body.count("call void @__objc3_block_dispose_helper_") >= 2, display_path(ir_path), "M262-C004-BLOCK-DISPOSE-COUNT", "make must dispose the block helper on both return paths", failures)
    checks_total += 1; checks_passed += require(make_body.count("call i32 @objc3_runtime_release_i32") >= 2, display_path(ir_path), "M262-C004-BLOCK-RELEASE-COUNT", "make must release tracked ARC-owned storage on both return paths", failures)
    checks_total += 1; checks_passed += require(bool(re.search(r"call i32 @objc3_runtime_autorelease_i32\(i32 %t\d+\)\n\s+call void @__objc3_block_dispose_helper_[^(]+\(ptr %block\.literal\.addr\.\d+\)\n\s+%t\d+ = load i32, ptr %value\.addr\.0, align 4\n\s+%t\d+ = call i32 @objc3_runtime_release_i32\(i32 %t\d+\)", make_body)), display_path(ir_path), "M262-C004-BLOCK-BRANCH-CLEANUP-ORDER", "block/autorelease branch path must keep autorelease, dispose, and release in order", failures)
    checks_total += 1; checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M262-C004-BLOCK-OBJ-SIZE", "block autorelease-return object output must be non-empty", failures)
    return checks_total, checks_passed, case


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = checks_passed = 0

    static_targets = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc_source, NATIVE_DOC_SOURCE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.syntax_spec, SYNTAX_SPEC_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.sema_pass_manager, SEMA_PM_SNIPPETS),
        (args.scaffold_header, SCAFFOLD_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    if not args.skip_dynamic_probes:
        for validator in (validate_autorelease_case, validate_block_case):
            case_total, case_passed, case_payload = validator(args=args, failures=failures)
            checks_total += case_total
            checks_passed += case_passed
            dynamic_cases.append(case_payload)

    summary = {
        "ok": not failures,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(summary_path)}", file=sys.stderr)
        return 1
    print(f"[ok] M262-C004 ARC/block autorelease-return contract satisfied")
    print(f"[info] wrote summary to {display_path(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
