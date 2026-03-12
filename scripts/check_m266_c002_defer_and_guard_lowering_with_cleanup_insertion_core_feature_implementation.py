#!/usr/bin/env python3
"""Truthful checker for M266-C002 defer/guard lowering with cleanup insertion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m266-c002-defer-and-guard-lowering-with-cleanup-insertion-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-part5-defer-guard-lowering-implementation/m266-c002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m266" / "M266-C002" / "defer_and_guard_lowering_with_cleanup_insertion_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_defer_and_guard_lowering_with_cleanup_insertion_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_c002_defer_and_guard_lowering_with_cleanup_insertion_core_feature_implementation_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
LOWERING_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
MIXED_MATCH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m266_guard_match_exhaustiveness_positive.objc3"

BOUNDARY_COMMENT = "; part5_control_flow_safety_lowering = "


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M266-C002-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
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
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def read_contract_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    semantic_surface = pipeline.get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing semantic surface in {display_path(manifest_path)}")
    payload = semantic_surface.get("objc_part5_control_flow_safety_lowering_contract")
    if not isinstance(payload, dict):
        raise TypeError(f"missing C002 lowering payload in {display_path(manifest_path)}")
    return payload


def write_probe(path: Path, source: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(source, encoding="utf-8")


def run_native_probe(native_exe: Path, source_path: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([
        str(native_exe),
        str(source_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])


def validate_payload(
    payload: dict[str, Any],
    artifact: str,
    failures: list[Finding],
    *,
    expected_guard_sites: int,
    expected_defer_sites: int,
    expected_guard_live: int,
    expected_defer_live: int,
) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected_exact: list[tuple[str, str, Any, str]] = [
        ("M266-C002-PAYLOAD-01", "contract_id", "objc3c-part5-control-flow-safety-lowering/m266-c001-v1", "contract id drifted"),
        ("M266-C002-PAYLOAD-02", "surface_path", SURFACE_PATH, "surface path drifted"),
        ("M266-C002-PAYLOAD-03", "guard_statement_sites", expected_guard_sites, "guard statement count mismatch"),
        ("M266-C002-PAYLOAD-04", "defer_statement_sites", expected_defer_sites, "defer statement count mismatch"),
        ("M266-C002-PAYLOAD-05", "live_guard_short_circuit_sites", expected_guard_live, "live guard lowering count mismatch"),
        ("M266-C002-PAYLOAD-06", "live_defer_cleanup_sites", expected_defer_live, "live defer cleanup insertion count mismatch"),
        ("M266-C002-PAYLOAD-07", "live_match_dispatch_sites", 0, "match lowering should remain out of scope for C002"),
        ("M266-C002-PAYLOAD-08", "fail_closed_guard_short_circuit_sites", expected_guard_sites - expected_guard_live, "guard fail-closed count mismatch"),
        ("M266-C002-PAYLOAD-09", "fail_closed_defer_cleanup_sites", expected_defer_sites - expected_defer_live, "defer fail-closed count mismatch"),
        ("M266-C002-PAYLOAD-10", "fail_closed_match_dispatch_sites", payload.get("match_statement_sites", 0), "match fail-closed count mismatch"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)

    for check_id, field, expected in [
        ("M266-C002-PAYLOAD-11", "ready_for_native_guard_lowering", True),
        ("M266-C002-PAYLOAD-12", "ready_for_native_match_lowering", False),
        ("M266-C002-PAYLOAD-13", "ready_for_native_defer_lowering", True),
        ("M266-C002-PAYLOAD-14", "deterministic", True),
    ]:
        checks_total += 1
        checks_passed += require(payload.get(field) is expected, artifact, check_id, f"{field} drifted", failures)

    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M266-C002-PAYLOAD-15", "replay key missing", failures)
    return checks_total, checks_passed


def make_guard_probe() -> str:
    return """module m266GuardLoweringProbe;\n\nfn classify(maybeValue: id?, flag: bool) -> i32 {\n  guard let ready = maybeValue, flag else {\n    return 7;\n  }\n  ready;\n  return 9;\n}\n\nfn main() -> i32 {\n  return classify(9, true);\n}\n"""


def make_defer_ordinary_probe() -> str:
    return """module m266DeferOrdinaryExitProbe;\n\nfn sample() -> i32 {\n  let seed = 4;\n  defer {\n    seed;\n  }\n  return seed;\n}\n\nfn main() -> i32 {\n  return sample();\n}\n"""


def make_defer_return_probe() -> str:
    return """module m266DeferReturnProbe;\n\nfn sample(flag: bool) -> i32 {\n  defer {\n    flag;\n  }\n  if (flag) {\n    return 3;\n  }\n  return 5;\n}\n\nfn main() -> i32 {\n  return sample(true);\n}\n"""


def validate_success_artifacts(out_dir: Path, failures: list[Finding], case_prefix: str) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    manifest_path = out_dir / "module.manifest.json"
    ll_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    checks_total += 3
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), f"{case_prefix}-ART-01", "manifest missing", failures)
    checks_passed += require(ll_path.exists(), display_path(ll_path), f"{case_prefix}-ART-02", "IR file missing", failures)
    checks_passed += require(obj_path.exists(), display_path(obj_path), f"{case_prefix}-ART-03", "object file missing", failures)
    payload: dict[str, Any] = {}
    if manifest_path.exists():
        payload = read_contract_payload(manifest_path)
    if ll_path.exists():
        ll_text = read_text(ll_path)
        checks_total += 1
        checks_passed += require(BOUNDARY_COMMENT in ll_text, display_path(ll_path), f"{case_prefix}-IR-01", "C002 boundary comment missing from IR", failures)
    return checks_total, checks_passed, {"manifest": display_path(manifest_path), "ir": display_path(ll_path), "obj": display_path(obj_path), "payload": payload}


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    ensure_summary = ROOT / "tmp" / "reports" / "m266" / "M266-C002" / "ensure_build_summary.json"
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m266-c002-readiness",
        "--summary-out",
        str(ensure_summary),
    ])
    checks_total += 2
    checks_passed += require(ensure_build.returncode == 0, display_path(ensure_summary), "M266-C002-DYN-01", "ensure build failed", failures)
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M266-C002-DYN-02", "native executable missing", failures)

    generated_root = ROOT / "tmp" / "reports" / "m266" / "M266-C002" / "generated_probes"
    guard_src = generated_root / "guard_positive.objc3"
    defer_ordinary_src = generated_root / "defer_ordinary_exit_positive.objc3"
    defer_return_src = generated_root / "defer_return_positive.objc3"
    write_probe(guard_src, make_guard_probe())
    write_probe(defer_ordinary_src, make_defer_ordinary_probe())
    write_probe(defer_return_src, make_defer_return_probe())

    probe_results: dict[str, Any] = {}

    guard_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "c002" / "guard_positive"
    guard = run_native_probe(args.native_exe, guard_src, guard_out)
    checks_total += 1
    checks_passed += require(guard.returncode == 0, display_path(guard_src), "M266-C002-DYN-03", "pure guard probe should compile successfully", failures)
    total, passed, payload_info = validate_success_artifacts(guard_out, failures, "M266-C002-DYN-GUARD")
    checks_total += total
    checks_passed += passed
    if payload_info.get("payload"):
        total, passed = validate_payload(
            payload_info["payload"],
            str(payload_info["manifest"]),
            failures,
            expected_guard_sites=1,
            expected_defer_sites=0,
            expected_guard_live=1,
            expected_defer_live=0,
        )
        checks_total += total
        checks_passed += passed
    probe_results["guard_positive"] = {
        "returncode": guard.returncode,
        **payload_info,
    }

    defer_ordinary_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "c002" / "defer_ordinary_exit"
    defer_ordinary = run_native_probe(args.native_exe, defer_ordinary_src, defer_ordinary_out)
    checks_total += 1
    checks_passed += require(defer_ordinary.returncode == 0, display_path(defer_ordinary_src), "M266-C002-DYN-04", "ordinary-exit defer probe should compile successfully", failures)
    total, passed, payload_info = validate_success_artifacts(defer_ordinary_out, failures, "M266-C002-DYN-DEFER-ORDINARY")
    checks_total += total
    checks_passed += passed
    if payload_info.get("payload"):
        total, passed = validate_payload(
            payload_info["payload"],
            str(payload_info["manifest"]),
            failures,
            expected_guard_sites=0,
            expected_defer_sites=1,
            expected_guard_live=0,
            expected_defer_live=1,
        )
        checks_total += total
        checks_passed += passed
    probe_results["defer_ordinary_exit_positive"] = {
        "returncode": defer_ordinary.returncode,
        **payload_info,
    }

    defer_return_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "c002" / "defer_return"
    defer_return = run_native_probe(args.native_exe, defer_return_src, defer_return_out)
    checks_total += 1
    checks_passed += require(defer_return.returncode == 0, display_path(defer_return_src), "M266-C002-DYN-05", "early-return defer probe should compile successfully", failures)
    total, passed, payload_info = validate_success_artifacts(defer_return_out, failures, "M266-C002-DYN-DEFER-RETURN")
    checks_total += total
    checks_passed += passed
    if payload_info.get("payload"):
        total, passed = validate_payload(
            payload_info["payload"],
            str(payload_info["manifest"]),
            failures,
            expected_guard_sites=0,
            expected_defer_sites=1,
            expected_guard_live=0,
            expected_defer_live=1,
        )
        checks_total += total
        checks_passed += passed
    probe_results["defer_return_positive"] = {
        "returncode": defer_return.returncode,
        **payload_info,
    }

    mixed_out = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "c002" / "guard_match_mixed_observation"
    mixed = run_native_probe(args.native_exe, MIXED_MATCH_FIXTURE, mixed_out)
    diagnostics_path = mixed_out / "module.diagnostics.txt"
    diagnostics_text = diagnostics_path.read_text(encoding="utf-8") if diagnostics_path.exists() else ""
    probe_results["guard_match_mixed_observation"] = {
        "fixture": display_path(MIXED_MATCH_FIXTURE),
        "returncode": mixed.returncode,
        "diagnostics_path": display_path(diagnostics_path),
        "diagnostics_excerpt": diagnostics_text[:400],
    }

    return checks_total, checks_passed, probe_results


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    bundle_snippet_map: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M266-C002-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M266-C002-DOC-02", "statement-form `match` remains fail-closed"),
            SnippetCheck("M266-C002-DOC-03", "frontend.pipeline.semantic_surface.objc_part5_defer_guard_lowering_implementation"),
            SnippetCheck("M266-C002-DOC-04", "live_defer_nonlocal_exit_cleanup_sites = 1"),
        ],
        PACKET_DOC: [
            SnippetCheck("M266-C002-PKT-01", "Packet: `M266-C002`"),
            SnippetCheck("M266-C002-PKT-02", "Issue: `#7263`"),
            SnippetCheck("M266-C002-PKT-03", "objc3c-part5-defer-guard-lowering-implementation/m266-c002-v1"),
            SnippetCheck("M266-C002-PKT-04", "Generate temporary native probes under `tmp/`"),
        ],
    }
    for path, snippets in bundle_snippet_map.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    future_anchor_snippet_map: dict[Path, list[SnippetCheck]] = {
        LOWERING_H: [
            SnippetCheck("M266-C002-SNIP-01", "kObjc3Part5ControlFlowSafetyLoweringContractId"),
            SnippetCheck("M266-C002-SNIP-02", "kObjc3Part5ControlFlowSafetyLoweringGuardModel"),
            SnippetCheck("M266-C002-SNIP-03", "kObjc3Part5ControlFlowSafetyLoweringDeferModel"),
        ],
        IR_EMITTER: [
            SnippetCheck("M266-C002-SNIP-04", "M266-C002 defer/guard lowering anchor"),
            SnippetCheck("M266-C002-SNIP-05", "pending_defer_scope_blocks"),
            SnippetCheck("M266-C002-SNIP-06", "guard_condition_exprs"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M266-C002-SNIP-07", "BuildPart5ControlFlowSafetyLoweringContractJson"),
            SnippetCheck("M266-C002-SNIP-08", "objc_part5_control_flow_safety_lowering_contract"),
            SnippetCheck("M266-C002-SNIP-09", "part5_control_flow_safety_lowering_replay_key"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M266-C002-SNIP-10", "M266-C002"),
            SnippetCheck("M266-C002-SNIP-11", "objc3c-part5-defer-guard-lowering-implementation/m266-c002-v1"),
        ],
        SPEC_AM: [
            SnippetCheck("M266-C002-SNIP-12", "M266-C002 lowering note"),
        ],
        SPEC_RULES: [
            SnippetCheck("M266-C002-SNIP-13", "Current Part 5 defer and guard lowering note"),
        ],
        ARCHITECTURE_DOC: [
            SnippetCheck("M266-C002-SNIP-14", "M266-C002"),
        ],
    }

    dynamic: dict[str, Any] = {
        "skipped": args.skip_dynamic_probes,
        "future_anchor_requirements": {
            display_path(path): [snippet.snippet for snippet in snippets]
            for path, snippets in future_anchor_snippet_map.items()
        },
    }
    if not args.skip_dynamic_probes:
        for path, snippets in future_anchor_snippet_map.items():
            checks_total += len(snippets)
            checks_passed += ensure_snippets(path, snippets, failures)
        total, passed, probe_dynamic = run_dynamic_probes(args, failures)
        checks_total += total
        checks_passed += passed
        dynamic["probes"] = probe_dynamic

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "surface_path": SURFACE_PATH,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "failures": [failure.__dict__ for failure in failures],
        "dynamic": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
