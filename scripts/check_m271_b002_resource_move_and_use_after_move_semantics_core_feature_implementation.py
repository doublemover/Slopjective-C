#!/usr/bin/env python3
"""Checker for M271-B002 resource move/use-after-move semantics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m271-b002-part8-resource-move-use-after-move-semantics-v1"
CONTRACT_ID = "objc3c-part8-resource-move-use-after-move-semantics/m271-b002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part8_resource_move_and_use_after_move_semantics"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m271" / "M271-B002" / "resource_move_use_after_move_semantics_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m271_resource_move_and_use_after_move_semantics_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m271" / "m271_b002_resource_move_and_use_after_move_semantics_core_feature_implementation_packet.md"
DOC_GRAMMAR = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_CONFORMANCE = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m271_resource_move_use_after_move_positive.objc3"
NEGATIVE_CASES = [
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m271_b002_non_resource_move_rejected.objc3", "non_resource_move", "O3S295", "move capture requires a cleanup or resource-backed local"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m271_b002_use_after_move_rejected.objc3", "use_after_move", "O3S296", "is used after move capture transferred cleanup ownership"),
    (ROOT / "tests" / "tooling" / "fixtures" / "native" / "m271_b002_duplicate_move_rejected.objc3", "duplicate_move", "O3S297", "was already transferred by an earlier move capture"),
]

EXPECTED_COUNTS = {
    "cleanup_owned_local_sites": 2,
    "resource_move_capture_sites": 1,
    "illegal_non_resource_move_sites": 0,
    "illegal_use_after_move_sites": 0,
    "illegal_duplicate_move_sites": 0,
}
EXPECTED_TRUE_FIELDS = [
    "dependency_required",
    "cleanup_ownership_transfer_enforced",
    "use_after_move_fail_closed",
    "duplicate_move_fail_closed",
    "borrowed_escape_semantics_deferred",
    "retainable_family_legality_deferred",
    "lowering_runtime_deferred",
    "deterministic",
    "ready_for_lowering_and_runtime",
]


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
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M271-B002-MISSING", f"missing artifact: {display_path(path)}"))
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
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def extract_payload(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    return manifest["frontend"]["pipeline"]["semantic_surface"]["objc_part8_resource_move_and_use_after_move_semantics"]


def validate_positive_payload(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    total = 0
    passed = 0
    total += 1
    passed += require(payload.get("contract_id") == CONTRACT_ID, artifact, "M271-B002-PAYLOAD-01", "contract_id mismatch", failures)
    total += 1
    passed += require(payload.get("dependency_contract_id") == "objc3c-part8-system-extension-semantic-model/m271-b001-v1", artifact, "M271-B002-PAYLOAD-02", "dependency_contract_id mismatch", failures)
    total += 1
    passed += require(payload.get("surface_path") == SURFACE_PATH, artifact, "M271-B002-PAYLOAD-03", "surface_path mismatch", failures)
    for index, (field, expected) in enumerate(EXPECTED_COUNTS.items(), start=4):
        total += 1
        passed += require(payload.get(field) == expected, artifact, f"M271-B002-PAYLOAD-{index:02d}", f"{field} mismatch", failures)
    for index, field in enumerate(EXPECTED_TRUE_FIELDS, start=20):
        total += 1
        passed += require(payload.get(field) is True, artifact, f"M271-B002-PAYLOAD-{index:02d}", f"{field} must stay true", failures)
    total += 1
    passed += require(payload.get("failure_reason") == "", artifact, "M271-B002-PAYLOAD-40", "failure_reason must stay empty", failures)
    total += 1
    passed += require(bool(payload.get("replay_key")), artifact, "M271-B002-PAYLOAD-41", "replay_key missing", failures)
    return total, passed


def run_negative_case(case_index: int, fixture: Path, label: str, code: str, phrase: str, args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m271" / "b002" / label
    out_dir.mkdir(parents=True, exist_ok=True)
    run = run_command([
        str(args.runner_exe),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    output = (run.stdout or "") + (run.stderr or "")
    total += 1
    passed += require(run.returncode != 0, display_path(fixture), f"M271-B002-DYN-{case_index:02d}", "negative fixture unexpectedly succeeded", failures)
    summary_path = out_dir / "module.c_api_summary.json"
    total += 1
    passed += require(summary_path.exists(), display_path(summary_path), f"M271-B002-DYN-{case_index + 1:02d}", "negative summary missing", failures)
    details: dict[str, Any] = {
        "fixture": display_path(fixture),
        "returncode": run.returncode,
        "output": output.strip(),
        "summary": display_path(summary_path),
    }
    if summary_path.exists():
        payload = load_json(summary_path)
        last_error = payload.get("last_error", "")
        diagnostics_path = payload.get("paths", {}).get("diagnostics", "")
        diagnostics_blob = last_error
        if diagnostics_path:
            diagnostics_file = ROOT / diagnostics_path
            if diagnostics_file.exists():
                diagnostics_blob += "\n" + diagnostics_file.read_text(encoding="utf-8")
                details["diagnostics"] = display_path(diagnostics_file)
        total += 1
        passed += require(code in diagnostics_blob, display_path(summary_path), f"M271-B002-DYN-{case_index + 2:02d}", f"missing {code}", failures)
        total += 1
        passed += require(phrase in diagnostics_blob, display_path(summary_path), f"M271-B002-DYN-{case_index + 3:02d}", f"missing wording: {phrase}", failures)
        details["last_error"] = last_error
    return total, passed, details


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m271-b002-readiness",
        "--summary-out",
        "tmp/reports/m271/M271-B002/ensure_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M271-B002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    total += 1
    passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M271-B002-DYN-02", "frontend runner missing after build", failures)
    if ensure_build.returncode != 0 or not args.runner_exe.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    positive_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m271" / "b002" / "positive"
    positive_out_dir.mkdir(parents=True, exist_ok=True)
    positive_run = run_command([
        str(args.runner_exe),
        str(POSITIVE_FIXTURE),
        "--out-dir",
        str(positive_out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    positive_output = (positive_run.stdout or "") + (positive_run.stderr or "")
    total += 1
    passed += require(positive_run.returncode == 0, display_path(POSITIVE_FIXTURE), "M271-B002-DYN-03", f"positive fixture failed: {positive_output}", failures)
    manifest_path = positive_out_dir / "module.manifest.json"
    total += 1
    passed += require(manifest_path.exists(), display_path(manifest_path), "M271-B002-DYN-04", "positive manifest missing", failures)

    dynamic: dict[str, Any] = {
        "positive_fixture": display_path(POSITIVE_FIXTURE),
        "positive_returncode": positive_run.returncode,
        "positive_output": positive_output.strip(),
        "positive_manifest": display_path(manifest_path),
    }
    if manifest_path.exists():
        payload = extract_payload(manifest_path)
        sub_total, sub_passed = validate_positive_payload(payload, display_path(manifest_path), failures)
        total += sub_total
        passed += sub_passed
        dynamic["objc_part8_resource_move_and_use_after_move_semantics"] = payload

    next_case = 5
    negative_results = []
    for fixture, label, code, phrase in NEGATIVE_CASES:
        sub_total, sub_passed, details = run_negative_case(next_case, fixture, label, code, phrase, args, failures)
        total += sub_total
        passed += sub_passed
        negative_results.append(details)
        next_case += 4
    dynamic["negative_cases"] = negative_results
    return total, passed, dynamic


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    total = 0
    passed = 0

    snippets: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M271-B002-EXP-01", "# M271 Resource Move And Use-After-Move Semantics Expectations (B002)"),
            SnippetCheck("M271-B002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M271-B002-EXP-03", SURFACE_PATH),
        ],
        PACKET_DOC: [
            SnippetCheck("M271-B002-PKT-01", "# M271-B002 Packet: Resource Move And Use-After-Move Semantics - Core Feature Implementation"),
            SnippetCheck("M271-B002-PKT-02", "consume the existing `M271-B001` sema packet"),
            SnippetCheck("M271-B002-PKT-03", "O3S295`, `O3S296`,"),
        ],
        DOC_GRAMMAR: [
            SnippetCheck("M271-B002-GRM-01", "## M271 resource move and use-after-move semantics"),
            SnippetCheck("M271-B002-GRM-02", SURFACE_PATH),
        ],
        DOC_NATIVE: [
            SnippetCheck("M271-B002-DOC-01", "## M271 resource move and use-after-move semantics"),
            SnippetCheck("M271-B002-DOC-02", SURFACE_PATH),
        ],
        SPEC_AM: [
            SnippetCheck("M271-B002-AM-01", "M271-B002 resource-move semantic note:"),
            SnippetCheck("M271-B002-AM-02", SURFACE_PATH),
        ],
        SPEC_ATTR: [
            SnippetCheck("M271-B002-ATTR-01", "M271-B002 semantic note:"),
            SnippetCheck("M271-B002-ATTR-02", "objc_part8_resource_move_and_use_after_move_semantics"),
        ],
        SPEC_CONFORMANCE: [
            SnippetCheck("M271-B002-CONF-01", "M271-B002 implementation note:"),
            SnippetCheck("M271-B002-CONF-02", "objc_part8_resource_move_and_use_after_move_semantics"),
        ],
        SEMA_CONTRACT: [
            SnippetCheck("M271-B002-SEMA-01", "kObjc3Part8ResourceMoveUseAfterMoveSemanticsContractId"),
            SnippetCheck("M271-B002-SEMA-02", "struct Objc3Part8ResourceMoveUseAfterMoveSemanticsSummary"),
            SnippetCheck("M271-B002-SEMA-03", "IsReadyObjc3Part8ResourceMoveUseAfterMoveSemanticsSummary("),
        ],
        SEMA_HEADER: [
            SnippetCheck("M271-B002-SEMAH-01", "BuildPart8ResourceMoveUseAfterMoveSemanticsSummary("),
        ],
        SEMA_CPP: [
            SnippetCheck("M271-B002-SEMACPP-01", "DiagnosePart8ResourceMoveSemantics("),
            SnippetCheck("M271-B002-SEMACPP-02", '"O3S295"'),
            SnippetCheck("M271-B002-SEMACPP-03", '"O3S296"'),
            SnippetCheck("M271-B002-SEMACPP-04", '"O3S297"'),
            SnippetCheck("M271-B002-SEMACPP-05", "BuildPart8ResourceMoveUseAfterMoveSemanticsSummary("),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M271-B002-TYP-01", "part8_resource_move_use_after_move_semantics_summary"),
        ],
        FRONTEND_PIPELINE: [
            SnippetCheck("M271-B002-PIPE-01", "result.part8_resource_move_use_after_move_semantics_summary ="),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M271-B002-ART-01", "BuildPart8ResourceMoveUseAfterMoveSemanticsSummaryJson("),
            SnippetCheck("M271-B002-ART-02", "objc_part8_resource_move_and_use_after_move_semantics"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M271-B002-PKG-01", '"check:objc3c:m271-b002-resource-move-and-use-after-move-semantics-core-feature-implementation"'),
            SnippetCheck("M271-B002-PKG-02", '"check:objc3c:m271-b002-lane-b-readiness"'),
        ],
        POSITIVE_FIXTURE: [
            SnippetCheck("M271-B002-FIX-01", "module m271b002;"),
            SnippetCheck("M271-B002-FIX-02", "@cleanup(ReleaseTemp) let temp = 1;"),
            SnippetCheck("M271-B002-FIX-03", "^[move temp, unowned peer, weak value]"),
        ],
    }

    for path, checks in snippets.items():
        total += len(checks)
        passed += ensure_snippets(path, checks, failures)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(args, failures)
        total += dyn_total
        passed += dyn_passed

    ok = not failures
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "surface_path": SURFACE_PATH,
        "ok": ok,
        "checks_total": total,
        "checks_passed": passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_summary": dynamic_payload,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if ok:
        print(f"[ok] M271-B002 resource-move checks passed ({passed}/{total})")
        return 0
    for finding in failures:
        print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
