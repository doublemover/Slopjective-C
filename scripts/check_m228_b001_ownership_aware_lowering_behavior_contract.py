#!/usr/bin/env python3
"""Fail-closed validator for M228-B001 ownership-aware lowering behavior freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b001-ownership-aware-lowering-behavior-freeze-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "lowering_contract_header": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h",
    "lowering_contract_source": ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    "sema_contract_header": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
    "contract_doc": ROOT / "docs" / "contracts" / "m228_ownership_aware_lowering_behavior_contract_freeze_b001_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "lowering_contract_header": (
        ("M228-B001-H-01", "kObjc3OwnershipQualifierLoweringLaneContract"),
        ("M228-B001-H-02", "kObjc3RetainReleaseOperationLoweringLaneContract"),
        ("M228-B001-H-03", "kObjc3AutoreleasePoolScopeLoweringLaneContract"),
        ("M228-B001-H-04", "kObjc3ArcDiagnosticsFixitLoweringLaneContract"),
        ("M228-B001-H-05", "struct Objc3OwnershipQualifierLoweringContract {"),
        ("M228-B001-H-06", "struct Objc3RetainReleaseOperationLoweringContract {"),
        ("M228-B001-H-07", "struct Objc3AutoreleasePoolScopeLoweringContract {"),
        ("M228-B001-H-08", "struct Objc3ArcDiagnosticsFixitLoweringContract {"),
    ),
    "lowering_contract_source": (
        ("M228-B001-CPP-01", "std::string Objc3OwnershipQualifierLoweringReplayKey("),
        ("M228-B001-CPP-02", "std::string Objc3RetainReleaseOperationLoweringReplayKey("),
        ("M228-B001-CPP-03", "std::string Objc3AutoreleasePoolScopeLoweringReplayKey("),
        ("M228-B001-CPP-04", "std::string Objc3ArcDiagnosticsFixitLoweringReplayKey("),
        ("M228-B001-CPP-05", '";lane_contract=" + kObjc3OwnershipQualifierLoweringLaneContract;'),
        ("M228-B001-CPP-06", '";lane_contract=" + kObjc3RetainReleaseOperationLoweringLaneContract;'),
        ("M228-B001-CPP-07", '";lane_contract=" + kObjc3AutoreleasePoolScopeLoweringLaneContract;'),
        ("M228-B001-CPP-08", '";lane_contract=" + kObjc3ArcDiagnosticsFixitLoweringLaneContract;'),
    ),
    "sema_contract_header": (
        ("M228-B001-SEM-01", "struct Objc3RetainReleaseOperationSummary {"),
        ("M228-B001-SEM-02", "struct Objc3ArcDiagnosticsFixitSummary {"),
        ("M228-B001-SEM-03", "struct Objc3AutoreleasePoolScopeSummary {"),
        ("M228-B001-SEM-04", "std::size_t ownership_qualifier_sites = 0;"),
    ),
    "artifacts_source": (
        ("M228-B001-ART-01", "BuildOwnershipQualifierLoweringContract("),
        ("M228-B001-ART-02", "BuildRetainReleaseOperationLoweringContract("),
        ("M228-B001-ART-03", "BuildAutoreleasePoolScopeLoweringContract("),
        ("M228-B001-ART-04", "BuildArcDiagnosticsFixitLoweringContract("),
        ("M228-B001-ART-05", "Objc3OwnershipQualifierLoweringReplayKey("),
        ("M228-B001-ART-06", "Objc3RetainReleaseOperationLoweringReplayKey("),
        ("M228-B001-ART-07", "Objc3AutoreleasePoolScopeLoweringReplayKey("),
        ("M228-B001-ART-08", "Objc3ArcDiagnosticsFixitLoweringReplayKey("),
        ("M228-B001-ART-09", '<< "\\",\\"lane_contract\\":\\"" << kObjc3OwnershipQualifierLoweringLaneContract'),
        ("M228-B001-ART-10", '<< "\\",\\"lane_contract\\":\\"" << kObjc3RetainReleaseOperationLoweringLaneContract'),
        ("M228-B001-ART-11", '<< "\\",\\"lane_contract\\":\\"" << kObjc3AutoreleasePoolScopeLoweringLaneContract'),
        ("M228-B001-ART-12", '<< "\\",\\"lane_contract\\":\\"" << kObjc3ArcDiagnosticsFixitLoweringLaneContract'),
    ),
    "architecture_doc": (
        ("M228-B001-ARCH-01", "M228 lane-B B001 ownership-aware lowering behavior freeze anchors"),
        ("M228-B001-ARCH-02", "ownership qualifier, retain/release, autoreleasepool, and ARC diagnostics"),
    ),
    "lowering_spec": (
        ("M228-B001-SPC-01", "ownership-aware lowering contracts (ownership qualifiers, retain/release,"),
    ),
    "metadata_spec": (
        ("M228-B001-META-01", "deterministic ownership-aware lowering replay keys for ownership qualifier,"),
    ),
    "package_json": (
        ("M228-B001-CFG-01", '"check:objc3c:m228-b001-ownership-aware-lowering-behavior-contract"'),
        ("M228-B001-CFG-02", '"test:tooling:m228-b001-ownership-aware-lowering-behavior-contract"'),
        ("M228-B001-CFG-03", '"check:objc3c:m228-b001-lane-b-readiness"'),
    ),
    "contract_doc": (
        ("M228-B001-DOC-01", "Contract ID: `objc3c-ownership-aware-lowering-behavior-freeze/m228-b001-v1`"),
        ("M228-B001-DOC-02", "kObjc3OwnershipQualifierLoweringLaneContract"),
        ("M228-B001-DOC-03", "BuildArcDiagnosticsFixitLoweringContract"),
        ("M228-B001-DOC-04", "tmp/reports/m228/M228-B001/ownership_aware_lowering_behavior_contract_summary.json"),
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
        default=Path("tmp/reports/m228/M228-B001/ownership_aware_lowering_behavior_contract_summary.json"),
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

    summary = {
        "mode": MODE,
        "ok": not findings,
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

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
