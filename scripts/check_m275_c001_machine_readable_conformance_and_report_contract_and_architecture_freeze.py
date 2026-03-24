#!/usr/bin/env python3
"""Checker for M275-C001 machine-readable conformance/report contract."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m275-c001-part12-machine-readable-conformance-report-contract-v1"
CONTRACT_ID = "objc3c-part12-machine-readable-conformance-report-contract/m275-c001-v1"
DEPENDENCY_CONTRACT_ID = "objc3c-part12-legacy-canonical-migration-semantics/m275-b003-v1"
LOWERING_CONTRACT_ID = "objc3c-versioned-conformance-report-lowering/m264-c001-v1"
RUNTIME_CAPABILITY_CONTRACT_ID = "objc3c-runtime-capability-reporting/m264-c002-v1"
SURFACE_KEY = "objc_part12_machine_readable_conformance_report_contract"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m275" / "M275-C001" / "machine_readable_conformance_report_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m275_machine_readable_conformance_and_report_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m275" / "m275_c001_machine_readable_conformance_and_report_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_ABSTRACT = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m275_c001_machine_readable_conformance_report_positive.objc3"
REPORT_SUFFIX = ".objc3-conformance-report.json"


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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M275-C001-MISSING", f"missing artifact: {display_path(path)}"))
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


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m275-c001-readiness",
        "--summary-out",
        "tmp/reports/m275/M275-C001/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M275-C001-DYN-01", ensure_build.stderr or ensure_build.stdout or "fast build failed", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "c001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-object",
        "--objc3-compat-mode",
        "canonical",
        "--objc3-migration-assist",
    ])
    manifest_path = out_dir / "module.manifest.json"
    summary_path = out_dir / "module.c_api_summary.json"
    diagnostics_path = out_dir / "module.diagnostics.json"
    ir_path = out_dir / "module.ll"
    report_path = out_dir / f"module{REPORT_SUFFIX}"

    checks_total += 5
    checks_passed += require(completed.returncode == 0, display_path(FIXTURE), "M275-C001-DYN-02", completed.stderr or completed.stdout or "frontend runner failed", failures)
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M275-C001-DYN-03", "manifest missing", failures)
    checks_passed += require(summary_path.exists(), display_path(summary_path), "M275-C001-DYN-04", "summary missing", failures)
    checks_passed += require(diagnostics_path.exists(), display_path(diagnostics_path), "M275-C001-DYN-05", "diagnostics missing", failures)
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M275-C001-DYN-06", "IR missing", failures)

    checks_total += 1
    checks_passed += require(report_path.exists(), display_path(report_path), "M275-C001-DYN-07", "conformance report sidecar missing", failures)

    manifest_payload = load_json(manifest_path) if manifest_path.exists() else {}
    summary_payload = load_json(summary_path) if summary_path.exists() else {}
    diagnostics_payload = load_json(diagnostics_path) if diagnostics_path.exists() else {}
    report_payload = load_json(report_path) if report_path.exists() else {}

    frontend = manifest_payload.get("frontend", {}) if isinstance(manifest_payload, dict) else {}
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    semantic_surface = pipeline.get("semantic_surface", {}) if isinstance(pipeline, dict) else {}
    packet = semantic_surface.get(SURFACE_KEY, {}) if isinstance(semantic_surface, dict) else {}

    checks_total += 13
    checks_passed += require(isinstance(packet, dict), display_path(manifest_path), "M275-C001-DYN-08", "part12 report packet missing", failures)
    checks_passed += require(packet.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M275-C001-DYN-09", "contract id mismatch", failures)
    checks_passed += require(packet.get("dependency_contract_id") == DEPENDENCY_CONTRACT_ID, display_path(manifest_path), "M275-C001-DYN-10", "dependency contract mismatch", failures)
    checks_passed += require(packet.get("lowering_contract_id") == LOWERING_CONTRACT_ID, display_path(manifest_path), "M275-C001-DYN-11", "lowering contract mismatch", failures)
    checks_passed += require(packet.get("runtime_capability_contract_id") == RUNTIME_CAPABILITY_CONTRACT_ID, display_path(manifest_path), "M275-C001-DYN-12", "runtime capability contract mismatch", failures)
    checks_passed += require(packet.get("effective_compatibility_mode") == "canonical", display_path(manifest_path), "M275-C001-DYN-13", "compatibility mode drifted", failures)
    checks_passed += require(packet.get("migration_semantics_ready") is True, display_path(manifest_path), "M275-C001-DYN-14", "migration semantics prerequisite missing", failures)
    checks_passed += require(packet.get("lowering_contract_ready") is True, display_path(manifest_path), "M275-C001-DYN-15", "lowering contract not ready", failures)
    checks_passed += require(packet.get("runtime_capability_surface_published") is True, display_path(manifest_path), "M275-C001-DYN-16", "runtime capability publication not ready", failures)
    checks_passed += require(packet.get("deterministic_handoff") is True, display_path(manifest_path), "M275-C001-DYN-17", "deterministic handoff missing", failures)
    checks_passed += require(packet.get("ready_for_runtime_publication") is True, display_path(manifest_path), "M275-C001-DYN-18", "runtime publication readiness missing", failures)
    checks_passed += require(packet.get("artifact_suffix") == REPORT_SUFFIX, display_path(manifest_path), "M275-C001-DYN-19", "artifact suffix drifted", failures)
    checks_passed += require(packet.get("artifact_schema_id") == "objc3c-versioned-conformance-report-v1", display_path(manifest_path), "M275-C001-DYN-20", "artifact schema drifted", failures)

    checks_total += 5
    checks_passed += require(report_payload.get("contract_id") == LOWERING_CONTRACT_ID, display_path(report_path), "M275-C001-DYN-21", "report contract id drifted", failures)
    checks_passed += require(report_payload.get("schema_id") == "objc3c-versioned-conformance-report-v1", display_path(report_path), "M275-C001-DYN-22", "report schema drifted", failures)
    checks_passed += require(report_payload.get("migration_assist_enabled") is True, display_path(report_path), "M275-C001-DYN-23", "report migration assist flag drifted", failures)
    checks_passed += require(isinstance(report_payload.get("runtime_capability_report"), dict), display_path(report_path), "M275-C001-DYN-24", "runtime capability payload missing", failures)
    checks_passed += require(isinstance(report_payload.get("public_conformance_report"), dict), display_path(report_path), "M275-C001-DYN-25", "public conformance payload missing", failures)

    checks_total += 3
    checks_passed += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M275-C001-DYN-26", "semantic stage should run", failures)
    diag_list = diagnostics_payload.get("diagnostics", []) if isinstance(diagnostics_payload, dict) else []
    checks_passed += require(isinstance(diag_list, list) and len(diag_list) == 0, display_path(diagnostics_path), "M275-C001-DYN-27", "positive fixture emitted diagnostics", failures)
    checks_passed += require("part12_machine_readable_conformance_report_contract = " in read_text(ir_path), display_path(ir_path), "M275-C001-DYN-28", "IR Part 12 report anchor missing", failures)

    return checks_total, checks_passed, {
        "manifest": display_path(manifest_path),
        "report": display_path(report_path),
        "ir": display_path(ir_path),
        "packet": packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M275-C001-EXP-01", "# M275 Machine-Readable Conformance And Report Contract And Architecture Freeze Expectations (C001)"),
            SnippetCheck("M275-C001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M275-C001-PKT-01", "# M275-C001 Packet: Machine-readable conformance and report contract - Contract and architecture freeze"),
            SnippetCheck("M275-C001-PKT-02", "frontend.pipeline.semantic_surface.objc_part12_machine_readable_conformance_report_contract"),
            SnippetCheck("M275-C001-PKT-03", "M275-C002"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M275-C001-DOCSRC-01", "## Part 12 machine-readable conformance/report contract (M275-C001)"),
            SnippetCheck("M275-C001-DOCSRC-02", "objc_part12_machine_readable_conformance_report_contract"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M275-C001-DOC-01", "## Part 12 machine-readable conformance/report contract (M275-C001)"),
            SnippetCheck("M275-C001-DOC-02", "objc_part12_machine_readable_conformance_report_contract"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M275-C001-CHK-01", "objc_part12_machine_readable_conformance_report_contract"),
        ],
        SPEC_ABSTRACT: [
            SnippetCheck("M275-C001-ABS-01", "M275-C001 machine-readable conformance/report contract note:"),
            SnippetCheck("M275-C001-ABS-02", "objc_part12_machine_readable_conformance_report_contract"),
        ],
        LOWERING_CONTRACT_H: [
            SnippetCheck("M275-C001-H-01", "kObjc3Part12MachineReadableConformanceReportContractId"),
            SnippetCheck("M275-C001-H-02", "kObjc3Part12MachineReadableConformanceReportSurfacePath"),
        ],
        LOWERING_CONTRACT_CPP: [
            SnippetCheck("M275-C001-CPP-01", "Objc3Part12MachineReadableConformanceReportContractLoweringSummary"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M275-C001-TYPE-01", "struct Objc3Part12MachineReadableConformanceReportContractSummary"),
            SnippetCheck("M275-C001-TYPE-02", "IsReadyObjc3Part12MachineReadableConformanceReportContractSummary"),
        ],
        FRONTEND_ARTIFACTS_H: [
            SnippetCheck("M275-C001-ARTH-01", "part12_machine_readable_conformance_report_contract_summary"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M275-C001-ART-01", "BuildPart12MachineReadableConformanceReportContractSummary("),
            SnippetCheck("M275-C001-ART-02", "objc_part12_machine_readable_conformance_report_contract"),
        ],
        IR_EMITTER: [
            SnippetCheck("M275-C001-IR-01", "part12_machine_readable_conformance_report_contract = "),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M275-C001-PKG-01", '"check:objc3c:m275-c001-machine-readable-conformance-report-contract"'),
            SnippetCheck("M275-C001-PKG-02", '"check:objc3c:m275-c001-lane-c-readiness"'),
        ],
    }

    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    dynamic_summary: dict[str, object] = {"skipped": True}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic_summary = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_summary": dynamic_summary,
        "next_issue": "M275-C002",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M275-C001 machine-readable conformance/report contract checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
