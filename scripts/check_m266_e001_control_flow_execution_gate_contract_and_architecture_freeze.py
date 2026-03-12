#!/usr/bin/env python3
"""Fail-closed checker for M266-E001 control-flow execution gate."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m266-e001-control-flow-execution-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-part5-control-flow-execution-gate/m266-e001-v1"
EVIDENCE_MODEL = "a002-b003-c003-d002-summary-chain-plus-integrated-runnable-probe"
ACTIVE_GATE_MODEL = (
    "integrated-lane-e-gate-consumes-source-sema-lowering-runtime-evidence-and-one-native-happy-path-probe"
)
NON_GOAL_MODEL = (
    "no-expression-match-no-guarded-patterns-no-type-test-patterns-no-public-cleanup-abi-no-broader-result-runtime-abi"
)
FAIL_CLOSED_MODEL = "fail-closed-on-control-flow-execution-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M266-E002"

A002_CONTRACT_ID = "objc3c-part5-control-flow-source-closure/m266-a002-v1"
C003_CONTRACT_ID = "objc3c-part5-match-lowering-runtime-alignment/m266-c003-v1"
D002_CONTRACT_ID = "objc3c-runtime-cleanup-unwind-integration/m266-d002-v1"
LOWERING_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract"
BOUNDARY_PREFIX = "; part5_control_flow_safety_lowering = "

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_control_flow_execution_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_e001_control_flow_execution_gate_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m266" / "M266-E001" / "control_flow_execution_gate_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "e001"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-A002" / "frontend_pattern_guard_surface_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-B003" / "defer_legality_cleanup_order_summary.json"
C003_SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-C003" / "match_lowering_dispatch_and_exhaustiveness_runtime_alignment_summary.json"
D002_SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-D002" / "runtime_cleanup_and_unwind_integration_summary.json"


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
        SnippetCheck("M266-E001-EXP-01", "# M266 Control-Flow Execution Gate Contract And Architecture Freeze Expectations (E001)"),
        SnippetCheck("M266-E001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M266-E001-EXP-03", "tmp/reports/m266/M266-D002/runtime_cleanup_and_unwind_integration_summary.json"),
        SnippetCheck("M266-E001-EXP-04", "The gate must explicitly hand off to `M266-E002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M266-E001-PKT-01", "# M266-E001 Control-Flow Execution Gate Contract And Architecture Freeze Packet"),
        SnippetCheck("M266-E001-PKT-02", "Packet: `M266-E001`"),
        SnippetCheck("M266-E001-PKT-03", "Issue: `#7267`"),
        SnippetCheck("M266-E001-PKT-04", "- `M266-D002`"),
        SnippetCheck("M266-E001-PKT-05", "compile, link, and run one integrated happy-path native program"),
        SnippetCheck("M266-E001-PKT-06", "`M266-E002` is the next issue."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M266-E001-SRC-01", "## M266 control-flow execution gate (E001)"),
        SnippetCheck("M266-E001-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M266-E001-SRC-03", "Lane E now freezes one integrated executable claim"),
        SnippetCheck("M266-E001-SRC-04", "`M266-E002` is the next issue"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M266-E001-NDOC-01", "## M266 control-flow execution gate (E001)"),
        SnippetCheck("M266-E001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M266-E001-NDOC-03", "Lane E now freezes one integrated executable claim"),
        SnippetCheck("M266-E001-NDOC-04", "`M266-E002` is the next issue"),
    ),
    SPEC_AM: (
        SnippetCheck("M266-E001-AM-01", "M266-E001 execution gate note:"),
        SnippetCheck("M266-E001-AM-02", "the runnable slice is intentionally narrow"),
    ),
    SPEC_RULES: (
        SnippetCheck("M266-E001-RULES-01", "Current Part 5 execution-gate note:"),
        SnippetCheck("M266-E001-RULES-02", "`M266-E001` freezes one integrated executable gate"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M266-E001-DRV-01", "M266-E001 control-flow execution gate anchor"),
        SnippetCheck("M266-E001-DRV-02", "truthful integrated proof surface for the currently runnable Part 5"),
    ),
    MANIFEST_ARTIFACTS_CPP: (
        SnippetCheck("M266-E001-MAN-01", "M266-E001 control-flow execution gate anchor"),
        SnippetCheck("M266-E001-MAN-02", "matching IR/object artifacts as one canonical proof"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M266-E001-FEA-01", "M266-E001 control-flow execution gate anchor"),
        SnippetCheck("M266-E001-FEA-02", "same native artifact triplet without a synthetic proof"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M266-E001-PKG-01", '"check:objc3c:m266-e001-control-flow-execution-gate-contract-and-architecture-freeze": "python scripts/check_m266_e001_control_flow_execution_gate_contract_and_architecture_freeze.py"'),
        SnippetCheck("M266-E001-PKG-02", '"test:tooling:m266-e001-control-flow-execution-gate-contract-and-architecture-freeze": "python -m pytest tests/tooling/test_check_m266_e001_control_flow_execution_gate_contract_and_architecture_freeze.py -q"'),
        SnippetCheck("M266-E001-PKG-03", '"check:objc3c:m266-e001-lane-e-readiness": "python scripts/run_m266_e001_lane_e_readiness.py"'),
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


def run_process(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT if cwd is None else cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, int, list[Finding]]:
    failures: list[Finding] = []
    total = len(snippets) + 1
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return total, 0, failures
    text = read_text(path)
    passed = 1
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return total, passed, failures


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def ensure_native_build(failures: list[Finding], summary_path: Path) -> tuple[int, int, dict[str, Any]]:
    completed = run_process(
        [
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m266-e001-control-flow-execution-gate",
            "--summary-out",
            str(summary_path),
        ]
    )
    payload = {}
    if summary_path.exists():
        payload = load_json(summary_path)
    total = 2
    passed = 0
    artifact = display_path(BUILD_HELPER)
    passed += require(completed.returncode == 0, artifact, "M266-E001-BLD-01", f"fast native build failed: {completed.stdout}{completed.stderr}", failures)
    passed += require(Path(NATIVE_EXE).exists(), display_path(NATIVE_EXE), "M266-E001-BLD-02", "native executable missing after build helper completed", failures)
    return total, passed, payload


def validate_upstream_summary(issue: str, path: Path, expected_contract: str | None, require_dynamic: bool, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks: list[tuple[str, bool, str]] = [
        (f"{issue}-SUM-01", payload.get("checks_passed") == payload.get("checks_total"), "summary must report checks_passed == checks_total"),
        (f"{issue}-SUM-02", payload.get("checks_total", 0) > 0, "summary must report at least one check"),
    ]
    if expected_contract is not None:
        checks.append((f"{issue}-SUM-03", payload.get("contract_id") == expected_contract, f"expected contract_id {expected_contract!r}"))
    if require_dynamic:
        checks.append((f"{issue}-SUM-04", payload.get("dynamic_probes_executed") is True, "summary must report dynamic_probes_executed=true"))
    total = len(checks)
    passed = 0
    for check_id, condition, detail in checks:
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, payload


def resolve_clangxx() -> str:
    candidates = (
        shutil.which("clang++.exe"),
        shutil.which("clang++"),
        r"C:\Program Files\LLVM\bin\clang++.exe",
    )
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(candidate)
    return "clang++"


def compile_fixture(source_path: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process([str(NATIVE_EXE), str(source_path), "--out-dir", str(out_dir), "--emit-prefix", "module"])


def link_executable(out_dir: Path) -> tuple[Path | None, subprocess.CompletedProcess[str] | None]:
    obj_path = out_dir / "module.obj"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    exe_path = out_dir / "module.exe"
    if not obj_path.exists() or not rsp_path.exists() or not registration_manifest_path.exists():
        return None, None
    registration_manifest = load_json(registration_manifest_path)
    runtime_library_relative_path = registration_manifest.get("runtime_support_library_archive_relative_path")
    if not isinstance(runtime_library_relative_path, str) or not runtime_library_relative_path.strip():
        return None, None
    runtime_library_path = (ROOT / runtime_library_relative_path).resolve()
    if not runtime_library_path.exists():
        return None, None
    result = run_process(
        [
            resolve_clangxx(),
            str(obj_path),
            str(runtime_library_path),
            f"@{rsp_path.resolve()}",
            "-o",
            str(exe_path),
        ],
        cwd=out_dir,
    )
    if result.returncode != 0 or not exe_path.exists():
        return None, result
    return exe_path, result


def boundary_line(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(BOUNDARY_PREFIX + "contract_id="):
            return line
    return ""


def parse_replay_key_map(replay_key: str) -> dict[str, str]:
    mapping: dict[str, str] = {}
    for segment in replay_key.split(";"):
        if "=" not in segment:
            continue
        key, value = segment.split("=", 1)
        mapping[key] = value
    return mapping


def make_integrated_positive_source() -> str:
    return """module m266ControlFlowExecutionGatePositive;

let trace = 0;

fn push(value: i32) -> void {
  trace = (trace * 10) + value;
}

fn classify(flag: bool) -> i32 {
  defer {
    push(9);
  }

  guard flag else {
    return 7;
  }

  match (flag) {
    case true: {
      push(1);
      return 5;
    }
    case false: {
      return 6;
    }
  }
}

fn main() -> i32 {
  trace = 0;
  let value = classify(true);
  return (trace * 10) + value;
}
"""


def validate_integrated_probe(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    probe_dir = PROBE_ROOT / "integrated_positive"
    probe_dir.mkdir(parents=True, exist_ok=True)
    source_path = probe_dir / "m266_control_flow_execution_gate_positive.objc3"
    source_path.write_text(make_integrated_positive_source(), encoding="utf-8")

    compile_result = compile_fixture(source_path, probe_dir)
    manifest_path = probe_dir / "module.manifest.json"
    ir_path = probe_dir / "module.ll"
    object_path = probe_dir / "module.obj"
    exe_path, link_result = link_executable(probe_dir)
    run_result = None
    if exe_path is not None:
        run_result = run_process([str(exe_path)], cwd=probe_dir)

    manifest_payload = load_json(manifest_path) if manifest_path.exists() else {}
    lowering_packet = manifest_payload.get("lowering_part5_control_flow_safety", {})
    if not isinstance(lowering_packet, dict):
        lowering_packet = {}
    replay_key_map = parse_replay_key_map(str(lowering_packet.get("replay_key", "")))
    ir_text = read_text(ir_path) if ir_path.exists() else ""
    boundary = boundary_line(ir_text)

    checks = [
        (display_path(source_path), "M266-E001-PROBE-01", compile_result.returncode == 0, f"native compile failed: {compile_result.stdout}{compile_result.stderr}"),
        (display_path(manifest_path), "M266-E001-PROBE-02", manifest_path.exists(), "manifest artifact missing"),
        (display_path(ir_path), "M266-E001-PROBE-03", ir_path.exists(), "IR artifact missing"),
        (display_path(object_path), "M266-E001-PROBE-04", object_path.exists(), "object artifact missing"),
        (display_path(ir_path), "M266-E001-PROBE-05", boundary != "", "IR missing Part 5 lowering boundary line"),
        (display_path(manifest_path), "M266-E001-PROBE-06", lowering_packet.get("lane_contract") == "m266-part5-control-flow-safety-lowering-v1", "manifest lowering packet missing expected lane contract"),
        (display_path(manifest_path), "M266-E001-PROBE-07", replay_key_map.get("guard_statement_sites") == "1", "expected one guard site in lowering replay key"),
        (display_path(manifest_path), "M266-E001-PROBE-08", replay_key_map.get("match_statement_sites") == "1", "expected one match site in lowering replay key"),
        (display_path(manifest_path), "M266-E001-PROBE-09", replay_key_map.get("defer_statement_sites") == "1", "expected one defer site in lowering replay key"),
        (display_path(manifest_path), "M266-E001-PROBE-10", replay_key_map.get("live_guard_short_circuit_sites") == "1", "expected one live guard lowering site"),
        (display_path(manifest_path), "M266-E001-PROBE-11", replay_key_map.get("live_match_dispatch_sites") == "1", "expected one live match lowering site"),
        (display_path(manifest_path), "M266-E001-PROBE-12", replay_key_map.get("live_defer_cleanup_sites") == "1", "expected one live defer cleanup site"),
        (display_path(probe_dir), "M266-E001-PROBE-13", exe_path is not None and link_result is not None and link_result.returncode == 0, f"native link failed: {'' if link_result is None else link_result.stdout + link_result.stderr}"),
        (display_path(probe_dir), "M266-E001-PROBE-14", run_result is not None and run_result.returncode == 195, f"native run returned {None if run_result is None else run_result.returncode}; expected 195; stdout={'' if run_result is None else run_result.stdout} stderr={'' if run_result is None else run_result.stderr}"),
    ]
    total = len(checks)
    passed = 0
    for artifact, check_id, condition, detail in checks:
        passed += require(condition, artifact, check_id, detail, failures)

    evidence = {
        "compile_returncode": compile_result.returncode,
        "link_returncode": None if link_result is None else link_result.returncode,
        "run_returncode": None if run_result is None else run_result.returncode,
        "manifest_lowering_packet": lowering_packet,
        "manifest_lowering_replay_key_map": replay_key_map,
        "ir_boundary": boundary,
        "probe_source_path": display_path(source_path),
        "manifest_path": display_path(manifest_path),
        "ir_path": display_path(ir_path),
        "object_path": display_path(object_path),
    }
    return total, passed, evidence


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        total, passed, file_failures = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += passed
        failures.extend(file_failures)

    build_summary_path = args.summary_out.parent / "ensure_objc3c_native_build_summary.json"
    total, passed, build_payload = ensure_native_build(failures, build_summary_path)
    checks_total += total
    checks_passed += passed

    upstream_summary_payloads: dict[str, Any] = {}
    for issue, path, contract_id, require_dynamic in (
        ("M266-A002", A002_SUMMARY, A002_CONTRACT_ID, True),
        ("M266-B003", B003_SUMMARY, None, False),
        ("M266-C003", C003_SUMMARY, C003_CONTRACT_ID, False),
        ("M266-D002", D002_SUMMARY, D002_CONTRACT_ID, True),
    ):
        total, passed, payload = validate_upstream_summary(issue, path, contract_id, require_dynamic, failures)
        checks_total += total
        checks_passed += passed
        upstream_summary_payloads[issue] = payload

    total, passed, integrated_probe = validate_integrated_probe(failures)
    checks_total += total
    checks_passed += passed

    ok = not failures
    summary = {
        "ok": ok,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "active_gate_model": ACTIVE_GATE_MODEL,
        "non_goal_model": NON_GOAL_MODEL,
        "fail_closed_model": FAIL_CLOSED_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "dynamic_probes_executed": True,
        "build_helper": build_payload,
        "upstream_summaries": upstream_summary_payloads,
        "integrated_probe": integrated_probe,
        "failures": [failure.__dict__ for failure in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if not ok:
        for failure in failures:
            print(f"[fail] {failure.artifact} {failure.check_id}: {failure.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print(f"[ok] M266-E001 control-flow execution gate validated ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
