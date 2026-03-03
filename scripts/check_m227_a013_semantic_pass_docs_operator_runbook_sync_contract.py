#!/usr/bin/env python3
"""Fail-closed validator for M227-A013 docs/operator runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-a013-semantic-pass-docs-operator-runbook-sync-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_docs_operator_runbook_sync_expectations.md",
    "runbook": ROOT / "docs" / "runbooks" / "m227_wave_execution_runbook.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_a013_semantic_pass_docs_operator_runbook_sync_packet.md",
    "a012_contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_cross_lane_integration_sync_expectations.md",
    "a011_contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_performance_quality_guardrails_expectations.md",
    "b007_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_diagnostics_hardening_b007_expectations.md",
    "c002_contract_doc": ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_modular_split_c002_expectations.md",
    "d001_contract_doc": ROOT / "docs" / "contracts" / "m227_runtime_facing_type_metadata_semantics_expectations.md",
    "e001_contract_doc": ROOT / "docs" / "contracts" / "m227_lane_e_semantic_conformance_quality_gate_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M227-A013-DOC-01",
            "Contract ID: `objc3c-semantic-pass-docs-operator-runbook-sync/m227-a013-v1`",
        ),
        ("M227-A013-DOC-02", "`docs/runbooks/m227_wave_execution_runbook.md`"),
        ("M227-A013-DOC-03", "`objc3c-semantic-pass-performance-quality-guardrails/m227-a011-v1`"),
        ("M227-A013-DOC-04", "`objc3c-type-system-objc3-forms-diagnostics-hardening/m227-b007-v1`"),
        ("M227-A013-DOC-05", "`objc3c-typed-sema-to-lowering-modular-split-scaffold/m227-c002-v1`"),
        ("M227-A013-DOC-06", "`objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1`"),
        ("M227-A013-DOC-07", "`objc3c-lane-e-semantic-conformance-quality-gate-contract/m227-e001-v1`"),
        ("M227-A013-DOC-08", "`objc3c-semantic-pass-cross-lane-integration-sync/m227-a012-v1`"),
        (
            "M227-A013-DOC-09",
            "scripts/check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py",
        ),
        (
            "M227-A013-DOC-10",
            "tests/tooling/test_check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py",
        ),
        (
            "M227-A013-DOC-11",
            "spec/planning/compiler/m227/m227_a013_semantic_pass_docs_operator_runbook_sync_packet.md",
        ),
        ("M227-A013-DOC-12", "tmp/reports/m227/M227-A013/docs_operator_runbook_sync_summary.json"),
        ("M227-A013-DOC-13", "`npm run check:objc3c:m227-a013-lane-a-readiness`"),
    ),
    "runbook": (
        ("M227-A013-RUN-01", "# M227 Wave Execution Runbook"),
        ("M227-A013-RUN-02", "objc3c-semantic-pass-performance-quality-guardrails/m227-a011-v1"),
        ("M227-A013-RUN-03", "objc3c-type-system-objc3-forms-diagnostics-hardening/m227-b007-v1"),
        ("M227-A013-RUN-04", "objc3c-typed-sema-to-lowering-modular-split-scaffold/m227-c002-v1"),
        ("M227-A013-RUN-05", "objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1"),
        ("M227-A013-RUN-06", "objc3c-lane-e-semantic-conformance-quality-gate-contract/m227-e001-v1"),
        ("M227-A013-RUN-07", "objc3c-semantic-pass-cross-lane-integration-sync/m227-a012-v1"),
        ("M227-A013-RUN-08", "objc3c-semantic-pass-docs-operator-runbook-sync/m227-a013-v1"),
        ("M227-A013-RUN-09", "npm run build:objc3c-native"),
        ("M227-A013-RUN-10", "npm run test:objc3c:execution-smoke"),
        ("M227-A013-RUN-11", "npm run test:objc3c:execution-replay-proof"),
        ("M227-A013-RUN-12", "python scripts/check_m227_a012_semantic_pass_cross_lane_integration_sync_contract.py"),
        ("M227-A013-RUN-13", "python scripts/check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py"),
        (
            "M227-A013-RUN-14",
            "python -m pytest tests/tooling/test_check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py -q",
        ),
        ("M227-A013-RUN-15", "npm run check:objc3c:m227-a013-lane-a-readiness"),
        ("M227-A013-RUN-16", "tmp/reports/m227/"),
    ),
    "packet_doc": (
        ("M227-A013-PKT-01", "Packet: `M227-A013`"),
        ("M227-A013-PKT-02", "Dependencies: `M227-A012`"),
        ("M227-A013-PKT-03", "docs/runbooks/m227_wave_execution_runbook.md"),
        ("M227-A013-PKT-04", "scripts/check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py"),
        (
            "M227-A013-PKT-05",
            "tests/tooling/test_check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py",
        ),
        ("M227-A013-PKT-06", "`npm run check:objc3c:m227-a013-lane-a-readiness`"),
        ("M227-A013-PKT-07", "tmp/reports/m227/M227-A013/docs_operator_runbook_sync_summary.json"),
    ),
    "a012_contract_doc": (
        (
            "M227-A013-DEP-01",
            "Contract ID: `objc3c-semantic-pass-cross-lane-integration-sync/m227-a012-v1`",
        ),
    ),
    "a011_contract_doc": (
        (
            "M227-A013-DEP-02",
            "Contract ID: `objc3c-semantic-pass-performance-quality-guardrails/m227-a011-v1`",
        ),
    ),
    "b007_contract_doc": (
        (
            "M227-A013-DEP-03",
            "Contract ID: `objc3c-type-system-objc3-forms-diagnostics-hardening/m227-b007-v1`",
        ),
    ),
    "c002_contract_doc": (
        (
            "M227-A013-DEP-04",
            "Contract ID: `objc3c-typed-sema-to-lowering-modular-split-scaffold/m227-c002-v1`",
        ),
    ),
    "d001_contract_doc": (
        (
            "M227-A013-DEP-05",
            "Contract ID: `objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1`",
        ),
    ),
    "e001_contract_doc": (
        (
            "M227-A013-DEP-06",
            "Contract ID: `objc3c-lane-e-semantic-conformance-quality-gate-contract/m227-e001-v1`",
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
        default=Path("tmp/reports/m227/M227-A013/docs_operator_runbook_sync_summary.json"),
    )
    return parser.parse_args(argv)


def maybe_load_text(path: Path) -> str | None:
    if not path.exists() or not path.is_file():
        return None
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = maybe_load_text(path)
        total_checks += 1
        if text is None:
            findings.append(
                Finding(
                    artifact,
                    f"M227-A013-MISS-{artifact.upper()}",
                    f"missing file: {path.as_posix()}",
                )
            )
            continue
        passed_checks += 1

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

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
