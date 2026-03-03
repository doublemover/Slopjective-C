#!/usr/bin/env python3
"""Fail-closed validator for M227-B012 ObjC3 type-form cross-lane integration sync contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-type-system-objc3-forms-cross-lane-integration-sync-b012-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.h",
    "scaffold_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.cpp",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h",
    "semantic_passes": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp",
    "b011_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_performance_quality_guardrails_b011_expectations.md",
    "b011_checker": ROOT
    / "scripts"
    / "check_m227_b011_type_system_objc3_forms_performance_quality_guardrails_contract.py",
    "b011_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m227_b011_type_system_objc3_forms_performance_quality_guardrails_contract.py",
    "b011_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b011_type_system_objc3_forms_performance_quality_guardrails_packet.md",
    "a012_contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_cross_lane_integration_sync_expectations.md",
    "a012_checker": ROOT / "scripts" / "check_m227_a012_semantic_pass_cross_lane_integration_sync_contract.py",
    "a012_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m227_a012_semantic_pass_cross_lane_integration_sync_contract.py",
    "a012_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_a012_semantic_pass_cross_lane_integration_sync_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_cross_lane_integration_sync_b012_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b012_type_system_objc3_forms_cross_lane_integration_sync_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M227-B012-HDR-01", "std::size_t performance_quality_required_guardrail_count = 0;"),
        ("M227-B012-HDR-02", "std::size_t performance_quality_passed_guardrail_count = 0;"),
        ("M227-B012-HDR-03", "std::size_t performance_quality_failed_guardrail_count = 0;"),
        ("M227-B012-HDR-04", "bool performance_quality_guardrails_consistent = false;"),
        ("M227-B012-HDR-05", "bool performance_quality_guardrails_ready = false;"),
        ("M227-B012-HDR-06", "std::string performance_quality_guardrails_key;"),
    ),
    "scaffold_source": (
        ("M227-B012-SRC-01", "summary.conformance_corpus_key ="),
        ("M227-B012-SRC-02", "type-form-conformance-corpus;matrix-consistent="),
        ("M227-B012-SRC-03", "summary.performance_quality_guardrails_key ="),
        ("M227-B012-SRC-04", "type-form-performance-quality-guardrails;required="),
        ("M227-B012-SRC-05", "summary.performance_quality_guardrails_ready ="),
        ("M227-B012-SRC-06", "!summary.conformance_corpus_key.empty() &&"),
        ("M227-B012-SRC-07", "summary.performance_quality_guardrails_ready &&"),
        ("M227-B012-SRC-08", "!summary.performance_quality_guardrails_key.empty();"),
    ),
    "sema_contract": (
        ("M227-B012-SEM-01", "std::string canonical_type_form_conformance_corpus_key;"),
        ("M227-B012-SEM-02", "std::size_t canonical_type_form_performance_quality_required_guardrail_count = 0;"),
        ("M227-B012-SEM-03", "std::size_t canonical_type_form_performance_quality_passed_guardrail_count = 0;"),
        ("M227-B012-SEM-04", "std::size_t canonical_type_form_performance_quality_failed_guardrail_count = 0;"),
        ("M227-B012-SEM-05", "bool canonical_type_form_performance_quality_guardrails_ready = false;"),
        ("M227-B012-SEM-06", "std::string canonical_type_form_performance_quality_guardrails_key;"),
    ),
    "semantic_passes": (
        (
            "M227-B012-PSS-01",
            "ApplyTypeFormScaffoldSummaryToIdClassSelObjectPointerTypeCheckingSummary(",
        ),
        (
            "M227-B012-PSS-02",
            "BuildIdClassSelObjectPointerTypeCheckingSummaryFromIntegrationSurface(",
        ),
        (
            "M227-B012-PSS-03",
            "BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(",
        ),
        (
            "M227-B012-PSS-04",
            "summary.canonical_type_form_performance_quality_required_guardrail_count =",
        ),
        (
            "M227-B012-PSS-05",
            "summary.canonical_type_form_performance_quality_guardrails_consistent =",
        ),
        (
            "M227-B012-PSS-06",
            "summary.canonical_type_form_performance_quality_guardrails_ready =",
        ),
        (
            "M227-B012-PSS-07",
            "summary.canonical_type_form_performance_quality_guardrails_key =",
        ),
        (
            "M227-B012-PSS-08",
            "summary.canonical_type_form_conformance_corpus_ready &&",
        ),
        (
            "M227-B012-PSS-09",
            "!summary.canonical_type_form_conformance_corpus_key.empty() &&",
        ),
        (
            "M227-B012-PSS-10",
            "summary.canonical_type_form_performance_quality_guardrails_ready &&",
        ),
        (
            "M227-B012-PSS-11",
            "!summary.canonical_type_form_performance_quality_guardrails_key.empty() &&",
        ),
        (
            "M227-B012-PSS-12",
            ".canonical_type_form_conformance_corpus_key ==",
        ),
        (
            "M227-B012-PSS-13",
            ".canonical_type_form_performance_quality_guardrails_ready ==",
        ),
        (
            "M227-B012-PSS-14",
            ".canonical_type_form_performance_quality_guardrails_key ==",
        ),
    ),
    "b011_contract_doc": (
        (
            "M227-B012-DEP-01",
            "Contract ID: `objc3c-type-system-objc3-forms-performance-quality-guardrails/m227-b011-v1`",
        ),
    ),
    "b011_checker": (
        (
            "M227-B012-DEP-02",
            'MODE = "m227-type-system-objc3-forms-performance-quality-guardrails-b011-v1"',
        ),
    ),
    "b011_tooling_test": (
        ("M227-B012-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b011_packet_doc": (
        ("M227-B012-DEP-04", "Packet: `M227-B011`"),
        ("M227-B012-DEP-05", "Issue: `#4852`"),
    ),
    "a012_contract_doc": (
        (
            "M227-B012-DEP-06",
            "Contract ID: `objc3c-semantic-pass-cross-lane-integration-sync/m227-a012-v1`",
        ),
    ),
    "a012_checker": (
        (
            "M227-B012-DEP-07",
            'MODE = "m227-a012-semantic-pass-cross-lane-integration-sync-contract-v1"',
        ),
    ),
    "a012_tooling_test": (
        (
            "M227-B012-DEP-08",
            "def test_contract_fails_closed_when_lane_contract_id_drifts(",
        ),
    ),
    "a012_packet_doc": (
        ("M227-B012-DEP-09", "Packet: `M227-A012`"),
        ("M227-B012-DEP-10", "Dependencies: `M227-A011`, `M227-B007`, `M227-C002`, `M227-D001`, `M227-E001`"),
    ),
    "contract_doc": (
        (
            "M227-B012-DOC-01",
            "Contract ID: `objc3c-type-system-objc3-forms-cross-lane-integration-sync/m227-b012-v1`",
        ),
        ("M227-B012-DOC-02", "Execute issue `#4853`"),
        ("M227-B012-DOC-03", "Dependencies: `M227-B011`, `M227-A012`"),
        (
            "M227-B012-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M227-B012-DOC-05", "canonical_type_form_performance_quality_guardrails_key"),
        ("M227-B012-DOC-06", "python scripts/check_m227_b012_type_system_objc3_forms_cross_lane_integration_sync_contract.py"),
        (
            "M227-B012-DOC-07",
            "python -m pytest tests/tooling/test_check_m227_b012_type_system_objc3_forms_cross_lane_integration_sync_contract.py -q",
        ),
        (
            "M227-B012-DOC-08",
            "tmp/reports/m227/M227-B012/type_system_objc3_forms_cross_lane_integration_sync_contract_summary.json",
        ),
    ),
    "planning_packet": (
        ("M227-B012-PKT-01", "Packet: `M227-B012`"),
        ("M227-B012-PKT-02", "Issue: `#4853`"),
        ("M227-B012-PKT-03", "Dependencies: `M227-B011`, `M227-A012`"),
        (
            "M227-B012-PKT-04",
            "scripts/check_m227_b012_type_system_objc3_forms_cross_lane_integration_sync_contract.py",
        ),
        (
            "M227-B012-PKT-05",
            "tmp/reports/m227/M227-B012/type_system_objc3_forms_cross_lane_integration_sync_contract_summary.json",
        ),
    ),
    "package_json": (
        ("M227-B012-PKG-01", '"check:objc3c:m227-b011-lane-b-readiness"'),
        ("M227-B012-PKG-02", '"check:objc3c:m227-a012-lane-a-readiness"'),
        ("M227-B012-PKG-03", '"test:objc3c:sema-pass-manager-diagnostics-bus"'),
        ("M227-B012-PKG-04", '"test:objc3c:lowering-regression"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_source": (
        ("M227-B012-FORB-01", "summary.performance_quality_guardrails_ready = true;"),
    ),
    "semantic_passes": (
        ("M227-B012-FORB-02", "summary.canonical_type_form_performance_quality_guardrails_ready = true;"),
        (
            "M227-B012-FORB-03",
            "handoff.id_class_sel_object_pointer_type_checking_summary.canonical_type_form_performance_quality_guardrails_ready = true;",
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
            "tmp/reports/m227/M227-B012/type_system_objc3_forms_cross_lane_integration_sync_contract_summary.json"
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
            findings.append(Finding(artifact, f"M227-B012-MISS-{artifact.upper()}", str(exc)))
            continue

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                checks_passed += 1
            else:
                findings.append(Finding(artifact, check_id, f"missing snippet: {snippet}"))

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                checks_passed += 1

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
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
