#!/usr/bin/env python3
"""Fail-closed checker for M243-D007 CLI/reporting output diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-d007-cli-reporting-output-contract-integration-diagnostics-hardening-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_diagnostics_hardening_d007_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_packet.md",
    "d006_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_d006_expectations.md",
    "d006_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_packet.md",
    "d006_checker": ROOT
    / "scripts"
    / "check_m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_contract.py",
    "d006_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_contract.py",
    "diagnostics_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_cli_reporting_output_contract_diagnostics_hardening_surface.h",
    "edge_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_cli_reporting_output_contract_edge_case_expansion_and_robustness_surface.h",
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
            "M243-D007-DOC-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-diagnostics-hardening/m243-d007-v1`",
        ),
        ("M243-D007-DOC-02", "Dependencies: `M243-D006`"),
        ("M243-D007-DOC-03", "diagnostics_hardening_key"),
        ("M243-D007-DOC-04", "diagnostics_hardening_consistent"),
        ("M243-D007-DOC-05", "diagnostics_hardening_ready"),
        (
            "M243-D007-DOC-06",
            "scripts/check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py",
        ),
        (
            "M243-D007-DOC-07",
            "tests/tooling/test_check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py",
        ),
        (
            "M243-D007-DOC-08",
            "python scripts/check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py --emit-json",
        ),
        ("M243-D007-DOC-09", "npm run check:objc3c:m243-d007-lane-d-readiness"),
    ),
    "packet_doc": (
        ("M243-D007-PKT-01", "Packet: `M243-D007`"),
        ("M243-D007-PKT-02", "Dependencies: `M243-D006`"),
        (
            "M243-D007-PKT-03",
            "scripts/check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py",
        ),
        (
            "M243-D007-PKT-04",
            "tests/tooling/test_check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py",
        ),
        ("M243-D007-PKT-05", "check:objc3c:m243-d007-lane-d-readiness"),
        (
            "M243-D007-PKT-06",
            "python scripts/check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py --emit-json",
        ),
    ),
    "d006_expectations_doc": (
        (
            "M243-D007-DEP-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-edge-case-expansion-and-robustness/m243-d006-v1`",
        ),
    ),
    "d006_packet_doc": (
        ("M243-D007-DEP-02", "Packet: `M243-D006`"),
        ("M243-D007-DEP-03", "Dependencies: `M243-D005`"),
    ),
    "d006_checker": (
        (
            "M243-D007-DEP-04",
            'MODE = "m243-d006-cli-reporting-output-contract-integration-edge-case-expansion-and-robustness-contract-v1"',
        ),
    ),
    "d006_tooling_test": (
        (
            "M243-D007-DEP-05",
            "check_m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_contract",
        ),
    ),
    "diagnostics_surface_header": (
        (
            "M243-D007-SUR-01",
            "struct Objc3CliReportingOutputContractDiagnosticsHardeningSurface {",
        ),
        ("M243-D007-SUR-02", "bool diagnostics_hardening_consistent = false;"),
        ("M243-D007-SUR-03", "bool diagnostics_hardening_ready = false;"),
        ("M243-D007-SUR-04", "bool diagnostics_hardening_key_ready = false;"),
        ("M243-D007-SUR-05", "std::string diagnostics_hardening_key;"),
        (
            "M243-D007-SUR-06",
            "BuildObjc3CliReportingOutputContractDiagnosticsHardeningKey(",
        ),
        (
            "M243-D007-SUR-07",
            "BuildObjc3CliReportingOutputContractDiagnosticsHardeningSurface(",
        ),
        ("M243-D007-SUR-08", "surface.diagnostics_hardening_consistent ="),
        ("M243-D007-SUR-09", "surface.diagnostics_hardening_ready ="),
        ("M243-D007-SUR-10", "surface.diagnostics_hardening_key_ready ="),
        ("M243-D007-SUR-11", "surface.core_feature_impl_ready ="),
        (
            "M243-D007-SUR-12",
            "cli/reporting output diagnostics hardening is inconsistent",
        ),
        (
            "M243-D007-SUR-13",
            "cli/reporting output diagnostics hardening is not ready",
        ),
        (
            "M243-D007-SUR-14",
            "cli/reporting output diagnostics hardening key is not ready",
        ),
        (
            "M243-D007-SUR-15",
            "IsObjc3CliReportingOutputContractDiagnosticsHardeningSurfaceReady(",
        ),
    ),
    "edge_surface_header": (
        (
            "M243-D007-EDG-01",
            "struct Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface {",
        ),
        ("M243-D007-EDG-02", "bool edge_case_robustness_ready = false;"),
        ("M243-D007-EDG-03", "std::string edge_case_robustness_key;"),
    ),
    "runner_source": (
        (
            "M243-D007-RUN-01",
            '#include "io/objc3_cli_reporting_output_contract_diagnostics_hardening_surface.h"',
        ),
        (
            "M243-D007-RUN-02",
            "BuildObjc3CliReportingOutputContractDiagnosticsHardeningSurface(",
        ),
        (
            "M243-D007-RUN-03",
            "IsObjc3CliReportingOutputContractDiagnosticsHardeningSurfaceReady(",
        ),
        (
            "M243-D007-RUN-04",
            "cli/reporting output diagnostics hardening fail-closed: ",
        ),
        ("M243-D007-RUN-05", "diagnostics_hardening_key"),
        ("M243-D007-RUN-06", "diagnostics_hardening_consistent"),
        ("M243-D007-RUN-07", "diagnostics_hardening_ready"),
        ("M243-D007-RUN-08", "core_feature_impl_ready"),
    ),
    "architecture_doc": (
        (
            "M243-D007-ARC-01",
            "M243 lane-D D006 edge-case expansion and robustness anchors CLI/reporting output contract integration",
        ),
    ),
    "lowering_spec": (
        (
            "M243-D007-SPC-01",
            "CLI/reporting and output edge-case expansion and robustness governance shall preserve",
        ),
    ),
    "metadata_spec": (
        (
            "M243-D007-META-01",
            "deterministic lane-D CLI/reporting output edge-case expansion and robustness metadata anchors for `M243-D006`",
        ),
    ),
    "package_json": (
        ("M243-D007-CFG-01", '"check:objc3c:m243-d006-lane-d-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "diagnostics_surface_header": (
        ("M243-D007-FORB-01", "surface.diagnostics_hardening_ready = true;"),
        ("M243-D007-FORB-02", "surface.diagnostics_hardening_key_ready = true;"),
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
            "tmp/reports/m243/M243-D007/cli_reporting_and_output_contract_integration_diagnostics_hardening_contract_summary.json"
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
