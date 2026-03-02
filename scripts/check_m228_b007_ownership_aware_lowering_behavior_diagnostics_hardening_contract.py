#!/usr/bin/env python3
"""Fail-closed validator for M228-B007 ownership-aware lowering diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b007-ownership-aware-lowering-behavior-diagnostics-hardening-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_ownership_aware_lowering_behavior_scaffold.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "b006_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_b006_expectations.md",
    "b006_checker": ROOT
    / "scripts"
    / "check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py",
    "b006_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py",
    "b006_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_diagnostics_hardening_b007_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M228-B007-SCA-01", "bool diagnostics_hardening_consistent = false;"),
        ("M228-B007-SCA-02", "bool diagnostics_hardening_ready = false;"),
        ("M228-B007-SCA-03", "std::string diagnostics_hardening_key;"),
        (
            "M228-B007-SCA-04",
            "BuildObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningKey(",
        ),
        ("M228-B007-SCA-05", "scaffold.diagnostics_hardening_consistent ="),
        ("M228-B007-SCA-06", "scaffold.diagnostics_hardening_key ="),
        ("M228-B007-SCA-07", "scaffold.diagnostics_hardening_ready ="),
        (
            "M228-B007-SCA-08",
            "scaffold.diagnostics_hardening_ready &&",
        ),
        (
            "M228-B007-SCA-09",
            "ownership-aware lowering diagnostics hardening is inconsistent",
        ),
        (
            "M228-B007-SCA-10",
            "ownership-aware lowering diagnostics hardening key is empty",
        ),
        (
            "M228-B007-SCA-11",
            "ownership-aware lowering diagnostics hardening is not ready",
        ),
        (
            "M228-B007-SCA-12",
            "inline bool IsObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningReady(",
        ),
        (
            "M228-B007-SCA-13",
            "scaffold.diagnostics_hardening_ready &&",
        ),
        (
            "M228-B007-SCA-14",
            "!scaffold.diagnostics_hardening_key.empty())",
        ),
    ),
    "artifacts_source": (
        (
            "M228-B007-ART-01",
            "IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(",
        ),
        ("M228-B007-ART-02", '"O3L312"'),
    ),
    "b006_contract_doc": (
        (
            "M228-B007-DEP-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness/m228-b006-v1`",
        ),
    ),
    "b006_checker": (
        (
            "M228-B007-DEP-02",
            'MODE = "m228-b006-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness-contract-v1"',
        ),
    ),
    "b006_tooling_test": (
        ("M228-B007-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b006_packet_doc": (
        ("M228-B007-DEP-04", "Packet: `M228-B006`"),
    ),
    "contract_doc": (
        (
            "M228-B007-DOC-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-diagnostics-hardening/m228-b007-v1`",
        ),
        ("M228-B007-DOC-02", "Dependencies: `M228-B006`"),
        (
            "M228-B007-DOC-03",
            "BuildObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningKey",
        ),
        (
            "M228-B007-DOC-04",
            "IsObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningReady",
        ),
        ("M228-B007-DOC-05", "Shared-file deltas required for full lane-B readiness"),
    ),
    "planning_packet": (
        ("M228-B007-PKT-01", "Packet: `M228-B007`"),
        ("M228-B007-PKT-02", "Dependencies: `M228-B006`"),
        (
            "M228-B007-PKT-03",
            "python scripts/check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py",
        ),
    ),
    "package_json": (
        (
            "M228-B007-PKG-01",
            '"check:objc3c:m228-b007-ownership-aware-lowering-behavior-diagnostics-hardening-contract"',
        ),
        (
            "M228-B007-PKG-02",
            '"test:tooling:m228-b007-ownership-aware-lowering-behavior-diagnostics-hardening-contract"',
        ),
        ("M228-B007-PKG-03", '"check:objc3c:m228-b007-lane-b-readiness"'),
    ),
    "architecture_doc": (
        ("M228-B007-ARC-01", "M228 lane-B B007 diagnostics hardening"),
    ),
    "lowering_spec": (
        ("M228-B007-SPEC-01", "ownership-aware lowering diagnostics hardening"),
    ),
    "metadata_spec": (
        ("M228-B007-META-01", "lane-B B007"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        (
            "M228-B007-FORB-01",
            "scaffold.diagnostics_hardening_ready = true;",
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
            "tmp/reports/m228/M228-B007/ownership_aware_lowering_behavior_diagnostics_hardening_contract_summary.json"
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
            findings.append(Finding(artifact, f"M228-B007-MISS-{artifact.upper()}", str(exc)))
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
