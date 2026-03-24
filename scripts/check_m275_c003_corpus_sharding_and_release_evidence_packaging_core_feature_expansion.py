#!/usr/bin/env python3
"""Checker for M275-C003 corpus sharding and release-evidence packaging."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m275-c003-part12-corpus-sharding-release-evidence-packaging-v1"
CONTRACT_ID = "objc3c-part12-corpus-sharding-release-evidence-packaging/m275-c003-v1"
DEPENDENCY_CONTRACT_ID = "objc3c-part12-feature-aware-conformance-report-emission/m275-c002-v1"
SURFACE_KEY = "objc_part12_corpus_sharding_release_evidence_packaging"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m275" / "M275-C003" / "corpus_sharding_release_evidence_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m275_corpus_sharding_and_release_evidence_packaging_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m275" / "m275_c003_corpus_sharding_and_release_evidence_packaging_core_feature_expansion_packet.md"
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
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m275_c003_corpus_sharding_release_evidence_positive.objc3"
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
        failures.append(Finding(display_path(path), "M275-C003-MISSING", f"missing artifact: {display_path(path)}"))
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
        "m275-c003-readiness",
        "--summary-out",
        "tmp/reports/m275/M275-C003/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M275-C003-DYN-01", ensure_build.stderr or ensure_build.stdout or "fast build failed", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m275" / "c003" / "positive"
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
    report_path = out_dir / f"module{REPORT_SUFFIX}"
    ir_path = out_dir / "module.ll"
    diagnostics_path = out_dir / "module.diagnostics.json"

    checks_total += 5
    checks_passed += require(completed.returncode == 0, display_path(FIXTURE), "M275-C003-DYN-02", completed.stderr or completed.stdout or "frontend runner failed", failures)
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M275-C003-DYN-03", "manifest missing", failures)
    checks_passed += require(report_path.exists(), display_path(report_path), "M275-C003-DYN-04", "report missing", failures)
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M275-C003-DYN-05", "IR missing", failures)
    checks_passed += require(diagnostics_path.exists(), display_path(diagnostics_path), "M275-C003-DYN-06", "diagnostics missing", failures)

    manifest_payload = load_json(manifest_path) if manifest_path.exists() else {}
    report_payload = load_json(report_path) if report_path.exists() else {}
    diagnostics_payload = load_json(diagnostics_path) if diagnostics_path.exists() else {}
    frontend = manifest_payload.get("frontend", {}) if isinstance(manifest_payload, dict) else {}
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    semantic_surface = pipeline.get("semantic_surface", {}) if isinstance(pipeline, dict) else {}
    packet = semantic_surface.get(SURFACE_KEY, {}) if isinstance(semantic_surface, dict) else {}
    advanced_payload = report_payload.get("advanced_feature_release_evidence", {}) if isinstance(report_payload, dict) else {}

    checks_total += 20
    checks_passed += require(isinstance(packet, dict), display_path(manifest_path), "M275-C003-DYN-07", "part12 C003 packet missing", failures)
    checks_passed += require(packet.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M275-C003-DYN-08", "contract id mismatch", failures)
    checks_passed += require(packet.get("dependency_contract_id") == DEPENDENCY_CONTRACT_ID, display_path(manifest_path), "M275-C003-DYN-09", "dependency contract mismatch", failures)
    checks_passed += require(packet.get("targeted_profile_ids") == ["strict", "strict-concurrency", "strict-system"], display_path(manifest_path), "M275-C003-DYN-10", "targeted profiles drifted", failures)
    checks_passed += require(packet.get("targeted_profile_count") == 3, display_path(manifest_path), "M275-C003-DYN-11", "targeted profile count drifted", failures)
    checks_passed += require(packet.get("corpus_shard_ids") == ["parser", "semantic", "lowering_abi", "module_roundtrip", "diagnostics"], display_path(manifest_path), "M275-C003-DYN-12", "corpus shard ids drifted", failures)
    checks_passed += require(packet.get("corpus_shard_count") == 5, display_path(manifest_path), "M275-C003-DYN-13", "corpus shard count drifted", failures)
    checks_passed += require(packet.get("release_evidence_artifact_ids") == ["EVID-01", "EVID-02", "EVID-03", "EVID-04", "EVID-07", "EVID-08", "EVID-09", "EVID-10", "EVID-11"], display_path(manifest_path), "M275-C003-DYN-14", "release evidence ids drifted", failures)
    checks_passed += require(packet.get("release_evidence_artifact_count") == 9, display_path(manifest_path), "M275-C003-DYN-15", "release evidence artifact count drifted", failures)
    checks_passed += require(packet.get("release_evidence_checklist_path") == "spec/conformance/profile_release_evidence_checklist.md", display_path(manifest_path), "M275-C003-DYN-16", "checklist path drifted", failures)
    checks_passed += require(packet.get("release_evidence_schema_path") == "spec/conformance/objc3_conformance_evidence_bundle_schema.md", display_path(manifest_path), "M275-C003-DYN-17", "schema path drifted", failures)
    checks_passed += require(packet.get("feature_report_payload_emitted") is True, display_path(manifest_path), "M275-C003-DYN-18", "feature report payload flag missing", failures)
    checks_passed += require(packet.get("ready_for_release_evidence_packaging") is True, display_path(manifest_path), "M275-C003-DYN-19", "release evidence packaging readiness missing", failures)
    checks_passed += require(isinstance(advanced_payload, dict), display_path(report_path), "M275-C003-DYN-20", "advanced_feature_release_evidence missing from report", failures)
    checks_passed += require(advanced_payload.get("contract_id") == CONTRACT_ID, display_path(report_path), "M275-C003-DYN-21", "report payload contract mismatch", failures)
    checks_passed += require(advanced_payload.get("corpus_shard_ids") == ["parser", "semantic", "lowering_abi", "module_roundtrip", "diagnostics"], display_path(report_path), "M275-C003-DYN-22", "report shard ids drifted", failures)
    checks_passed += require(advanced_payload.get("release_evidence_artifact_ids") == ["EVID-01", "EVID-02", "EVID-03", "EVID-04", "EVID-07", "EVID-08", "EVID-09", "EVID-10", "EVID-11"], display_path(report_path), "M275-C003-DYN-23", "report release evidence ids drifted", failures)
    checks_passed += require(advanced_payload.get("ready_for_release_evidence_packaging") is True, display_path(report_path), "M275-C003-DYN-24", "report packaging readiness missing", failures)
    checks_passed += require(advanced_payload.get("targeted_profile_ids") == ["strict", "strict-concurrency", "strict-system"], display_path(report_path), "M275-C003-DYN-25", "report targeted profiles drifted", failures)
    checks_passed += require(read_text(ir_path).find("part12_corpus_sharding_release_evidence_packaging = ") >= 0, display_path(ir_path), "M275-C003-DYN-26", "IR Part 12 C003 anchor missing", failures)

    checks_total += 1
    diag_list = diagnostics_payload.get("diagnostics", []) if isinstance(diagnostics_payload, dict) else []
    checks_passed += require(isinstance(diag_list, list) and len(diag_list) == 0, display_path(diagnostics_path), "M275-C003-DYN-27", "positive fixture emitted diagnostics", failures)

    return checks_total, checks_passed, {
        "manifest": display_path(manifest_path),
        "report": display_path(report_path),
        "ir": display_path(ir_path),
        "packet": packet,
        "advanced_feature_release_evidence": advanced_payload,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M275-C003-EXP-01", "# M275 Corpus Sharding And Release-Evidence Packaging - Core Feature Expansion Expectations (C003)"),
            SnippetCheck("M275-C003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M275-C003-PKT-01", "# M275-C003 Packet: Corpus sharding and release-evidence packaging - Core feature expansion"),
            SnippetCheck("M275-C003-PKT-02", "frontend.pipeline.semantic_surface.objc_part12_corpus_sharding_release_evidence_packaging"),
            SnippetCheck("M275-C003-PKT-03", "M275-D001"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M275-C003-DOCSRC-01", "## Part 12 corpus sharding and release-evidence packaging (M275-C003)"),
            SnippetCheck("M275-C003-DOCSRC-02", "advanced_feature_release_evidence"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M275-C003-DOC-01", "## Part 12 corpus sharding and release-evidence packaging (M275-C003)"),
            SnippetCheck("M275-C003-DOC-02", "advanced_feature_release_evidence"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M275-C003-CHK-01", "objc_part12_corpus_sharding_release_evidence_packaging"),
            SnippetCheck("M275-C003-CHK-02", "advanced_feature_release_evidence"),
        ],
        SPEC_ABSTRACT: [
            SnippetCheck("M275-C003-ABS-01", "M275-C003 corpus sharding and release-evidence packaging note:"),
            SnippetCheck("M275-C003-ABS-02", "advanced_feature_release_evidence"),
        ],
        LOWERING_CONTRACT_H: [
            SnippetCheck("M275-C003-H-01", "kObjc3Part12CorpusShardingReleaseEvidencePackagingContractId"),
            SnippetCheck("M275-C003-H-02", "kObjc3Part12CorpusShardingReleaseEvidencePackagingSurfacePath"),
        ],
        LOWERING_CONTRACT_CPP: [
            SnippetCheck("M275-C003-CPP-01", "Objc3Part12CorpusShardingReleaseEvidencePackagingLoweringSummary"),
        ],
        FRONTEND_TYPES: [
            SnippetCheck("M275-C003-TYPE-01", "struct Objc3Part12CorpusShardingReleaseEvidencePackagingSummary"),
            SnippetCheck("M275-C003-TYPE-02", "IsReadyObjc3Part12CorpusShardingReleaseEvidencePackagingSummary"),
        ],
        FRONTEND_ARTIFACTS_H: [
            SnippetCheck("M275-C003-ARTH-01", "part12_corpus_sharding_release_evidence_packaging_summary"),
        ],
        FRONTEND_ARTIFACTS: [
            SnippetCheck("M275-C003-ART-01", "BuildPart12CorpusShardingReleaseEvidencePackagingSummary("),
            SnippetCheck("M275-C003-ART-02", "advanced_feature_release_evidence"),
            SnippetCheck("M275-C003-ART-03", "objc_part12_corpus_sharding_release_evidence_packaging"),
        ],
        IR_EMITTER: [
            SnippetCheck("M275-C003-IR-01", "part12_corpus_sharding_release_evidence_packaging = "),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M275-C003-PKG-01", '"check:objc3c:m275-c003-corpus-sharding-release-evidence-packaging"'),
            SnippetCheck("M275-C003-PKG-02", '"check:objc3c:m275-c003-lane-c-readiness"'),
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
        "next_issue": "M275-D001",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M275-C003 corpus sharding and release-evidence packaging checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
