#!/usr/bin/env python3
"""Fail-closed checker for M243-D009 CLI/reporting output conformance matrix implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-d009-cli-reporting-output-contract-integration-conformance-matrix-implementation-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_d009_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_packet.md",
    "d008_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_d008_expectations.md",
    "d008_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_packet.md",
    "d008_checker": ROOT
    / "scripts"
    / "check_m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_contract.py",
    "d008_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_contract.py",
    "conformance_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h",
    "recovery_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_cli_reporting_output_contract_recovery_determinism_hardening_surface.h",
    "runner_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "tools"
    / "objc3c_frontend_c_api_runner.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-D009-DOC-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-conformance-matrix-implementation/m243-d009-v1`",
        ),
        ("M243-D009-DOC-02", "Dependencies: `M243-D008`"),
        ("M243-D009-DOC-03", "conformance_matrix_key"),
        ("M243-D009-DOC-04", "conformance_matrix_consistent"),
        ("M243-D009-DOC-05", "conformance_matrix_ready"),
        ("M243-D009-DOC-06", "conformance_matrix_key_ready"),
        (
            "M243-D009-DOC-07",
            "scripts/check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py",
        ),
        (
            "M243-D009-DOC-08",
            "tests/tooling/test_check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py",
        ),
        (
            "M243-D009-DOC-09",
            "python scripts/check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py --emit-json",
        ),
        ("M243-D009-DOC-10", "npm run check:objc3c:m243-d009-lane-d-readiness"),
    ),
    "packet_doc": (
        ("M243-D009-PKT-01", "Packet: `M243-D009`"),
        ("M243-D009-PKT-02", "Dependencies: `M243-D008`"),
        (
            "M243-D009-PKT-03",
            "scripts/check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py",
        ),
        (
            "M243-D009-PKT-04",
            "tests/tooling/test_check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py",
        ),
        ("M243-D009-PKT-05", "check:objc3c:m243-d009-lane-d-readiness"),
        (
            "M243-D009-PKT-06",
            "python scripts/check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py --emit-json",
        ),
    ),
    "d008_expectations_doc": (
        (
            "M243-D009-DEP-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-recovery-determinism-hardening/m243-d008-v1`",
        ),
    ),
    "d008_packet_doc": (
        ("M243-D009-DEP-02", "Packet: `M243-D008`"),
        ("M243-D009-DEP-03", "Dependencies: `M243-D007`"),
    ),
    "d008_checker": (
        (
            "M243-D009-DEP-04",
            'MODE = "m243-d008-cli-reporting-output-contract-integration-recovery-determinism-hardening-contract-v1"',
        ),
    ),
    "d008_tooling_test": (
        (
            "M243-D009-DEP-05",
            "check_m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_contract",
        ),
    ),
    "conformance_surface_header": (
        (
            "M243-D009-SUR-01",
            "struct Objc3CliReportingOutputContractConformanceMatrixImplementationSurface {",
        ),
        ("M243-D009-SUR-02", "bool conformance_matrix_consistent = false;"),
        ("M243-D009-SUR-03", "bool conformance_matrix_ready = false;"),
        ("M243-D009-SUR-04", "bool conformance_matrix_key_ready = false;"),
        ("M243-D009-SUR-05", "std::string conformance_matrix_key;"),
        (
            "M243-D009-SUR-06",
            "BuildObjc3CliReportingOutputContractConformanceMatrixImplementationKey(",
        ),
        (
            "M243-D009-SUR-07",
            "BuildObjc3CliReportingOutputContractConformanceMatrixImplementationSurface(",
        ),
        ("M243-D009-SUR-08", "surface.conformance_matrix_consistent ="),
        ("M243-D009-SUR-09", "surface.conformance_matrix_ready ="),
        ("M243-D009-SUR-10", "surface.conformance_matrix_key ="),
        ("M243-D009-SUR-11", "surface.conformance_matrix_key_ready ="),
        ("M243-D009-SUR-12", "surface.core_feature_impl_ready ="),
        (
            "M243-D009-SUR-13",
            "cli/reporting output conformance matrix implementation is inconsistent",
        ),
        (
            "M243-D009-SUR-14",
            "cli/reporting output conformance matrix implementation is not ready",
        ),
        (
            "M243-D009-SUR-15",
            "cli/reporting output conformance matrix implementation key is not ready",
        ),
        (
            "M243-D009-SUR-16",
            "IsObjc3CliReportingOutputContractConformanceMatrixImplementationSurfaceReady(",
        ),
    ),
    "recovery_surface_header": (
        (
            "M243-D009-REC-01",
            "struct Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface {",
        ),
        ("M243-D009-REC-02", "bool recovery_determinism_ready = false;"),
        ("M243-D009-REC-03", "std::string recovery_determinism_key;"),
    ),
    "runner_source": (
        (
            "M243-D009-RUN-01",
            '#include "io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h"',
        ),
        (
            "M243-D009-RUN-02",
            "BuildObjc3CliReportingOutputContractConformanceMatrixImplementationSurface(",
        ),
        (
            "M243-D009-RUN-03",
            "IsObjc3CliReportingOutputContractConformanceMatrixImplementationSurfaceReady(",
        ),
        (
            "M243-D009-RUN-04",
            "cli/reporting output conformance matrix implementation fail-closed: ",
        ),
        ("M243-D009-RUN-05", "conformance_matrix_key"),
        ("M243-D009-RUN-06", "conformance_matrix_consistent"),
        ("M243-D009-RUN-07", "conformance_matrix_ready"),
        ("M243-D009-RUN-08", "conformance_matrix_key_ready"),
        ("M243-D009-RUN-09", "recovery_determinism_key"),
        ("M243-D009-RUN-10", "core_feature_impl_ready"),
    ),
    "architecture_doc": (
        (
            "M243-D009-ARC-01",
            "M243 lane-D D009 conformance matrix implementation anchors CLI/reporting output contract integration",
        ),
    ),
    "lowering_spec": (
        (
            "M243-D009-SPC-01",
            "CLI/reporting and output conformance matrix implementation governance shall preserve",
        ),
    ),
    "metadata_spec": (
        (
            "M243-D009-META-01",
            "deterministic lane-D CLI/reporting output conformance matrix implementation metadata anchors for `M243-D009`",
        ),
    ),
    "package_json": (
        (
            "M243-D009-CFG-01",
            '"check:objc3c:m243-d009-cli-reporting-output-contract-integration-conformance-matrix-implementation-contract"',
        ),
        (
            "M243-D009-CFG-02",
            '"test:tooling:m243-d009-cli-reporting-output-contract-integration-conformance-matrix-implementation-contract"',
        ),
        ("M243-D009-CFG-03", '"check:objc3c:m243-d009-lane-d-readiness"'),
        (
            "M243-D009-CFG-04",
            "npm run check:objc3c:m243-d008-lane-d-readiness && npm run check:objc3c:m243-d009-cli-reporting-output-contract-integration-conformance-matrix-implementation-contract && npm run test:tooling:m243-d009-cli-reporting-output-contract-integration-conformance-matrix-implementation-contract",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "conformance_surface_header": (
        ("M243-D009-FORB-01", "surface.conformance_matrix_ready = true;"),
        ("M243-D009-FORB-02", "surface.conformance_matrix_key_ready = true;"),
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
            "tmp/reports/m243/M243-D009/cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
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

    if args.emit_json:
        print(canonical_json(summary), end="")

    if ok:
        if not args.emit_json:
            print(f"[ok] {MODE}: {passed_checks}/{total_checks} checks passed")
        return 0

    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
