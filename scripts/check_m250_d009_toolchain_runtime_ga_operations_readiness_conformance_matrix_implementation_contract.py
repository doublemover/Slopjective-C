#!/usr/bin/env python3
"""Fail-closed validator for M250-D009 toolchain/runtime GA conformance matrix."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-toolchain-runtime-ga-operations-readiness-conformance-matrix-implementation-contract-d009-v1"

ARTIFACTS: dict[str, Path] = {
    "parse_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "d008_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_recovery_determinism_hardening_d008_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_conformance_matrix_implementation_d009_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_d009_toolchain_runtime_ga_operations_readiness_conformance_matrix_implementation_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parse_surface_header": (
        ("M250-D009-SUR-01", "IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixConsistent("),
        ("M250-D009-SUR-02", "IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady("),
        ("M250-D009-SUR-03", "BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey("),
        (
            "M250-D009-SUR-04",
            "toolchain_runtime_ga_operations_conformance_matrix_consistent",
        ),
        ("M250-D009-SUR-05", "toolchain_runtime_ga_operations_conformance_matrix_ready"),
        ("M250-D009-SUR-06", "toolchain_runtime_ga_operations_conformance_matrix_key"),
        (
            "M250-D009-SUR-07",
            "toolchain/runtime GA operations conformance matrix is inconsistent",
        ),
        (
            "M250-D009-SUR-08",
            "toolchain/runtime GA operations conformance matrix is not ready",
        ),
    ),
    "architecture_doc": (
        ("M250-D009-ARC-01", "M250 lane-D D009 conformance matrix"),
        ("M250-D009-ARC-02", "toolchain/runtime conformance-matrix"),
    ),
    "d008_contract_doc": (
        (
            "M250-D009-DEP-01",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-recovery-determinism-hardening/m250-d008-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M250-D009-DOC-01",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-conformance-matrix-implementation/m250-d009-v1`",
        ),
        ("M250-D009-DOC-02", "IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixConsistent"),
        ("M250-D009-DOC-03", "IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady"),
        (
            "M250-D009-DOC-04",
            "scripts/check_m250_d009_toolchain_runtime_ga_operations_readiness_conformance_matrix_implementation_contract.py",
        ),
        (
            "M250-D009-DOC-05",
            "tests/tooling/test_check_m250_d009_toolchain_runtime_ga_operations_readiness_conformance_matrix_implementation_contract.py",
        ),
        ("M250-D009-DOC-06", "npm run check:objc3c:m250-d009-lane-d-readiness"),
    ),
    "packet_doc": (
        ("M250-D009-PKT-01", "Packet: `M250-D009`"),
        ("M250-D009-PKT-02", "Dependencies: `M250-D008`"),
        (
            "M250-D009-PKT-03",
            "scripts/check_m250_d009_toolchain_runtime_ga_operations_readiness_conformance_matrix_implementation_contract.py",
        ),
    ),
    "package_json": (
        (
            "M250-D009-CFG-01",
            '"check:objc3c:m250-d009-toolchain-runtime-ga-operations-readiness-conformance-matrix-implementation-contract"',
        ),
        (
            "M250-D009-CFG-02",
            '"test:tooling:m250-d009-toolchain-runtime-ga-operations-readiness-conformance-matrix-implementation-contract"',
        ),
        ("M250-D009-CFG-03", '"check:objc3c:m250-d009-lane-d-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parse_surface_header": (
        (
            "M250-D009-FORB-01",
            "toolchain_runtime_ga_operations_conformance_matrix_consistent = true;",
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
            "tmp/reports/m250/M250-D009/toolchain_runtime_ga_operations_readiness_conformance_matrix_implementation_contract_summary.json"
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
