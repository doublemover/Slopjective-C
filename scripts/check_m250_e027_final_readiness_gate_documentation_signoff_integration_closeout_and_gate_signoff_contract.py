#!/usr/bin/env python3
"""Fail-closed validator for M250-E027 final readiness integration closeout sign-off."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-final-readiness-gate-documentation-signoff-integration-closeout-and-gate-signoff-contract-e027-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_integration_closeout_and_gate_signoff_e027_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e027_final_readiness_gate_documentation_signoff_integration_closeout_and_gate_signoff_packet.md",
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
    "e026_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_advanced_performance_workpack_shard2_e026_expectations.md",
    "a010_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_integration_closeout_signoff_a010_expectations.md",
    "b012_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_integration_closeout_signoff_b012_expectations.md",
    "c013_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_integration_closeout_signoff_c013_expectations.md",
    "d022_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_integration_closeout_signoff_d022_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M250-E027-DOC-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-integration-closeout-and-gate-signoff/m250-e027-v1`",
        ),
        ("M250-E027-DOC-02", "`M250-E026`"),
        ("M250-E027-DOC-03", "`M250-A010`"),
        ("M250-E027-DOC-04", "`M250-B012`"),
        ("M250-E027-DOC-05", "`M250-C013`"),
        ("M250-E027-DOC-06", "`M250-D022`"),
        (
            "M250-E027-DOC-07",
            "scripts/check_m250_e027_final_readiness_gate_documentation_signoff_integration_closeout_and_gate_signoff_contract.py",
        ),
        (
            "M250-E027-DOC-08",
            "tests/tooling/test_check_m250_e027_final_readiness_gate_documentation_signoff_integration_closeout_and_gate_signoff_contract.py",
        ),
        ("M250-E027-DOC-09", "npm run check:objc3c:m250-e027-lane-e-readiness"),
    ),
    "packet_doc": (
        ("M250-E027-PKT-01", "Packet: `M250-E027`"),
        (
            "M250-E027-PKT-02",
            "Dependencies: `M250-E026`, `M250-A010`, `M250-B012`, `M250-C013`, `M250-D022`",
        ),
        (
            "M250-E027-PKT-03",
            "scripts/check_m250_e027_final_readiness_gate_documentation_signoff_integration_closeout_and_gate_signoff_contract.py",
        ),
    ),
    "core_surface_header": (
        (
            "M250-E027-SUR-01",
            "BuildObjc3FinalReadinessGateIntegrationCloseoutSignoffKey(",
        ),
        ("M250-E027-SUR-02", "const bool lane_integration_closeout_signoff_consistent ="),
        ("M250-E027-SUR-03", "const bool integration_closeout_signoff_consistent ="),
        ("M250-E027-SUR-04", "const bool integration_closeout_signoff_ready ="),
        ("M250-E027-SUR-05", "surface.integration_closeout_signoff_consistent ="),
        ("M250-E027-SUR-06", "surface.integration_closeout_signoff_ready ="),
        ("M250-E027-SUR-07", "surface.integration_closeout_signoff_key ="),
        ("M250-E027-SUR-08", "final-readiness-gate-integration-closeout-signoff:v1:"),
        ("M250-E027-SUR-09", ";integration_closeout_signoff_consistent="),
        ("M250-E027-SUR-10", ";integration_closeout_signoff_ready="),
        ("M250-E027-SUR-11", ";integration_closeout_signoff_key_ready="),
        ("M250-E027-SUR-12", "surface.integration_closeout_signoff_ready &&"),
        ("M250-E027-SUR-13", "final readiness gate integration closeout and gate sign-off is inconsistent"),
        (
            "M250-E027-SUR-14",
            "final readiness gate integration closeout and gate sign-off consistency is not satisfied",
        ),
        ("M250-E027-SUR-15", "final readiness gate integration closeout and gate sign-off is not ready"),
        ("M250-E027-SUR-16", "final readiness gate integration closeout and gate sign-off key is not ready"),
    ),
    "frontend_types": (
        ("M250-E027-TYP-01", "bool integration_closeout_signoff_consistent = false;"),
        ("M250-E027-TYP-02", "bool integration_closeout_signoff_ready = false;"),
        ("M250-E027-TYP-03", "std::string integration_closeout_signoff_key;"),
    ),
    "architecture_doc": (
        (
            "M250-E027-ARC-01",
            "M250 lane-E E027 integration closeout sign-off anchors explicit final",
        ),
        ("M250-E027-ARC-02", "integration_closeout_signoff_*"),
    ),
    "package_json": (
        (
            "M250-E027-CFG-01",
            '"check:objc3c:m250-e027-final-readiness-gate-documentation-signoff-integration-closeout-and-gate-signoff-contract"',
        ),
        (
            "M250-E027-CFG-02",
            '"test:tooling:m250-e027-final-readiness-gate-documentation-signoff-integration-closeout-and-gate-signoff-contract"',
        ),
        ("M250-E027-CFG-03", '"check:objc3c:m250-e027-lane-e-readiness"'),
        ("M250-E027-CFG-04", "check:objc3c:m250-e026-lane-e-readiness"),
        ("M250-E027-CFG-05", "check:objc3c:m250-a010-lane-a-readiness"),
        ("M250-E027-CFG-06", "check:objc3c:m250-b012-lane-b-readiness"),
        ("M250-E027-CFG-07", "check:objc3c:m250-c013-lane-c-readiness"),
        ("M250-E027-CFG-08", "check:objc3c:m250-d022-lane-d-readiness"),
    ),
    "e026_contract": (
        (
            "M250-E027-DEP-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-advanced-performance-workpack-shard2/m250-e026-v1`",
        ),
    ),
    "a010_contract": (
        (
            "M250-E027-DEP-02",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-integration-closeout-signoff/m250-a010-v1`",
        ),
    ),
    "b012_contract": (
        (
            "M250-E027-DEP-03",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-integration-closeout-signoff/m250-b012-v1`",
        ),
    ),
    "c013_contract": (
        (
            "M250-E027-DEP-04",
            "Contract ID: `objc3c-lowering-runtime-stability-integration-closeout-signoff/m250-c013-v1`",
        ),
    ),
    "d022_contract": (
        (
            "M250-E027-DEP-05",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-integration-closeout-signoff/m250-d022-v1`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-E027-FORB-01", "surface.integration_closeout_signoff_ready = true;"),
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
            "tmp/reports/m250/M250-E027/final_readiness_gate_documentation_signoff_integration_closeout_and_gate_signoff_contract_summary.json"
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








