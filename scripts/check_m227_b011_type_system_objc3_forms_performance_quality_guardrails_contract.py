#!/usr/bin/env python3
"""Fail-closed validator for M227-B011 ObjC3 type-form performance/quality guardrails contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-type-system-objc3-forms-performance-quality-guardrails-b011-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.h",
    "scaffold_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.cpp",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h",
    "semantic_passes": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp",
    "b010_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_conformance_corpus_expansion_b010_expectations.md",
    "b010_checker": ROOT
    / "scripts"
    / "check_m227_b010_type_system_objc3_forms_conformance_corpus_expansion_contract.py",
    "b010_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m227_b010_type_system_objc3_forms_conformance_corpus_expansion_contract.py",
    "b010_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b010_type_system_objc3_forms_conformance_corpus_expansion_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_performance_quality_guardrails_b011_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b011_type_system_objc3_forms_performance_quality_guardrails_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M227-B011-HDR-01", "std::size_t performance_quality_required_guardrail_count = 0;"),
        ("M227-B011-HDR-02", "std::size_t performance_quality_passed_guardrail_count = 0;"),
        ("M227-B011-HDR-03", "std::size_t performance_quality_failed_guardrail_count = 0;"),
        ("M227-B011-HDR-04", "bool performance_quality_guardrails_consistent = false;"),
        ("M227-B011-HDR-05", "bool performance_quality_guardrails_ready = false;"),
        ("M227-B011-HDR-06", "std::string performance_quality_guardrails_key;"),
    ),
    "scaffold_source": (
        ("M227-B011-SRC-01", "summary.performance_quality_required_guardrail_count = 9u;"),
        ("M227-B011-SRC-02", "summary.performance_quality_passed_guardrail_count = 0u;"),
        ("M227-B011-SRC-03", "summary.performance_quality_failed_guardrail_count ="),
        ("M227-B011-SRC-04", "summary.performance_quality_guardrails_consistent ="),
        ("M227-B011-SRC-05", "summary.performance_quality_guardrails_key ="),
        ("M227-B011-SRC-06", "type-form-performance-quality-guardrails;required="),
        ("M227-B011-SRC-07", "summary.performance_quality_guardrails_ready ="),
        (
            "M227-B011-SRC-08",
            "summary.performance_quality_passed_guardrail_count ==\n                              summary\n                                  .performance_quality_required_guardrail_count",
        ),
    ),
    "sema_contract": (
        (
            "M227-B011-SEM-01",
            "std::size_t canonical_type_form_performance_quality_required_guardrail_count = 0;",
        ),
        (
            "M227-B011-SEM-02",
            "std::size_t canonical_type_form_performance_quality_passed_guardrail_count = 0;",
        ),
        (
            "M227-B011-SEM-03",
            "std::size_t canonical_type_form_performance_quality_failed_guardrail_count = 0;",
        ),
        (
            "M227-B011-SEM-04",
            "bool canonical_type_form_performance_quality_guardrails_consistent = false;",
        ),
        (
            "M227-B011-SEM-05",
            "bool canonical_type_form_performance_quality_guardrails_ready = false;",
        ),
        (
            "M227-B011-SEM-06",
            "std::string canonical_type_form_performance_quality_guardrails_key;",
        ),
    ),
    "semantic_passes": (
        (
            "M227-B011-PSS-01",
            "summary.canonical_type_form_performance_quality_required_guardrail_count =",
        ),
        (
            "M227-B011-PSS-02",
            "summary.canonical_type_form_performance_quality_passed_guardrail_count =",
        ),
        (
            "M227-B011-PSS-03",
            "summary.canonical_type_form_performance_quality_failed_guardrail_count =",
        ),
        (
            "M227-B011-PSS-04",
            "summary.canonical_type_form_performance_quality_guardrails_consistent =",
        ),
        (
            "M227-B011-PSS-05",
            "summary.canonical_type_form_performance_quality_guardrails_ready =",
        ),
        (
            "M227-B011-PSS-06",
            "summary.canonical_type_form_performance_quality_guardrails_key =",
        ),
        (
            "M227-B011-PSS-07",
            ".canonical_type_form_performance_quality_required_guardrail_count ==",
        ),
        (
            "M227-B011-PSS-08",
            ".canonical_type_form_performance_quality_guardrails_consistent &&",
        ),
        (
            "M227-B011-PSS-09",
            ".canonical_type_form_performance_quality_guardrails_ready &&",
        ),
        (
            "M227-B011-PSS-10",
            ".canonical_type_form_performance_quality_guardrails_key.empty()",
        ),
    ),
    "b010_contract_doc": (
        (
            "M227-B011-DEP-01",
            "Contract ID: `objc3c-type-system-objc3-forms-conformance-corpus-expansion/m227-b010-v1`",
        ),
    ),
    "b010_checker": (
        (
            "M227-B011-DEP-02",
            'MODE = "m227-type-system-objc3-forms-conformance-corpus-expansion-b010-v1"',
        ),
    ),
    "b010_tooling_test": (
        ("M227-B011-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b010_packet_doc": (
        ("M227-B011-DEP-04", "Packet: `M227-B010`"),
    ),
    "contract_doc": (
        (
            "M227-B011-DOC-01",
            "Contract ID: `objc3c-type-system-objc3-forms-performance-quality-guardrails/m227-b011-v1`",
        ),
        ("M227-B011-DOC-02", "Execute issue `#4852`"),
        ("M227-B011-DOC-03", "Dependencies: `M227-B010`"),
        (
            "M227-B011-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M227-B011-DOC-05", "performance_quality_required_guardrail_count"),
        ("M227-B011-DOC-06", "canonical_type_form_performance_quality_guardrails_ready"),
        ("M227-B011-DOC-07", "check:objc3c:m227-b011-lane-b-readiness"),
    ),
    "planning_packet": (
        ("M227-B011-PKT-01", "Packet: `M227-B011`"),
        ("M227-B011-PKT-02", "Issue: `#4852`"),
        ("M227-B011-PKT-03", "Dependencies: `M227-B010`"),
        (
            "M227-B011-PKT-04",
            "python scripts/check_m227_b011_type_system_objc3_forms_performance_quality_guardrails_contract.py",
        ),
    ),
    "package_json": (
        (
            "M227-B011-PKG-01",
            '"check:objc3c:m227-b011-type-system-objc3-forms-performance-quality-guardrails-contract"',
        ),
        (
            "M227-B011-PKG-02",
            '"test:tooling:m227-b011-type-system-objc3-forms-performance-quality-guardrails-contract"',
        ),
        ("M227-B011-PKG-03", '"check:objc3c:m227-b011-lane-b-readiness"'),
        ("M227-B011-PKG-04", '"check:objc3c:m227-b010-lane-b-readiness"'),
        ("M227-B011-PKG-05", '"test:objc3c:sema-pass-manager-diagnostics-bus"'),
        ("M227-B011-PKG-06", '"test:objc3c:lowering-regression"'),
    ),
    "architecture_doc": (
        (
            "M227-B011-ARC-01",
            "M227 lane-B B011 type-system performance and quality guardrails anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M227-B011-SPC-01",
            "type-system performance and quality guardrails governance shall preserve explicit lane-B dependency anchors (`M227-B011`, `M227-B010`)",
        ),
    ),
    "metadata_spec": (
        (
            "M227-B011-META-01",
            "deterministic lane-B type-system performance and quality guardrails metadata anchors for `M227-B011`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_source": (
        ("M227-B011-FORB-01", "summary.performance_quality_guardrails_ready = true;"),
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
            "tmp/reports/m227/M227-B011/type_system_objc3_forms_performance_quality_guardrails_contract_summary.json"
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
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
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
