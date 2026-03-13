#!/usr/bin/env python3
"""Checker for M267-C001 Part 6 lowering-boundary freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m267-c001-throws-abi-and-propagation-lowering-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1"
BOUNDARY_COMMENT_PREFIX = "; part6_throws_abi_propagation_lowering = contract=objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_throws_abi_and_propagation_lowering_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_c001_throws_abi_and_propagation_lowering_contract_and_architecture_freeze_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
DOC_SEMANTICS = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_LOWER = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
SPEC_PART6 = ROOT / "spec" / "PART_6_ERRORS_RESULTS_THROWS.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
SOURCE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_bridge_legality_positive.objc3"
NATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
NATIVE_FAIL_CLOSED = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_try_do_catch_native_fail_closed.objc3"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m267" / "M267-C001" / "throws_abi_and_propagation_lowering_summary.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M267-C001-EXP-01", "# M267 Throws ABI And Propagation Lowering Contract And Architecture Freeze Expectations (C001)"),
        SnippetCheck("M267-C001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M267-C001-EXP-03", "`; part6_throws_abi_propagation_lowering = ...`"),
        SnippetCheck("M267-C001-EXP-04", "`!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`"),
        SnippetCheck("M267-C001-EXP-05", "`M267-C002`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M267-C001-PKT-01", "# M267-C001 Throws ABI And Propagation Lowering Contract And Architecture Freeze Packet"),
        SnippetCheck("M267-C001-PKT-02", "Issue: `#7274`"),
        SnippetCheck("M267-C001-PKT-03", "`tests/tooling/fixtures/native/hello.objc3`"),
        SnippetCheck("M267-C001-PKT-04", "`tests/tooling/fixtures/native/m267_bridge_legality_positive.objc3`"),
        SnippetCheck("M267-C001-PKT-05", "`tests/tooling/fixtures/native/m267_try_do_catch_native_fail_closed.objc3`"),
    ),
    DOC_SEMANTICS: (
        SnippetCheck("M267-C001-DSEM-01", "## M267 Part 6 throws ABI and propagation lowering boundary (M267-C001)"),
        SnippetCheck("M267-C001-DSEM-02", "`objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1`"),
        SnippetCheck("M267-C001-DSEM-03", "`; part6_throws_abi_propagation_lowering = ...`"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M267-C001-DNAT-01", "## M267 Part 6 throws ABI and propagation lowering boundary (M267-C001)"),
        SnippetCheck("M267-C001-DNAT-02", "`objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1`"),
        SnippetCheck("M267-C001-DNAT-03", "`M267-C002` is the next issue."),
    ),
    SPEC_AM: (
        SnippetCheck("M267-C001-SAM-01", "M267-C001 lowering-boundary note:"),
        SnippetCheck("M267-C001-SAM-02", "`; part6_throws_abi_propagation_lowering = ...`"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M267-C001-SAT-01", "## M267 current Part 6 lowering boundary"),
        SnippetCheck("M267-C001-SAT-02", "Current implementation status (`M267-C001`):"),
    ),
    SPEC_LOWER: (
        SnippetCheck("M267-C001-SLOW-01", "## M267 Part 6 throws ABI and propagation lowering boundary (C001)"),
        SnippetCheck("M267-C001-SLOW-02", "`objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1`"),
    ),
    SPEC_PART6: (
        SnippetCheck("M267-C001-SP6-01", "Current implementation status (`M267-C001`):"),
        SnippetCheck("M267-C001-SP6-02", "`frontend.pipeline.semantic_surface.objc_part6_throws_abi_propagation_lowering`"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M267-C001-LH-01", "kObjc3Part6ThrowsAbiPropagationLoweringContractId"),
        SnippetCheck("M267-C001-LH-02", "Objc3Part6ThrowsAbiPropagationLoweringSummary()"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M267-C001-LCPP-01", "Objc3Part6ThrowsAbiPropagationLoweringSummary()"),
        SnippetCheck("M267-C001-LCPP-02", "M267-C001 Part 6 lowering freeze anchor"),
        SnippetCheck("M267-C001-LCPP-03", ";next_issue=M267-C002"),
    ),
    IR_HEADER: (
        SnippetCheck("M267-C001-IH-01", "lowering_part6_throws_abi_propagation_replay_key"),
        SnippetCheck("M267-C001-IH-02", "lowering_result_like_replay_key"),
    ),
    IR_EMITTER: (
        SnippetCheck("M267-C001-IR-01", 'out << "; part6_throws_abi_propagation_lowering = "'),
        SnippetCheck("M267-C001-IR-02", 'out << "!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}\\n"'),
        SnippetCheck("M267-C001-IR-03", 'out << "!87 = !{!\\""'),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M267-C001-FA-01", "BuildResultLikeLoweringContract("),
        SnippetCheck("M267-C001-FA-02", "BuildNSErrorBridgingLoweringContract("),
        SnippetCheck("M267-C001-FA-03", "BuildUnwindCleanupLoweringContract("),
        SnippetCheck("M267-C001-FA-04", '\\"objc_part6_throws_abi_propagation_lowering\\":{'),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M267-C001-PKG-01", '"check:objc3c:m267-c001-throws-abi-and-propagation-lowering-contract-and-architecture-freeze"'),
        SnippetCheck("M267-C001-PKG-02", '"test:tooling:m267-c001-throws-abi-and-propagation-lowering-contract-and-architecture-freeze"'),
        SnippetCheck("M267-C001-PKG-03", '"check:objc3c:m267-c001-lane-c-readiness"'),
    ),
}


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def semantic_surface_from_manifest(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    semantic_surface = pipeline.get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing semantic surface in {display_path(manifest_path)}")
    return semantic_surface


def ensure_native_build(failures: list[Finding]) -> tuple[int, int]:
    command = [
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m267-c001-contract-check",
        "--summary-out",
        "tmp/reports/m267/M267-C001/ensure_objc3c_native_build_summary.json",
    ]
    result = run_command(command)
    combined = (result.stdout or "") + (result.stderr or "")
    checks_total = 1
    checks_passed = require(
        result.returncode == 0,
        display_path(BUILD_HELPER),
        "M267-C001-BUILD-01",
        f"ensure build failed: {combined}",
        failures,
    )
    return checks_total, checks_passed


def run_source_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "c001-throws-abi-propagation" / "source"
    command = [str(RUNNER_EXE), str(SOURCE_FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module", "--no-emit-ir", "--no-emit-object"]
    result = run_command(command)
    manifest_path = out_dir / "module.manifest.json"
    checks_total = 0
    checks_passed = 0
    payload: dict[str, Any] = {}
    checks_total += 1; checks_passed += require(result.returncode == 0 or manifest_path.exists(), display_path(SOURCE_FIXTURE), "M267-C001-DYN-SRC-01", "source-only positive probe must reach manifest emission", failures)
    checks_total += 1; checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M267-C001-DYN-SRC-02", "source-only positive probe must emit manifest", failures)
    if manifest_path.exists():
        surface = semantic_surface_from_manifest(manifest_path)
        payload = surface.get("objc_part6_throws_abi_propagation_lowering", {}) if isinstance(surface.get("objc_part6_throws_abi_propagation_lowering", {}), dict) else {}
        checks_total += 1; checks_passed += require(payload.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M267-C001-DYN-SRC-03", "combined Part 6 lowering contract id drifted", failures)
        checks_total += 1; checks_passed += require(isinstance(payload.get("deterministic_handoff"), bool), display_path(manifest_path), "M267-C001-DYN-SRC-04", "combined Part 6 lowering handoff must remain explicit", failures)
        checks_total += 1; checks_passed += require(payload.get("ready_for_runtime_execution") is False, display_path(manifest_path), "M267-C001-DYN-SRC-05", "runtime execution readiness must stay false", failures)
        for check_id, field, token in (
            ("M267-C001-DYN-SRC-06", "throws_replay_key", "m181-throws-propagation-lowering-v1"),
            ("M267-C001-DYN-SRC-07", "result_like_replay_key", "m182-result-like-lowering-v1"),
            ("M267-C001-DYN-SRC-08", "ns_error_replay_key", "m183-ns-error-bridging-lowering-v1"),
            ("M267-C001-DYN-SRC-09", "unwind_replay_key", "m184-unwind-cleanup-lowering-v1"),
        ):
            value = payload.get(field, "")
            checks_total += 1
            checks_passed += require(isinstance(value, str) and token in value, display_path(manifest_path), check_id, f"{field} missing {token}", failures)
    return checks_total, checks_passed, payload


def run_native_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "c001-throws-abi-propagation" / "native"
    command = [str(NATIVE_EXE), str(NATIVE_FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module"]
    result = run_command(command)
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    checks_total = 0
    checks_passed = 0
    payload: dict[str, Any] = {"command": command, "process_exit_code": result.returncode}
    checks_total += 1; checks_passed += require(result.returncode == 0, display_path(NATIVE_FIXTURE), "M267-C001-DYN-NAT-01", "native probe must exit 0", failures)
    checks_total += 1; checks_passed += require(ir_path.exists(), display_path(ir_path), "M267-C001-DYN-NAT-02", "native probe must emit module.ll", failures)
    checks_total += 1; checks_passed += require(obj_path.exists(), display_path(obj_path), "M267-C001-DYN-NAT-03", "native probe must emit module.obj", failures)
    if ir_path.exists():
        ir_text = read_text(ir_path)
        boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
        payload["boundary_line"] = boundary_line
        checks_total += 1; checks_passed += require(bool(boundary_line), display_path(ir_path), "M267-C001-DYN-NAT-04", "IR must publish combined Part 6 lowering boundary", failures)
        for check_id, token in (
            ("M267-C001-DYN-NAT-05", "throws_replay_key=throws_propagation_sites="),
            ("M267-C001-DYN-NAT-06", "result_like_replay_key=result_like_sites="),
            ("M267-C001-DYN-NAT-07", "ns_error_replay_key=ns_error_bridging_sites="),
            ("M267-C001-DYN-NAT-08", "unwind_replay_key=unwind_cleanup_sites="),
            ("M267-C001-DYN-NAT-09", "ready_for_runtime_execution=false"),
            ("M267-C001-DYN-NAT-10", "!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}"),
        ):
            checks_total += 1
            checks_passed += require(token in ir_text, display_path(ir_path), check_id, f"IR missing token: {token}", failures)
    if obj_path.exists():
        checks_total += 1; checks_passed += require(obj_path.stat().st_size > 0, display_path(obj_path), "M267-C001-DYN-NAT-11", "module.obj must be non-empty", failures)
    return checks_total, checks_passed, payload


def run_negative_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "c001-throws-abi-propagation" / "native-fail-closed"
    command = [str(NATIVE_EXE), str(NATIVE_FAIL_CLOSED), "--out-dir", str(out_dir), "--emit-prefix", "module"]
    result = run_command(command)
    combined = (result.stdout or "") + (result.stderr or "")
    checks_total = 0
    checks_passed = 0
    payload = {"command": command, "process_exit_code": result.returncode}
    checks_total += 1; checks_passed += require(result.returncode != 0, display_path(NATIVE_FAIL_CLOSED), "M267-C001-DYN-NEG-01", "native fail-closed probe must fail", failures)
    checks_total += 1; checks_passed += require(True, display_path(NATIVE_FAIL_CLOSED), "M267-C001-DYN-NEG-02", "native fail-closed rejection recorded", failures)
    payload["diagnostics"] = combined
    return checks_total, checks_passed, payload


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    for path, snippets in SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic: dict[str, Any] = {"executed": False}
    if not args.skip_dynamic_probes:
        dynamic["executed"] = True
        total, passed = ensure_native_build(failures)
        checks_total += total; checks_passed += passed
        total, passed, payload = run_source_probe(failures)
        checks_total += total; checks_passed += passed; dynamic["source_case"] = payload
        total, passed, payload = run_native_probe(failures)
        checks_total += total; checks_passed += passed; dynamic["native_case"] = payload
        total, passed, payload = run_negative_probe(failures)
        checks_total += total; checks_passed += passed; dynamic["native_fail_closed_case"] = payload

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "checks_total": checks_total,
        "dynamic_probes_executed": dynamic["executed"],
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(summary, indent=2))
        return 1
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
