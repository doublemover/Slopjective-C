#!/usr/bin/env python3
"""Fail-closed validator for M243-C001 lowering/runtime diagnostics surfacing freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-lowering-runtime-diagnostics-surfacing-freeze-contract-c001-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_c001_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c001_lowering_runtime_diagnostics_surfacing_contract_and_architecture_freeze_packet.md",
    "frontend_artifacts_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_artifacts.cpp",
    "diagnostics_artifacts_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_diagnostics_artifacts.cpp",
    "cli_driver_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "driver"
    / "objc3_objc3_path.cpp",
    "c_api_frontend_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "libobjc3c_frontend"
    / "frontend_anchor.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M243-C001-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-freeze/m243-c001-v1`",
        ),
        (
            "M243-C001-DOC-02",
            "scripts/check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py",
        ),
        (
            "M243-C001-DOC-03",
            "tests/tooling/test_check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py",
        ),
        (
            "M243-C001-DOC-04",
            "tmp/reports/m243/M243-C001/lowering_runtime_diagnostics_surfacing_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M243-C001-PKT-01", "Packet: `M243-C001`"),
        ("M243-C001-PKT-02", "Dependencies: none"),
        (
            "M243-C001-PKT-03",
            "scripts/check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py",
        ),
    ),
    "frontend_artifacts_source": (
        ("M243-C001-ART-01", "bundle.diagnostics = FlattenStageDiagnostics(bundle.stage_diagnostics);"),
        ("M243-C001-ART-02", "LLVM IR emission failed: parse-to-lowering readiness check failed: "),
        ("M243-C001-ART-03", "IsObjc3IREmissionCompletenessCoreFeatureReady("),
        ("M243-C001-ART-04", '"O3L302"'),
        (
            "M243-C001-ART-05",
            "if (!EmitObjc3IRText(pipeline_result.program.ast, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {",
        ),
        (
            "M243-C001-ART-06",
            'bundle.post_pipeline_diagnostics = {MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: " + ir_error)};',
        ),
    ),
    "diagnostics_artifacts_source": (
        (
            "M243-C001-IO-01",
            "diagnostics.reserve(stage_diagnostics.size() + post_pipeline_diagnostics.size());",
        ),
        (
            "M243-C001-IO-02",
            "diagnostics.insert(diagnostics.end(), post_pipeline_diagnostics.begin(), post_pipeline_diagnostics.end());",
        ),
        (
            "M243-C001-IO-03",
            "const std::vector<std::string> diagnostics = FlattenStageDiagnostics(stage_diagnostics, post_pipeline_diagnostics);",
        ),
        ("M243-C001-IO-04", "const DiagSortKey key = ParseDiagSortKey(diagnostics[i]);"),
    ),
    "cli_driver_source": (
        ("M243-C001-CLI-01", "WriteDiagnosticsArtifacts(cli_options.out_dir,"),
        ("M243-C001-CLI-02", "artifacts.stage_diagnostics,"),
        ("M243-C001-CLI-03", "artifacts.post_pipeline_diagnostics);"),
    ),
    "c_api_frontend_source": (
        (
            "M243-C001-CAPI-01",
            "std::vector<std::string> emit_diagnostics = product.artifact_bundle.post_pipeline_diagnostics;",
        ),
        ("M243-C001-CAPI-02", "BuildDiagnosticsJson(product.artifact_bundle.diagnostics)"),
        (
            "M243-C001-CAPI-03",
            "result->emit = BuildStageSummary(OBJC3C_FRONTEND_STAGE_EMIT, emit_attempted, emit_skipped, emit_diagnostics);",
        ),
    ),
    "architecture_doc": (
        (
            "M243-C001-ARC-01",
            "M243 lane-C C001 lowering/runtime diagnostics surfacing anchors explicit",
        ),
        ("M243-C001-ARC-02", "pipeline/objc3_frontend_artifacts.cpp"),
        ("M243-C001-ARC-03", "io/objc3_diagnostics_artifacts.cpp"),
    ),
    "package_json": (
        (
            "M243-C001-CFG-01",
            '"check:objc3c:m243-c001-lowering-runtime-diagnostics-surfacing-contract"',
        ),
        (
            "M243-C001-CFG-02",
            '"test:tooling:m243-c001-lowering-runtime-diagnostics-surfacing-contract"',
        ),
        ("M243-C001-CFG-03", '"check:objc3c:m243-c001-lane-c-readiness"'),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m243/M243-C001/lowering_runtime_diagnostics_surfacing_contract_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
