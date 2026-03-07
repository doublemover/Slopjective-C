#!/usr/bin/env python3
"""Fail-closed checker for M243-C017 lowering/runtime diagnostics surfacing integration closeout and gate sign-off."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-c017-lowering-runtime-diagnostics-surfacing-"
    "integration-closeout-gate-sign-off-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_c017_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_packet.md",
    "c016_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_c016_expectations.md",
    "c016_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c016_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_packet.md",
    "c016_checker": ROOT
    / "scripts"
    / "check_m243_c016_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_contract.py",
    "c016_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_c016_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_contract.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-C017-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-integration-closeout-gate-sign-off/m243-c017-v1`",
        ),
        ("M243-C017-DOC-02", "- Dependencies: `M243-C016`"),
        ("M243-C017-DOC-03", "integration_closeout_gate_sign_off_consistent"),
        ("M243-C017-DOC-04", "integration_closeout_gate_sign_off_ready"),
        ("M243-C017-DOC-05", "integration_closeout_gate_sign_off_key_ready"),
        (
            "M243-C017-DOC-06",
            "scripts/check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py",
        ),
        (
            "M243-C017-DOC-07",
            "tests/tooling/test_check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py",
        ),
        (
            "M243-C017-DOC-08",
            "python scripts/check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py --emit-json",
        ),
        (
            "M243-C017-DOC-09",
            "tmp/reports/m243/M243-C017/lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract_summary.json",
        ),
        ("M243-C017-DOC-10", "npm run check:objc3c:m243-c017-lane-c-readiness"),
    ),
    "packet_doc": (
        ("M243-C017-PKT-01", "Packet: `M243-C017`"),
        ("M243-C017-PKT-02", "Dependencies: `M243-C016`"),
        (
            "M243-C017-PKT-03",
            "scripts/check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py",
        ),
        (
            "M243-C017-PKT-04",
            "tests/tooling/test_check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py",
        ),
        ("M243-C017-PKT-05", "check:objc3c:m243-c017-lane-c-readiness"),
        (
            "M243-C017-PKT-06",
            "python scripts/check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py --emit-json",
        ),
        (
            "M243-C017-PKT-07",
            "tmp/reports/m243/M243-C017/lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract_summary.json",
        ),
        (
            "M243-C017-PKT-08",
            "python scripts/check_m243_c016_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_contract.py",
        ),
    ),
    "c016_expectations_doc": (
        (
            "M243-C017-DEP-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-advanced-edge-compatibility-workpack-shard-1/m243-c016-v1`",
        ),
    ),
    "c016_packet_doc": (
        ("M243-C017-DEP-02", "Packet: `M243-C016`"),
        ("M243-C017-DEP-03", "Dependencies: `M243-C015`"),
    ),
    "c016_checker": (
        (
            "M243-C017-DEP-04",
            "advanced-edge-compatibility-workpack-shard-1-contract-v1",
        ),
    ),
    "c016_tooling_test": (
        (
            "M243-C017-DEP-05",
            "check_m243_c016_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_contract",
        ),
    ),
    "architecture_doc": (
        (
            "M243-C017-ARC-01",
            "M243 lane-C C017 integration closeout and gate sign-off anchors lowering/runtime diagnostics surfacing",
        ),
    ),
    "lowering_spec": (
        (
            "M243-C017-SPC-01",
            "lowering/runtime diagnostics surfacing integration closeout and gate sign-off shall preserve",
        ),
        ("M243-C017-SPC-02", "lane-C dependency anchors (`M243-C016`)"),
    ),
    "metadata_spec": (
        (
            "M243-C017-META-01",
            "deterministic lane-C lowering/runtime diagnostics surfacing integration closeout and gate sign-off metadata anchors for `M243-C017` with explicit",
        ),
        ("M243-C017-META-02", "`M243-C016` dependency continuity"),
    ),
    "package_json": (
        (
            "M243-C017-PKG-01",
            '"check:objc3c:m243-c017-lowering-runtime-diagnostics-surfacing-integration-closeout-gate-sign-off-contract"',
        ),
        (
            "M243-C017-PKG-02",
            '"test:tooling:m243-c017-lowering-runtime-diagnostics-surfacing-integration-closeout-gate-sign-off-contract"',
        ),
        ("M243-C017-PKG-03", '"check:objc3c:m243-c017-lane-c-readiness"'),
        (
            "M243-C017-PKG-04",
            "npm run check:objc3c:m243-c016-lane-c-readiness && npm run check:objc3c:m243-c017-lowering-runtime-diagnostics-surfacing-integration-closeout-gate-sign-off-contract && npm run test:tooling:m243-c017-lowering-runtime-diagnostics-surfacing-integration-closeout-gate-sign-off-contract",
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
            "tmp/reports/m243/M243-C017/lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract_summary.json"
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






