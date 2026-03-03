#!/usr/bin/env python3
"""Fail-closed checker for M243-D010 CLI/reporting output conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-d010-cli-reporting-output-contract-integration-"
    "conformance-corpus-expansion-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_d010_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_packet.md",
    "d009_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_d009_expectations.md",
    "d009_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_packet.md",
    "d009_checker": ROOT
    / "scripts"
    / "check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py",
    "d009_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py",
    "conformance_corpus_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_cli_reporting_output_contract_conformance_corpus_expansion_surface.h",
    "conformance_matrix_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h",
    "runner_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "tools"
    / "objc3c_frontend_c_api_runner.cpp",
    "conformance_manifest": ROOT
    / "tests"
    / "conformance"
    / "diagnostics"
    / "manifest.json",
    "conformance_accept_fixture": ROOT
    / "tests"
    / "conformance"
    / "diagnostics"
    / "M243-D010-C001.json",
    "conformance_reject_fixture": ROOT
    / "tests"
    / "conformance"
    / "diagnostics"
    / "M243-D010-R001.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-D010-DOC-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-conformance-corpus-expansion/m243-d010-v1`",
        ),
        ("M243-D010-DOC-02", "- Dependencies: `M243-D009`"),
        ("M243-D010-DOC-03", "conformance_corpus_key"),
        ("M243-D010-DOC-04", "conformance_corpus_consistent"),
        ("M243-D010-DOC-05", "conformance_corpus_ready"),
        ("M243-D010-DOC-06", "conformance_corpus_key_ready"),
        ("M243-D010-DOC-07", "M243-D010-C001"),
        ("M243-D010-DOC-08", "M243-D010-R001"),
        (
            "M243-D010-DOC-09",
            "scripts/check_m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract.py",
        ),
        (
            "M243-D010-DOC-10",
            "tests/tooling/test_check_m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract.py",
        ),
        (
            "M243-D010-DOC-11",
            "python scripts/check_m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract.py --emit-json",
        ),
        ("M243-D010-DOC-12", "npm run check:objc3c:m243-d010-lane-d-readiness"),
    ),
    "packet_doc": (
        ("M243-D010-PKT-01", "Packet: `M243-D010`"),
        ("M243-D010-PKT-02", "Dependencies: `M243-D009`"),
        (
            "M243-D010-PKT-03",
            "scripts/check_m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract.py",
        ),
        (
            "M243-D010-PKT-04",
            "tests/tooling/test_check_m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract.py",
        ),
        ("M243-D010-PKT-05", "M243-D010-C001"),
        ("M243-D010-PKT-06", "M243-D010-R001"),
        ("M243-D010-PKT-07", "check:objc3c:m243-d010-lane-d-readiness"),
        (
            "M243-D010-PKT-08",
            "python scripts/check_m243_d010_cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract.py --emit-json",
        ),
        (
            "M243-D010-PKT-09",
            "tmp/reports/m243/M243-D010/cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract_summary.json",
        ),
    ),
    "d009_expectations_doc": (
        (
            "M243-D010-DEP-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-conformance-matrix-implementation/m243-d009-v1`",
        ),
    ),
    "d009_packet_doc": (
        ("M243-D010-DEP-02", "Packet: `M243-D009`"),
        ("M243-D010-DEP-03", "Dependencies: `M243-D008`"),
    ),
    "d009_checker": (
        (
            "M243-D010-DEP-04",
            'MODE = "m243-d009-cli-reporting-output-contract-integration-conformance-matrix-implementation-contract-v1"',
        ),
    ),
    "d009_tooling_test": (
        (
            "M243-D010-DEP-05",
            "check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract",
        ),
    ),
    "conformance_corpus_surface_header": (
        (
            "M243-D010-SUR-01",
            "struct Objc3CliReportingOutputContractConformanceCorpusExpansionSurface {",
        ),
        ("M243-D010-SUR-02", "bool conformance_corpus_consistent = false;"),
        ("M243-D010-SUR-03", "bool conformance_corpus_ready = false;"),
        ("M243-D010-SUR-04", "bool conformance_corpus_key_ready = false;"),
        ("M243-D010-SUR-05", "std::size_t conformance_corpus_case_count = 0u;"),
        (
            "M243-D010-SUR-06",
            "kObjc3CliReportingOutputContractConformanceCorpusAcceptCaseId",
        ),
        (
            "M243-D010-SUR-07",
            "kObjc3CliReportingOutputContractConformanceCorpusRejectCaseId",
        ),
        (
            "M243-D010-SUR-08",
            "BuildObjc3CliReportingOutputContractConformanceCorpusExpansionKey(",
        ),
        (
            "M243-D010-SUR-09",
            "BuildObjc3CliReportingOutputContractConformanceCorpusExpansionSurface(",
        ),
        ("M243-D010-SUR-10", "surface.conformance_corpus_consistent ="),
        ("M243-D010-SUR-11", "surface.conformance_corpus_ready ="),
        ("M243-D010-SUR-12", "surface.conformance_corpus_key ="),
        ("M243-D010-SUR-13", "surface.conformance_corpus_key_ready ="),
        ("M243-D010-SUR-14", "surface.core_feature_impl_ready ="),
        (
            "M243-D010-SUR-15",
            "cli/reporting output conformance corpus expansion is inconsistent",
        ),
        (
            "M243-D010-SUR-16",
            "cli/reporting output conformance corpus expansion is not ready",
        ),
        (
            "M243-D010-SUR-17",
            "cli/reporting output conformance corpus expansion key is not ready",
        ),
        (
            "M243-D010-SUR-18",
            "IsObjc3CliReportingOutputContractConformanceCorpusExpansionSurfaceReady(",
        ),
        ("M243-D010-SUR-19", "M243-D010-C001"),
        ("M243-D010-SUR-20", "M243-D010-R001"),
    ),
    "conformance_matrix_surface_header": (
        (
            "M243-D010-MAT-01",
            "struct Objc3CliReportingOutputContractConformanceMatrixImplementationSurface {",
        ),
        ("M243-D010-MAT-02", "std::string conformance_matrix_key;"),
    ),
    "runner_source": (
        (
            "M243-D010-RUN-01",
            '#include "io/objc3_cli_reporting_output_contract_conformance_corpus_expansion_surface.h"',
        ),
        (
            "M243-D010-RUN-02",
            "BuildObjc3CliReportingOutputContractConformanceCorpusExpansionSurface(",
        ),
        (
            "M243-D010-RUN-03",
            "IsObjc3CliReportingOutputContractConformanceCorpusExpansionSurfaceReady(",
        ),
        (
            "M243-D010-RUN-04",
            "cli/reporting output conformance corpus expansion fail-closed: ",
        ),
        ("M243-D010-RUN-05", "conformance_corpus_key"),
        ("M243-D010-RUN-06", "conformance_corpus_case_count"),
        ("M243-D010-RUN-07", "conformance_corpus_accept_case_count"),
        ("M243-D010-RUN-08", "conformance_corpus_reject_case_count"),
        ("M243-D010-RUN-09", "conformance_corpus_consistent"),
        ("M243-D010-RUN-10", "conformance_corpus_ready"),
        ("M243-D010-RUN-11", "conformance_corpus_key_ready"),
        ("M243-D010-RUN-12", "cli_reporting_output_contract_conformance_corpus_surface"),
    ),
    "conformance_manifest": (
        (
            "M243-D010-CFM-01",
            '"name": "issue_6474_m243_lane_d_conformance_corpus_expansion"',
        ),
        ("M243-D010-CFM-02", '"issue": 6474'),
        ("M243-D010-CFM-03", '"M243-D010-C001.json"'),
        ("M243-D010-CFM-04", '"M243-D010-R001.json"'),
    ),
    "conformance_accept_fixture": (
        ("M243-D010-FIX-01", '"id": "M243-D010-C001"'),
        ("M243-D010-FIX-02", "Issue #6474 / M243-D010"),
        ("M243-D010-FIX-03", '"parse": "accept"'),
        ("M243-D010-FIX-04", '"diagnostics": []'),
    ),
    "conformance_reject_fixture": (
        ("M243-D010-FIX-05", '"id": "M243-D010-R001"'),
        ("M243-D010-FIX-06", "Issue #6474 / M243-D010"),
        ("M243-D010-FIX-07", '"code": "OBJC3-D-CONFORMANCE-CORPUS-KEY-DRIFT"'),
        ("M243-D010-FIX-08", '"replacement": "@\\"conformance=canonical-m243-d010\\""' ),
    ),
    "architecture_doc": (
        (
            "M243-D010-ARC-01",
            "M243 lane-D D010 conformance corpus expansion anchors CLI/reporting output contract integration",
        ),
    ),
    "lowering_spec": (
        (
            "M243-D010-SPC-01",
            "CLI/reporting and output conformance corpus expansion governance shall preserve",
        ),
    ),
    "metadata_spec": (
        (
            "M243-D010-META-01",
            "deterministic lane-D CLI/reporting output conformance corpus expansion metadata anchors for `M243-D010`",
        ),
    ),
    "package_json": (
        (
            "M243-D010-PKG-01",
            '"check:objc3c:m243-d010-cli-reporting-output-contract-integration-conformance-corpus-expansion-contract"',
        ),
        (
            "M243-D010-PKG-02",
            '"test:tooling:m243-d010-cli-reporting-output-contract-integration-conformance-corpus-expansion-contract"',
        ),
        ("M243-D010-PKG-03", '"check:objc3c:m243-d010-lane-d-readiness"'),
        (
            "M243-D010-PKG-04",
            "npm run check:objc3c:m243-d009-lane-d-readiness && npm run check:objc3c:m243-d010-cli-reporting-output-contract-integration-conformance-corpus-expansion-contract && npm run test:tooling:m243-d010-cli-reporting-output-contract-integration-conformance-corpus-expansion-contract",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "conformance_corpus_surface_header": (
        ("M243-D010-FORB-01", "surface.conformance_corpus_ready = true;"),
        ("M243-D010-FORB-02", "surface.conformance_corpus_key_ready = true;"),
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
            "tmp/reports/m243/M243-D010/cli_reporting_and_output_contract_integration_conformance_corpus_expansion_contract_summary.json"
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

