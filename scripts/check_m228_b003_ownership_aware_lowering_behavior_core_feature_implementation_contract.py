#!/usr/bin/env python3
"""Fail-closed validator for M228-B003 ownership-aware lowering core-feature implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b003-ownership-aware-lowering-behavior-core-feature-implementation-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_ownership_aware_lowering_behavior_scaffold.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "b001_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_contract_freeze_b001_expectations.md",
    "b002_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_modular_split_scaffolding_b002_expectations.md",
    "b001_checker": ROOT / "scripts" / "check_m228_b001_ownership_aware_lowering_behavior_contract.py",
    "b002_checker": ROOT
    / "scripts"
    / "check_m228_b002_ownership_aware_lowering_behavior_modular_split_scaffolding_contract.py",
    "package_json": ROOT / "package.json",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_core_feature_implementation_b003_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B003-SCA-01", "struct Objc3OwnershipAwareLoweringBehaviorScaffold {"),
        ("M228-B003-SCA-02", "BuildObjc3OwnershipAwareLoweringBehaviorScaffold("),
        ("M228-B003-SCA-03", "IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady("),
        ("M228-B003-SCA-04", "scaffold.modular_split_ready ="),
        ("M228-B003-SCA-05", "ownership qualifier lowering contract is not ready"),
        ("M228-B003-SCA-06", "retain/release lowering contract is not ready"),
        ("M228-B003-SCA-07", "autoreleasepool scope lowering contract is not ready"),
        ("M228-B003-SCA-08", "ARC diagnostics/fix-it lowering contract is not ready"),
        ("M228-B003-SCA-09", "ownership-aware lowering replay keys are incomplete"),
        (
            "M228-B003-SCA-10",
            "ownership-aware lowering replay keys are not lane-contract deterministic",
        ),
    ),
    "artifacts_source": (
        ("M228-B003-ART-01", "BuildObjc3OwnershipAwareLoweringBehaviorScaffold("),
        ("M228-B003-ART-02", "IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady("),
        ("M228-B003-ART-03", '"O3L305"'),
        ("M228-B003-ART-04", "ownership-aware lowering modular split scaffold check failed"),
        ("M228-B003-ART-05", "ownership_qualifier_lowering_replay_key"),
        ("M228-B003-ART-06", "retain_release_operation_lowering_replay_key"),
        ("M228-B003-ART-07", "autoreleasepool_scope_lowering_replay_key"),
        ("M228-B003-ART-08", "arc_diagnostics_fixit_lowering_replay_key"),
    ),
    "architecture_doc": (
        (
            "M228-B003-ARCH-01",
            "M228 lane-B B003 core feature implementation anchors ownership-aware lowering",
        ),
        ("M228-B003-ARCH-02", "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h"),
    ),
    "lowering_spec": (
        (
            "M228-B003-SPC-01",
            "ownership-aware lowering core feature implementation shall remain fail-closed",
        ),
    ),
    "metadata_spec": (
        (
            "M228-B003-META-01",
            "ownership-aware lowering core-feature implementation readiness markers",
        ),
    ),
    "b001_contract_doc": (
        (
            "M228-B003-DEP-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-freeze/m228-b001-v1`",
        ),
    ),
    "b002_contract_doc": (
        (
            "M228-B003-DEP-02",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-modular-split-scaffolding/m228-b002-v1`",
        ),
    ),
    "b001_checker": (
        ("M228-B003-DEP-03", "MODE = \"m228-b001-ownership-aware-lowering-behavior-freeze-contract-v1\""),
    ),
    "b002_checker": (
        (
            "M228-B003-DEP-04",
            "MODE = \"m228-b002-ownership-aware-lowering-behavior-modular-split-scaffolding-contract-v1\"",
        ),
    ),
    "package_json": (
        (
            "M228-B003-CFG-01",
            '"check:objc3c:m228-b003-ownership-aware-lowering-behavior-core-feature-implementation-contract"',
        ),
        (
            "M228-B003-CFG-02",
            '"test:tooling:m228-b003-ownership-aware-lowering-behavior-core-feature-implementation-contract"',
        ),
        ("M228-B003-CFG-03", '"check:objc3c:m228-b003-lane-b-readiness"'),
        (
            "M228-B003-CFG-04",
            "npm run check:objc3c:m228-b002-lane-b-readiness && npm run check:objc3c:m228-b003-ownership-aware-lowering-behavior-core-feature-implementation-contract",
        ),
    ),
    "contract_doc": (
        (
            "M228-B003-DOC-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-core-feature-implementation/m228-b003-v1`",
        ),
        ("M228-B003-DOC-02", "Objc3OwnershipAwareLoweringBehaviorScaffold"),
        ("M228-B003-DOC-03", "BuildObjc3OwnershipAwareLoweringBehaviorScaffold"),
        ("M228-B003-DOC-04", "O3L305"),
        (
            "M228-B003-DOC-05",
            "scripts/check_m228_b003_ownership_aware_lowering_behavior_core_feature_implementation_contract.py",
        ),
        (
            "M228-B003-DOC-06",
            "tests/tooling/test_check_m228_b003_ownership_aware_lowering_behavior_core_feature_implementation_contract.py",
        ),
        ("M228-B003-DOC-07", "npm run check:objc3c:m228-b003-lane-b-readiness"),
        ("M228-B003-DOC-08", "npm run build:objc3c-native"),
        (
            "M228-B003-DOC-09",
            "tmp/reports/m228/M228-B003/ownership_aware_lowering_behavior_core_feature_implementation_contract_summary.json",
        ),
        ("M228-B003-DOC-10", "Code/spec anchors and milestone optimization improvements are mandatory scope"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B003-FORB-01", "scaffold.modular_split_ready = true;"),
        ("M228-B003-FORB-02", "scaffold.deterministic_replay_surface = true;"),
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
            "tmp/reports/m228/M228-B003/"
            "ownership_aware_lowering_behavior_core_feature_implementation_contract_summary.json"
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
