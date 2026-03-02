#!/usr/bin/env python3
"""Fail-closed validator for M228-A011 lowering pipeline pass-graph performance and quality guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-a011-lowering-pipeline-pass-graph-performance-quality-guardrails-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_lowering_pipeline_pass_graph_core_feature_surface.h",
    "core_surface_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "ir_header": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h",
    "ir_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "a010_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_conformance_corpus_expansion_a010_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_a011_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-A011-H-01", "BuildObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsKey("),
        ("M228-A011-H-02", "IsObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsReady("),
    ),
    "core_surface_source": (
        ("M228-A011-CPP-01", "surface.performance_quality_guardrails_consistent ="),
        ("M228-A011-CPP-02", "surface.performance_quality_guardrails_ready ="),
        ("M228-A011-CPP-03", "surface.performance_quality_guardrails_key ="),
        (
            "M228-A011-CPP-04",
            "BuildObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsKey(",
        ),
        (
            "M228-A011-CPP-05",
            "\"pass-graph performance quality guardrails are inconsistent\"",
        ),
        (
            "M228-A011-CPP-06",
            "\"pass-graph performance quality guardrails are not ready\"",
        ),
        (
            "M228-A011-CPP-07",
            "\"pass-graph performance quality guardrails key is not ready\"",
        ),
    ),
    "frontend_types_header": (
        ("M228-A011-TYP-01", "bool performance_quality_guardrails_consistent = false;"),
        ("M228-A011-TYP-02", "bool performance_quality_guardrails_ready = false;"),
        ("M228-A011-TYP-03", "std::string performance_quality_guardrails_key;"),
    ),
    "artifacts_source": (
        (
            "M228-A011-ART-01",
            "IsObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsReady(",
        ),
        ("M228-A011-ART-02", '"O3L315"'),
        (
            "M228-A011-ART-03",
            "ir_frontend_metadata.lowering_pass_graph_performance_quality_guardrails_ready =",
        ),
        (
            "M228-A011-ART-04",
            "ir_frontend_metadata.lowering_pass_graph_performance_quality_guardrails_key =",
        ),
    ),
    "ir_header": (
        (
            "M228-A011-IRH-01",
            "bool lowering_pass_graph_performance_quality_guardrails_ready = false;",
        ),
        (
            "M228-A011-IRH-02",
            "std::string lowering_pass_graph_performance_quality_guardrails_key;",
        ),
    ),
    "ir_source": (
        (
            "M228-A011-IRS-01",
            'out << "; lowering_pass_graph_performance_quality_guardrails = "',
        ),
        (
            "M228-A011-IRS-02",
            'out << "; lowering_pass_graph_performance_quality_guardrails_ready = "',
        ),
    ),
    "package_json": (
        (
            "M228-A011-CFG-01",
            '"check:objc3c:m228-a011-lowering-pipeline-pass-graph-performance-quality-guardrails-contract"',
        ),
        (
            "M228-A011-CFG-02",
            '"test:tooling:m228-a011-lowering-pipeline-pass-graph-performance-quality-guardrails-contract"',
        ),
        ("M228-A011-CFG-03", '"check:objc3c:m228-a011-lane-a-readiness"'),
        (
            "M228-A011-CFG-04",
            "npm run check:objc3c:m228-a010-lane-a-readiness && npm run check:objc3c:m228-a011-lowering-pipeline-pass-graph-performance-quality-guardrails-contract",
        ),
    ),
    "architecture_doc": (
        ("M228-A011-ARCH-01", "M228 lane-A A011 performance and quality guardrails extends"),
        ("M228-A011-ARCH-02", "performance_quality_guardrails_consistent"),
    ),
    "lowering_spec": (
        (
            "M228-A011-SPC-01",
            "performance and quality guardrails shall include deterministic",
        ),
    ),
    "metadata_spec": (
        (
            "M228-A011-META-01",
            "deterministic performance-quality guardrails readiness/key anchors for",
        ),
    ),
    "a010_contract_doc": (
        (
            "M228-A011-DEP-01",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-conformance-corpus-expansion/m228-a010-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M228-A011-DOC-01",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-performance-quality-guardrails/m228-a011-v1`",
        ),
        ("M228-A011-DOC-02", "O3L315"),
        (
            "M228-A011-DOC-03",
            "lowering_pass_graph_performance_quality_guardrails_key",
        ),
        (
            "M228-A011-DOC-04",
            "scripts/check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py",
        ),
        (
            "M228-A011-DOC-05",
            "tests/tooling/test_check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py",
        ),
        (
            "M228-A011-DOC-06",
            "spec/planning/compiler/m228/m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_packet.md",
        ),
        (
            "M228-A011-DOC-07",
            "tmp/reports/m228/M228-A011/lowering_pipeline_pass_graph_performance_quality_guardrails_contract_summary.json",
        ),
    ),
    "packet_doc": (
        (
            "M228-A011-PKT-01",
            "# M228-A011 Lowering Pipeline Decomposition and Pass-Graph Performance and Quality Guardrails Packet",
        ),
        ("M228-A011-PKT-02", "Packet: `M228-A011`"),
        ("M228-A011-PKT-03", "Dependencies: `M228-A010`"),
        (
            "M228-A011-PKT-04",
            "m228_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_a011_expectations.md",
        ),
        (
            "M228-A011-PKT-05",
            "scripts/check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py",
        ),
        (
            "M228-A011-PKT-06",
            "tests/tooling/test_check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py",
        ),
        ("M228-A011-PKT-07", "`check:objc3c:m228-a011-lane-a-readiness`"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_source": (
        ("M228-A011-FORB-01", "surface.performance_quality_guardrails_ready = true;"),
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
        default=Path(
            "tmp/reports/m228/M228-A011/lowering_pipeline_pass_graph_performance_quality_guardrails_contract_summary.json"
        ),
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
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
