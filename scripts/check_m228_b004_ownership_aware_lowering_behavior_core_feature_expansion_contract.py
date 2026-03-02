#!/usr/bin/env python3
"""Fail-closed validator for M228-B004 ownership-aware lowering core-feature expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b004-ownership-aware-lowering-behavior-core-feature-expansion-contract-v1"

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
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "b003_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_core_feature_implementation_b003_expectations.md",
    "b003_checker": ROOT
    / "scripts"
    / "check_m228_b003_ownership_aware_lowering_behavior_core_feature_implementation_contract.py",
    "package_json": ROOT / "package.json",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_core_feature_expansion_b004_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B004-SCA-01", "struct Objc3OwnershipAwareLoweringBehaviorScaffold {"),
        ("M228-B004-SCA-02", "bool weak_unowned_semantics_contract_ready = false;"),
        ("M228-B004-SCA-03", "bool ownership_profile_accounting_consistent = false;"),
        ("M228-B004-SCA-04", "bool expansion_replay_keys_ready = false;"),
        ("M228-B004-SCA-05", "bool expansion_deterministic_replay_surface = false;"),
        ("M228-B004-SCA-06", "bool expansion_ready = false;"),
        ("M228-B004-SCA-07", "std::string weak_unowned_semantics_replay_key;"),
        ("M228-B004-SCA-08", "std::string expansion_key;"),
        ("M228-B004-SCA-09", "BuildObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionKey("),
        ("M228-B004-SCA-10", "scaffold.expansion_replay_keys_ready ="),
        ("M228-B004-SCA-11", "scaffold.expansion_deterministic_replay_surface ="),
        ("M228-B004-SCA-12", "kObjc3WeakUnownedSemanticsLoweringLaneContract"),
        ("M228-B004-SCA-13", "scaffold.expansion_ready ="),
        (
            "M228-B004-SCA-14",
            "BuildObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionKey(scaffold);",
        ),
        (
            "M228-B004-SCA-15",
            "IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady(",
        ),
        (
            "M228-B004-SCA-16",
            "ownership-aware lowering expansion accounting is inconsistent",
        ),
        (
            "M228-B004-SCA-17",
            "ownership-aware lowering core feature expansion is not ready",
        ),
    ),
    "artifacts_source": (
        ("M228-B004-ART-01", "BuildObjc3OwnershipAwareLoweringBehaviorScaffold("),
        ("M228-B004-ART-02", "weak_unowned_semantics_lowering_contract,"),
        ("M228-B004-ART-03", "weak_unowned_semantics_lowering_replay_key,"),
        ("M228-B004-ART-04", "IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady("),
        ("M228-B004-ART-05", '"O3L310"'),
        (
            "M228-B004-ART-06",
            "ownership-aware lowering core feature expansion check failed",
        ),
        (
            "M228-B004-ART-07",
            "ir_frontend_metadata.ownership_aware_lowering_core_feature_expansion_ready =",
        ),
        (
            "M228-B004-ART-08",
            "ir_frontend_metadata.ownership_aware_lowering_core_feature_expansion_key =",
        ),
    ),
    "ir_header": (
        (
            "M228-B004-IRH-01",
            "bool ownership_aware_lowering_core_feature_expansion_ready = false;",
        ),
        (
            "M228-B004-IRH-02",
            "std::string ownership_aware_lowering_core_feature_expansion_key;",
        ),
    ),
    "ir_source": (
        (
            "M228-B004-IRS-01",
            'out << "; ownership_aware_lowering_core_feature_expansion = "',
        ),
        (
            "M228-B004-IRS-02",
            'out << "; ownership_aware_lowering_core_feature_expansion_ready = "',
        ),
    ),
    "architecture_doc": (
        ("M228-B004-ARCH-01", "M228 lane-B B004 core feature expansion extends"),
        (
            "M228-B004-ARCH-02",
            "ownership-aware lowering fails closed when weak/unowned expansion",
        ),
    ),
    "lowering_spec": (
        (
            "M228-B004-SPC-01",
            "ownership-aware lowering core feature expansion shall remain fail-closed for",
        ),
    ),
    "metadata_spec": (
        (
            "M228-B004-META-01",
            "ownership-aware lowering core-feature expansion readiness/key markers",
        ),
    ),
    "b003_contract_doc": (
        (
            "M228-B004-DEP-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-core-feature-implementation/m228-b003-v1`",
        ),
    ),
    "b003_checker": (
        (
            "M228-B004-DEP-02",
            'MODE = "m228-b003-ownership-aware-lowering-behavior-core-feature-implementation-contract-v1"',
        ),
    ),
    "package_json": (
        (
            "M228-B004-CFG-01",
            '"check:objc3c:m228-b004-ownership-aware-lowering-behavior-core-feature-expansion-contract"',
        ),
        (
            "M228-B004-CFG-02",
            '"test:tooling:m228-b004-ownership-aware-lowering-behavior-core-feature-expansion-contract"',
        ),
        ("M228-B004-CFG-03", '"check:objc3c:m228-b004-lane-b-readiness"'),
        (
            "M228-B004-CFG-04",
            "npm run check:objc3c:m228-b003-lane-b-readiness && npm run check:objc3c:m228-b004-ownership-aware-lowering-behavior-core-feature-expansion-contract",
        ),
    ),
    "contract_doc": (
        (
            "M228-B004-DOC-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-core-feature-expansion/m228-b004-v1`",
        ),
        (
            "M228-B004-DOC-02",
            "BuildObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionKey",
        ),
        (
            "M228-B004-DOC-03",
            "IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady",
        ),
        ("M228-B004-DOC-04", "O3L310"),
        (
            "M228-B004-DOC-05",
            "scripts/check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py",
        ),
        (
            "M228-B004-DOC-06",
            "tests/tooling/test_check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py",
        ),
        ("M228-B004-DOC-07", "npm run check:objc3c:m228-b004-lane-b-readiness"),
        (
            "M228-B004-DOC-08",
            "tmp/reports/m228/M228-B004/ownership_aware_lowering_behavior_core_feature_expansion_contract_summary.json",
        ),
        ("M228-B004-DOC-09", "Dependencies: `M228-B003`"),
        (
            "M228-B004-DOC-10",
            "spec/planning/compiler/m228/m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_packet.md",
        ),
    ),
    "planning_packet": (
        (
            "M228-B004-PKT-01",
            "# M228-B004 Ownership-Aware Lowering Behavior Core Feature Expansion Packet",
        ),
        ("M228-B004-PKT-02", "Packet: `M228-B004`"),
        ("M228-B004-PKT-03", "Milestone: `M228`"),
        ("M228-B004-PKT-04", "Dependencies: `M228-B003`"),
        (
            "M228-B004-PKT-05",
            "docs/contracts/m228_ownership_aware_lowering_behavior_core_feature_expansion_b004_expectations.md",
        ),
        (
            "M228-B004-PKT-06",
            "scripts/check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py",
        ),
        (
            "M228-B004-PKT-07",
            "tests/tooling/test_check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py",
        ),
        ("M228-B004-PKT-08", "check:objc3c:m228-b004-lane-b-readiness"),
        (
            "M228-B004-PKT-09",
            "tmp/reports/m228/M228-B004/ownership_aware_lowering_behavior_core_feature_expansion_contract_summary.json",
        ),
        ("M228-B004-PKT-10", "weak/unowned expansion accounting"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B004-FORB-01", "scaffold.expansion_ready = true;"),
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
            "tmp/reports/m228/M228-B004/"
            "ownership_aware_lowering_behavior_core_feature_expansion_contract_summary.json"
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
