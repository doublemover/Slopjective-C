#!/usr/bin/env python3
"""Fail-closed validator for M228-B008 ownership-aware lowering recovery/determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b008-ownership-aware-lowering-behavior-recovery-determinism-hardening-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_ownership_aware_lowering_behavior_scaffold.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "b007_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_diagnostics_hardening_b007_expectations.md",
    "b007_checker": ROOT
    / "scripts"
    / "check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py",
    "b007_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py",
    "b007_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_recovery_determinism_hardening_b008_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B008-SCA-01", "bool recovery_determinism_consistent = false;"),
        ("M228-B008-SCA-02", "bool recovery_determinism_ready = false;"),
        ("M228-B008-SCA-03", "std::string recovery_determinism_key;"),
        (
            "M228-B008-SCA-04",
            "BuildObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismKey(",
        ),
        ("M228-B008-SCA-05", "scaffold.recovery_determinism_consistent ="),
        ("M228-B008-SCA-06", "scaffold.recovery_determinism_key ="),
        ("M228-B008-SCA-07", "scaffold.recovery_determinism_ready ="),
        ("M228-B008-SCA-08", "parse_recovery_determinism_hardening_consistent"),
        ("M228-B008-SCA-09", "parse_recovery_determinism_hardening_key"),
        (
            "M228-B008-SCA-10",
            "ownership-aware lowering recovery determinism hardening is inconsistent",
        ),
        (
            "M228-B008-SCA-11",
            "ownership-aware lowering recovery determinism hardening key is empty",
        ),
        (
            "M228-B008-SCA-12",
            "ownership-aware lowering recovery determinism hardening is not ready",
        ),
        (
            "M228-B008-SCA-13",
            "inline bool IsObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismReady(",
        ),
        (
            "M228-B008-SCA-14",
            "scaffold.recovery_determinism_ready &&",
        ),
        (
            "M228-B008-SCA-15",
            "!scaffold.recovery_determinism_key.empty())",
        ),
    ),
    "artifacts_source": (
        (
            "M228-B008-ART-01",
            "IsObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismReady(",
        ),
        ("M228-B008-ART-02", '"O3L318"'),
        (
            "M228-B008-ART-03",
            "ownership-aware lowering recovery determinism check failed",
        ),
        ("M228-B008-ART-04", ".parse_recovery_determinism_hardening_consistent"),
        ("M228-B008-ART-05", ".parse_recovery_determinism_hardening_key"),
    ),
    "b007_contract_doc": (
        (
            "M228-B008-DEP-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-diagnostics-hardening/m228-b007-v1`",
        ),
    ),
    "b007_checker": (
        (
            "M228-B008-DEP-02",
            'MODE = "m228-b007-ownership-aware-lowering-behavior-diagnostics-hardening-contract-v1"',
        ),
    ),
    "b007_tooling_test": (
        ("M228-B008-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b007_packet_doc": (
        ("M228-B008-DEP-04", "Packet: `M228-B007`"),
    ),
    "contract_doc": (
        (
            "M228-B008-DOC-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-recovery-determinism-hardening/m228-b008-v1`",
        ),
        ("M228-B008-DOC-02", "Dependencies: `M228-B007`"),
        (
            "M228-B008-DOC-03",
            "BuildObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismKey",
        ),
        (
            "M228-B008-DOC-04",
            "IsObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismReady",
        ),
        ("M228-B008-DOC-05", "O3L318"),
        ("M228-B008-DOC-06", "Shared-file deltas required for full lane-B readiness"),
    ),
    "planning_packet": (
        ("M228-B008-PKT-01", "Packet: `M228-B008`"),
        ("M228-B008-PKT-02", "Dependencies: `M228-B007`"),
        (
            "M228-B008-PKT-03",
            "python scripts/check_m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_contract.py",
        ),
    ),
    "package_json": (
        (
            "M228-B008-PKG-01",
            '"check:objc3c:m228-b008-ownership-aware-lowering-behavior-recovery-determinism-hardening-contract"',
        ),
        (
            "M228-B008-PKG-02",
            '"test:tooling:m228-b008-ownership-aware-lowering-behavior-recovery-determinism-hardening-contract"',
        ),
        ("M228-B008-PKG-03", '"check:objc3c:m228-b008-lane-b-readiness"'),
    ),
    "architecture_doc": (
        ("M228-B008-ARC-01", "M228 lane-B B008 recovery and determinism hardening"),
    ),
    "lowering_spec": (
        ("M228-B008-SPEC-01", "ownership-aware lowering recovery and determinism hardening"),
    ),
    "metadata_spec": (
        ("M228-B008-META-01", "lane-B B008 recovery and determinism"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        (
            "M228-B008-FORB-01",
            "scaffold.recovery_determinism_ready = true;",
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
            "tmp/reports/m228/M228-B008/ownership_aware_lowering_behavior_recovery_determinism_hardening_contract_summary.json"
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
            findings.append(Finding(artifact, f"M228-B008-MISS-{artifact.upper()}", str(exc)))
            continue

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                checks_passed += 1
            else:
                findings.append(Finding(artifact, check_id, f"missing snippet: {snippet}"))

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
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
