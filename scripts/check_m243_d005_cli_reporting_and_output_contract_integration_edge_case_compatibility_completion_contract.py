#!/usr/bin/env python3
"""Fail-closed checker for M243-D005 CLI/reporting output edge-case compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-d005-cli-reporting-output-contract-integration-edge-case-compatibility-completion-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_d005_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_packet.md",
    "d004_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_core_feature_expansion_d004_expectations.md",
    "d004_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_packet.md",
    "d004_checker": ROOT
    / "scripts"
    / "check_m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_contract.py",
    "d004_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_contract.py",
    "edge_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_cli_reporting_output_contract_edge_case_compatibility_surface.h",
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
            "M243-D005-DOC-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-edge-case-compatibility-completion/m243-d005-v1`",
        ),
        ("M243-D005-DOC-02", "Dependencies: `M243-D004`"),
        ("M243-D005-DOC-03", "edge_case_compatibility_key"),
        (
            "M243-D005-DOC-04",
            "scripts/check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py",
        ),
        (
            "M243-D005-DOC-05",
            "tests/tooling/test_check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py",
        ),
        ("M243-D005-DOC-06", "npm run check:objc3c:m243-d005-lane-d-readiness"),
    ),
    "packet_doc": (
        ("M243-D005-PKT-01", "Packet: `M243-D005`"),
        ("M243-D005-PKT-02", "Dependencies: `M243-D004`"),
        (
            "M243-D005-PKT-03",
            "scripts/check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py",
        ),
        (
            "M243-D005-PKT-04",
            "tests/tooling/test_check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py",
        ),
        ("M243-D005-PKT-05", "check:objc3c:m243-d005-lane-d-readiness"),
    ),
    "d004_expectations_doc": (
        (
            "M243-D005-DEP-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-core-feature-expansion/m243-d004-v1`",
        ),
    ),
    "d004_packet_doc": (
        ("M243-D005-DEP-02", "Packet: `M243-D004`"),
        ("M243-D005-DEP-03", "Dependencies: `M243-D003`"),
    ),
    "d004_checker": (
        (
            "M243-D005-DEP-04",
            'MODE = "m243-d004-cli-reporting-output-contract-integration-core-feature-expansion-contract-v1"',
        ),
    ),
    "d004_tooling_test": (
        (
            "M243-D005-DEP-05",
            "check_m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_contract",
        ),
    ),
    "edge_surface_header": (
        (
            "M243-D005-SUR-01",
            "struct Objc3CliReportingOutputContractEdgeCaseCompatibilitySurface {",
        ),
        ("M243-D005-SUR-02", "bool edge_case_compatibility_consistent = false;"),
        ("M243-D005-SUR-03", "bool edge_case_compatibility_ready = false;"),
        ("M243-D005-SUR-04", "std::string edge_case_compatibility_key;"),
        (
            "M243-D005-SUR-05",
            "BuildObjc3CliReportingOutputContractEdgeCaseCompatibilityKey(",
        ),
        (
            "M243-D005-SUR-06",
            "BuildObjc3CliReportingOutputContractEdgeCaseCompatibilitySurface(",
        ),
        ("M243-D005-SUR-07", "surface.case_folded_paths_distinct ="),
        ("M243-D005-SUR-08", "surface.output_paths_control_char_free ="),
        ("M243-D005-SUR-09", "surface.core_feature_impl_ready ="),
        (
            "M243-D005-SUR-10",
            "IsObjc3CliReportingOutputContractEdgeCaseCompatibilitySurfaceReady(",
        ),
    ),
    "runner_source": (
        (
            "M243-D005-RUN-01",
            '#include "io/objc3_cli_reporting_output_contract_edge_case_compatibility_surface.h"',
        ),
        (
            "M243-D005-RUN-02",
            "BuildObjc3CliReportingOutputContractEdgeCaseCompatibilitySurface(",
        ),
        (
            "M243-D005-RUN-03",
            "IsObjc3CliReportingOutputContractEdgeCaseCompatibilitySurfaceReady(",
        ),
        ("M243-D005-RUN-04", "edge_case_compatibility_key"),
        ("M243-D005-RUN-05", "edge_case_compatibility_ready"),
        (
            "M243-D005-RUN-06",
            "cli/reporting output edge-case compatibility fail-closed: ",
        ),
    ),
    "architecture_doc": (
        (
            "M243-D005-ARC-01",
            "M243 lane-D D005 edge-case compatibility completion anchors CLI/reporting output contract integration",
        ),
    ),
    "lowering_spec": (
        (
            "M243-D005-SPC-01",
            "CLI/reporting and output edge-case compatibility completion governance",
        ),
    ),
    "metadata_spec": (
        (
            "M243-D005-META-01",
            "deterministic lane-D CLI/reporting output edge-case compatibility completion metadata anchors for `M243-D005`",
        ),
    ),
    "package_json": (
        (
            "M243-D005-CFG-01",
            '"check:objc3c:m243-d005-cli-reporting-output-contract-integration-edge-case-compatibility-completion-contract"',
        ),
        (
            "M243-D005-CFG-02",
            '"test:tooling:m243-d005-cli-reporting-output-contract-integration-edge-case-compatibility-completion-contract"',
        ),
        ("M243-D005-CFG-03", '"check:objc3c:m243-d005-lane-d-readiness"'),
        (
            "M243-D005-CFG-04",
            "npm run check:objc3c:m243-d004-lane-d-readiness && npm run check:objc3c:m243-d005-cli-reporting-output-contract-integration-edge-case-compatibility-completion-contract && npm run test:tooling:m243-d005-cli-reporting-output-contract-integration-edge-case-compatibility-completion-contract",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "edge_surface_header": (
        ("M243-D005-FORB-01", "surface.edge_case_compatibility_ready = true;"),
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
            "tmp/reports/m243/M243-D005/cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract_summary.json"
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
