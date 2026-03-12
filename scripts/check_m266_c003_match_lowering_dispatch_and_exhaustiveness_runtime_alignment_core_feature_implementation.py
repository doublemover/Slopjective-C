#!/usr/bin/env python3
"""Truthful checker for M266-C003 match lowering/runtime alignment."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence
from uuid import uuid4

ROOT = Path(__file__).resolve().parents[1]
MODE = "m266-c003-match-lowering-dispatch-and-exhaustiveness-runtime-alignment-v1"
ISSUE = "#7264"
ISSUE_CONTRACT_ID = "objc3c-part5-match-lowering-runtime-alignment/m266-c003-v1"
LOWERING_CONTRACT_ID = "objc3c-part5-control-flow-safety-lowering/m266-c001-v1"
SEMANTIC_CONTRACT_ID = "objc3c-part5-control-flow-semantic-model/m266-b001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m266" / "M266-C003" / "match_lowering_dispatch_and_exhaustiveness_runtime_alignment_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_match_lowering_dispatch_and_exhaustiveness_runtime_alignment_core_feature_implementation_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_c003_match_lowering_dispatch_and_exhaustiveness_runtime_alignment_core_feature_implementation_packet.md"
READINESS_RUNNER = ROOT / "scripts" / "run_m266_c003_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m266_c003_match_lowering_dispatch_and_exhaustiveness_runtime_alignment_core_feature_implementation.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
LOWERING_KEY = "objc_part5_control_flow_safety_lowering_contract"
SEMANTIC_KEY = "objc_part5_control_flow_semantic_model"
BOUNDARY_COMMENT = "; part5_control_flow_safety_lowering = "


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M266-C003-EXP-01", f"Contract ID: `{ISSUE_CONTRACT_ID}`"),
        SnippetCheck("M266-C003-EXP-02", f"Issue: `{ISSUE}`"),
        SnippetCheck("M266-C003-EXP-03", f"`{SURFACE_PATH}`"),
        SnippetCheck("M266-C003-EXP-04", "result-case observation probe still fails closed deterministically with `O3L300`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M266-C003-PKT-01", "Packet: `M266-C003`"),
        SnippetCheck("M266-C003-PKT-02", f"Issue: `{ISSUE}`"),
        SnippetCheck("M266-C003-PKT-03", "literal/default/wildcard/binding statement-form `match`"),
        SnippetCheck("M266-C003-PKT-04", "result-case payload matching explicitly deferred"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M266-C003-RUN-01", "check_m266_b002_guard_refinement_and_match_exhaustiveness_semantics_core_feature_implementation.py"),
        SnippetCheck("M266-C003-RUN-02", "check_m266_c001_control_flow_safety_lowering_contract_and_architecture_freeze.py"),
        SnippetCheck("M266-C003-RUN-03", "check_m266_c002_defer_and_guard_lowering_with_cleanup_insertion_core_feature_implementation.py"),
        SnippetCheck("M266-C003-RUN-04", "check_m266_c003_match_lowering_dispatch_and_exhaustiveness_runtime_alignment_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M266-C003-TEST-01", "def test_m266_c003_static_bundle_contract_check_runs_truthfully() -> None:"),
        SnippetCheck("M266-C003-TEST-02", ISSUE_CONTRACT_ID),
        SnippetCheck("M266-C003-TEST-03", SURFACE_PATH),
    ),
}


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def load_semantic_surface(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing semantic surface in {display_path(manifest_path)}")
    return semantic_surface


def diagnostics_text(out_dir: Path) -> str:
    diagnostics = out_dir / "module.diagnostics.txt"
    return read_text(diagnostics) if diagnostics.exists() else ""


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


def make_bool_positive_probe() -> str:
    return """module m266MatchBoolPositive;

fn classify(flag: bool) -> i32 {
  match (flag) {
    case true: {
      return 10;
    }
    case false: {
      return 20;
    }
  }
  return 30;
}

fn main(flag: bool) -> i32 {
  return classify(flag);
}
"""


def make_binding_positive_probe() -> str:
    return """module m266MatchBindingPositive;

fn classify(tag: i32) -> i32 {
  match (tag) {
    case 1: {
      return 10;
    }
    case let capture: {
      capture;
      return capture;
    }
  }
  return 30;
}

fn main(tag: i32) -> i32 {
  return classify(tag);
}
"""


def make_bool_negative_probe() -> str:
    return """module m266MatchBoolNegative;

fn classify(flag: bool) -> i32 {
  match (flag) {
    case true: {
      return 10;
    }
  }
  return 30;
}

fn main(flag: bool) -> i32 {
  return classify(flag);
}
"""


def make_result_negative_probe() -> str:
    return """module m266MatchResultNegative;

fn classify(token: id) -> i32 {
  match (token) {
    case .Ok(let value): {
      return 20;
    }
  }
  return 30;
}

fn main(token: id) -> i32 {
  return classify(token);
}
"""


def make_result_observation_probe() -> str:
    return """module m266MatchResultObservation;

fn classify(token: id) -> i32 {
  match (token) {
    case .Ok(let value): {
      value;
      return 20;
    }
    case .Err(let error): {
      error;
      return 21;
    }
  }
  return 30;
}

fn main(token: id) -> i32 {
  return classify(token);
}
"""


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
    semantic_surface: dict[str, Any] = {}
    if manifest_path.exists():
        semantic_surface = load_semantic_surface(manifest_path)
    if ll_path.exists():
        ll_text = read_text(ll_path)
        checks_total += 1
        checks_passed += require(BOUNDARY_COMMENT in ll_text, display_path(ll_path), f"{case_prefix}-IR-01", "Part 5 lowering boundary comment missing from IR", failures)
    return checks_total, checks_passed, {
        "manifest": display_path(manifest_path),
        "ir": display_path(ll_path),
        "obj": display_path(obj_path),
        "semantic_surface": semantic_surface,
    }


def validate_semantic_payload(semantic_surface: dict[str, Any], artifact: str, failures: list[Finding], *, expected_bool_exhaustive_sites: int, expected_non_exhaustive_sites: int) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    payload = semantic_surface.get(SEMANTIC_KEY)
    checks_total += 1
    checks_passed += require(isinstance(payload, dict), artifact, "M266-C003-SEMA-01", "semantic payload missing", failures)
    if not isinstance(payload, dict):
        return checks_total, checks_passed
    expected_exact = [
        ("M266-C003-SEMA-02", "contract_id", SEMANTIC_CONTRACT_ID, "semantic contract id drifted"),
        ("M266-C003-SEMA-03", "match_statement_semantic_sites", 1, "semantic match statement count mismatch"),
        ("M266-C003-SEMA-04", "match_bool_exhaustive_sites", expected_bool_exhaustive_sites, "bool exhaustiveness count mismatch"),
        ("M266-C003-SEMA-05", "match_non_exhaustive_diagnostic_sites", expected_non_exhaustive_sites, "non-exhaustive count mismatch"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)
    checks_total += 1
    checks_passed += require(payload.get("match_exhaustiveness_semantics_landed") is True, artifact, "M266-C003-SEMA-06", "match exhaustiveness semantics must stay live", failures)
    return checks_total, checks_passed


def validate_lowering_payload(semantic_surface: dict[str, Any], artifact: str, failures: list[Finding], *, expected_live_match_sites: int, expected_fail_closed_match_sites: int, expected_ready: bool) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    payload = semantic_surface.get(LOWERING_KEY)
    checks_total += 1
    checks_passed += require(isinstance(payload, dict), artifact, "M266-C003-LOW-01", "lowering payload missing", failures)
    if not isinstance(payload, dict):
        return checks_total, checks_passed, {}
    expected_exact = [
        ("M266-C003-LOW-02", "contract_id", LOWERING_CONTRACT_ID, "lowering contract id drifted"),
        ("M266-C003-LOW-03", "surface_path", SURFACE_PATH, "lowering surface path drifted"),
        ("M266-C003-LOW-04", "match_statement_sites", 1, "match statement count mismatch"),
        ("M266-C003-LOW-05", "live_match_dispatch_sites", expected_live_match_sites, "live match site count mismatch"),
        ("M266-C003-LOW-06", "fail_closed_match_dispatch_sites", expected_fail_closed_match_sites, "fail-closed match site count mismatch"),
    ]
    for check_id, field, expected, detail in expected_exact:
        checks_total += 1
        checks_passed += require(payload.get(field) == expected, artifact, check_id, detail, failures)
    checks_total += 1
    checks_passed += require(payload.get("ready_for_native_match_lowering") is expected_ready, artifact, "M266-C003-LOW-07", "ready_for_native_match_lowering drifted", failures)
    checks_total += 1
    checks_passed += require(bool(payload.get("replay_key")), artifact, "M266-C003-LOW-08", "replay key missing", failures)
    return checks_total, checks_passed, payload


def validate_negative_probe(completed: subprocess.CompletedProcess[str], out_dir: Path, artifact: str, failures: list[Finding], *, expected_code: str, expected_text: str) -> tuple[int, int]:
    checks_total = 2
    checks_passed = 0
    output = (completed.stdout or "") + (completed.stderr or "") + diagnostics_text(out_dir)
    checks_passed += require(completed.returncode != 0, artifact, "NEG-01", "negative probe should fail", failures)
    checks_passed += require(expected_code in output and expected_text in output, artifact, "NEG-02", f"expected diagnostic {expected_code} missing", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    ensure_summary = ROOT / "tmp" / "reports" / "m266" / "M266-C003" / "ensure_build_summary.json"
    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m266-c003-readiness",
        "--summary-out",
        str(ensure_summary),
    ])
    checks_total += 2
    checks_passed += require(ensure_build.returncode == 0, display_path(ensure_summary), "M266-C003-DYN-01", "ensure build failed", failures)
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M266-C003-DYN-02", "native executable missing", failures)

    run_tag = f"run_{uuid4().hex[:10]}"
    generated_root = ROOT / "tmp" / "reports" / "m266" / "M266-C003" / "generated_probes" / run_tag
    compilation_root = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m266" / "c003" / run_tag
    bool_positive_src = generated_root / "bool_positive.objc3"
    binding_positive_src = generated_root / "binding_positive.objc3"
    bool_negative_src = generated_root / "bool_negative.objc3"
    result_negative_src = generated_root / "result_negative.objc3"
    result_observation_src = generated_root / "result_observation.objc3"

    write_probe(bool_positive_src, make_bool_positive_probe())
    write_probe(binding_positive_src, make_binding_positive_probe())
    write_probe(bool_negative_src, make_bool_negative_probe())
    write_probe(result_negative_src, make_result_negative_probe())
    write_probe(result_observation_src, make_result_observation_probe())

    probe_results: dict[str, Any] = {}

    bool_positive_out = compilation_root / "bool_positive"
    bool_positive = run_native_probe(args.native_exe, bool_positive_src, bool_positive_out)
    checks_total += 1
    checks_passed += require(bool_positive.returncode == 0, display_path(bool_positive_src), "M266-C003-DYN-03", "bool match probe should compile", failures)
    bool_positive_info: dict[str, Any] = {"diagnostics": diagnostics_text(bool_positive_out)}
    if bool_positive.returncode == 0:
        total, passed, success_info = validate_success_artifacts(bool_positive_out, failures, "M266-C003-BOOL")
        checks_total += total
        checks_passed += passed
        bool_positive_info.update(success_info)
        if success_info["semantic_surface"]:
            total, passed = validate_semantic_payload(success_info["semantic_surface"], str(success_info["manifest"]), failures, expected_bool_exhaustive_sites=1, expected_non_exhaustive_sites=0)
            checks_total += total
            checks_passed += passed
            total, passed, payload = validate_lowering_payload(success_info["semantic_surface"], str(success_info["manifest"]), failures, expected_live_match_sites=1, expected_fail_closed_match_sites=0, expected_ready=True)
            checks_total += total
            checks_passed += passed
            bool_positive_info["lowering_payload"] = payload
    probe_results["bool_positive"] = {"returncode": bool_positive.returncode, **bool_positive_info}

    binding_positive_out = compilation_root / "binding_positive"
    binding_positive = run_native_probe(args.native_exe, binding_positive_src, binding_positive_out)
    checks_total += 1
    checks_passed += require(binding_positive.returncode == 0, display_path(binding_positive_src), "M266-C003-DYN-04", "binding match probe should compile", failures)
    binding_positive_info: dict[str, Any] = {"diagnostics": diagnostics_text(binding_positive_out)}
    if binding_positive.returncode == 0:
        total, passed, success_info = validate_success_artifacts(binding_positive_out, failures, "M266-C003-BIND")
        checks_total += total
        checks_passed += passed
        binding_positive_info.update(success_info)
        if success_info["semantic_surface"]:
            total, passed = validate_semantic_payload(success_info["semantic_surface"], str(success_info["manifest"]), failures, expected_bool_exhaustive_sites=0, expected_non_exhaustive_sites=0)
            checks_total += total
            checks_passed += passed
            total, passed, payload = validate_lowering_payload(success_info["semantic_surface"], str(success_info["manifest"]), failures, expected_live_match_sites=1, expected_fail_closed_match_sites=0, expected_ready=True)
            checks_total += total
            checks_passed += passed
            binding_positive_info["lowering_payload"] = payload
    probe_results["binding_positive"] = {"returncode": binding_positive.returncode, **binding_positive_info}

    bool_negative_out = compilation_root / "bool_negative"
    bool_negative = run_native_probe(args.native_exe, bool_negative_src, bool_negative_out)
    total, passed = validate_negative_probe(bool_negative, bool_negative_out, display_path(bool_negative_src), failures, expected_code="O3S206", expected_text="match statement must be exhaustive")
    checks_total += total
    checks_passed += passed
    probe_results["bool_negative"] = {"returncode": bool_negative.returncode, "diagnostics": diagnostics_text(bool_negative_out)}

    result_negative_out = compilation_root / "result_negative"
    result_negative = run_native_probe(args.native_exe, result_negative_src, result_negative_out)
    total, passed = validate_negative_probe(result_negative, result_negative_out, display_path(result_negative_src), failures, expected_code="O3S206", expected_text="match statement must be exhaustive")
    checks_total += total
    checks_passed += passed
    probe_results["result_negative"] = {"returncode": result_negative.returncode, "diagnostics": diagnostics_text(result_negative_out)}

    result_observation_out = compilation_root / "result_observation"
    result_observation = run_native_probe(args.native_exe, result_observation_src, result_observation_out)
    total, passed = validate_negative_probe(result_observation, result_observation_out, display_path(result_observation_src), failures, expected_code="O3L300", expected_text="result-case match lowering remains fail-closed")
    checks_total += total
    checks_passed += passed
    probe_results["result_observation"] = {"returncode": result_observation.returncode, "diagnostics": diagnostics_text(result_observation_out)}

    dynamic = {
        "skipped": False,
        "ensure_build_summary": display_path(ensure_summary),
        "run_tag": run_tag,
        "probe_results": probe_results,
    }
    return checks_total, checks_passed, dynamic


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        total, static_failures = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += total - len(static_failures)
        failures.extend(static_failures)

    dynamic: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        total, passed, dynamic = run_dynamic_probes(args, failures)
        checks_total += total
        checks_passed += passed

    summary = {
        "ok": not failures,
        "mode": MODE,
        "issue": ISSUE,
        "contract_id": ISSUE_CONTRACT_ID,
        "surface_path": SURFACE_PATH,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
