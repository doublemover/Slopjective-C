#!/usr/bin/env python3
"""Fail-closed validator for M227-A014 semantic pass release replay dry-run contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-semantic-pass-release-replay-dry-run-contract-a014-v1"

ARTIFACTS: dict[str, Path] = {
    "a013_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_semantic_pass_docs_operator_runbook_sync_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_semantic_pass_release_candidate_replay_dry_run_expectations.md",
    "run_script": ROOT / "scripts" / "run_m227_a014_semantic_pass_release_replay_dry_run.ps1",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_a014_semantic_pass_release_candidate_replay_dry_run_packet.md",
    "runbook": ROOT / "docs" / "runbooks" / "m227_wave_execution_runbook.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "a013_contract_doc": (
        (
            "M227-A014-DEP-01",
            "Contract ID: `objc3c-semantic-pass-docs-operator-runbook-sync/m227-a013-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M227-A014-DOC-01",
            "Contract ID: `objc3c-semantic-pass-release-replay-dry-run/m227-a014-v1`",
        ),
        ("M227-A014-DOC-02", "scripts/run_m227_a014_semantic_pass_release_replay_dry_run.ps1"),
        ("M227-A014-DOC-03", "Dependencies: `M227-A013`"),
        ("M227-A014-DOC-04", "tmp/reports/m227/M227-A014/replay_dry_run_summary.json"),
        (
            "M227-A014-DOC-05",
            "scripts/check_m227_a014_semantic_pass_release_candidate_replay_dry_run_contract.py",
        ),
    ),
    "run_script": (
        ("M227-A014-RUN-01", '"module.manifest.json"'),
        ("M227-A014-RUN-02", '"module.diagnostics.json"'),
        ("M227-A014-RUN-03", '"module.ll"'),
        ("M227-A014-RUN-04", '"module.object-backend.txt"'),
        ("M227-A014-RUN-05", "parserStage.deterministic_handoff"),
        ("M227-A014-RUN-06", "parserStage.recovery_replay_ready"),
        ("M227-A014-RUN-07", "readiness.ready_for_lowering"),
        ("M227-A014-RUN-08", "readiness.parse_artifact_replay_key_deterministic"),
        ("M227-A014-RUN-09", "objc3c-semantic-pass-release-replay-dry-run/m227-a014-v1"),
        ("M227-A014-RUN-10", "replay_dry_run_summary.json"),
        (
            "M227-A014-RUN-11",
            "manifest parse_lowering_readiness.parse_artifact_replay_key_deterministic is false",
        ),
    ),
    "packet_doc": (
        ("M227-A014-PKT-01", "Packet: `M227-A014`"),
        ("M227-A014-PKT-02", "Dependencies: `M227-A013`"),
        (
            "M227-A014-PKT-03",
            "scripts/check_m227_a014_semantic_pass_release_candidate_replay_dry_run_contract.py",
        ),
        ("M227-A014-PKT-04", "scripts/run_m227_a014_semantic_pass_release_replay_dry_run.ps1"),
        ("M227-A014-PKT-05", "tmp/reports/m227/M227-A014/replay_dry_run_summary.json"),
    ),
    "runbook": (
        (
            "M227-A014-RBK-01",
            "objc3c-semantic-pass-release-replay-dry-run/m227-a014-v1",
        ),
        (
            "M227-A014-RBK-02",
            "python scripts/check_m227_a014_semantic_pass_release_candidate_replay_dry_run_contract.py",
        ),
        (
            "M227-A014-RBK-03",
            "python -m pytest tests/tooling/test_check_m227_a014_semantic_pass_release_candidate_replay_dry_run_contract.py -q",
        ),
        (
            "M227-A014-RBK-04",
            "powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m227_a014_semantic_pass_release_replay_dry_run.ps1",
        ),
        ("M227-A014-RBK-05", "npm run check:objc3c:m227-a014-lane-a-readiness"),
    ),
    "package_json": (
        (
            "M227-A014-CFG-01",
            '"check:objc3c:m227-a014-semantic-pass-release-candidate-replay-dry-run-contract"',
        ),
        (
            "M227-A014-CFG-02",
            '"test:tooling:m227-a014-semantic-pass-release-candidate-replay-dry-run-contract"',
        ),
        (
            "M227-A014-CFG-03",
            '"run:objc3c:m227-a014-semantic-pass-release-replay-dry-run"',
        ),
        ("M227-A014-CFG-04", '"check:objc3c:m227-a014-lane-a-readiness"'),
        ("M227-A014-CFG-05", "npm run test:objc3c:execution-replay-proof"),
    ),
    "architecture_doc": (
        (
            "M227-A014-ARC-01",
            "M227 lane-A A014 release-candidate replay dry-run anchors deterministic",
        ),
        ("M227-A014-ARC-02", "run_m227_a014_semantic_pass_release_replay_dry_run.ps1"),
    ),
    "lowering_spec": (
        (
            "M227-A014-SPC-01",
            "semantic-pass release-candidate and replay dry-run governance shall preserve explicit lane-A dependency anchor (`M227-A014`)",
        ),
    ),
    "metadata_spec": (
        (
            "M227-A014-META-01",
            "deterministic lane-A semantic-pass release-candidate replay dry-run metadata anchors for `M227-A014`",
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
        default=Path("tmp/reports/m227/M227-A014/release_replay_dry_run_contract_summary.json"),
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
