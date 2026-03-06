#!/usr/bin/env python3
"""Fail-closed validator for M228-B012 ownership-aware lowering cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b012-ownership-aware-lowering-behavior-cross-lane-integration-sync-contract-v1"

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
    "b011_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_performance_quality_guardrails_b011_expectations.md",
    "b011_checker": ROOT
    / "scripts"
    / "check_m228_b011_ownership_aware_lowering_behavior_performance_quality_guardrails_contract.py",
    "b011_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_b011_ownership_aware_lowering_behavior_performance_quality_guardrails_contract.py",
    "b011_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b011_ownership_aware_lowering_behavior_performance_quality_guardrails_packet.md",
    "a012_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_a012_expectations.md",
    "a012_checker": ROOT
    / "scripts"
    / "check_m228_a012_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_contract.py",
    "a012_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_a012_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_contract.py",
    "a012_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_a012_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_cross_lane_integration_sync_b012_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b012_ownership_aware_lowering_behavior_cross_lane_integration_sync_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B012-SCA-01", "bool lowering_pass_graph_conformance_corpus_ready = false;"),
        (
            "M228-B012-SCA-02",
            "bool lowering_pass_graph_performance_quality_guardrails_ready = false;",
        ),
        ("M228-B012-SCA-03", "bool cross_lane_integration_consistent = false;"),
        ("M228-B012-SCA-04", "bool cross_lane_integration_ready = false;"),
        ("M228-B012-SCA-05", "std::string lowering_pass_graph_conformance_corpus_key;"),
        (
            "M228-B012-SCA-06",
            "std::string lowering_pass_graph_performance_quality_guardrails_key;",
        ),
        ("M228-B012-SCA-07", "std::string cross_lane_integration_key;"),
        (
            "M228-B012-SCA-08",
            "BuildObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationKey(",
        ),
        ("M228-B012-SCA-09", "scaffold.cross_lane_integration_consistent ="),
        ("M228-B012-SCA-10", "scaffold.cross_lane_integration_ready ="),
        ("M228-B012-SCA-11", "scaffold.cross_lane_integration_key ="),
        (
            "M228-B012-SCA-12",
            "inline bool IsObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationReady(",
        ),
        (
            "M228-B012-SCA-13",
            "ownership-aware lowering lane-A pass-graph conformance corpus is not ready",
        ),
        (
            "M228-B012-SCA-14",
            "ownership-aware lowering cross-lane integration is not ready",
        ),
    ),
    "artifacts_source": (
        ("M228-B012-ART-01", "BuildObjc3OwnershipAwareLoweringBehaviorScaffold("),
        ("M228-B012-ART-02", ".conformance_corpus_ready,"),
        ("M228-B012-ART-03", ".conformance_corpus_key,"),
        ("M228-B012-ART-04", ".performance_quality_guardrails_ready,"),
        ("M228-B012-ART-05", ".performance_quality_guardrails_key);"),
        (
            "M228-B012-ART-06",
            "IsObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationReady(",
        ),
        ("M228-B012-ART-07", '"O3L329"'),
        (
            "M228-B012-ART-08",
            "ownership-aware lowering cross-lane integration check failed",
        ),
        (
            "M228-B012-ART-09",
            "ir_frontend_metadata.ownership_aware_lowering_cross_lane_integration_ready =",
        ),
        (
            "M228-B012-ART-10",
            "ir_frontend_metadata.ownership_aware_lowering_cross_lane_integration_key =",
        ),
    ),
    "ir_header": (
        (
            "M228-B012-IRH-01",
            "bool ownership_aware_lowering_cross_lane_integration_ready = false;",
        ),
        (
            "M228-B012-IRH-02",
            "std::string ownership_aware_lowering_cross_lane_integration_key;",
        ),
    ),
    "ir_source": (
        (
            "M228-B012-IRS-01",
            'out << "; ownership_aware_lowering_cross_lane_integration = "',
        ),
        (
            "M228-B012-IRS-02",
            'out << "; ownership_aware_lowering_cross_lane_integration_ready = "',
        ),
    ),
    "b011_contract_doc": (
        (
            "M228-B012-DEP-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-performance-quality-guardrails/m228-b011-v1`",
        ),
    ),
    "b011_checker": (
        (
            "M228-B012-DEP-02",
            'MODE = "m228-b011-ownership-aware-lowering-behavior-performance-quality-guardrails-contract-v1"',
        ),
    ),
    "b011_tooling_test": (
        ("M228-B012-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b011_packet_doc": (
        ("M228-B012-DEP-04", "Packet: `M228-B011`"),
        ("M228-B012-DEP-05", "Issue: `#5205`"),
    ),
    "a012_contract_doc": (
        (
            "M228-B012-DEP-06",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-cross-lane-integration-sync/m228-a012-v1`",
        ),
    ),
    "a012_checker": (
        (
            "M228-B012-DEP-07",
            'MODE = "m228-a012-lowering-pipeline-pass-graph-cross-lane-integration-sync-contract-v1"',
        ),
    ),
    "a012_tooling_test": (
        (
            "M228-B012-DEP-08",
            "def test_contract_fails_closed_when_lane_contract_id_drifts(",
        ),
    ),
    "a012_packet_doc": (
        ("M228-B012-DEP-09", "Packet: `M228-A012`"),
        ("M228-B012-DEP-10", "Dependencies: `M228-A011`"),
    ),
    "contract_doc": (
        (
            "M228-B012-DOC-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-cross-lane-integration-sync/m228-b012-v1`",
        ),
        ("M228-B012-DOC-02", "Execute issue `#5206`"),
        ("M228-B012-DOC-03", "Dependencies: `M228-B011`, `M228-A012`"),
        (
            "M228-B012-DOC-04",
            "BuildObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationKey",
        ),
        (
            "M228-B012-DOC-05",
            "IsObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationReady",
        ),
        ("M228-B012-DOC-06", "O3L329"),
        (
            "M228-B012-DOC-07",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        (
            "M228-B012-DOC-08",
            "check:objc3c:m228-b012-lane-b-readiness",
        ),
        ("M228-B012-DOC-09", "test:objc3c:lowering-replay-proof"),
    ),
    "planning_packet": (
        (
            "M228-B012-PKT-01",
            "# M228-B012 Ownership-Aware Lowering Behavior Cross-Lane Integration Sync Packet",
        ),
        ("M228-B012-PKT-02", "Packet: `M228-B012`"),
        ("M228-B012-PKT-03", "Issue: `#5206`"),
        ("M228-B012-PKT-04", "Dependencies: `M228-B011`, `M228-A012`"),
        (
            "M228-B012-PKT-05",
            "scripts/check_m228_b012_ownership_aware_lowering_behavior_cross_lane_integration_sync_contract.py",
        ),
        (
            "M228-B012-PKT-06",
            "scripts/check_m228_a012_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_contract.py",
        ),
        (
            "M228-B012-PKT-07",
            "tmp/reports/m228/M228-B012/ownership_aware_lowering_behavior_cross_lane_integration_sync_contract_summary.json",
        ),
    ),
    "package_json": (
        (
            "M228-B012-PKG-01",
            '"check:objc3c:m228-b012-ownership-aware-lowering-behavior-cross-lane-integration-sync-contract"',
        ),
        (
            "M228-B012-PKG-02",
            '"test:tooling:m228-b012-ownership-aware-lowering-behavior-cross-lane-integration-sync-contract"',
        ),
        ("M228-B012-PKG-03", '"check:objc3c:m228-b012-lane-b-readiness"'),
        ("M228-B012-PKG-04", '"check:objc3c:m228-b011-lane-b-readiness"'),
        (
            "M228-B012-PKG-05",
            '"check:objc3c:m228-a012-lowering-pipeline-pass-graph-cross-lane-integration-sync-contract"',
        ),
        (
            "M228-B012-PKG-06",
            '"test:tooling:m228-a012-lowering-pipeline-pass-graph-cross-lane-integration-sync-contract"',
        ),
    ),
    "architecture_doc": (
        ("M228-B012-ARC-01", "M228 lane-B B012 cross-lane integration sync extends"),
    ),
    "lowering_spec": (
        (
            "M228-B012-SPEC-01",
            "ownership-aware lowering cross-lane integration sync shall include",
        ),
    ),
    "metadata_spec": (
        (
            "M228-B012-META-01",
            "cross-lane-integration-key anchors for lane-B",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        (
            "M228-B012-FORB-01",
            "scaffold.cross_lane_integration_ready = true;",
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
            "tmp/reports/m228/M228-B012/ownership_aware_lowering_behavior_cross_lane_integration_sync_contract_summary.json"
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
            findings.append(Finding(artifact, f"M228-B012-MISS-{artifact.upper()}", str(exc)))
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
