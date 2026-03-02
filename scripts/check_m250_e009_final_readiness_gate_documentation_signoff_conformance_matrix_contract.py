#!/usr/bin/env python3
"""Fail-closed validator for M250-E009 final readiness conformance matrix."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-final-readiness-gate-documentation-signoff-conformance-matrix-contract-e009-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_conformance_matrix_e009_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e009_final_readiness_gate_documentation_signoff_conformance_matrix_packet.md",
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
    "e008_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_recovery_determinism_hardening_e008_expectations.md",
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
            "M250-E009-DOC-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-conformance-matrix/m250-e009-v1`",
        ),
        ("M250-E009-DOC-02", "`M250-E008`"),
        ("M250-E009-DOC-03", "`M250-A003`"),
        ("M250-E009-DOC-04", "`M250-B004`"),
        ("M250-E009-DOC-05", "`M250-C004`"),
        ("M250-E009-DOC-06", "`M250-D007`"),
        (
            "M250-E009-DOC-07",
            "scripts/check_m250_e009_final_readiness_gate_documentation_signoff_conformance_matrix_contract.py",
        ),
        (
            "M250-E009-DOC-08",
            "tests/tooling/test_check_m250_e009_final_readiness_gate_documentation_signoff_conformance_matrix_contract.py",
        ),
        ("M250-E009-DOC-09", "npm run check:objc3c:m250-e009-lane-e-readiness"),
    ),
    "packet_doc": (
        ("M250-E009-PKT-01", "Packet: `M250-E009`"),
        (
            "M250-E009-PKT-02",
            "Dependencies: `M250-E008`, `M250-A003`, `M250-B004`, `M250-C004`, `M250-D007`",
        ),
        (
            "M250-E009-PKT-03",
            "scripts/check_m250_e009_final_readiness_gate_documentation_signoff_conformance_matrix_contract.py",
        ),
    ),
    "core_surface_header": (
        ("M250-E009-SUR-01", "BuildObjc3FinalReadinessGateConformanceMatrixKey("),
        ("M250-E009-SUR-02", "const bool lane_conformance_matrix_consistent ="),
        ("M250-E009-SUR-03", "const bool conformance_matrix_consistent ="),
        ("M250-E009-SUR-04", "const bool conformance_matrix_ready ="),
        ("M250-E009-SUR-05", "surface.conformance_matrix_consistent ="),
        ("M250-E009-SUR-06", "surface.conformance_matrix_ready ="),
        ("M250-E009-SUR-07", "surface.conformance_matrix_key ="),
        ("M250-E009-SUR-08", "final-readiness-gate-conformance-matrix:v1:"),
        ("M250-E009-SUR-09", ";conformance_matrix_consistent="),
        ("M250-E009-SUR-10", ";conformance_matrix_ready="),
        ("M250-E009-SUR-11", ";conformance_matrix_key_ready="),
        ("M250-E009-SUR-12", "surface.conformance_matrix_ready &&"),
        ("M250-E009-SUR-13", "final readiness gate conformance matrix is inconsistent"),
        ("M250-E009-SUR-14", "final readiness gate conformance matrix consistency is not satisfied"),
        ("M250-E009-SUR-15", "final readiness gate conformance matrix is not ready"),
        ("M250-E009-SUR-16", "final readiness gate conformance matrix key is not ready"),
    ),
    "frontend_types": (
        ("M250-E009-TYP-01", "bool conformance_matrix_consistent = false;"),
        ("M250-E009-TYP-02", "bool conformance_matrix_ready = false;"),
        ("M250-E009-TYP-03", "std::string conformance_matrix_key;"),
    ),
    "architecture_doc": (
        (
            "M250-E009-ARC-01",
            "M250 lane-E E009 conformance-matrix implementation anchors explicit final",
        ),
        ("M250-E009-ARC-02", "conformance_matrix_*"),
    ),
    "package_json": (
        (
            "M250-E009-CFG-01",
            '"check:objc3c:m250-e009-final-readiness-gate-documentation-signoff-conformance-matrix-contract"',
        ),
        (
            "M250-E009-CFG-02",
            '"test:tooling:m250-e009-final-readiness-gate-documentation-signoff-conformance-matrix-contract"',
        ),
        ("M250-E009-CFG-03", '"check:objc3c:m250-e009-lane-e-readiness"'),
        ("M250-E009-CFG-04", "check:objc3c:m250-e008-lane-e-readiness"),
        ("M250-E009-CFG-05", "check:objc3c:m250-a003-lane-a-readiness"),
        ("M250-E009-CFG-06", "check:objc3c:m250-b004-lane-b-readiness"),
        ("M250-E009-CFG-07", "check:objc3c:m250-c004-lane-c-readiness"),
        ("M250-E009-CFG-08", "check:objc3c:m250-d007-lane-d-readiness"),
    ),
    "e008_contract": (
        (
            "M250-E009-DEP-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-recovery-determinism-hardening/m250-e008-v1`",
        ),
    ),
    "a003_contract": (
        (
            "M250-E009-DEP-02",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-core-feature-implementation/m250-a003-v1`",
        ),
    ),
    "b004_contract": (
        (
            "M250-E009-DEP-03",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-core-feature-expansion/m250-b004-v1`",
        ),
    ),
    "c004_contract": (
        (
            "M250-E009-DEP-04",
            "Contract ID: `objc3c-lowering-runtime-stability-core-feature-expansion/m250-c004-v1`",
        ),
    ),
    "d007_contract": (
        (
            "M250-E009-DEP-05",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-diagnostics-hardening/m250-d007-v1`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-E009-FORB-01", "surface.conformance_matrix_ready = true;"),
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
            "tmp/reports/m250/M250-E009/final_readiness_gate_documentation_signoff_conformance_matrix_contract_summary.json"
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
