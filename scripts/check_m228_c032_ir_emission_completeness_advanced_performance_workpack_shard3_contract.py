#!/usr/bin/env python3
"""Fail-closed validator for M228-C032 IR-emission advanced integration shard1 contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-c032-ir-emission-completeness-advanced-performance-workpack-shard3-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_ir_emission_core_feature_implementation_surface.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "ir_header": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h",
    "ir_source": ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    "runbook_doc": ROOT / "docs" / "runbooks" / "m228_wave_execution_runbook.md",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
    "c031_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_advanced_integration_workpack_shard3_c031_expectations.md",
    "c031_checker": ROOT
    / "scripts"
    / "check_m228_c031_ir_emission_completeness_advanced_integration_workpack_shard3_contract.py",
    "c031_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_c031_ir_emission_completeness_advanced_integration_workpack_shard3_contract.py",
    "c031_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c031_ir_emission_completeness_advanced_integration_workpack_shard3_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_advanced_performance_workpack_shard3_c032_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        (
            "M228-C032-SUR-01",
            "bool pass_graph_advanced_integration_shard1_ready = false;",
        ),
        (
            "M228-C032-SUR-02",
            "bool parse_artifact_advanced_integration_shard1_consistent = false;",
        ),
        (
            "M228-C032-SUR-03",
            "bool typed_handoff_advanced_integration_shard1_consistent = false;",
        ),
        (
            "M228-C032-SUR-04",
            "bool advanced_integration_shard1_consistent = false;",
        ),
        (
            "M228-C032-SUR-05",
            "bool advanced_integration_shard1_key_transport_ready = false;",
        ),
        (
            "M228-C032-SUR-06",
            "bool core_feature_advanced_integration_shard1_ready = false;",
        ),
        (
            "M228-C032-SUR-07",
            "std::string pass_graph_advanced_integration_shard1_key;",
        ),
        (
            "M228-C032-SUR-08",
            "std::string parse_artifact_advanced_integration_shard1_key;",
        ),
        (
            "M228-C032-SUR-09",
            "std::string typed_handoff_advanced_integration_shard1_key;",
        ),
        (
            "M228-C032-SUR-10",
            "std::string advanced_integration_shard1_key;",
        ),
        (
            "M228-C032-SUR-11",
            "std::string advanced_integration_shard1_failure_reason;",
        ),
        (
            "M228-C032-SUR-12",
            "BuildObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Key(",
        ),
        (
            "M228-C032-SUR-13",
            "surface.parse_artifact_advanced_integration_shard1_consistent =",
        ),
        (
            "M228-C032-SUR-14",
            "surface.typed_handoff_advanced_integration_shard1_consistent =",
        ),
        (
            "M228-C032-SUR-15",
            "surface.advanced_integration_shard1_consistent =",
        ),
        (
            "M228-C032-SUR-16",
            "surface.advanced_integration_shard1_key_transport_ready =",
        ),
        (
            "M228-C032-SUR-17",
            "surface.core_feature_advanced_integration_shard1_ready =",
        ),
        (
            "M228-C032-SUR-18",
            "surface.advanced_integration_shard1_key =",
        ),
        (
            "M228-C032-SUR-19",
            "IR emission core feature parse artifact advanced integration shard 1 is inconsistent",
        ),
        (
            "M228-C032-SUR-20",
            "IR emission core feature typed handoff advanced integration shard 1 is inconsistent",
        ),
        (
            "M228-C032-SUR-21",
            "IR emission core feature advanced integration shard 1 key transport is not ready",
        ),
        (
            "M228-C032-SUR-22",
            "inline bool IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(",
        ),
        (
            "M228-C032-SUR-23",
            "!surface.advanced_integration_shard1_key.empty())",
        ),
    ),
    "artifacts_source": (
        (
            "M228-C032-ART-01",
            "IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(",
        ),
        ("M228-C032-ART-02", '"O3L337"'),
        (
            "M228-C032-ART-03",
            "IR emission core feature advanced integration shard 1 check failed",
        ),
        (
            "M228-C032-ART-04",
            "ir_frontend_metadata.ir_emission_core_feature_advanced_integration_shard1_ready =",
        ),
        (
            "M228-C032-ART-05",
            "ir_frontend_metadata.ir_emission_core_feature_advanced_integration_shard1_key =",
        ),
    ),
    "ir_header": (
        (
            "M228-C032-IRH-01",
            "bool ir_emission_core_feature_advanced_integration_shard1_ready = false;",
        ),
        (
            "M228-C032-IRH-02",
            "std::string ir_emission_core_feature_advanced_integration_shard1_key;",
        ),
    ),
    "ir_source": (
        (
            "M228-C032-IRS-01",
            'out << "; ir_emission_core_feature_advanced_integration_shard1 = "',
        ),
        (
            "M228-C032-IRS-02",
            'out << "; ir_emission_core_feature_advanced_integration_shard1_ready = "',
        ),
    ),
    "runbook_doc": (
        (
            "M228-C032-RUN-01",
            "objc3c-ir-emission-completeness-advanced-performance-workpack-shard3/m228-c032-v1",
        ),
        (
            "M228-C032-RUN-02",
            "python scripts/check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py",
        ),
        (
            "M228-C032-RUN-03",
            "python -m pytest tests/tooling/test_check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py -q",
        ),
        ("M228-C032-RUN-04", "npm run check:objc3c:m228-c032-lane-c-readiness"),
    ),
    "architecture_doc": (
        (
            "M228-C032-ARC-01",
            "M228 lane-C C032 IR-emission advanced performance workpack (shard 3) anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M228-C032-SPC-01",
            "IR-emission advanced performance workpack (shard 3) governance shall",
        ),
        (
            "M228-C032-SPC-02",
            "lane-C dependency anchors (`M228-C032`, `M228-C031`)",
        ),
    ),
    "metadata_spec": (
        (
            "M228-C032-META-01",
            "deterministic lane-C IR-emission advanced-integration-shard1 metadata anchors",
        ),
    ),
    "package_json": (
        ("M228-C032-PKG-01", '"check:objc3c:m228-c031-lane-c-readiness"'),
        (
            "M228-C032-PKG-02",
            '"check:objc3c:m228-c032-ir-emission-completeness-advanced-performance-workpack-shard3-contract"',
        ),
        (
            "M228-C032-PKG-03",
            '"test:tooling:m228-c032-ir-emission-completeness-advanced-performance-workpack-shard3-contract"',
        ),
        ("M228-C032-PKG-04", '"check:objc3c:m228-c032-lane-c-readiness"'),
        (
            "M228-C032-PKG-05",
            '"check:objc3c:m228-c032-lane-c-readiness": "npm run check:objc3c:m228-c031-lane-c-readiness &&',
        ),
    ),
    "c031_contract_doc": (
        (
            "M228-C032-DEP-01",
            "Contract ID: `objc3c-ir-emission-completeness-advanced-integration-workpack-shard3/m228-c031-v1`",
        ),
    ),
    "c031_checker": (
        (
            "M228-C032-DEP-02",
            'MODE = "m228-c031-ir-emission-completeness-advanced-integration-workpack-shard3-contract-v1"',
        ),
    ),
    "c031_tooling_test": (
        ("M228-C032-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "c031_packet_doc": (
        ("M228-C032-DEP-04", "Packet: `M228-C031`"),
        ("M228-C032-DEP-05", "Issue: `#5257`"),
    ),
    "contract_doc": (
        (
            "M228-C032-DOC-01",
            "Contract ID: `objc3c-ir-emission-completeness-advanced-performance-workpack-shard3/m228-c032-v1`",
        ),
        ("M228-C032-DOC-02", "Execute issue `#5258`"),
        ("M228-C032-DOC-03", "Dependencies: `M228-C031`"),
        (
            "M228-C032-DOC-04",
            "BuildObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Key",
        ),
        (
            "M228-C032-DOC-05",
            "IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready",
        ),
        ("M228-C032-DOC-06", "O3L337"),
        (
            "M228-C032-DOC-07",
            "scripts/check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py",
        ),
        (
            "M228-C032-DOC-08",
            "tests/tooling/test_check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py",
        ),
        (
            "M228-C032-DOC-09",
            "spec/planning/compiler/m228/m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_packet.md",
        ),
        (
            "M228-C032-DOC-10",
            "tmp/reports/m228/M228-C032/ir_emission_completeness_advanced_performance_workpack_shard3_contract_summary.json",
        ),
    ),
    "packet_doc": (
        (
            "M228-C032-PKT-01",
            "# M228-C032 IR Emission Completeness Advanced Performance Workpack (Shard 3) Packet",
        ),
        ("M228-C032-PKT-02", "Packet: `M228-C032`"),
        ("M228-C032-PKT-03", "Issue: `#5258`"),
        ("M228-C032-PKT-04", "Dependencies: `M228-C031`"),
        (
            "M228-C032-PKT-05",
            "docs/contracts/m228_ir_emission_completeness_advanced_performance_workpack_shard3_c032_expectations.md",
        ),
        (
            "M228-C032-PKT-06",
            "scripts/check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py",
        ),
        (
            "M228-C032-PKT-07",
            "tests/tooling/test_check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py",
        ),
        (
            "M228-C032-PKT-08",
            "tmp/reports/m228/M228-C032/ir_emission_completeness_advanced_performance_workpack_shard3_contract_summary.json",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M228-C032-FORB-01",
            '"check:objc3c:m228-c032-lane-c-readiness": "npm run check:objc3c:m228-c017-lane-c-readiness',
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
            "tmp/reports/m228/M228-C032/"
            "ir_emission_completeness_advanced_performance_workpack_shard3_contract_summary.json"
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
















