#!/usr/bin/env python3
"""Fail-closed validator for M250-E008 final readiness recovery/determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-final-readiness-gate-documentation-signoff-recovery-determinism-hardening-contract-e008-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_recovery_determinism_hardening_e008_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e008_final_readiness_gate_documentation_signoff_recovery_determinism_hardening_packet.md",
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
    "e007_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_diagnostics_hardening_e007_expectations.md",
    "a003_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_core_feature_implementation_a003_expectations.md",
    "b004_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_core_feature_expansion_b004_expectations.md",
    "c004_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_core_feature_expansion_c004_expectations.md",
    "d007_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_diagnostics_hardening_d007_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M250-E008-DOC-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-recovery-determinism-hardening/m250-e008-v1`",
        ),
        ("M250-E008-DOC-02", "`M250-E007`"),
        ("M250-E008-DOC-03", "`M250-A003`"),
        ("M250-E008-DOC-04", "`M250-B004`"),
        ("M250-E008-DOC-05", "`M250-C004`"),
        ("M250-E008-DOC-06", "`M250-D007`"),
        (
            "M250-E008-DOC-07",
            "scripts/check_m250_e008_final_readiness_gate_documentation_signoff_recovery_determinism_hardening_contract.py",
        ),
        (
            "M250-E008-DOC-08",
            "tests/tooling/test_check_m250_e008_final_readiness_gate_documentation_signoff_recovery_determinism_hardening_contract.py",
        ),
        ("M250-E008-DOC-09", "npm run check:objc3c:m250-e008-lane-e-readiness"),
    ),
    "packet_doc": (
        ("M250-E008-PKT-01", "Packet: `M250-E008`"),
        (
            "M250-E008-PKT-02",
            "Dependencies: `M250-E007`, `M250-A003`, `M250-B004`, `M250-C004`, `M250-D007`",
        ),
        (
            "M250-E008-PKT-03",
            "scripts/check_m250_e008_final_readiness_gate_documentation_signoff_recovery_determinism_hardening_contract.py",
        ),
    ),
    "core_surface_header": (
        ("M250-E008-SUR-01", "BuildObjc3FinalReadinessGateRecoveryDeterminismKey("),
        ("M250-E008-SUR-02", "const bool lane_recovery_determinism_consistent ="),
        ("M250-E008-SUR-03", "const bool recovery_determinism_consistent ="),
        ("M250-E008-SUR-04", "const bool recovery_determinism_ready ="),
        ("M250-E008-SUR-05", "surface.recovery_determinism_consistent ="),
        ("M250-E008-SUR-06", "surface.recovery_determinism_ready ="),
        ("M250-E008-SUR-07", "surface.recovery_determinism_key ="),
        ("M250-E008-SUR-08", "final-readiness-gate-recovery-determinism:v1:"),
        ("M250-E008-SUR-09", ";recovery_determinism_consistent="),
        ("M250-E008-SUR-10", ";recovery_determinism_ready="),
        ("M250-E008-SUR-11", ";recovery_determinism_key_ready="),
        ("M250-E008-SUR-12", "surface.recovery_determinism_ready &&"),
        (
            "M250-E008-SUR-13",
            "final readiness gate recovery and determinism hardening is inconsistent",
        ),
        (
            "M250-E008-SUR-14",
            "final readiness gate recovery and determinism consistency is not satisfied",
        ),
        (
            "M250-E008-SUR-15",
            "final readiness gate recovery and determinism hardening is not ready",
        ),
        ("M250-E008-SUR-16", "final readiness gate recovery and determinism key is not ready"),
    ),
    "frontend_types": (
        ("M250-E008-TYP-01", "bool recovery_determinism_consistent = false;"),
        ("M250-E008-TYP-02", "bool recovery_determinism_ready = false;"),
        ("M250-E008-TYP-03", "std::string recovery_determinism_key;"),
    ),
    "architecture_doc": (
        (
            "M250-E008-ARC-01",
            "M250 lane-E E008 recovery/determinism hardening anchors explicit final",
        ),
        ("M250-E008-ARC-02", "recovery_determinism_*"),
    ),
    "package_json": (
        (
            "M250-E008-CFG-01",
            '"check:objc3c:m250-e008-final-readiness-gate-documentation-signoff-recovery-determinism-hardening-contract"',
        ),
        (
            "M250-E008-CFG-02",
            '"test:tooling:m250-e008-final-readiness-gate-documentation-signoff-recovery-determinism-hardening-contract"',
        ),
        ("M250-E008-CFG-03", '"check:objc3c:m250-e008-lane-e-readiness"'),
        ("M250-E008-CFG-04", "check:objc3c:m250-e007-lane-e-readiness"),
        ("M250-E008-CFG-05", "check:objc3c:m250-a003-lane-a-readiness"),
        ("M250-E008-CFG-06", "check:objc3c:m250-b004-lane-b-readiness"),
        ("M250-E008-CFG-07", "check:objc3c:m250-c004-lane-c-readiness"),
        ("M250-E008-CFG-08", "check:objc3c:m250-d007-lane-d-readiness"),
    ),
    "e007_contract": (
        (
            "M250-E008-DEP-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-diagnostics-hardening/m250-e007-v1`",
        ),
    ),
    "a003_contract": (
        (
            "M250-E008-DEP-02",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-core-feature-implementation/m250-a003-v1`",
        ),
    ),
    "b004_contract": (
        (
            "M250-E008-DEP-03",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-core-feature-expansion/m250-b004-v1`",
        ),
    ),
    "c004_contract": (
        (
            "M250-E008-DEP-04",
            "Contract ID: `objc3c-lowering-runtime-stability-core-feature-expansion/m250-c004-v1`",
        ),
    ),
    "d007_contract": (
        (
            "M250-E008-DEP-05",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-diagnostics-hardening/m250-d007-v1`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-E008-FORB-01", "surface.recovery_determinism_ready = true;"),
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
            "tmp/reports/m250/M250-E008/final_readiness_gate_documentation_signoff_recovery_determinism_hardening_contract_summary.json"
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
