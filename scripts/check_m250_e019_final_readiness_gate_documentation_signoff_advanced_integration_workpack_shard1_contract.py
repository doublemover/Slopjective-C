#!/usr/bin/env python3
"""Fail-closed validator for M250-E019 final readiness advanced integration shard1."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-final-readiness-gate-documentation-signoff-advanced-integration-workpack-shard1-contract-e019-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_advanced_integration_workpack_shard1_e019_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e019_final_readiness_gate_documentation_signoff_advanced_integration_workpack_shard1_packet.md",
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
    "e018_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_advanced_conformance_workpack_shard1_e018_expectations.md",
    "a007_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_diagnostics_hardening_a007_expectations.md",
    "b008_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_recovery_determinism_hardening_b008_expectations.md",
    "c009_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_conformance_matrix_c009_expectations.md",
    "d015_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_advanced_core_workpack_shard1_d015_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M250-E019-DOC-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-advanced-integration-workpack-shard1/m250-e019-v1`",
        ),
        ("M250-E019-DOC-02", "`M250-E018`"),
        ("M250-E019-DOC-03", "`M250-A007`"),
        ("M250-E019-DOC-04", "`M250-B008`"),
        ("M250-E019-DOC-05", "`M250-C009`"),
        ("M250-E019-DOC-06", "`M250-D015`"),
        (
            "M250-E019-DOC-07",
            "scripts/check_m250_e019_final_readiness_gate_documentation_signoff_advanced_integration_workpack_shard1_contract.py",
        ),
        (
            "M250-E019-DOC-08",
            "tests/tooling/test_check_m250_e019_final_readiness_gate_documentation_signoff_advanced_integration_workpack_shard1_contract.py",
        ),
        ("M250-E019-DOC-09", "npm run check:objc3c:m250-e019-lane-e-readiness"),
    ),
    "packet_doc": (
        ("M250-E019-PKT-01", "Packet: `M250-E019`"),
        (
            "M250-E019-PKT-02",
            "Dependencies: `M250-E018`, `M250-A007`, `M250-B008`, `M250-C009`, `M250-D015`",
        ),
        (
            "M250-E019-PKT-03",
            "scripts/check_m250_e019_final_readiness_gate_documentation_signoff_advanced_integration_workpack_shard1_contract.py",
        ),
    ),
    "core_surface_header": (
        (
            "M250-E019-SUR-01",
            "BuildObjc3FinalReadinessGateAdvancedIntegrationShard1Key(",
        ),
        ("M250-E019-SUR-02", "const bool lane_advanced_integration_shard1_consistent ="),
        ("M250-E019-SUR-03", "const bool advanced_integration_shard1_consistent ="),
        ("M250-E019-SUR-04", "const bool advanced_integration_shard1_ready ="),
        ("M250-E019-SUR-05", "surface.advanced_integration_shard1_consistent ="),
        ("M250-E019-SUR-06", "surface.advanced_integration_shard1_ready ="),
        ("M250-E019-SUR-07", "surface.advanced_integration_shard1_key ="),
        ("M250-E019-SUR-08", "final-readiness-gate-advanced-integration-shard1:v1:"),
        ("M250-E019-SUR-09", ";advanced_integration_shard1_consistent="),
        ("M250-E019-SUR-10", ";advanced_integration_shard1_ready="),
        ("M250-E019-SUR-11", ";advanced_integration_shard1_key_ready="),
        ("M250-E019-SUR-12", "surface.advanced_integration_shard1_ready &&"),
        ("M250-E019-SUR-13", "final readiness gate advanced integration workpack shard1 is inconsistent"),
        (
            "M250-E019-SUR-14",
            "final readiness gate advanced integration workpack shard1 consistency is not satisfied",
        ),
        ("M250-E019-SUR-15", "final readiness gate advanced integration workpack shard1 is not ready"),
        ("M250-E019-SUR-16", "final readiness gate advanced integration workpack shard1 key is not ready"),
    ),
    "frontend_types": (
        ("M250-E019-TYP-01", "bool advanced_integration_shard1_consistent = false;"),
        ("M250-E019-TYP-02", "bool advanced_integration_shard1_ready = false;"),
        ("M250-E019-TYP-03", "std::string advanced_integration_shard1_key;"),
    ),
    "architecture_doc": (
        (
            "M250-E019-ARC-01",
            "M250 lane-E E019 advanced integration shard1 anchors explicit final",
        ),
        ("M250-E019-ARC-02", "advanced_integration_shard1_*"),
    ),
    "package_json": (
        (
            "M250-E019-CFG-01",
            '"check:objc3c:m250-e019-final-readiness-gate-documentation-signoff-advanced-integration-workpack-shard1-contract"',
        ),
        (
            "M250-E019-CFG-02",
            '"test:tooling:m250-e019-final-readiness-gate-documentation-signoff-advanced-integration-workpack-shard1-contract"',
        ),
        ("M250-E019-CFG-03", '"check:objc3c:m250-e019-lane-e-readiness"'),
        ("M250-E019-CFG-04", "check:objc3c:m250-e018-lane-e-readiness"),
        ("M250-E019-CFG-05", "check:objc3c:m250-a007-lane-a-readiness"),
        ("M250-E019-CFG-06", "check:objc3c:m250-b008-lane-b-readiness"),
        ("M250-E019-CFG-07", "check:objc3c:m250-c009-lane-c-readiness"),
        ("M250-E019-CFG-08", "check:objc3c:m250-d015-lane-d-readiness"),
    ),
    "e018_contract": (
        (
            "M250-E019-DEP-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-advanced-conformance-workpack-shard1/m250-e018-v1`",
        ),
    ),
    "a007_contract": (
        (
            "M250-E019-DEP-02",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-diagnostics-hardening/m250-a007-v1`",
        ),
    ),
    "b008_contract": (
        (
            "M250-E019-DEP-03",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-recovery-determinism-hardening/m250-b008-v1`",
        ),
    ),
    "c009_contract": (
        (
            "M250-E019-DEP-04",
            "Contract ID: `objc3c-lowering-runtime-stability-conformance-matrix/m250-c009-v1`",
        ),
    ),
    "d015_contract": (
        (
            "M250-E019-DEP-05",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-advanced-core-workpack-shard1/m250-d015-v1`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-E019-FORB-01", "surface.advanced_integration_shard1_ready = true;"),
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
            "tmp/reports/m250/M250-E019/final_readiness_gate_documentation_signoff_advanced_integration_workpack_shard1_contract_summary.json"
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


