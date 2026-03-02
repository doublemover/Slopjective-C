#!/usr/bin/env python3
"""Fail-closed validator for M228-C006 IR emission edge-case expansion and robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-c006-ir-emission-completeness-edge-case-expansion-and-robustness-contract-v1"

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
    "c005_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_edge_case_and_compatibility_completion_c005_expectations.md",
    "c005_checker": ROOT
    / "scripts"
    / "check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py",
    "c005_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py",
    "c005_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_edge_case_expansion_and_robustness_c006_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-C006-SUR-01", "bool pass_graph_edge_case_robustness_ready = false;"),
        ("M228-C006-SUR-02", "bool edge_case_expansion_consistent = false;"),
        ("M228-C006-SUR-03", "bool parse_artifact_edge_case_robustness_ready = false;"),
        (
            "M228-C006-SUR-04",
            "bool edge_case_robustness_key_transport_ready = false;",
        ),
        (
            "M228-C006-SUR-05",
            "bool core_feature_edge_case_robustness_ready = false;",
        ),
        (
            "M228-C006-SUR-06",
            "std::string pass_graph_edge_case_robustness_key;",
        ),
        (
            "M228-C006-SUR-07",
            "std::string parse_artifact_edge_case_expansion_key;",
        ),
        ("M228-C006-SUR-08", "std::string edge_case_robustness_key;"),
        (
            "M228-C006-SUR-09",
            "std::string edge_case_robustness_failure_reason;",
        ),
        (
            "M228-C006-SUR-10",
            "BuildObjc3IREmissionCoreFeatureEdgeCaseRobustnessKey(",
        ),
        (
            "M228-C006-SUR-11",
            "surface.pass_graph_edge_case_robustness_ready =",
        ),
        ("M228-C006-SUR-12", "surface.edge_case_expansion_consistent ="),
        (
            "M228-C006-SUR-13",
            "surface.parse_artifact_edge_case_robustness_ready =",
        ),
        ("M228-C006-SUR-14", "surface.pass_graph_edge_case_robustness_key ="),
        (
            "M228-C006-SUR-15",
            "surface.parse_artifact_edge_case_expansion_key =",
        ),
        (
            "M228-C006-SUR-16",
            "surface.edge_case_robustness_key_transport_ready =",
        ),
        (
            "M228-C006-SUR-17",
            "surface.core_feature_edge_case_robustness_ready =",
        ),
        ("M228-C006-SUR-18", "surface.edge_case_robustness_key ="),
        (
            "M228-C006-SUR-19",
            "IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady(",
        ),
        (
            "M228-C006-SUR-20",
            "IR emission core feature edge-case expansion is inconsistent",
        ),
        (
            "M228-C006-SUR-21",
            "IR emission core feature parse artifact edge-case robustness is not ready",
        ),
        (
            "M228-C006-SUR-22",
            "IR emission core feature edge-case robustness key transport is not ready",
        ),
    ),
    "artifacts_source": (
        (
            "M228-C006-ART-01",
            "IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady(",
        ),
        ("M228-C006-ART-02", '"O3L317"'),
        (
            "M228-C006-ART-03",
            "IR emission core feature edge-case robustness check failed",
        ),
        (
            "M228-C006-ART-04",
            "ir_frontend_metadata.ir_emission_core_feature_edge_case_robustness_ready =",
        ),
        (
            "M228-C006-ART-05",
            "ir_frontend_metadata.ir_emission_core_feature_edge_case_robustness_key =",
        ),
    ),
    "ir_header": (
        (
            "M228-C006-IRH-01",
            "bool ir_emission_core_feature_edge_case_robustness_ready = false;",
        ),
        (
            "M228-C006-IRH-02",
            "std::string ir_emission_core_feature_edge_case_robustness_key;",
        ),
    ),
    "ir_source": (
        (
            "M228-C006-IRS-01",
            'out << "; ir_emission_core_feature_edge_case_robustness = "',
        ),
        (
            "M228-C006-IRS-02",
            'out << "; ir_emission_core_feature_edge_case_robustness_ready = "',
        ),
    ),
    "c005_contract_doc": (
        (
            "M228-C006-DEP-01",
            "Contract ID: `objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1`",
        ),
    ),
    "c005_checker": (
        (
            "M228-C006-DEP-02",
            'MODE = "m228-c005-ir-emission-completeness-edge-case-and-compatibility-completion-contract-v1"',
        ),
    ),
    "c005_tooling_test": (
        (
            "M228-C006-DEP-03",
            "check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract",
        ),
    ),
    "c005_packet_doc": (
        ("M228-C006-DEP-04", "Packet: `M228-C005`"),
        ("M228-C006-DEP-05", "Dependencies: `M228-C004`"),
    ),
    "contract_doc": (
        (
            "M228-C006-DOC-01",
            "Contract ID: `objc3c-ir-emission-completeness-edge-case-expansion-and-robustness/m228-c006-v1`",
        ),
        (
            "M228-C006-DOC-02",
            "BuildObjc3IREmissionCoreFeatureEdgeCaseRobustnessKey",
        ),
        (
            "M228-C006-DOC-03",
            "IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady",
        ),
        ("M228-C006-DOC-04", "O3L317"),
        ("M228-C006-DOC-05", "Dependencies: `M228-C005`"),
        (
            "M228-C006-DOC-06",
            "scripts/check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M228-C006-DOC-07",
            "tests/tooling/test_check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M228-C006-DOC-08",
            "spec/planning/compiler/m228/m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_packet.md",
        ),
        (
            "M228-C006-DOC-09",
            "tmp/reports/m228/M228-C006/ir_emission_completeness_edge_case_expansion_and_robustness_contract_summary.json",
        ),
        ("M228-C006-DOC-10", "package.json"),
        ("M228-C006-DOC-11", "native/objc3c/src/ARCHITECTURE.md"),
        ("M228-C006-DOC-12", "spec/LOWERING_AND_RUNTIME_CONTRACTS.md"),
        ("M228-C006-DOC-13", "spec/MODULE_METADATA_AND_ABI_TABLES.md"),
    ),
    "packet_doc": (
        (
            "M228-C006-PKT-01",
            "# M228-C006 IR Emission Completeness Edge-Case Expansion and Robustness Packet",
        ),
        ("M228-C006-PKT-02", "Packet: `M228-C006`"),
        ("M228-C006-PKT-03", "Milestone: `M228`"),
        ("M228-C006-PKT-04", "Dependencies: `M228-C005`"),
        (
            "M228-C006-PKT-05",
            "docs/contracts/m228_ir_emission_completeness_edge_case_expansion_and_robustness_c006_expectations.md",
        ),
        (
            "M228-C006-PKT-06",
            "scripts/check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M228-C006-PKT-07",
            "tests/tooling/test_check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M228-C006-PKT-08",
            "tmp/reports/m228/M228-C006/ir_emission_completeness_edge_case_expansion_and_robustness_contract_summary.json",
        ),
        ("M228-C006-PKT-09", "Shared-file deltas required"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        (
            "M228-C006-FORB-01",
            "surface.core_feature_edge_case_robustness_ready = true;",
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
            "tmp/reports/m228/M228-C006/"
            "ir_emission_completeness_edge_case_expansion_and_robustness_contract_summary.json"
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
