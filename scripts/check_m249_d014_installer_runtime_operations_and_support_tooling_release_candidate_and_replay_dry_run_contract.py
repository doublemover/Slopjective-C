#!/usr/bin/env python3
"""Fail-closed checker for M249-D014 installer/runtime release replay dry-run."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m249-d014-installer-runtime-operations-support-tooling-release-candidate-replay-dry-run-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m249_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_d014_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_d014_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_packet.md",
    "d013_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m249_installer_runtime_operations_and_support_tooling_docs_and_operator_runbook_synchronization_d013_expectations.md",
    "d013_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_d013_installer_runtime_operations_and_support_tooling_docs_and_operator_runbook_synchronization_packet.md",
    "d013_checker": ROOT
    / "scripts"
    / "check_m249_d013_installer_runtime_operations_and_support_tooling_docs_and_operator_runbook_synchronization_contract.py",
    "d013_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m249_d013_installer_runtime_operations_and_support_tooling_docs_and_operator_runbook_synchronization_contract.py",
    "d013_readiness_runner": ROOT / "scripts" / "run_m249_d013_lane_d_readiness.py",
    "d014_run_script": ROOT
    / "scripts"
    / "run_m249_d014_installer_runtime_operations_and_support_tooling_release_replay_dry_run.ps1",
    "d014_readiness_runner": ROOT / "scripts" / "run_m249_d014_lane_d_readiness.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M249-D014-DOC-01",
            "Contract ID: `objc3c-installer-runtime-operations-support-tooling-release-candidate-replay-dry-run/m249-d014-v1`",
        ),
        ("M249-D014-DOC-02", "Dependencies: `M249-D013`"),
        ("M249-D014-DOC-03", "Issue `#6941`"),
        ("M249-D014-DOC-04", "toolchain_runtime_ga_operations_docs_runbook_sync_consistent"),
        ("M249-D014-DOC-05", "toolchain_runtime_ga_operations_docs_runbook_sync_ready"),
        ("M249-D014-DOC-06", "long_tail_grammar_integration_closeout_consistent"),
        ("M249-D014-DOC-07", "long_tail_grammar_gate_signoff_ready"),
        (
            "M249-D014-DOC-08",
            "scripts/run_m249_d014_installer_runtime_operations_and_support_tooling_release_replay_dry_run.ps1",
        ),
        ("M249-D014-DOC-09", "npm run check:objc3c:m249-d014-lane-d-readiness"),
        (
            "M249-D014-DOC-10",
            "tmp/reports/m249/M249-D014/replay_dry_run_summary.json",
        ),
        (
            "M249-D014-DOC-11",
            "tmp/reports/m249/M249-D014/installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M249-D014-PKT-01", "Packet: `M249-D014`"),
        ("M249-D014-PKT-02", "Issue: `#6941`"),
        ("M249-D014-PKT-03", "Dependencies"),
        ("M249-D014-PKT-04", "`M249-D013`"),
        (
            "M249-D014-PKT-05",
            "`scripts/check_m249_d014_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_contract.py`",
        ),
        (
            "M249-D014-PKT-06",
            "`tests/tooling/test_check_m249_d014_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_contract.py`",
        ),
        (
            "M249-D014-PKT-07",
            "`scripts/run_m249_d014_installer_runtime_operations_and_support_tooling_release_replay_dry_run.ps1`",
        ),
        ("M249-D014-PKT-08", "`check:objc3c:m249-d014-lane-d-readiness`"),
        (
            "M249-D014-PKT-09",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
    ),
    "d013_expectations_doc": (
        (
            "M249-D014-DEP-01",
            "Contract ID: `objc3c-installer-runtime-operations-support-tooling-docs-and-operator-runbook-synchronization/m249-d013-v1`",
        ),
    ),
    "d013_packet_doc": (
        ("M249-D014-DEP-02", "Packet: `M249-D013`"),
        ("M249-D014-DEP-03", "Dependencies"),
        ("M249-D014-DEP-04", "`M249-D012`"),
    ),
    "d013_checker": (
        (
            "M249-D014-DEP-05",
            "m249-d013-installer-runtime-operations-support-tooling-",
        ),
    ),
    "d013_tooling_test": (
        (
            "M249-D014-DEP-06",
            "check_m249_d013_installer_runtime_operations_and_support_tooling_docs_and_operator_runbook_synchronization_contract",
        ),
    ),
    "d013_readiness_runner": (
        (
            "M249-D014-DEP-07",
            "scripts/check_m249_d013_installer_runtime_operations_and_support_tooling_docs_and_operator_runbook_synchronization_contract.py",
        ),
    ),
    "d014_run_script": (
        ("M249-D014-RUN-01", '"module.manifest.json"'),
        ("M249-D014-RUN-02", '"module.diagnostics.json"'),
        ("M249-D014-RUN-03", '"module.ll"'),
        ("M249-D014-RUN-04", '"module.object-backend.txt"'),
        ("M249-D014-RUN-05", "readiness.toolchain_runtime_ga_operations_docs_runbook_sync_consistent"),
        ("M249-D014-RUN-06", "readiness.toolchain_runtime_ga_operations_docs_runbook_sync_ready"),
        ("M249-D014-RUN-07", "readiness.long_tail_grammar_integration_closeout_consistent"),
        ("M249-D014-RUN-08", "readiness.long_tail_grammar_gate_signoff_ready"),
        ("M249-D014-RUN-09", "toolchain_runtime_ga_operations_docs_runbook_sync_key="),
        (
            "M249-D014-RUN-10",
            "objc3c-installer-runtime-operations-support-tooling-release-candidate-replay-dry-run/m249-d014-v1",
        ),
        ("M249-D014-RUN-11", "replay_dry_run_summary.json"),
    ),
    "d014_readiness_runner": (
        (
            "M249-D014-DEP-08",
            "scripts/run_m249_d013_lane_d_readiness.py",
        ),
        (
            "M249-D014-DEP-09",
            "scripts/check_m249_d014_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_contract.py",
        ),
        (
            "M249-D014-DEP-10",
            "tests/tooling/test_check_m249_d014_installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_contract.py",
        ),
        (
            "M249-D014-DEP-11",
            "scripts/run_m249_d014_installer_runtime_operations_and_support_tooling_release_replay_dry_run.ps1",
        ),
    ),
    "architecture_doc": (
        (
            "M249-D014-ARC-01",
            "M249 lane-D D014 release-candidate replay dry-run anchors installer/runtime operations and support tooling contract integration",
        ),
    ),
    "lowering_spec": (
        (
            "M249-D014-SPC-01",
            "installer/runtime operations and support tooling release-candidate replay dry-run governance shall preserve",
        ),
    ),
    "metadata_spec": (
        (
            "M249-D014-META-01",
            "deterministic lane-D installer/runtime operations and support tooling release-candidate replay dry-run metadata anchors for `M249-D014`",
        ),
    ),
    "package_json": (
        (
            "M249-D014-PKG-01",
            '"check:objc3c:m249-d014-installer-runtime-operations-support-tooling-release-candidate-replay-dry-run-contract"',
        ),
        (
            "M249-D014-PKG-02",
            '"test:tooling:m249-d014-installer-runtime-operations-support-tooling-release-candidate-replay-dry-run-contract"',
        ),
        (
            "M249-D014-PKG-03",
            '"run:objc3c:m249-d014-installer-runtime-operations-support-tooling-release-replay-dry-run"',
        ),
        ("M249-D014-PKG-04", '"check:objc3c:m249-d014-lane-d-readiness"'),
        (
            "M249-D014-PKG-05",
            "python scripts/run_m249_d014_lane_d_readiness.py",
        ),
        ("M249-D014-PKG-06", '"compile:objc3c"'),
        ("M249-D014-PKG-07", '"proof:objc3c"'),
        ("M249-D014-PKG-08", '"test:objc3c:execution-replay-proof"'),
        ("M249-D014-PKG-09", '"test:objc3c:perf-budget"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M249-D014-FORB-01",
            '"check:objc3c:m249-d014-lane-d-readiness": "npm run check:objc3c:m249-d013-lane-d-readiness',
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
            "tmp/reports/m249/M249-D014/installer_runtime_operations_and_support_tooling_release_candidate_and_replay_dry_run_contract_summary.json"
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
        raise FileNotFoundError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        total_checks += 1
        try:
            text = load_text(path, artifact=artifact)
            passed_checks += 1
        except FileNotFoundError as exc:
            findings.append(Finding(artifact, f"M249-D014-MISSING-{artifact.upper()}", str(exc)))
            continue

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
