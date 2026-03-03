#!/usr/bin/env python3
"""Fail-closed validator for M243-D001 CLI/reporting output-contract integration freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-d001-cli-reporting-output-contract-integration-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_d001_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d001_cli_reporting_and_output_contract_integration_contract_and_architecture_freeze_packet.md",
    "cli_frontend_source": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.cpp",
    "diagnostics_artifacts_source": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp",
    "c_api_runner_source": ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp",
    "pipeline_contract": ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M243-D001-DOC-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-freeze/m243-d001-v1`",
        ),
        (
            "M243-D001-DOC-02",
            "scripts/check_m243_d001_cli_reporting_and_output_contract_integration_contract.py",
        ),
        (
            "M243-D001-DOC-03",
            "tests/tooling/test_check_m243_d001_cli_reporting_and_output_contract_integration_contract.py",
        ),
        (
            "M243-D001-DOC-04",
            "tmp/reports/m243/M243-D001/cli_reporting_and_output_contract_integration_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M243-D001-PKT-01", "Packet: `M243-D001`"),
        ("M243-D001-PKT-02", "Dependencies: none"),
        (
            "M243-D001-PKT-03",
            "scripts/check_m243_d001_cli_reporting_and_output_contract_integration_contract.py",
        ),
        ("M243-D001-PKT-04", "`test:objc3c:perf-budget`"),
    ),
    "cli_frontend_source": (
        ("M243-D001-CLI-01", "Objc3FrontendArtifactBundle CompileObjc3SourceForCli("),
        ("M243-D001-CLI-02", "return std::move(product.artifact_bundle);"),
    ),
    "diagnostics_artifacts_source": (
        ("M243-D001-DIA-01", 'out << "  \\"schema_version\\": \\"1.0.0\\",\\n";'),
        ("M243-D001-DIA-02", 'WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());'),
    ),
    "c_api_runner_source": (
        ("M243-D001-RUN-01", 'out << "  \\"mode\\": \\"objc3c-frontend-c-api-runner-v1\\",\\n";'),
        ("M243-D001-RUN-02", 'WriteStageSummaryJson(out, "emit", result.emit, false);'),
        (
            "M243-D001-RUN-03",
            'options.summary_out.empty() ? (options.out_dir / (options.emit_prefix + ".c_api_summary.json")) : options.summary_out;',
        ),
        ("M243-D001-RUN-04", 'std::cout << "wrote summary: " << summary_path.generic_string() << "\\n";'),
    ),
    "pipeline_contract": (
        ("M243-D001-PIP-01", 'std::string output_dir = "tmp/artifacts/compilation/objc3c-native";'),
        ("M243-D001-PIP-02", "ErrorPropagationModel error_model = ErrorPropagationModel::NoThrowFailClosed;"),
        ("M243-D001-PIP-03", "std::string diagnostics_path;"),
    ),
    "architecture_doc": (
        (
            "M243-D001-ARC-01",
            "M243 lane-D D001 CLI/reporting and output contract integration anchors explicit",
        ),
        (
            "M243-D001-ARC-02",
            "docs/contracts/m243_cli_reporting_and_output_contract_integration_d001_expectations.md",
        ),
    ),
    "lowering_spec": (
        (
            "M243-D001-SPC-01",
            "CLI/reporting and output contract integration governance shall preserve",
        ),
        (
            "M243-D001-SPC-02",
            "summary payload, diagnostics artifact path, or stage-report output",
        ),
    ),
    "metadata_spec": (
        (
            "M243-D001-META-01",
            "deterministic lane-D CLI/reporting output metadata anchors for `M243-D001`",
        ),
        ("M243-D001-META-02", "diagnostics artifact and summary payload continuity"),
    ),
    "package_json": (
        (
            "M243-D001-PKG-01",
            '"check:objc3c:m243-d001-cli-reporting-output-contract-integration-contract"',
        ),
        (
            "M243-D001-PKG-02",
            '"test:tooling:m243-d001-cli-reporting-output-contract-integration-contract"',
        ),
        (
            "M243-D001-PKG-03",
            '"check:objc3c:m243-d001-lane-d-readiness": '
            '"npm run check:objc3c:m243-d001-cli-reporting-output-contract-integration-contract '
            '&& npm run test:tooling:m243-d001-cli-reporting-output-contract-integration-contract"',
        ),
        ("M243-D001-PKG-04", '"compile:objc3c": '),
        ("M243-D001-PKG-05", '"proof:objc3c": '),
        ("M243-D001-PKG-06", '"test:objc3c:execution-replay-proof": '),
        ("M243-D001-PKG-07", '"test:objc3c:perf-budget": '),
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
            "tmp/reports/m243/M243-D001/cli_reporting_and_output_contract_integration_contract_summary.json"
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
