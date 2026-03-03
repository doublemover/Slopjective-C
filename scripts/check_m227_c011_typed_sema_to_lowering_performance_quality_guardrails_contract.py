#!/usr/bin/env python3
"""Fail-closed validator for M227-C011 typed sema-to-lowering performance/quality guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-typed-sema-to-lowering-performance-quality-guardrails-c011-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "typed_surface": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_typed_sema_to_lowering_contract_surface.h",
    "parse_readiness": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h",
    "c010_contract_doc": ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_conformance_corpus_expansion_c010_expectations.md",
    "c010_checker": ROOT / "scripts" / "check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py",
    "c010_tooling_test": ROOT / "tests" / "tooling" / "test_check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py",
    "c010_packet_doc": ROOT / "spec" / "planning" / "compiler" / "m227" / "m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_packet.md",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_performance_quality_guardrails_c011_expectations.md",
    "packet_doc": ROOT / "spec" / "planning" / "compiler" / "m227" / "m227_c011_typed_sema_to_lowering_performance_quality_guardrails_packet.md",
    "package_json": ROOT / "package.json",
    "architecture": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_contracts": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_tables": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M227-C011-TYP-01", "bool typed_performance_quality_guardrails_consistent = false;"),
        ("M227-C011-TYP-02", "bool typed_performance_quality_guardrails_ready = false;"),
        ("M227-C011-TYP-03", "std::size_t typed_performance_quality_guardrails_case_count = 0;"),
        ("M227-C011-TYP-04", "std::size_t typed_performance_quality_guardrails_passed_case_count = 0;"),
        ("M227-C011-TYP-05", "std::size_t typed_performance_quality_guardrails_failed_case_count = 0;"),
        ("M227-C011-TYP-06", "std::string typed_performance_quality_guardrails_key;"),
        ("M227-C011-TYP-07", "bool typed_sema_performance_quality_guardrails_consistent = false;"),
        ("M227-C011-TYP-08", "bool typed_sema_performance_quality_guardrails_ready = false;"),
        ("M227-C011-TYP-09", "std::size_t typed_sema_performance_quality_guardrails_case_count = 0;"),
        ("M227-C011-TYP-10", "std::size_t typed_sema_performance_quality_guardrails_passed_case_count = 0;"),
        ("M227-C011-TYP-11", "std::size_t typed_sema_performance_quality_guardrails_failed_case_count = 0;"),
        ("M227-C011-TYP-12", "std::string typed_sema_performance_quality_guardrails_key;"),
    ),
    "typed_surface": (
        ("M227-C011-SUR-01", "kObjc3TypedSemaToLoweringPerformanceQualityGuardrailsCaseCount"),
        ("M227-C011-SUR-02", "BuildObjc3TypedSemaToLoweringPerformanceQualityGuardrailsKey("),
        ("M227-C011-SUR-03", "surface.typed_performance_quality_guardrails_case_count ="),
        ("M227-C011-SUR-04", "surface.typed_performance_quality_guardrails_passed_case_count ="),
        ("M227-C011-SUR-05", "surface.typed_performance_quality_guardrails_failed_case_count ="),
        ("M227-C011-SUR-06", "surface.typed_performance_quality_guardrails_consistent ="),
        ("M227-C011-SUR-07", "surface.typed_performance_quality_guardrails_ready ="),
        ("M227-C011-SUR-08", "surface.typed_performance_quality_guardrails_key ="),
        ("M227-C011-SUR-09", "typed_performance_quality_guardrails_key_ready"),
        ("M227-C011-SUR-10", "typed sema-to-lowering performance/quality guardrails are inconsistent"),
        ("M227-C011-SUR-11", "typed sema-to-lowering performance/quality guardrails are not ready"),
        ("M227-C011-SUR-12", "typed sema-to-lowering performance/quality guardrails key is empty"),
    ),
    "parse_readiness": (
        ("M227-C011-REA-01", "surface.typed_sema_performance_quality_guardrails_consistent ="),
        ("M227-C011-REA-02", "surface.typed_sema_performance_quality_guardrails_ready ="),
        ("M227-C011-REA-03", "surface.typed_sema_performance_quality_guardrails_case_count ="),
        ("M227-C011-REA-04", "surface.typed_sema_performance_quality_guardrails_passed_case_count ="),
        ("M227-C011-REA-05", "surface.typed_sema_performance_quality_guardrails_failed_case_count ="),
        ("M227-C011-REA-06", "surface.typed_sema_performance_quality_guardrails_key ="),
        ("M227-C011-REA-07", "const bool typed_performance_quality_guardrails_alignment ="),
        ("M227-C011-REA-08", "typed_performance_quality_guardrails_alignment &&"),
        ("M227-C011-REA-09", "!surface.typed_sema_performance_quality_guardrails_key.empty() &&"),
        ("M227-C011-REA-10", "typed sema-to-lowering performance/quality guardrails drifted from parse/lowering readiness"),
    ),
    "c010_contract_doc": (("M227-C011-DEP-01", "m227-c010-v1"),),
    "c010_checker": (("M227-C011-DEP-02", 'MODE = "m227-typed-sema-to-lowering-conformance-corpus-expansion-c010-v1"'),),
    "c010_tooling_test": (("M227-C011-DEP-03", "def test_contract_passes_on_repository_sources"),),
    "c010_packet_doc": (("M227-C011-DEP-04", "Packet: `M227-C010`"),),
    "contract_doc": (
        ("M227-C011-DOC-01", "Contract ID: `objc3c-typed-sema-to-lowering-performance-quality-guardrails/m227-c011-v1`"),
        ("M227-C011-DOC-02", "Execute issue `#5131`"),
        ("M227-C011-DOC-03", "Dependencies: `M227-C010`"),
    ),
    "packet_doc": (
        ("M227-C011-PKT-01", "Packet: `M227-C011`"),
        ("M227-C011-PKT-02", "Issue: `#5131`"),
        ("M227-C011-PKT-03", "Dependencies: `M227-C010`"),
    ),
    "package_json": (
        ("M227-C011-PKG-01", '"check:objc3c:m227-c011-typed-sema-to-lowering-performance-quality-guardrails-contract"'),
        ("M227-C011-PKG-02", '"test:tooling:m227-c011-typed-sema-to-lowering-performance-quality-guardrails-contract"'),
        ("M227-C011-PKG-03", '"check:objc3c:m227-c011-lane-c-readiness"'),
        ("M227-C011-PKG-04", "scripts/check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py"),
    ),
    "architecture": (("M227-C011-ARC-01", "M227 lane-C C011 typed sema-to-lowering performance/quality guardrails anchors"),),
    "lowering_contracts": (("M227-C011-SPC-01", "typed sema-to-lowering performance/quality guardrails governance shall preserve explicit lane-C dependency anchors (`M227-C011`, `M227-C010`)"),),
    "metadata_tables": (("M227-C011-META-01", "deterministic lane-C typed sema-to-lowering performance/quality guardrails metadata anchors for `M227-C011`"),),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "typed_surface": (("M227-C011-FORB-01", "surface.typed_performance_quality_guardrails_ready = true;"),),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m227/M227-C011/typed_sema_to_lowering_performance_quality_guardrails_contract_summary.json"
        ),
    )
    return parser.parse_args(argv)


def load_text(path: Path, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact)
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

    ok = not findings
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(
        json.dumps(
            {
                "mode": MODE,
                "ok": ok,
                "checks_total": total_checks,
                "checks_passed": passed_checks,
                "failures": [
                    {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in findings
                ],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
