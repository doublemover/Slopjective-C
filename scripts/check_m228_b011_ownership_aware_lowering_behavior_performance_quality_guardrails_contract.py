#!/usr/bin/env python3
"""Fail-closed validator for M228-B011 ownership-aware lowering performance/quality guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b011-ownership-aware-lowering-behavior-performance-quality-guardrails-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_ownership_aware_lowering_behavior_scaffold.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "ir_header": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h",
    "ir_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "b010_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_conformance_corpus_expansion_b010_expectations.md",
    "b010_checker": ROOT
    / "scripts"
    / "check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py",
    "b010_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py",
    "b010_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_performance_quality_guardrails_b011_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b011_ownership_aware_lowering_behavior_performance_quality_guardrails_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B011-SCA-01", "bool performance_quality_guardrails_consistent = false;"),
        ("M228-B011-SCA-02", "bool performance_quality_guardrails_ready = false;"),
        (
            "M228-B011-SCA-03",
            "bool parse_lowering_performance_quality_guardrails_consistent = false;",
        ),
        (
            "M228-B011-SCA-04",
            "std::size_t parse_lowering_performance_quality_guardrails_case_count = 0;",
        ),
        (
            "M228-B011-SCA-05",
            "std::size_t parse_lowering_performance_quality_guardrails_passed_case_count = 0;",
        ),
        (
            "M228-B011-SCA-06",
            "std::size_t parse_lowering_performance_quality_guardrails_failed_case_count = 0;",
        ),
        ("M228-B011-SCA-07", "std::string performance_quality_guardrails_key;"),
        (
            "M228-B011-SCA-08",
            "std::string parse_lowering_performance_quality_guardrails_key;",
        ),
        (
            "M228-B011-SCA-09",
            "BuildObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsKey(",
        ),
        (
            "M228-B011-SCA-10",
            "scaffold.parse_lowering_performance_quality_guardrails_consistent =",
        ),
        (
            "M228-B011-SCA-11",
            "scaffold.parse_lowering_performance_quality_guardrails_case_count =",
        ),
        (
            "M228-B011-SCA-12",
            "scaffold.parse_lowering_performance_quality_guardrails_passed_case_count =",
        ),
        (
            "M228-B011-SCA-13",
            "scaffold.parse_lowering_performance_quality_guardrails_failed_case_count =",
        ),
        (
            "M228-B011-SCA-14",
            "scaffold.parse_lowering_performance_quality_guardrails_key =",
        ),
        (
            "M228-B011-SCA-15",
            "parse_lowering_performance_quality_guardrails_case_accounting_consistent",
        ),
        (
            "M228-B011-SCA-16",
            "scaffold.performance_quality_guardrails_consistent =",
        ),
        ("M228-B011-SCA-17", "scaffold.performance_quality_guardrails_key ="),
        ("M228-B011-SCA-18", "scaffold.performance_quality_guardrails_ready ="),
        (
            "M228-B011-SCA-19",
            "inline bool IsObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsReady(",
        ),
        (
            "M228-B011-SCA-20",
            "ownership-aware lowering performance quality guardrails are inconsistent",
        ),
        (
            "M228-B011-SCA-21",
            "ownership-aware lowering performance quality guardrails are not ready",
        ),
        (
            "M228-B011-SCA-22",
            "ownership-aware lowering parse performance quality guardrails include failing cases",
        ),
    ),
    "artifacts_source": (
        (
            "M228-B011-ART-01",
            "IsObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsReady(",
        ),
        ("M228-B011-ART-02", '"O3L328"'),
        (
            "M228-B011-ART-03",
            "ownership-aware lowering performance quality guardrails check failed",
        ),
        (
            "M228-B011-ART-04",
            ".parse_lowering_performance_quality_guardrails_consistent",
        ),
        (
            "M228-B011-ART-05",
            ".parse_lowering_performance_quality_guardrails_case_count",
        ),
        (
            "M228-B011-ART-06",
            ".parse_lowering_performance_quality_guardrails_passed_case_count",
        ),
        (
            "M228-B011-ART-07",
            ".parse_lowering_performance_quality_guardrails_failed_case_count",
        ),
        (
            "M228-B011-ART-08",
            ".parse_lowering_performance_quality_guardrails_key",
        ),
        (
            "M228-B011-ART-09",
            "ir_frontend_metadata.ownership_aware_lowering_performance_quality_guardrails_ready =",
        ),
        (
            "M228-B011-ART-10",
            "ir_frontend_metadata.ownership_aware_lowering_performance_quality_guardrails_key =",
        ),
    ),
    "ir_header": (
        (
            "M228-B011-IRH-01",
            "bool ownership_aware_lowering_performance_quality_guardrails_ready = false;",
        ),
        (
            "M228-B011-IRH-02",
            "std::string ownership_aware_lowering_performance_quality_guardrails_key;",
        ),
    ),
    "ir_source": (
        (
            "M228-B011-IRS-01",
            'out << "; ownership_aware_lowering_performance_quality_guardrails = "',
        ),
        (
            "M228-B011-IRS-02",
            'out << "; ownership_aware_lowering_performance_quality_guardrails_ready = "',
        ),
    ),
    "b010_contract_doc": (
        (
            "M228-B011-DEP-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-conformance-corpus-expansion/m228-b010-v1`",
        ),
    ),
    "b010_checker": (
        (
            "M228-B011-DEP-02",
            'MODE = "m228-b010-ownership-aware-lowering-behavior-conformance-corpus-expansion-contract-v1"',
        ),
    ),
    "b010_tooling_test": (
        ("M228-B011-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b010_packet_doc": (
        ("M228-B011-DEP-04", "Packet: `M228-B010`"),
        ("M228-B011-DEP-05", "Dependencies: `M228-B009`"),
    ),
    "contract_doc": (
        (
            "M228-B011-DOC-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-performance-quality-guardrails/m228-b011-v1`",
        ),
        ("M228-B011-DOC-02", "Execute issue `#5205`"),
        ("M228-B011-DOC-03", "Dependencies: `M228-B010`"),
        (
            "M228-B011-DOC-04",
            "BuildObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsKey",
        ),
        (
            "M228-B011-DOC-05",
            "IsObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsReady",
        ),
        ("M228-B011-DOC-06", "O3L328"),
        (
            "M228-B011-DOC-07",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        (
            "M228-B011-DOC-08",
            "check:objc3c:m228-b011-lane-b-readiness",
        ),
    ),
    "planning_packet": (
        (
            "M228-B011-PKT-01",
            "# M228-B011 Ownership-Aware Lowering Behavior Performance and Quality Guardrails Packet",
        ),
        ("M228-B011-PKT-02", "Packet: `M228-B011`"),
        ("M228-B011-PKT-03", "Issue: `#5205`"),
        ("M228-B011-PKT-04", "Dependencies: `M228-B010`"),
        (
            "M228-B011-PKT-05",
            "scripts/check_m228_b011_ownership_aware_lowering_behavior_performance_quality_guardrails_contract.py",
        ),
        (
            "M228-B011-PKT-06",
            "tests/tooling/test_check_m228_b011_ownership_aware_lowering_behavior_performance_quality_guardrails_contract.py",
        ),
        (
            "M228-B011-PKT-07",
            "tmp/reports/m228/M228-B011/ownership_aware_lowering_behavior_performance_quality_guardrails_contract_summary.json",
        ),
    ),
    "package_json": (
        (
            "M228-B011-PKG-01",
            '"check:objc3c:m228-b011-ownership-aware-lowering-behavior-performance-quality-guardrails-contract"',
        ),
        (
            "M228-B011-PKG-02",
            '"test:tooling:m228-b011-ownership-aware-lowering-behavior-performance-quality-guardrails-contract"',
        ),
        ("M228-B011-PKG-03", '"check:objc3c:m228-b011-lane-b-readiness"'),
        ("M228-B011-PKG-04", '"check:objc3c:m228-b010-lane-b-readiness"'),
    ),
    "architecture_doc": (
        (
            "M228-B011-ARC-01",
            "M228 lane-B B011 performance and quality guardrails extends",
        ),
    ),
    "lowering_spec": (
        (
            "M228-B011-SPEC-01",
            "ownership-aware lowering performance and quality guardrails shall include",
        ),
    ),
    "metadata_spec": (
        (
            "M228-B011-META-01",
            "lane-B B011 evidence",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        (
            "M228-B011-FORB-01",
            "scaffold.performance_quality_guardrails_ready = true;",
        ),
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
            "tmp/reports/m228/M228-B011/ownership_aware_lowering_behavior_performance_quality_guardrails_contract_summary.json"
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
    checks_total = 0
    checks_passed = 0

    for artifact, path in ARTIFACTS.items():
        try:
            text = load_text(path, artifact=artifact)
        except ValueError as exc:
            checks_total += 1
            findings.append(Finding(artifact, f"M228-B011-MISS-{artifact.upper()}", str(exc)))
            continue

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):  # noqa: B007
            checks_total += 1
            if snippet in text:
                checks_passed += 1
            else:
                findings.append(Finding(artifact, check_id, f"missing snippet: {snippet}"))

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):  # noqa: B007
            checks_total += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                checks_passed += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in findings
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
