#!/usr/bin/env python3
"""Fail-closed validator for M243-D002 CLI/reporting output-contract modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-d002-cli-reporting-output-contract-integration-modular-split-scaffolding-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_cli_reporting_output_contract_scaffold.h",
    "diagnostics_artifacts_source": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp",
    "c_api_runner_source": ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp",
    "cli_frontend_source": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.cpp",
    "pipeline_contract": ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_packet.md",
    "d001_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_d001_expectations.md",
    "d001_checker": ROOT / "scripts" / "check_m243_d001_cli_reporting_and_output_contract_integration_contract.py",
    "d001_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_d001_cli_reporting_and_output_contract_integration_contract.py",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_modular_split_scaffolding_d002_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M243-D002-SCA-01", "struct Objc3CliReportingOutputContractScaffold {"),
        ("M243-D002-SCA-02", "bool diagnostics_schema_version_pinned = false;"),
        ("M243-D002-SCA-03", "bool summary_mode_pinned = false;"),
        (
            "M243-D002-SCA-04",
            'inline constexpr const char *kObjc3CliReportingDiagnosticsSchemaVersion = "1.0.0";',
        ),
        (
            "M243-D002-SCA-05",
            'inline constexpr const char *kObjc3CliReportingSummaryMode = "objc3c-frontend-c-api-runner-v1";',
        ),
        ("M243-D002-SCA-06", "inline std::string BuildObjc3CliReportingOutputContractScaffoldKey("),
        ("M243-D002-SCA-07", "inline Objc3CliReportingOutputContractScaffold BuildObjc3CliReportingOutputContractScaffold("),
        ("M243-D002-SCA-08", "scaffold.modular_split_ready ="),
        ("M243-D002-SCA-09", "inline bool IsObjc3CliReportingOutputContractScaffoldReady("),
        (
            "M243-D002-SCA-10",
            'reason = scaffold.failure_reason.empty() ? "cli/reporting output contract scaffold not ready"',
        ),
        (
            "M243-D002-SCA-11",
            'scaffold.failure_reason = "stage-report output contract is not ready";',
        ),
    ),
    "diagnostics_artifacts_source": (
        ("M243-D002-DIA-01", 'out << "  \\"schema_version\\": \\"1.0.0\\",\\n";'),
        ("M243-D002-DIA-02", 'WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());'),
    ),
    "c_api_runner_source": (
        ("M243-D002-RUN-01", '#include "io/objc3_cli_reporting_output_contract_scaffold.h"'),
        (
            "M243-D002-RUN-02",
            'out << "  \\"mode\\": \\"objc3c-frontend-c-api-runner-v1\\",\\n";',
        ),
        (
            "M243-D002-RUN-03",
            "const Objc3CliReportingOutputContractScaffold cli_reporting_output_contract_scaffold =",
        ),
        (
            "M243-D002-RUN-04",
            "BuildObjc3CliReportingOutputContractScaffold(",
        ),
        (
            "M243-D002-RUN-05",
            "IsObjc3CliReportingOutputContractScaffoldReady(",
        ),
        (
            "M243-D002-RUN-06",
            "cli/reporting output scaffold fail-closed: ",
        ),
        (
            "M243-D002-RUN-07",
            'WriteStageSummaryJson(out, "emit", result.emit, false);',
        ),
        (
            "M243-D002-RUN-08",
            'options.summary_out.empty() ? (options.out_dir / (options.emit_prefix + ".c_api_summary.json")) : options.summary_out;',
        ),
    ),
    "cli_frontend_source": (
        ("M243-D002-CLI-01", "Objc3FrontendArtifactBundle CompileObjc3SourceForCli("),
        ("M243-D002-CLI-02", "return std::move(product.artifact_bundle);"),
    ),
    "pipeline_contract": (
        ("M243-D002-PIP-01", 'std::string output_dir = "tmp/artifacts/compilation/objc3c-native";'),
        ("M243-D002-PIP-02", "ErrorPropagationModel error_model = ErrorPropagationModel::NoThrowFailClosed;"),
    ),
    "architecture_doc": (
        (
            "M243-D002-ARCH-01",
            "M243 lane-D D002 modular split scaffolding anchors CLI/reporting output contract integration",
        ),
        (
            "M243-D002-ARCH-02",
            "`io/objc3_cli_reporting_output_contract_scaffold.h`",
        ),
    ),
    "lowering_spec": (
        (
            "M243-D002-SPC-01",
            "CLI/reporting and output modular split scaffolding governance shall preserve",
        ),
        (
            "M243-D002-SPC-02",
            "explicit lane-D dependency anchors (`M243-D001`) and fail closed on",
        ),
    ),
    "metadata_spec": (
        (
            "M243-D002-META-01",
            "deterministic lane-D CLI/reporting output modular split scaffold metadata anchors for `M243-D002`",
        ),
        (
            "M243-D002-META-02",
            "with explicit `M243-D001` dependency continuity",
        ),
    ),
    "package_json": (
        (
            "M243-D002-CFG-01",
            '"check:objc3c:m243-d002-cli-reporting-output-contract-integration-modular-split-scaffolding-contract": '
            '"python scripts/check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py"',
        ),
        (
            "M243-D002-CFG-02",
            '"test:tooling:m243-d002-cli-reporting-output-contract-integration-modular-split-scaffolding-contract": '
            '"python -m pytest tests/tooling/test_check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py -q"',
        ),
        (
            "M243-D002-CFG-03",
            '"check:objc3c:m243-d002-lane-d-readiness": '
            '"npm run check:objc3c:m243-d001-lane-d-readiness '
            "&& npm run check:objc3c:m243-d002-cli-reporting-output-contract-integration-modular-split-scaffolding-contract "
            '&& npm run test:tooling:m243-d002-cli-reporting-output-contract-integration-modular-split-scaffolding-contract"',
        ),
        ("M243-D002-CFG-04", '"compile:objc3c": '),
        ("M243-D002-CFG-05", '"proof:objc3c": '),
        ("M243-D002-CFG-06", '"test:objc3c:execution-replay-proof": '),
        ("M243-D002-CFG-07", '"test:objc3c:perf-budget": '),
    ),
    "packet_doc": (
        (
            "M243-D002-PKT-01",
            "# M243-D002 CLI/Reporting and Output Contract Integration Modular Split/Scaffolding Packet",
        ),
        (
            "M243-D002-PKT-02",
            "Packet: `M243-D002`",
        ),
        (
            "M243-D002-PKT-03",
            "Dependencies: `M243-D001`",
        ),
        (
            "M243-D002-PKT-04",
            "scripts/check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py",
        ),
        (
            "M243-D002-PKT-05",
            "tests/tooling/test_check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py",
        ),
        (
            "M243-D002-PKT-06",
            "`check:objc3c:m243-d002-lane-d-readiness`",
        ),
        (
            "M243-D002-PKT-07",
            "`proof:objc3c`",
        ),
        (
            "M243-D002-PKT-08",
            "including code/spec anchors and milestone optimization",
        ),
    ),
    "d001_contract_doc": (
        (
            "M243-D002-D001-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-freeze/m243-d001-v1`",
        ),
    ),
    "d001_checker": (
        (
            "M243-D002-D001-02",
            'MODE = "m243-d001-cli-reporting-output-contract-integration-contract-v1"',
        ),
    ),
    "d001_tooling_test": (
        (
            "M243-D002-D001-03",
            "check_m243_d001_cli_reporting_and_output_contract_integration_contract",
        ),
    ),
    "contract_doc": (
        (
            "M243-D002-DOC-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-modular-split-scaffolding/m243-d002-v1`",
        ),
        (
            "M243-D002-DOC-02",
            "BuildObjc3CliReportingOutputContractScaffold",
        ),
        (
            "M243-D002-DOC-03",
            "Dependencies: `M243-D001`",
        ),
        (
            "M243-D002-DOC-04",
            "spec/planning/compiler/m243/m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_packet.md",
        ),
        (
            "M243-D002-DOC-05",
            "python scripts/check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py",
        ),
        (
            "M243-D002-DOC-06",
            "python -m pytest tests/tooling/test_check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py -q",
        ),
        (
            "M243-D002-DOC-07",
            "npm run check:objc3c:m243-d002-lane-d-readiness",
        ),
        (
            "M243-D002-DOC-08",
            "tmp/reports/m243/M243-D002/cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract_summary.json",
        ),
        (
            "M243-D002-DOC-09",
            "`proof:objc3c`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M243-D002-FORB-01", "scaffold.modular_split_ready = true;"),
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
            "tmp/reports/m243/M243-D002/cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract_summary.json"
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
