#!/usr/bin/env python3
"""Fail-closed validator for M228-C005 IR emission edge-case compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-c005-ir-emission-completeness-edge-case-and-compatibility-completion-contract-v1"

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
    "c004_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_core_feature_expansion_c004_expectations.md",
    "c004_checker": ROOT / "scripts" / "check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py",
    "c004_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py",
    "c004_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c004_ir_emission_completeness_core_feature_expansion_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_edge_case_and_compatibility_completion_c005_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-C005-SUR-01", "bool pass_graph_edge_case_compatibility_ready = false;"),
        ("M228-C005-SUR-02", "bool compatibility_handoff_consistent = false;"),
        (
            "M228-C005-SUR-03",
            "bool language_version_pragma_coordinate_order_consistent = false;",
        ),
        (
            "M228-C005-SUR-04",
            "bool parse_artifact_edge_case_robustness_consistent = false;",
        ),
        ("M228-C005-SUR-05", "bool parse_artifact_replay_key_deterministic = false;"),
        (
            "M228-C005-SUR-06",
            "bool edge_case_compatibility_key_transport_ready = false;",
        ),
        (
            "M228-C005-SUR-07",
            "bool core_feature_edge_case_compatibility_ready = false;",
        ),
        ("M228-C005-SUR-08", "std::string pass_graph_edge_case_compatibility_key;"),
        ("M228-C005-SUR-09", "std::string compatibility_handoff_key;"),
        ("M228-C005-SUR-10", "std::string parse_artifact_edge_robustness_key;"),
        ("M228-C005-SUR-11", "std::string edge_case_compatibility_key;"),
        (
            "M228-C005-SUR-12",
            "BuildObjc3IREmissionCoreFeatureEdgeCaseCompatibilityKey(",
        ),
        ("M228-C005-SUR-13", "surface.pass_graph_edge_case_compatibility_ready ="),
        ("M228-C005-SUR-14", "surface.compatibility_handoff_consistent ="),
        (
            "M228-C005-SUR-15",
            "surface.language_version_pragma_coordinate_order_consistent =",
        ),
        (
            "M228-C005-SUR-16",
            "surface.parse_artifact_edge_case_robustness_consistent =",
        ),
        ("M228-C005-SUR-17", "surface.parse_artifact_replay_key_deterministic ="),
        ("M228-C005-SUR-18", "surface.edge_case_compatibility_key_transport_ready ="),
        ("M228-C005-SUR-19", "surface.core_feature_edge_case_compatibility_ready ="),
        (
            "M228-C005-SUR-20",
            "surface.edge_case_compatibility_key =",
        ),
        (
            "M228-C005-SUR-21",
            "IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady(",
        ),
        (
            "M228-C005-SUR-22",
            "IR emission core feature edge-case compatibility key transport is not ready",
        ),
    ),
    "artifacts_source": (
        (
            "M228-C005-ART-01",
            "IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady(",
        ),
        ("M228-C005-ART-02", '"O3L316"'),
        (
            "M228-C005-ART-03",
            "IR emission core feature edge-case compatibility check failed",
        ),
        (
            "M228-C005-ART-04",
            "ir_frontend_metadata.ir_emission_core_feature_edge_case_compatibility_ready =",
        ),
        (
            "M228-C005-ART-05",
            "ir_frontend_metadata.ir_emission_core_feature_edge_case_compatibility_key =",
        ),
    ),
    "ir_header": (
        (
            "M228-C005-IRH-01",
            "bool ir_emission_core_feature_edge_case_compatibility_ready = false;",
        ),
        (
            "M228-C005-IRH-02",
            "std::string ir_emission_core_feature_edge_case_compatibility_key;",
        ),
    ),
    "ir_source": (
        (
            "M228-C005-IRS-01",
            'out << "; ir_emission_core_feature_edge_case_compatibility = "',
        ),
        (
            "M228-C005-IRS-02",
            'out << "; ir_emission_core_feature_edge_case_compatibility_ready = "',
        ),
    ),
    "c004_contract_doc": (
        (
            "M228-C005-DEP-01",
            "Contract ID: `objc3c-ir-emission-completeness-core-feature-expansion/m228-c004-v1`",
        ),
    ),
    "c004_checker": (
        (
            "M228-C005-DEP-02",
            'MODE = "m228-c004-ir-emission-completeness-core-feature-expansion-contract-v1"',
        ),
    ),
    "c004_tooling_test": (
        (
            "M228-C005-DEP-03",
            "check_m228_c004_ir_emission_completeness_core_feature_expansion_contract",
        ),
    ),
    "c004_packet_doc": (
        ("M228-C005-DEP-04", "Packet: `M228-C004`"),
        ("M228-C005-DEP-05", "Dependencies: `M228-C003`"),
    ),
    "contract_doc": (
        (
            "M228-C005-DOC-01",
            "Contract ID: `objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1`",
        ),
        (
            "M228-C005-DOC-02",
            "BuildObjc3IREmissionCoreFeatureEdgeCaseCompatibilityKey",
        ),
        (
            "M228-C005-DOC-03",
            "IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady",
        ),
        ("M228-C005-DOC-04", "O3L316"),
        ("M228-C005-DOC-05", "Dependencies: `M228-C004`"),
        (
            "M228-C005-DOC-06",
            "scripts/check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M228-C005-DOC-07",
            "tests/tooling/test_check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M228-C005-DOC-08",
            "spec/planning/compiler/m228/m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_packet.md",
        ),
        (
            "M228-C005-DOC-09",
            "tmp/reports/m228/M228-C005/ir_emission_completeness_edge_case_and_compatibility_completion_contract_summary.json",
        ),
        ("M228-C005-DOC-10", "package.json"),
        ("M228-C005-DOC-11", "native/objc3c/src/ARCHITECTURE.md"),
        ("M228-C005-DOC-12", "spec/LOWERING_AND_RUNTIME_CONTRACTS.md"),
        ("M228-C005-DOC-13", "spec/MODULE_METADATA_AND_ABI_TABLES.md"),
    ),
    "packet_doc": (
        (
            "M228-C005-PKT-01",
            "# M228-C005 IR Emission Completeness Edge-Case and Compatibility Completion Packet",
        ),
        ("M228-C005-PKT-02", "Packet: `M228-C005`"),
        ("M228-C005-PKT-03", "Milestone: `M228`"),
        ("M228-C005-PKT-04", "Dependencies: `M228-C004`"),
        (
            "M228-C005-PKT-05",
            "docs/contracts/m228_ir_emission_completeness_edge_case_and_compatibility_completion_c005_expectations.md",
        ),
        (
            "M228-C005-PKT-06",
            "scripts/check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M228-C005-PKT-07",
            "tests/tooling/test_check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M228-C005-PKT-08",
            "tmp/reports/m228/M228-C005/ir_emission_completeness_edge_case_and_compatibility_completion_contract_summary.json",
        ),
        ("M228-C005-PKT-09", "Shared-file deltas required"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        (
            "M228-C005-FORB-01",
            "surface.core_feature_edge_case_compatibility_ready = true;",
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
            "tmp/reports/m228/M228-C005/"
            "ir_emission_completeness_edge_case_and_compatibility_completion_contract_summary.json"
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
