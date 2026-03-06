#!/usr/bin/env python3
"""Fail-closed validator for M228-D012 object emission/link-path performance guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-d012-object-emission-link-path-reliability-cross-lane-integration-sync-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_toolchain_runtime_ga_operations_core_feature_surface.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "runbook_doc": ROOT / "docs" / "runbooks" / "m228_wave_execution_runbook.md",
    "package_json": ROOT / "package.json",
    "d011_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_performance_quality_guardrails_d011_expectations.md",
    "d011_checker": ROOT
    / "scripts"
    / "check_m228_d011_object_emission_link_path_reliability_performance_quality_guardrails_contract.py",
    "d011_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_d011_object_emission_link_path_reliability_performance_quality_guardrails_contract.py",
    "d011_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d011_object_emission_link_path_reliability_performance_quality_guardrails_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_cross_lane_integration_sync_d012_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        (
            "M228-D012-SUR-01",
            "bool cross_lane_integration_sync_consistent = false;",
        ),
        ("M228-D012-SUR-02", "bool cross_lane_integration_sync_ready = false;"),
        (
            "M228-D012-SUR-03",
            "bool cross_lane_integration_sync_key_ready = false;",
        ),
        ("M228-D012-SUR-04", "std::string cross_lane_integration_sync_key;"),
        (
            "M228-D012-SUR-05",
            "BuildObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsKey(",
        ),
        (
            "M228-D012-SUR-06",
            "toolchain-runtime-ga-operations-cross-lane-integration-sync:v1:",
        ),
        (
            "M228-D012-SUR-07",
            "surface.cross_lane_integration_sync_consistent =",
        ),
        ("M228-D012-SUR-08", "surface.cross_lane_integration_sync_ready ="),
        ("M228-D012-SUR-09", "surface.cross_lane_integration_sync_key ="),
        ("M228-D012-SUR-10", "surface.cross_lane_integration_sync_key_ready ="),
        (
            "M228-D012-SUR-11",
            "surface.core_feature_impl_ready =\n      surface.core_feature_impl_ready && surface.cross_lane_integration_sync_ready;",
        ),
        (
            "M228-D012-SUR-12",
            "surface.core_feature_impl_ready =\n      surface.core_feature_impl_ready && surface.cross_lane_integration_sync_key_ready;",
        ),
        ("M228-D012-SUR-13", ";cross_lane_integration_sync_consistent="),
        ("M228-D012-SUR-14", ";cross_lane_integration_sync_ready="),
        ("M228-D012-SUR-15", ";cross_lane_integration_sync_key_ready="),
        (
            "M228-D012-SUR-16",
            "toolchain/runtime performance quality guardrails are inconsistent",
        ),
        (
            "M228-D012-SUR-17",
            "toolchain/runtime performance quality guardrails are not ready",
        ),
        (
            "M228-D012-SUR-18",
            "toolchain/runtime performance quality guardrails key is not ready",
        ),
        (
            "M228-D012-SUR-19",
            "inline bool IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsReady(",
        ),
        (
            "M228-D012-SUR-20",
            "surface.cross_lane_integration_sync_key.find(\";conformance_corpus_key_ready=true\") !=",
        ),
        (
            "M228-D012-SUR-21",
            "surface.conformance_corpus_key_ready &&",
        ),
    ),
    "architecture_doc": (
        (
            "M228-D012-ARC-01",
            "M228 lane-D D012 cross-lane integration sync anchors deterministic",
        ),
        ("M228-D012-ARC-02", "(`cross_lane_integration_sync_*`)"),
    ),
    "lowering_spec": (
        (
            "M228-D012-SPC-01",
            "toolchain/runtime cross-lane integration sync shall remain",
        ),
    ),
    "metadata_spec": (
        (
            "M228-D012-META-01",
            "deterministic lane-D toolchain/runtime cross-lane integration sync",
        ),
    ),
    "runbook_doc": (
        (
            "M228-D012-RUN-01",
            "objc3c-object-emission-link-path-reliability-cross-lane-integration-sync/m228-d012-v1",
        ),
        ("M228-D012-RUN-02", "npm run check:objc3c:m228-d012-lane-d-readiness"),
    ),
    "package_json": (
        (
            "M228-D012-CFG-01",
            '"check:objc3c:m228-d011-object-emission-link-path-reliability-performance-quality-guardrails-contract"',
        ),
        (
            "M228-D012-CFG-02",
            '"check:objc3c:m228-d012-object-emission-link-path-reliability-cross-lane-integration-sync-contract"',
        ),
        (
            "M228-D012-CFG-03",
            '"test:tooling:m228-d012-object-emission-link-path-reliability-cross-lane-integration-sync-contract"',
        ),
        ("M228-D012-CFG-04", '"check:objc3c:m228-d012-lane-d-readiness"'),
        (
            "M228-D012-CFG-05",
            "npm run check:objc3c:m228-d011-lane-d-readiness && npm run check:objc3c:m228-d012-object-emission-link-path-reliability-cross-lane-integration-sync-contract",
        ),
    ),
    "d011_contract_doc": (
        (
            "M228-D012-DEP-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-performance-quality-guardrails/m228-d011-v1`",
        ),
    ),
    "d011_checker": (
        (
            "M228-D012-DEP-02",
            'MODE = "m228-d011-object-emission-link-path-reliability-performance-quality-guardrails-contract-v1"',
        ),
    ),
    "d011_tooling_test": (
        (
            "M228-D012-DEP-03",
            "check_m228_d011_object_emission_link_path_reliability_performance_quality_guardrails_contract",
        ),
    ),
    "d011_packet_doc": (
        ("M228-D012-DEP-04", "Packet: `M228-D011`"),
        ("M228-D012-DEP-05", "Dependencies: `M228-D010`"),
    ),
    "contract_doc": (
        (
            "M228-D012-DOC-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-cross-lane-integration-sync/m228-d012-v1`",
        ),
        ("M228-D012-DOC-02", "Dependencies: `M228-D011`"),
        ("M228-D012-DOC-03", "cross_lane_integration_sync_consistent"),
        ("M228-D012-DOC-04", "cross_lane_integration_sync_ready"),
        ("M228-D012-DOC-05", "cross_lane_integration_sync_key_ready"),
        (
            "M228-D012-DOC-06",
            "BuildObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsKey",
        ),
        (
            "M228-D012-DOC-07",
            "IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsReady",
        ),
        (
            "M228-D012-DOC-08",
            "scripts/check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract.py",
        ),
        (
            "M228-D012-DOC-09",
            "tests/tooling/test_check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract.py",
        ),
        (
            "M228-D012-DOC-10",
            "tmp/reports/m228/M228-D012/object_emission_link_path_reliability_cross_lane_integration_sync_contract_summary.json",
        ),
        ("M228-D012-DOC-11", "Shared-file deltas required for full lane-D readiness"),
        ("M228-D012-DOC-12", "package.json"),
        ("M228-D012-DOC-13", "docs/runbooks/m228_wave_execution_runbook.md"),
        ("M228-D012-DOC-14", "native/objc3c/src/ARCHITECTURE.md"),
        ("M228-D012-DOC-15", "spec/LOWERING_AND_RUNTIME_CONTRACTS.md"),
        ("M228-D012-DOC-16", "spec/MODULE_METADATA_AND_ABI_TABLES.md"),
    ),
    "planning_packet": (
        (
            "M228-D012-PKT-01",
            "# M228-D012 Object Emission and Link Path Reliability Cross-lane Integration Sync Packet",
        ),
        ("M228-D012-PKT-02", "Packet: `M228-D012`"),
        ("M228-D012-PKT-03", "Milestone: `M228`"),
        ("M228-D012-PKT-04", "Dependencies: `M228-D011`"),
        (
            "M228-D012-PKT-05",
            "docs/contracts/m228_object_emission_link_path_reliability_cross_lane_integration_sync_d012_expectations.md",
        ),
        (
            "M228-D012-PKT-06",
            "scripts/check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract.py",
        ),
        (
            "M228-D012-PKT-07",
            "tests/tooling/test_check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract.py",
        ),
        (
            "M228-D012-PKT-08",
            "tmp/reports/m228/M228-D012/object_emission_link_path_reliability_cross_lane_integration_sync_contract_summary.json",
        ),
        ("M228-D012-PKT-09", "cross-lane integration sync"),
        ("M228-D012-PKT-10", "Shared-file deltas required for full lane-D readiness"),
        (
            "M228-D012-PKT-11",
            "python scripts/check_m228_d011_object_emission_link_path_reliability_performance_quality_guardrails_contract.py",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        (
            "M228-D012-FORB-01",
            "surface.cross_lane_integration_sync_ready = true;",
        ),
        (
            "M228-D012-FORB-02",
            "surface.cross_lane_integration_sync_consistent = true;",
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
            "tmp/reports/m228/M228-D012/"
            "object_emission_link_path_reliability_cross_lane_integration_sync_contract_summary.json"
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

