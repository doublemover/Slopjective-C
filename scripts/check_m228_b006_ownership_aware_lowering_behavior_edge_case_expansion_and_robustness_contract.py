#!/usr/bin/env python3
"""Fail-closed validator for M228-B006 ownership-aware lowering edge-case expansion and robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b006-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_ownership_aware_lowering_behavior_scaffold.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "b005_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_b005_expectations.md",
    "b005_checker": ROOT
    / "scripts"
    / "check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_b006_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B006-SCA-01", "bool edge_case_expansion_consistent = false;"),
        ("M228-B006-SCA-02", "bool edge_case_robustness_ready = false;"),
        ("M228-B006-SCA-03", "std::string edge_case_robustness_key;"),
        (
            "M228-B006-SCA-04",
            "BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessKey(",
        ),
        ("M228-B006-SCA-05", "scaffold.edge_case_expansion_consistent ="),
        ("M228-B006-SCA-06", "scaffold.edge_case_robustness_ready ="),
        ("M228-B006-SCA-07", "scaffold.edge_case_robustness_key ="),
        (
            "M228-B006-SCA-08",
            "BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessKey(scaffold);",
        ),
        (
            "M228-B006-SCA-09",
            "IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessReady(",
        ),
        (
            "M228-B006-SCA-10",
            "ownership-aware lowering edge-case expansion is inconsistent",
        ),
        (
            "M228-B006-SCA-11",
            "ownership-aware lowering edge-case robustness is not ready",
        ),
        (
            "M228-B006-SCA-12",
            "ownership-aware lowering edge-case robustness key is empty",
        ),
        ("M228-B006-SCA-13", "scaffold.edge_case_compatibility_ready &&"),
        ("M228-B006-SCA-14", "scaffold.edge_case_robustness_ready &&"),
        ("M228-B006-SCA-15", "!scaffold.edge_case_robustness_key.empty())"),
    ),
    "artifacts_source": (
        (
            "M228-B006-ART-01",
            "IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(",
        ),
        ("M228-B006-ART-02", '"O3L312"'),
        (
            "M228-B006-ART-03",
            "ownership-aware lowering edge-case compatibility check failed",
        ),
    ),
    "b005_contract_doc": (
        (
            "M228-B006-DEP-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-edge-case-and-compatibility-completion/m228-b005-v1`",
        ),
    ),
    "b005_checker": (
        (
            "M228-B006-DEP-02",
            'MODE = "m228-b005-ownership-aware-lowering-behavior-edge-case-and-compatibility-completion-contract-v1"',
        ),
    ),
    "contract_doc": (
        (
            "M228-B006-DOC-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness/m228-b006-v1`",
        ),
        ("M228-B006-DOC-02", "Dependencies: `M228-B005`"),
        (
            "M228-B006-DOC-03",
            "BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessKey",
        ),
        (
            "M228-B006-DOC-04",
            "IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady",
        ),
        (
            "M228-B006-DOC-05",
            "scripts/check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M228-B006-DOC-06",
            "tests/tooling/test_check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M228-B006-DOC-07",
            "tmp/reports/m228/M228-B006/ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract_summary.json",
        ),
        ("M228-B006-DOC-08", "check:objc3c:m228-b006-lane-b-readiness"),
        (
            "M228-B006-DOC-09",
            "native/objc3c/src/ARCHITECTURE.md",
        ),
    ),
    "planning_packet": (
        (
            "M228-B006-PKT-01",
            "# M228-B006 Ownership-Aware Lowering Behavior Edge-Case Expansion and Robustness Packet",
        ),
        ("M228-B006-PKT-02", "Packet: `M228-B006`"),
        ("M228-B006-PKT-03", "Milestone: `M228`"),
        ("M228-B006-PKT-04", "Dependencies: `M228-B005`"),
        (
            "M228-B006-PKT-05",
            "docs/contracts/m228_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_b006_expectations.md",
        ),
        (
            "M228-B006-PKT-06",
            "scripts/check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M228-B006-PKT-07",
            "tests/tooling/test_check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M228-B006-PKT-08",
            "tmp/reports/m228/M228-B006/ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract_summary.json",
        ),
        ("M228-B006-PKT-09", "edge-case expansion and robustness"),
        ("M228-B006-PKT-10", "`check:objc3c:m228-b006-lane-b-readiness`"),
        ("M228-B006-PKT-11", "`native/objc3c/src/ARCHITECTURE.md`"),
    ),
    "package_json": (
        (
            "M228-B006-CFG-01",
            '"check:objc3c:m228-b006-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness-contract"',
        ),
        (
            "M228-B006-CFG-02",
            '"test:tooling:m228-b006-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness-contract"',
        ),
        ("M228-B006-CFG-03", '"check:objc3c:m228-b006-lane-b-readiness"'),
        (
            "M228-B006-CFG-04",
            "npm run check:objc3c:m228-b005-lane-b-readiness && npm run check:objc3c:m228-b006-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness-contract && npm run test:tooling:m228-b006-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness-contract",
        ),
    ),
    "architecture_doc": (
        (
            "M228-B006-ARCH-01",
            "M228 lane-B B006 edge-case expansion and robustness extends",
        ),
    ),
    "lowering_spec": (
        (
            "M228-B006-SPC-01",
            "ownership-aware lowering edge-case expansion and robustness shall include",
        ),
    ),
    "metadata_spec": (
        (
            "M228-B006-META-01",
            "deterministic ownership-aware lowering edge-case robustness",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B006-FORB-01", "scaffold.edge_case_robustness_ready = true;"),
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
            "tmp/reports/m228/M228-B006/"
            "ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract_summary.json"
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
