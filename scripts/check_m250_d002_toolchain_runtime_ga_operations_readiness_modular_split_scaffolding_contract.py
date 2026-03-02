#!/usr/bin/env python3
"""Fail-closed validator for M250-D002 toolchain/runtime GA operations modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-toolchain-runtime-ga-operations-readiness-modular-split-scaffolding-contract-d002-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_toolchain_runtime_ga_operations_scaffold.h",
    "objc3_path_source": ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "d001_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_contract_freeze_d001_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_d002_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_d002_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M250-D002-SCA-01", "struct Objc3ToolchainRuntimeGaOperationsScaffold {"),
        ("M250-D002-SCA-02", "bool compile_route_ready = false;"),
        ("M250-D002-SCA-03", "bool modular_split_ready = false;"),
        ("M250-D002-SCA-04", "inline std::string BuildObjc3ToolchainRuntimeGaOperationsScaffoldKey("),
        ("M250-D002-SCA-05", "inline Objc3ToolchainRuntimeGaOperationsScaffold BuildObjc3ToolchainRuntimeGaOperationsScaffold("),
        ("M250-D002-SCA-06", "scaffold.compile_route_ready ="),
        (
            "M250-D002-SCA-07",
            "scaffold.modular_split_ready = scaffold.compile_route_ready &&",
        ),
        ("M250-D002-SCA-08", 'scaffold.failure_reason = "llvm-direct backend unavailable in this build";'),
        ("M250-D002-SCA-09", 'scaffold.failure_reason = "toolchain/runtime compile route is not ready";'),
        ("M250-D002-SCA-10", "inline bool IsObjc3ToolchainRuntimeGaOperationsScaffoldReady("),
        (
            "M250-D002-SCA-11",
            'reason = scaffold.failure_reason.empty() ? "toolchain/runtime ga operations scaffold not ready"',
        ),
    ),
    "objc3_path_source": (
        ("M250-D002-PTH-01", '#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"'),
        (
            "M250-D002-PTH-02",
            "const Objc3ToolchainRuntimeGaOperationsScaffold toolchain_runtime_ga_operations_scaffold =",
        ),
        (
            "M250-D002-PTH-03",
            "BuildObjc3ToolchainRuntimeGaOperationsScaffold(",
        ),
        (
            "M250-D002-PTH-04",
            "IsObjc3ToolchainRuntimeGaOperationsScaffoldReady(",
        ),
        (
            "M250-D002-PTH-05",
            "toolchain/runtime readiness scaffold fail-closed: ",
        ),
    ),
    "architecture_doc": (
        (
            "M250-D002-ARCH-01",
            "M250 lane-D D002 modular split scaffolding anchors toolchain/runtime GA",
        ),
        (
            "M250-D002-ARCH-02",
            "`io/objc3_toolchain_runtime_ga_operations_scaffold.h`",
        ),
    ),
    "d001_contract_doc": (
        (
            "M250-D002-D001-01",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-freeze/m250-d001-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M250-D002-DOC-01",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-modular-split-scaffolding/m250-d002-v1`",
        ),
        ("M250-D002-DOC-02", "Objc3ToolchainRuntimeGaOperationsScaffold"),
        ("M250-D002-DOC-03", "BuildObjc3ToolchainRuntimeGaOperationsScaffold"),
        (
            "M250-D002-DOC-04",
            "python scripts/check_m250_d002_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract.py",
        ),
        (
            "M250-D002-DOC-05",
            "python -m pytest tests/tooling/test_check_m250_d002_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract.py -q",
        ),
        ("M250-D002-DOC-06", "npm run check:objc3c:m250-d002-lane-d-readiness"),
        (
            "M250-D002-DOC-07",
            "tmp/reports/m250/M250-D002/toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M250-D002-PKT-01", "Packet: `M250-D002`"),
        ("M250-D002-PKT-02", "Lane: `D`"),
        ("M250-D002-PKT-03", "Dependencies: `M250-D001`"),
        (
            "M250-D002-PKT-04",
            "scripts/check_m250_d002_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract.py",
        ),
        (
            "M250-D002-PKT-05",
            "tests/tooling/test_check_m250_d002_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract.py",
        ),
    ),
    "package_json": (
        (
            "M250-D002-CFG-01",
            '"check:objc3c:m250-d002-toolchain-runtime-ga-operations-readiness-modular-split-scaffolding-contract"',
        ),
        (
            "M250-D002-CFG-02",
            '"test:tooling:m250-d002-toolchain-runtime-ga-operations-readiness-modular-split-scaffolding-contract"',
        ),
        (
            "M250-D002-CFG-03",
            '"check:objc3c:m250-d002-lane-d-readiness"',
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M250-D002-FORB-01", "scaffold.compile_route_ready = true;"),
        ("M250-D002-FORB-02", "scaffold.modular_split_ready = true;"),
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
            "tmp/reports/m250/M250-D002/"
            "toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract_summary.json"
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
