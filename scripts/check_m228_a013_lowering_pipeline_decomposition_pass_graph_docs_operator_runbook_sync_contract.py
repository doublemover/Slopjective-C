#!/usr/bin/env python3
"""Fail-closed validator for M228-A013 docs/operator runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-a013-lowering-pipeline-pass-graph-docs-operator-runbook-sync-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_a013_expectations.md",
    "runbook": ROOT / "docs" / "runbooks" / "m228_wave_execution_runbook.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "a012_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_a012_expectations.md",
    "a011_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_a011_expectations.md",
    "b007_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_diagnostics_hardening_b007_expectations.md",
    "c005_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_edge_case_and_compatibility_completion_c005_expectations.md",
    "d006_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_edge_case_expansion_and_robustness_d006_expectations.md",
    "e006_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lane_e_replay_proof_and_performance_closeout_gate_edge_case_expansion_and_robustness_e006_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M228-A013-DOC-01",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-docs-operator-runbook-sync/m228-a013-v1`",
        ),
        ("M228-A013-DOC-02", "`docs/runbooks/m228_wave_execution_runbook.md`"),
        (
            "M228-A013-DOC-03",
            "`objc3c-lowering-pipeline-pass-graph-performance-quality-guardrails/m228-a011-v1`",
        ),
        (
            "M228-A013-DOC-04",
            "`objc3c-ownership-aware-lowering-behavior-diagnostics-hardening/m228-b007-v1`",
        ),
        (
            "M228-A013-DOC-05",
            "`objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1`",
        ),
        (
            "M228-A013-DOC-06",
            "`objc3c-object-emission-link-path-reliability-edge-case-expansion-and-robustness/m228-d006-v1`",
        ),
        (
            "M228-A013-DOC-07",
            "`objc3c-lane-e-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract/m228-e006-v1`",
        ),
        (
            "M228-A013-DOC-08",
            "`objc3c-lowering-pipeline-pass-graph-cross-lane-integration-sync/m228-a012-v1`",
        ),
        (
            "M228-A013-DOC-09",
            "scripts/check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py",
        ),
        (
            "M228-A013-DOC-10",
            "tests/tooling/test_check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py",
        ),
        (
            "M228-A013-DOC-11",
            "spec/planning/compiler/m228/m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_packet.md",
        ),
        (
            "M228-A013-DOC-12",
            "tmp/reports/m228/M228-A013/docs_operator_runbook_sync_contract_summary.json",
        ),
        ("M228-A013-DOC-13", "`npm run check:objc3c:m228-a013-lane-a-readiness`"),
    ),
    "runbook": (
        ("M228-A013-RUN-01", "# M228 Wave Execution Runbook"),
        (
            "M228-A013-RUN-02",
            "objc3c-lowering-pipeline-pass-graph-performance-quality-guardrails/m228-a011-v1",
        ),
        (
            "M228-A013-RUN-03",
            "objc3c-ownership-aware-lowering-behavior-diagnostics-hardening/m228-b007-v1",
        ),
        (
            "M228-A013-RUN-04",
            "objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1",
        ),
        (
            "M228-A013-RUN-05",
            "objc3c-object-emission-link-path-reliability-edge-case-expansion-and-robustness/m228-d006-v1",
        ),
        (
            "M228-A013-RUN-06",
            "objc3c-lane-e-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract/m228-e006-v1",
        ),
        (
            "M228-A013-RUN-07",
            "objc3c-lowering-pipeline-pass-graph-cross-lane-integration-sync/m228-a012-v1",
        ),
        (
            "M228-A013-RUN-08",
            "objc3c-lowering-pipeline-pass-graph-docs-operator-runbook-sync/m228-a013-v1",
        ),
        ("M228-A013-RUN-09", "npm run build:objc3c-native"),
        ("M228-A013-RUN-10", "scripts/objc3c_native_compile.ps1"),
        (
            "M228-A013-RUN-11",
            "python scripts/check_m228_a012_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_contract.py",
        ),
        (
            "M228-A013-RUN-12",
            "python scripts/check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py",
        ),
        (
            "M228-A013-RUN-13",
            "python -m pytest tests/tooling/test_check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py -q",
        ),
        ("M228-A013-RUN-14", "npm run check:objc3c:m228-a013-lane-a-readiness"),
        ("M228-A013-RUN-15", "tmp/reports/m228/"),
    ),
    "packet_doc": (
        (
            "M228-A013-PKT-01",
            "# M228-A013 Lowering Pipeline Decomposition and Pass-Graph Docs and Operator Runbook Synchronization Packet",
        ),
        ("M228-A013-PKT-02", "Packet: `M228-A013`"),
        ("M228-A013-PKT-03", "Dependencies: `M228-A012`"),
        ("M228-A013-PKT-04", "docs/runbooks/m228_wave_execution_runbook.md"),
        (
            "M228-A013-PKT-05",
            "scripts/check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py",
        ),
        (
            "M228-A013-PKT-06",
            "tests/tooling/test_check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py",
        ),
        ("M228-A013-PKT-07", "`check:objc3c:m228-a013-lane-a-readiness`"),
        (
            "M228-A013-PKT-08",
            "tmp/reports/m228/M228-A013/docs_operator_runbook_sync_contract_summary.json",
        ),
    ),
    "package_json": (
        (
            "M228-A013-CFG-01",
            '"check:objc3c:m228-a013-lowering-pipeline-pass-graph-docs-operator-runbook-sync-contract"',
        ),
        (
            "M228-A013-CFG-02",
            '"test:tooling:m228-a013-lowering-pipeline-pass-graph-docs-operator-runbook-sync-contract"',
        ),
        ("M228-A013-CFG-03", '"check:objc3c:m228-a013-lane-a-readiness"'),
        (
            "M228-A013-CFG-04",
            "npm run check:objc3c:m228-a012-lane-a-readiness && npm run check:objc3c:m228-a013-lowering-pipeline-pass-graph-docs-operator-runbook-sync-contract",
        ),
    ),
    "architecture_doc": (
        (
            "M228-A013-ARC-01",
            "M228 lane-A A013 docs and operator runbook synchronization anchors explicit",
        ),
        ("M228-A013-ARC-02", "docs/runbooks/m228_wave_execution_runbook.md"),
    ),
    "lowering_spec": (
        (
            "M228-A013-SPC-01",
            "docs and operator runbook synchronization shall preserve deterministic lane-A",
        ),
    ),
    "metadata_spec": (
        (
            "M228-A013-META-01",
            "deterministic docs/operator runbook synchronization anchors for `A011`,",
        ),
    ),
    "a012_contract_doc": (
        (
            "M228-A013-DEP-01",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-cross-lane-integration-sync/m228-a012-v1`",
        ),
    ),
    "a011_contract_doc": (
        (
            "M228-A013-DEP-02",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-performance-quality-guardrails/m228-a011-v1`",
        ),
    ),
    "b007_contract_doc": (
        (
            "M228-A013-DEP-03",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-diagnostics-hardening/m228-b007-v1`",
        ),
    ),
    "c005_contract_doc": (
        (
            "M228-A013-DEP-04",
            "Contract ID: `objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1`",
        ),
    ),
    "d006_contract_doc": (
        (
            "M228-A013-DEP-05",
            "Contract ID: `objc3c-object-emission-link-path-reliability-edge-case-expansion-and-robustness/m228-d006-v1`",
        ),
    ),
    "e006_contract_doc": (
        (
            "M228-A013-DEP-06",
            "Contract ID: `objc3c-lane-e-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract/m228-e006-v1`",
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
        default=Path("tmp/reports/m228/M228-A013/docs_operator_runbook_sync_contract_summary.json"),
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
                    f"M228-A013-MISS-{artifact.upper()}",
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
