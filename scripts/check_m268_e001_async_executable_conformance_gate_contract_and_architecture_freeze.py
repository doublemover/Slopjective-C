#!/usr/bin/env python3
"""Fail-closed checker for M268-E001 async executable conformance gate."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m268-e001-async-executable-conformance-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-async-executable-conformance-gate/m268-e001-v1"
EVIDENCE_MODEL = "a002-b003-c003-d002-summary-chain-plus-canonical-async-fixture-proof"
FAILURE_MODEL = "fail-closed-on-async-gate-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M268-E002"

A002_CONTRACT_ID = "objc3c-part7-async-source-closure/m268-a002-v1"
B003_CONTRACT_ID = "objc3c-part7-async-diagnostics-compatibility-completion/m268-b003-v1"
C003_CONTRACT_ID = "objc3c-part7-suspension-autorelease-cleanup-integration/m268-c003-v1"
D002_CONTRACT_ID = "objc3c-part7-live-continuation-runtime-integration/m268-d002-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m268_async_executable_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m268" / "m268_e001_async_executable_conformance_gate_contract_and_architecture_freeze_packet.md"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m268_e001_lane_e_readiness.py"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m268_d002_live_continuation_runtime_integration_positive.objc3"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m268" / "M268-E001" / "async_executable_conformance_gate_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m268" / "e001"
IR_BOUNDARY_PREFIX = "; part7_live_continuation_runtime_integration = contract=" + D002_CONTRACT_ID
IR_NAMED_METADATA = "!objc3.objc_part7_live_continuation_runtime_integration = !{!92}"

UPSTREAM_SUMMARIES: tuple[tuple[str, Path, str], ...] = (
    ("M268-A002", ROOT / "tmp" / "reports" / "m268" / "M268-A002" / "async_semantic_packet_summary.json", A002_CONTRACT_ID),
    ("M268-B003", ROOT / "tmp" / "reports" / "m268" / "M268-B003" / "async_diagnostics_compatibility_completion_summary.json", B003_CONTRACT_ID),
    ("M268-C003", ROOT / "tmp" / "reports" / "m268" / "M268-C003" / "async_cleanup_integration_summary.json", C003_CONTRACT_ID),
    ("M268-D002", ROOT / "tmp" / "reports" / "m268" / "M268-D002" / "live_continuation_runtime_integration_summary.json", D002_CONTRACT_ID),
)


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
        SnippetCheck("M268-E001-EXP-01", "# M268 Async Executable Conformance Gate Contract And Architecture Freeze Expectations (E001)"),
        SnippetCheck("M268-E001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M268-E001-EXP-03", "M268-A002"),
        SnippetCheck("M268-E001-EXP-04", "The gate must explicitly hand off to `M268-E002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M268-E001-PKT-01", "# M268-E001 Async Executable Conformance Gate Contract And Architecture Freeze Packet"),
        SnippetCheck("M268-E001-PKT-02", "Issue: `#7292`"),
        SnippetCheck("M268-E001-PKT-03", "- `M268-A002`"),
        SnippetCheck("M268-E001-PKT-04", "- `M268-D002`"),
        SnippetCheck("M268-E001-PKT-05", "Next issue: `M268-E002`"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M268-E001-SRC-01", "## M268 async executable conformance gate (M268-E001)"),
        SnippetCheck("M268-E001-SRC-02", "M268-A002"),
        SnippetCheck("M268-E001-SRC-03", "M268-E002"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M268-E001-NDOC-01", "## M268 async executable conformance gate (M268-E001)"),
        SnippetCheck("M268-E001-NDOC-02", "M268-A002"),
        SnippetCheck("M268-E001-NDOC-03", "M268-E002"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M268-E001-ABS-01", "M268-E001 async executable conformance gate note:"),
        SnippetCheck("M268-E001-ABS-02", "M268-D002"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M268-E001-ATTR-01", "Current implementation status (`M268-E001`):"),
        SnippetCheck("M268-E001-ATTR-02", "M268-B003"),
        SnippetCheck("M268-E001-ATTR-03", "M268-E002"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M268-E001-DRV-01", "M268-E001 async executable conformance gate anchor"),
        SnippetCheck("M268-E001-DRV-02", "emitted manifest/IR/object triplet"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M268-E001-MAN-01", "M268-E001 async executable conformance gate anchor"),
        SnippetCheck("M268-E001-MAN-02", "paired IR/object artifacts"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M268-E001-FAPI-01", "M268-E001 async executable conformance gate anchor"),
        SnippetCheck("M268-E001-FAPI-02", "second frontend async proof channel"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M268-E001-RUN-01", "run_m268_a002_lane_a_readiness.py"),
        SnippetCheck("M268-E001-RUN-02", "run_m268_b003_lane_b_readiness.py"),
        SnippetCheck("M268-E001-RUN-03", "run_m268_c003_lane_c_readiness.py"),
        SnippetCheck("M268-E001-RUN-04", "run_m268_d002_lane_d_readiness.py"),
        SnippetCheck("M268-E001-RUN-05", "test_check_m268_e001_async_executable_conformance_gate_contract_and_architecture_freeze.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M268-E001-PKG-01", '"check:objc3c:m268-e001-async-executable-conformance-gate"'),
        SnippetCheck("M268-E001-PKG-02", '"test:tooling:m268-e001-async-executable-conformance-gate"'),
        SnippetCheck("M268-E001-PKG-03", '"check:objc3c:m268-e001-lane-e-readiness"'),
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


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_process(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT if cwd is None else cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M268-E001-MISSING", f"required artifact missing: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def semantic_surface_from_manifest(path: Path) -> dict[str, Any]:
    manifest = read_json(path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    surface = pipeline.get("semantic_surface")
    if not isinstance(surface, dict):
        raise TypeError(f"missing semantic surface in {display_path(path)}")
    return surface


def compile_fixture(*, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    args = [str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"]
    return run_process(args)


def validate_upstream_summaries(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    upstream: dict[str, Any] = {}
    for issue, path, contract_id in UPSTREAM_SUMMARIES:
        artifact = display_path(path)
        checks_total += 1
        checks_passed += require(path.exists(), artifact, f"{issue}-SUM-01", "missing upstream summary", failures)
        if not path.exists():
            upstream[issue] = {"missing": True}
            continue
        payload = read_json(path)
        checks_total += 1
        checks_passed += require(payload.get("contract_id") == contract_id, artifact, f"{issue}-SUM-02", "upstream contract drifted", failures)
        checks_total += 1
        checks_passed += require(payload.get("checks_total", 0) == payload.get("checks_passed", -1), artifact, f"{issue}-SUM-03", "upstream summary lost full coverage", failures)
        checks_total += 1
        checks_passed += require(payload.get("checks_total", 0) > 0, artifact, f"{issue}-SUM-04", "upstream summary reports zero checks", failures)
        upstream[issue] = {
            "contract_id": payload.get("contract_id"),
            "checks_total": payload.get("checks_total"),
            "checks_passed": payload.get("checks_passed"),
        }
    return checks_total, checks_passed, upstream


def validate_happy_path(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    out_dir = PROBE_ROOT / "fixture"
    out_dir.mkdir(parents=True, exist_ok=True)

    completed = compile_fixture(fixture=FIXTURE, out_dir=out_dir)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"

    checks_total += 1
    checks_passed += require(completed.returncode == 0, display_path(FIXTURE), "M268-E001-DYN-01", completed.stderr or completed.stdout or "fixture compile failed", failures)
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M268-E001-DYN-02", "manifest missing", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M268-E001-DYN-03", "IR missing", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(obj_path), "M268-E001-DYN-04", "object missing", failures)
    if completed.returncode != 0 or not manifest_path.exists() or not ir_path.exists() or not obj_path.exists():
        return checks_passed, checks_total, {"compile_stdout": completed.stdout, "compile_stderr": completed.stderr}

    surface = semantic_surface_from_manifest(manifest_path)
    checks_total += 1
    checks_passed += require(surface.get("objc_part7_async_source_closure", {}).get("contract_id") == A002_CONTRACT_ID, display_path(manifest_path), "M268-E001-DYN-05", "A002 semantic packet missing", failures)
    checks_total += 1
    checks_passed += require(surface.get("objc_part7_async_diagnostics_and_compatibility_completion", {}).get("contract_id") == B003_CONTRACT_ID, display_path(manifest_path), "M268-E001-DYN-06", "B003 semantic packet missing", failures)
    checks_total += 1
    checks_passed += require(surface.get("objc_part7_suspension_autorelease_and_cleanup_integration", {}).get("contract_id") == C003_CONTRACT_ID, display_path(manifest_path), "M268-E001-DYN-07", "C003 semantic packet missing", failures)

    ir_text = read_text(ir_path)
    checks_total += 1
    checks_passed += require(IR_BOUNDARY_PREFIX in ir_text, display_path(ir_path), "M268-E001-DYN-08", "D002 IR boundary missing", failures)
    checks_total += 1
    checks_passed += require(IR_NAMED_METADATA in ir_text, display_path(ir_path), "M268-E001-DYN-09", "D002 IR metadata missing", failures)
    checks_total += 1
    checks_passed += require("define i32 @runTask()" in ir_text, display_path(ir_path), "M268-E001-DYN-10", "runTask definition missing from gate fixture IR", failures)
    checks_total += 1
    checks_passed += require("define i32 @objc3_method_Loader_instance_loadValue()" in ir_text, display_path(ir_path), "M268-E001-DYN-11", "async Objective-C method missing from gate fixture IR", failures)

    return checks_passed, checks_total, {
        "manifest_path": display_path(manifest_path),
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
        "manifest_contracts": {
            "a002": surface.get("objc_part7_async_source_closure", {}).get("contract_id"),
            "b003": surface.get("objc_part7_async_diagnostics_and_compatibility_completion", {}).get("contract_id"),
            "c003": surface.get("objc_part7_suspension_autorelease_and_cleanup_integration", {}).get("contract_id"),
        },
        "ir_live_boundary_present": IR_BOUNDARY_PREFIX in ir_text and IR_NAMED_METADATA in ir_text,
        "object_artifact_present": obj_path.exists(),
    }


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_summary: dict[str, Any] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        total = len(snippets)
        passed = ensure_snippets(path, snippets, failures)
        checks_total += total
        checks_passed += passed
        static_summary[display_path(path)] = {"checks": total, "ok": passed == total}

    upstream_total, upstream_passed, upstream_summary = validate_upstream_summaries(failures)
    checks_total += upstream_total
    checks_passed += upstream_passed

    dynamic: dict[str, Any] = {"skipped": skip_dynamic_probes}
    if not skip_dynamic_probes:
        build = run_process([
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m268-e001-async-executable-conformance-gate",
            "--summary-out",
            str(ROOT / "tmp" / "reports" / "m268" / "M268-E001" / "ensure_objc3c_native_build_summary.json"),
        ])
        checks_total += 1
        checks_passed += require(build.returncode == 0, display_path(BUILD_HELPER), "M268-E001-BUILD-01", build.stderr or build.stdout or "fast native build failed", failures)
        checks_total += 1
        checks_passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M268-E001-BUILD-02", "native executable missing after build", failures)
        if not failures:
            dyn_total, dyn_passed, dyn_payload = validate_happy_path(failures)
            checks_total += dyn_total
            checks_passed += dyn_passed
            dynamic = {"skipped": False, "happy_path": dyn_payload}

    summary = {
        "issue": "M268-E001",
        "contract_id": CONTRACT_ID,
        "mode": MODE,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "ok": not failures,
        "dynamic_probes_executed": not skip_dynamic_probes,
        "static": static_summary,
        "upstream": upstream_summary,
        "dynamic": dynamic,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }
    return summary, failures


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    summary, failures = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
