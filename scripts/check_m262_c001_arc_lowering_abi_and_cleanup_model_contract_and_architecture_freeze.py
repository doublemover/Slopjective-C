#!/usr/bin/env python3
"""Fail-closed contract checker for M262-C001 ARC lowering ABI/cleanup freeze."""

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
MODE = "m262-c001-arc-lowering-abi-and-cleanup-model-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1"
BOUNDARY_COMMENT_PREFIX = "; arc_lowering_abi_cleanup_model = contract=objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m262_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze_c001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m262" / "m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
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
DEFAULT_PROPERTY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m262_arc_property_interaction_positive.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m262" / "c001-arc-lowering-abi-cleanup"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m262/M262-C001/arc_lowering_abi_cleanup_model_contract_summary.json")


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
    SnippetCheck("M262-C001-DOC-EXP-01", "# M262 ARC Lowering ABI And Cleanup Model Contract And Architecture Freeze Expectations (C001)"),
    SnippetCheck("M262-C001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M262-C001-DOC-EXP-03", "`objc3_runtime_retain_i32`"),
    SnippetCheck("M262-C001-DOC-EXP-04", "`helper-call-plus-autoreleasepool-scope-lowering-without-general-cleanup-stack-or-return-slot-optimization`"),
    SnippetCheck("M262-C001-DOC-EXP-05", "`tmp/reports/m262/M262-C001/arc_lowering_abi_cleanup_model_contract_summary.json`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M262-C001-DOC-PKT-01", "# M262-C001 ARC Lowering ABI And Cleanup Model Contract And Architecture Freeze Packet"),
    SnippetCheck("M262-C001-DOC-PKT-02", "Packet: `M262-C001`"),
    SnippetCheck("M262-C001-DOC-PKT-03", "Issue: `#7199`"),
    SnippetCheck("M262-C001-DOC-PKT-04", "tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3"),
    SnippetCheck("M262-C001-DOC-PKT-05", "tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M262-C001-ARCH-01", "## M262 ARC Lowering ABI And Cleanup Model (C001)"),
    SnippetCheck("M262-C001-ARCH-02", "lowering owns helper-call ABI publication and later cleanup scheduling work"),
    SnippetCheck("M262-C001-ARCH-03", "`; arc_lowering_abi_cleanup_model = ...`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M262-C001-NDOC-01", "## M262 ARC lowering ABI and cleanup model (M262-C001)"),
    SnippetCheck("M262-C001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C001-NDOC-03", "`objc3_runtime_autorelease_i32`"),
    SnippetCheck("M262-C001-NDOC-04", "`M262-C002` is the next issue."),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M262-C001-SPC-01", "## M262 ARC lowering ABI and cleanup model (C001)"),
    SnippetCheck("M262-C001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M262-C001-SPC-03", "`M262-C002` is the next issue."),
)
SYNTAX_SPEC_SNIPPETS = (
    SnippetCheck("M262-C001-SYN-01", "### B.2.8 ARC lowering ABI and cleanup boundary (implementation note) {#b-2-8}"),
    SnippetCheck("M262-C001-SYN-02", "Current implementation status (`M262-C001`):"),
    SnippetCheck("M262-C001-SYN-03", "this freeze does not yet claim general ARC cleanup insertion,"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M262-C001-LHDR-01", "kObjc3ArcLoweringAbiCleanupModelContractId"),
    SnippetCheck("M262-C001-LHDR-02", "kObjc3ArcLoweringAbiCleanupModelAbiModel"),
    SnippetCheck("M262-C001-LHDR-03", "kObjc3ArcLoweringAbiCleanupModelCleanupModel"),
    SnippetCheck("M262-C001-LHDR-04", "Objc3ArcLoweringAbiCleanupModelSummary()"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M262-C001-LCPP-01", "Objc3ArcLoweringAbiCleanupModelSummary()"),
    SnippetCheck("M262-C001-LCPP-02", "M262-C001 ARC lowering ABI/cleanup freeze anchor"),
    SnippetCheck("M262-C001-LCPP-03", ";next_issue=M262-C002"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M262-C001-SEMA-01", "M262-C001 ARC lowering ABI/cleanup freeze anchor: sema continues to own"),
)
SEMA_PM_SNIPPETS = (
    SnippetCheck("M262-C001-PM-01", "M262-C001 ARC lowering ABI/cleanup freeze anchor: this handoff remains"),
)
SCAFFOLD_SNIPPETS = (
    SnippetCheck("M262-C001-SCAF-01", "M262-C001 ARC lowering ABI/cleanup freeze anchor: this scaffold remains a"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M262-C001-IR-01", 'out << "; arc_lowering_abi_cleanup_model = "'),
    SnippetCheck("M262-C001-IR-02", "M262-C001 ARC lowering ABI/cleanup freeze anchor:"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M262-C001-PKG-01", '"check:objc3c:m262-c001-arc-lowering-abi-and-cleanup-model-contract": "python scripts/check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py"'),
    SnippetCheck("M262-C001-PKG-02", '"test:tooling:m262-c001-arc-lowering-abi-and-cleanup-model-contract": "python -m pytest tests/tooling/test_check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py -q"'),
    SnippetCheck("M262-C001-PKG-03", '"check:objc3c:m262-c001-lane-c-readiness": "python scripts/run_m262_c001_lane_c_readiness.py"'),
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
    parser.add_argument("--property-fixture", type=Path, default=DEFAULT_PROPERTY_FIXTURE)
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


def run_positive_probe(*, args: argparse.Namespace, fixture: Path, case_id: str, out_leaf: str, required_tokens: Sequence[str], failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    probe_dir = args.probe_root.resolve() / out_leaf
    command = [str(args.native_exe.resolve()), str(fixture.resolve()), "--out-dir", str(probe_dir), "--emit-prefix", "module"]
    result = run_command(command, ROOT)
    ir_path = probe_dir / "module.ll"
    obj_path = probe_dir / "module.obj"
    case = {"case_id": case_id, "fixture": display_path(fixture), "command": command, "process_exit_code": result.returncode, "ir_path": display_path(ir_path), "object_path": display_path(obj_path)}
    checks_total = 0
    checks_passed = 0
    checks_total += 1; checks_passed += require(result.returncode == 0, display_path(ir_path), f"{case_id}-STATUS", "native probe must exit 0", failures)
    checks_total += 1; checks_passed += require(ir_path.exists(), display_path(ir_path), f"{case_id}-IR", "native probe must emit module.ll", failures)
    checks_total += 1; checks_passed += require(obj_path.exists(), display_path(obj_path), f"{case_id}-OBJ", "native probe must emit module.obj", failures)
    if not ir_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case
    ir_text = read_text(ir_path)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    case["boundary_line"] = boundary_line
    checks_total += 1; checks_passed += require(bool(boundary_line), display_path(ir_path), f"{case_id}-BOUNDARY", "IR must publish the ARC lowering ABI/cleanup boundary line", failures)
    checks_total += 1; checks_passed += require("abi_model=private-runtime-helper-call-boundary-over-retain-release-autorelease-weak-property-and-block-helpers" in boundary_line, display_path(ir_path), f"{case_id}-ABI-MODEL", "boundary line must carry the ABI model", failures)
    checks_total += 1; checks_passed += require("cleanup_model=helper-call-plus-autoreleasepool-scope-lowering-without-general-cleanup-stack-or-return-slot-optimization" in boundary_line, display_path(ir_path), f"{case_id}-CLEANUP-MODEL", "boundary line must carry the cleanup model", failures)
    for index, token in enumerate(required_tokens, start=1):
        checks_total += 1
        checks_passed += require(token in ir_text, display_path(ir_path), f"{case_id}-TOKEN-{index:02d}", f"IR must retain token: {token}", failures)
    checks_total += 1; checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), f"{case_id}-OBJ-SIZE", "module.obj must be non-empty", failures)
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
        total, passed, case = run_positive_probe(
            args=args,
            fixture=args.inference_fixture,
            case_id="M262-C001-CASE-INFERENCE",
            out_leaf="inference-positive",
            required_tokens=(
                "declare i32 @objc3_runtime_retain_i32(i32)",
                "declare i32 @objc3_runtime_release_i32(i32)",
                "declare i32 @objc3_runtime_autorelease_i32(i32)",
                "@__objc3_block_copy_helper_",
                "@__objc3_block_dispose_helper_",
            ),
            failures=failures,
        )
        checks_total += total
        checks_passed += passed
        dynamic_cases.append(case)
        total, passed, case = run_positive_probe(
            args=args,
            fixture=args.property_fixture,
            case_id="M262-C001-CASE-PROPERTY",
            out_leaf="property-positive",
            required_tokens=(
                "declare i32 @objc3_runtime_load_weak_current_property_i32()",
                "declare void @objc3_runtime_store_weak_current_property_i32(i32)",
                "declare i32 @objc3_runtime_autorelease_i32(i32)",
                "call i32 @objc3_runtime_autorelease_i32",
                "call i32 @objc3_runtime_load_weak_current_property_i32()",
            ),
            failures=failures,
        )
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
    args.summary_out.write_text(canonical_json(payload), encoding='utf-8')
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
