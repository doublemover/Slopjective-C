#!/usr/bin/env python3
"""Fail-closed contract checker for M262-C002 ARC automatic insertion."""

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
MODE = "m262-c002-automatic-retain-release-autorelease-insertion-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-arc-automatic-insertion/m262-c002-v1"
BOUNDARY_COMMENT_PREFIX = "; arc_automatic_insertions = contract=objc3c-arc-automatic-insertion/m262-c002-v1"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_automatic_retain_release_autorelease_insertion_core_feature_implementation_c002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation_packet.md"
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
DEFAULT_INFERENCE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_inference_lifetime_positive.objc3"
DEFAULT_AUTORELEASE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_autorelease_return_positive.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "c002-automatic-insertion"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m262/M262-C002/arc_automatic_insertion_summary.json")


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
    SnippetCheck("M262-C002-DOC-EXP-01", "# M262 Automatic Retain Release Autorelease Insertion Core Feature Implementation Expectations (C002)"),
    SnippetCheck("M262-C002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M262-C002-DOC-EXP-03", "`; arc_automatic_insertions = ...`"),
    SnippetCheck("M262-C002-DOC-EXP-04", "`!objc3.objc_arc_automatic_insertions = !{...}`"),
    SnippetCheck("M262-C002-DOC-EXP-05", "`tmp/reports/m262/M262-C002/arc_automatic_insertion_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M262-C002-DOC-PKT-01", "# M262-C002 Automatic Retain Release Autorelease Insertion Core Feature Implementation Packet"),
    SnippetCheck("M262-C002-DOC-PKT-02", "Packet: `M262-C002`"),
    SnippetCheck("M262-C002-DOC-PKT-03", "Issue: `#7200`"),
    SnippetCheck("M262-C002-DOC-PKT-04", "tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3"),
    SnippetCheck("M262-C002-DOC-PKT-05", "tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M262-C002-ARCH-01", "## M262 ARC Automatic Retain Release Autorelease Insertion (C002)"),
    SnippetCheck("M262-C002-ARCH-02", "lowering now consumes the semantic insertion flags and emits real helper calls"),
    SnippetCheck("M262-C002-ARCH-03", "`!objc3.objc_arc_automatic_insertions`"),
)
NATIVE_DOC_SOURCE_SNIPPETS = (
    SnippetCheck("M262-C002-NSRC-01", "## M262 ARC automatic retain release autorelease insertion (M262-C002)"),
    SnippetCheck("M262-C002-NSRC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C002-NSRC-03", "`objc3_runtime_autorelease_i32`"),
    SnippetCheck("M262-C002-NSRC-04", "`M262-C003` is the next issue."),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M262-C002-NDOC-01", "## M262 ARC automatic retain release autorelease insertion (M262-C002)"),
    SnippetCheck("M262-C002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C002-NDOC-03", "`!objc3.objc_arc_automatic_insertions`"),
    SnippetCheck("M262-C002-NDOC-04", "`M262-C003` is the next issue."),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M262-C002-SPC-01", "## M262 ARC automatic retain release autorelease insertion (C002)"),
    SnippetCheck("M262-C002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C002-SPC-03", "`M262-C003` is the next issue."),
)
SYNTAX_SPEC_SNIPPETS = (
    SnippetCheck("M262-C002-SYN-01", "### B.2.9 ARC automatic insertion boundary (implementation note) {#b-2-9}"),
    SnippetCheck("M262-C002-SYN-02", "Current implementation status (`M262-C002`):"),
    SnippetCheck("M262-C002-SYN-03", "lane C now consumes the ARC semantic insertion packets and emits real helper calls for the supported runnable slice"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M262-C002-LHDR-01", "kObjc3ArcAutomaticInsertionContractId"),
    SnippetCheck("M262-C002-LHDR-02", "kObjc3ArcAutomaticInsertionSourceModel"),
    SnippetCheck("M262-C002-LHDR-03", "kObjc3ArcAutomaticInsertionLoweringModel"),
    SnippetCheck("M262-C002-LHDR-04", "Objc3ArcAutomaticInsertionSummary()"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M262-C002-LCPP-01", "Objc3ArcAutomaticInsertionSummary()"),
    SnippetCheck("M262-C002-LCPP-02", "M262-C002 ARC automatic-insertion anchor"),
    SnippetCheck("M262-C002-LCPP-03", ";next_issue=M262-C003"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M262-C002-SEMA-01", "M262-C002 ARC automatic-insertion implementation anchor:"),
)
SEMA_PM_SNIPPETS = (
    SnippetCheck("M262-C002-PM-01", "M262-C002 ARC automatic-insertion implementation anchor:"),
)
SCAFFOLD_SNIPPETS = (
    SnippetCheck("M262-C002-SCAF-01", "M262-C002 ARC automatic-insertion implementation anchor:"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M262-C002-IR-01", 'out << "; arc_automatic_insertions = "'),
    SnippetCheck("M262-C002-IR-02", "M262-C002 ARC automatic-insertion implementation anchor:"),
    SnippetCheck("M262-C002-IR-03", "!objc3.objc_arc_automatic_insertions = !{!80}"),
    SnippetCheck("M262-C002-IR-04", "RegisterArcOwnedCleanupPtr"),
    SnippetCheck("M262-C002-IR-05", "EmitArcOwnedCleanupReleases"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M262-C002-PKG-01", '"check:objc3c:m262-c002-automatic-retain-release-autorelease-insertion-contract": "python scripts/check_m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation.py"'),
    SnippetCheck("M262-C002-PKG-02", '"test:tooling:m262-c002-automatic-retain-release-autorelease-insertion-contract": "python -m pytest tests/tooling/test_check_m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation.py -q"'),
    SnippetCheck("M262-C002-PKG-03", '"check:objc3c:m262-c002-lane-c-readiness": "python scripts/run_m262_c002_lane_c_readiness.py"'),
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
    parser.add_argument("--inference-fixture", type=Path, default=DEFAULT_INFERENCE_FIXTURE)
    parser.add_argument("--autorelease-fixture", type=Path, default=DEFAULT_AUTORELEASE_FIXTURE)
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


def extract_function_body(ir_text: str, name: str) -> str:
    pattern = rf"define i32 @{re.escape(name)}\([^)]*\) \{{\n(?P<body>.*?)\n\}}"
    match = re.search(pattern, ir_text, flags=re.DOTALL)
    return match.group("body") if match else ""


def run_probe(*, args: argparse.Namespace, fixture: Path, out_leaf: str, extra_args: Sequence[str] = ()) -> tuple[subprocess.CompletedProcess[str], Path, Path]:
    probe_dir = args.probe_root.resolve() / out_leaf
    command = [str(args.native_exe.resolve()), str(fixture.resolve()), "--out-dir", str(probe_dir), "--emit-prefix", "module", *extra_args]
    result = run_command(command, ROOT)
    return result, probe_dir / "module.ll", probe_dir / "module.obj"


def validate_arc_inference_positive(*, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    result, ir_path, obj_path = run_probe(args=args, fixture=args.inference_fixture, out_leaf="inference-positive", extra_args=("-fobjc-arc",))
    case: dict[str, Any] = {
        "case_id": "M262-C002-CASE-INFERENCE-ARC",
        "fixture": display_path(args.inference_fixture),
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }
    checks_total = checks_passed = 0
    checks_total += 1; checks_passed += require(result.returncode == 0, display_path(ir_path), "M262-C002-INF-STATUS", "ARC inference probe must exit 0", failures)
    checks_total += 1; checks_passed += require(ir_path.exists(), display_path(ir_path), "M262-C002-INF-IR", "ARC inference probe must emit module.ll", failures)
    checks_total += 1; checks_passed += require(obj_path.exists(), display_path(obj_path), "M262-C002-INF-OBJ", "ARC inference probe must emit module.obj", failures)
    if not ir_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    case["boundary_line"] = boundary_line
    checks_total += 1; checks_passed += require(bool(boundary_line), display_path(ir_path), "M262-C002-INF-BOUNDARY", "IR must publish the ARC automatic-insertion boundary line", failures)
    checks_total += 1; checks_passed += require("!objc3.objc_arc_automatic_insertions = !{" in ir_text, display_path(ir_path), "M262-C002-INF-METADATA", "IR must publish ARC automatic-insertion named metadata", failures)
    project_body = extract_function_body(ir_text, "project")
    method_body = extract_function_body(ir_text, "objc3_method_Box_instance_currentValueToken_")
    case["project_body"] = project_body
    case["method_body"] = method_body
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_retain_i32(i32 %arg0)" in project_body and "store i32 %arg0.retained.1, ptr %value.addr.0" in project_body, display_path(ir_path), "M262-C002-INF-PROJECT-ENTRY", "project must retain the ARC-owned parameter on entry before storing", failures)
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_retain_i32(i32 %t11)" in project_body, display_path(ir_path), "M262-C002-INF-PROJECT-RETURN-RETAIN", "project must retain the returned value before exit cleanup", failures)
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_release_i32(i32 %t13)" in project_body, display_path(ir_path), "M262-C002-INF-PROJECT-EXIT-RELEASE", "project must release tracked ARC-owned storage on exit", failures)
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_retain_i32(i32 %arg0)" in method_body and "store i32 %arg0.retained.1, ptr %value.addr.0" in method_body, display_path(ir_path), "M262-C002-INF-METHOD-ENTRY", "instance method must retain the ARC-owned parameter on entry before storing", failures)
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_retain_i32(i32 %t11)" in method_body, display_path(ir_path), "M262-C002-INF-METHOD-RETURN-RETAIN", "instance method must retain the returned value before exit cleanup", failures)
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_release_i32(i32 %t13)" in method_body, display_path(ir_path), "M262-C002-INF-METHOD-EXIT-RELEASE", "instance method must release tracked ARC-owned storage on exit", failures)
    checks_total += 1; checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M262-C002-INF-OBJ-SIZE", "ARC inference object output must be non-empty", failures)
    return checks_total, checks_passed, case


def validate_inference_baseline(*, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    result, ir_path, obj_path = run_probe(args=args, fixture=args.inference_fixture, out_leaf="inference-baseline")
    case: dict[str, Any] = {
        "case_id": "M262-C002-CASE-INFERENCE-NONARC",
        "fixture": display_path(args.inference_fixture),
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }
    checks_total = checks_passed = 0
    checks_total += 1; checks_passed += require(result.returncode == 0, display_path(ir_path), "M262-C002-BASE-STATUS", "non-ARC inference probe must exit 0", failures)
    checks_total += 1; checks_passed += require(ir_path.exists(), display_path(ir_path), "M262-C002-BASE-IR", "non-ARC inference probe must emit module.ll", failures)
    checks_total += 1; checks_passed += require(obj_path.exists(), display_path(obj_path), "M262-C002-BASE-OBJ", "non-ARC inference probe must emit module.obj", failures)
    if not ir_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    project_body = extract_function_body(ir_text, "project")
    case["project_body"] = project_body
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_retain_i32(i32 %arg0)" not in project_body, display_path(ir_path), "M262-C002-BASE-NO-ENTRY-RETAIN", "non-ARC project must not auto-retain the parameter on entry", failures)
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_retain_i32(i32 %t11)" not in project_body, display_path(ir_path), "M262-C002-BASE-NO-RETURN-RETAIN", "non-ARC project must not auto-retain the return value", failures)
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_release_i32(i32 %t13)" not in project_body, display_path(ir_path), "M262-C002-BASE-NO-EXIT-RELEASE", "non-ARC project must not auto-release tracked storage on exit", failures)
    checks_total += 1; checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M262-C002-BASE-OBJ-SIZE", "non-ARC inference object output must be non-empty", failures)
    return checks_total, checks_passed, case


def validate_autorelease_positive(*, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    result, ir_path, obj_path = run_probe(args=args, fixture=args.autorelease_fixture, out_leaf="autorelease-positive", extra_args=("-fobjc-arc",))
    case: dict[str, Any] = {
        "case_id": "M262-C002-CASE-AUTORELEASE-ARC",
        "fixture": display_path(args.autorelease_fixture),
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }
    checks_total = checks_passed = 0
    checks_total += 1; checks_passed += require(result.returncode == 0, display_path(ir_path), "M262-C002-AUTO-STATUS", "ARC autorelease probe must exit 0", failures)
    checks_total += 1; checks_passed += require(ir_path.exists(), display_path(ir_path), "M262-C002-AUTO-IR", "ARC autorelease probe must emit module.ll", failures)
    checks_total += 1; checks_passed += require(obj_path.exists(), display_path(obj_path), "M262-C002-AUTO-OBJ", "ARC autorelease probe must emit module.obj", failures)
    if not ir_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    case["boundary_line"] = boundary_line
    checks_total += 1; checks_passed += require(bool(boundary_line), display_path(ir_path), "M262-C002-AUTO-BOUNDARY", "autorelease IR must publish the ARC automatic-insertion boundary line", failures)
    checks_total += 1; checks_passed += require("!objc3.objc_arc_automatic_insertions = !{" in ir_text, display_path(ir_path), "M262-C002-AUTO-METADATA", "autorelease IR must publish ARC automatic-insertion named metadata", failures)
    bounce_body = extract_function_body(ir_text, "bounce")
    case["bounce_body"] = bounce_body
    checks_total += 1; checks_passed += require("call i32 @objc3_runtime_autorelease_i32(i32 %t1)" in bounce_body, display_path(ir_path), "M262-C002-AUTO-BOUNCE", "bounce must lower through objc3_runtime_autorelease_i32 under ARC", failures)
    checks_total += 1; checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), "M262-C002-AUTO-OBJ-SIZE", "ARC autorelease object output must be non-empty", failures)
    return checks_total, checks_passed, case


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
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
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    dynamic_probes_executed = False
    if not args.skip_dynamic_probes:
        dynamic_probes_executed = True
        for validator in (validate_arc_inference_positive, validate_inference_baseline, validate_autorelease_positive):
            total, passed, case = validator(args=args, failures=failures)
            checks_total += total
            checks_passed += passed
            dynamic_cases.append(case)

    payload = {
        "mode": MODE,
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "evidence_path": str(args.summary_out).replace('\\', '/'),
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
