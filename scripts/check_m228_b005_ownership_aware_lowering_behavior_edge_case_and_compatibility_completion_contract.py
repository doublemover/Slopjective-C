#!/usr/bin/env python3
"""Fail-closed validator for M228-B005 ownership-aware lowering edge-case compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b005-ownership-aware-lowering-behavior-edge-case-and-compatibility-completion-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_ownership_aware_lowering_behavior_scaffold.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "b004_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_core_feature_expansion_b004_expectations.md",
    "b004_checker": ROOT
    / "scripts"
    / "check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_b005_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B005-SCA-01", "bool compatibility_handoff_consistent = false;"),
        (
            "M228-B005-SCA-02",
            "bool language_version_pragma_coordinate_order_consistent = false;",
        ),
        (
            "M228-B005-SCA-03",
            "bool parse_artifact_edge_case_robustness_consistent = false;",
        ),
        (
            "M228-B005-SCA-04",
            "bool parse_artifact_replay_key_deterministic = false;",
        ),
        ("M228-B005-SCA-05", "bool edge_case_compatibility_ready = false;"),
        ("M228-B005-SCA-06", "std::string compatibility_handoff_key;"),
        ("M228-B005-SCA-07", "std::string parse_artifact_edge_robustness_key;"),
        ("M228-B005-SCA-08", "std::string edge_case_compatibility_key;"),
        (
            "M228-B005-SCA-09",
            "BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityKey(",
        ),
        ("M228-B005-SCA-10", "scaffold.edge_case_compatibility_ready ="),
        ("M228-B005-SCA-11", "scaffold.compatibility_handoff_consistent &&"),
        (
            "M228-B005-SCA-12",
            "scaffold.language_version_pragma_coordinate_order_consistent &&",
        ),
        (
            "M228-B005-SCA-13",
            "scaffold.parse_artifact_edge_case_robustness_consistent &&",
        ),
        (
            "M228-B005-SCA-14",
            "scaffold.parse_artifact_replay_key_deterministic &&",
        ),
        ("M228-B005-SCA-15", "!scaffold.compatibility_handoff_key.empty() &&"),
        (
            "M228-B005-SCA-16",
            "!scaffold.parse_artifact_edge_robustness_key.empty();",
        ),
        ("M228-B005-SCA-17", "scaffold.edge_case_compatibility_key ="),
        (
            "M228-B005-SCA-18",
            "BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityKey(scaffold);",
        ),
        (
            "M228-B005-SCA-19",
            "IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(",
        ),
        (
            "M228-B005-SCA-20",
            "ownership-aware lowering edge-case compatibility is not ready",
        ),
    ),
    "artifacts_source": (
        ("M228-B005-ART-01", "BuildObjc3OwnershipAwareLoweringBehaviorScaffold("),
        (
            "M228-B005-ART-02",
            "pipeline_result.parse_lowering_readiness_surface\n              .compatibility_handoff_consistent,",
        ),
        (
            "M228-B005-ART-03",
            "pipeline_result.parse_lowering_readiness_surface\n              .language_version_pragma_coordinate_order_consistent,",
        ),
        (
            "M228-B005-ART-04",
            "pipeline_result.parse_lowering_readiness_surface\n              .parse_artifact_edge_case_robustness_consistent,",
        ),
        (
            "M228-B005-ART-05",
            "pipeline_result.parse_lowering_readiness_surface\n              .parse_artifact_replay_key_deterministic,",
        ),
        (
            "M228-B005-ART-06",
            "pipeline_result.parse_lowering_readiness_surface.compatibility_handoff_key,",
        ),
        (
            "M228-B005-ART-07",
            "pipeline_result.parse_lowering_readiness_surface\n              .parse_artifact_edge_robustness_key);",
        ),
        (
            "M228-B005-ART-08",
            "IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(",
        ),
        ("M228-B005-ART-09", '"O3L312"'),
        (
            "M228-B005-ART-10",
            "ownership-aware lowering edge-case compatibility check failed",
        ),
    ),
    "b004_contract_doc": (
        (
            "M228-B005-DEP-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-core-feature-expansion/m228-b004-v1`",
        ),
    ),
    "b004_checker": (
        (
            "M228-B005-DEP-02",
            'MODE = "m228-b004-ownership-aware-lowering-behavior-core-feature-expansion-contract-v1"',
        ),
    ),
    "contract_doc": (
        (
            "M228-B005-DOC-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-edge-case-and-compatibility-completion/m228-b005-v1`",
        ),
        (
            "M228-B005-DOC-02",
            "BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityKey",
        ),
        (
            "M228-B005-DOC-03",
            "IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady",
        ),
        ("M228-B005-DOC-04", "O3L312"),
        (
            "M228-B005-DOC-05",
            "scripts/check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M228-B005-DOC-06",
            "tests/tooling/test_check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py",
        ),
        ("M228-B005-DOC-07", "Dependencies: `M228-B004`"),
        (
            "M228-B005-DOC-08",
            "spec/planning/compiler/m228/m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_packet.md",
        ),
        (
            "M228-B005-DOC-09",
            "tmp/reports/m228/M228-B005/ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract_summary.json",
        ),
    ),
    "planning_packet": (
        (
            "M228-B005-PKT-01",
            "# M228-B005 Ownership-Aware Lowering Behavior Edge-Case and Compatibility Completion Packet",
        ),
        ("M228-B005-PKT-02", "Packet: `M228-B005`"),
        ("M228-B005-PKT-03", "Milestone: `M228`"),
        ("M228-B005-PKT-04", "Dependencies: `M228-B004`"),
        (
            "M228-B005-PKT-05",
            "docs/contracts/m228_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_b005_expectations.md",
        ),
        (
            "M228-B005-PKT-06",
            "scripts/check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M228-B005-PKT-07",
            "tests/tooling/test_check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M228-B005-PKT-08",
            "tmp/reports/m228/M228-B005/ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract_summary.json",
        ),
        ("M228-B005-PKT-09", "edge-case compatibility completion"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B005-FORB-01", "scaffold.edge_case_compatibility_ready = true;"),
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
            "tmp/reports/m228/M228-B005/"
            "ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract_summary.json"
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
                findings.append(
                    Finding(artifact, check_id, f"expected snippet missing: {snippet}")
                )

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(
                    Finding(artifact, check_id, f"forbidden snippet present: {snippet}")
                )
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
