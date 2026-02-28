#!/usr/bin/env python3
"""Fail-closed contract validator for M135 direct-LLVM closeout packet wiring."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m135-direct-llvm-contract-v1"

DEFAULT_M135_DIR = ROOT / "spec" / "planning" / "compiler" / "m135"
DEFAULT_DISPATCH_PLAN = "m135_parallel_dispatch_plan_20260228.md"
DEFAULT_ISSUE_PACKETS = "m135_issue_packets_20260228.md"
DEFAULT_CLOSEOUT_PACKET = "m135_closeout_evidence_20260228.md"
DEFAULT_DIRECT_LLVM_CONTRACT_PATH = ROOT / "docs" / "contracts" / "direct_llvm_emission_expectations.md"
DEFAULT_PLANNING_LINT_WORKFLOW = ROOT / ".github" / "workflows" / "planning-lint.yml"
DEFAULT_TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
PACKAGE_SCRIPT_NAME = "check:compiler-closeout:m135"

ARTIFACT_ORDER = (
    "dispatch_plan",
    "issue_packets",
    "closeout_packet",
    "direct_llvm_contract",
    "planning_lint_workflow",
    "task_hygiene_workflow",
    "package_json",
)
ARTIFACT_RANK = {artifact: index for index, artifact in enumerate(ARTIFACT_ORDER)}


@dataclass(frozen=True)
class SnippetRule:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class DriftFinding:
    artifact: str
    check_id: str
    detail: str


REQUIRED_SNIPPETS: dict[str, tuple[SnippetRule, ...]] = {
    "dispatch_plan": (
        SnippetRule("M135-DSP-01", "# M135 Parallel Dispatch Plan (2026-02-28)"),
        SnippetRule("M135-DSP-02", "Final closeout sequence for M135: `A -> B -> C -> D -> E`."),
        SnippetRule("M135-DSP-03", "`npm run test:objc3c:lane-e`"),
    ),
    "issue_packets": (
        SnippetRule("M135-PKT-01", "# M135 Issue Packets (2026-02-28)"),
        SnippetRule("M135-PKT-02", "`M135-A001` -> `#4264`"),
        SnippetRule("M135-PKT-03", "`M135-B001` -> `#4265`"),
        SnippetRule("M135-PKT-04", "`M135-C001` -> `#4266`"),
        SnippetRule("M135-PKT-05", "`M135-D001` -> `#4267`"),
        SnippetRule("M135-PKT-06", "`M135-E001` -> `#4268`"),
    ),
    "closeout_packet": (
        SnippetRule("M135-CLO-01", "# M135 Closeout Evidence Packet (Draft, 2026-02-28)"),
        SnippetRule("M135-CLO-02", "Gate issue: [#4268](https://github.com/doublemover/Slopjective-C/issues/4268)"),
        SnippetRule("M135-CLO-03", "`npm run check:compiler-closeout:m135`"),
        SnippetRule(
            "M135-CLO-04",
            "Closeout remains blocked until dependency issues `#4264` through `#4267` are closed",
        ),
    ),
    "direct_llvm_contract": (
        SnippetRule("M135-CTR-01", "# Direct LLVM Emission Expectations (M135)"),
        SnippetRule("M135-CTR-02", "Contract ID: `objc3c-direct-llvm-emission/m135-v1`"),
        SnippetRule(
            "M135-CTR-03",
            "Fallback path `RunIRCompile(...clang -x ir...)` is forbidden for `.objc3` production emission.",
        ),
        SnippetRule("M135-CTR-04", "`python scripts/check_m135_direct_llvm_contract.py`"),
    ),
    "planning_lint_workflow": (
        SnippetRule("M135-CI-PLN-01", "python scripts/check_m135_direct_llvm_contract.py"),
    ),
    "task_hygiene_workflow": (
        SnippetRule("M135-CI-TH-01", "npm run check:compiler-closeout:m135"),
    ),
}

PACKAGE_SCRIPT_REQUIRED_TOKENS: tuple[tuple[str, str], ...] = (
    (
        "M135-PKG-02",
        "python scripts/check_m135_direct_llvm_contract.py",
    ),
    (
        "M135-PKG-03",
        "python scripts/spec_lint.py --glob \"spec/planning/compiler/m135/*.md\"",
    ),
    (
        "M135-PKG-04",
        "--glob \"docs/contracts/direct_llvm_emission_expectations.md\"",
    ),
)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def resolve_input_path(path: Path) -> Path:
    if path.is_absolute():
        return path
    return ROOT / path


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists():
        raise ValueError(f"{artifact} file does not exist: {display_path(path)}")
    if not path.is_file():
        raise ValueError(f"{artifact} path is not a file: {display_path(path)}")
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise ValueError(f"{artifact} file is not valid UTF-8: {display_path(path)}") from exc
    except OSError as exc:
        raise ValueError(f"unable to read {artifact} file {display_path(path)}: {exc}") from exc


def load_json(path: Path, *, artifact: str) -> dict[str, object]:
    text = load_text(path, artifact=artifact)
    try:
        payload = json.loads(text)
    except json.JSONDecodeError as exc:
        raise ValueError(f"{artifact} parse error ({display_path(path)}): {exc}") from exc
    if not isinstance(payload, dict):
        raise ValueError(f"{artifact} root must be an object: {display_path(path)}")
    return payload


def finding_sort_key(finding: DriftFinding) -> tuple[int, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in drift finding: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id)


def collect_snippet_findings(*, artifact: str, content: str) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    for rule in REQUIRED_SNIPPETS[artifact]:
        if rule.snippet not in content:
            findings.append(
                DriftFinding(
                    artifact=artifact,
                    check_id=rule.check_id,
                    detail=f"expected snippet: {rule.snippet}",
                )
            )
    return findings


def collect_package_findings(*, package_path: Path) -> list[DriftFinding]:
    payload = load_json(package_path, artifact="package_json")
    scripts = payload.get("scripts")
    findings: list[DriftFinding] = []

    if not isinstance(scripts, dict):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M135-PKG-00",
                detail="package.json field 'scripts' must be an object.",
            )
        )
        return findings

    script_command = scripts.get(PACKAGE_SCRIPT_NAME)
    if not isinstance(script_command, str):
        findings.append(
            DriftFinding(
                artifact="package_json",
                check_id="M135-PKG-01",
                detail=f"missing scripts entry '{PACKAGE_SCRIPT_NAME}'.",
            )
        )
        return findings

    for check_id, token in PACKAGE_SCRIPT_REQUIRED_TOKENS:
        if token not in script_command:
            findings.append(
                DriftFinding(
                    artifact="package_json",
                    check_id=check_id,
                    detail=f"scripts['{PACKAGE_SCRIPT_NAME}'] missing token: {token}",
                )
            )
    return findings


def validate_contract(
    *,
    dispatch_plan_content: str,
    issue_packets_content: str,
    closeout_packet_content: str,
    direct_llvm_contract_content: str,
    planning_lint_workflow_content: str,
    task_hygiene_workflow_content: str,
    package_json_path: Path,
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    findings.extend(
        collect_snippet_findings(artifact="dispatch_plan", content=dispatch_plan_content)
    )
    findings.extend(
        collect_snippet_findings(artifact="issue_packets", content=issue_packets_content)
    )
    findings.extend(
        collect_snippet_findings(artifact="closeout_packet", content=closeout_packet_content)
    )
    findings.extend(
        collect_snippet_findings(
            artifact="direct_llvm_contract",
            content=direct_llvm_contract_content,
        )
    )
    findings.extend(
        collect_snippet_findings(
            artifact="planning_lint_workflow",
            content=planning_lint_workflow_content,
        )
    )
    findings.extend(
        collect_snippet_findings(
            artifact="task_hygiene_workflow",
            content=task_hygiene_workflow_content,
        )
    )
    findings.extend(collect_package_findings(package_path=package_json_path))
    return sorted(findings, key=finding_sort_key)


def render_drift_report(*, findings: list[DriftFinding], rerun_tokens: list[str]) -> str:
    ordered = sorted(findings, key=finding_sort_key)
    lines = [
        "m135-direct-llvm-contract: contract drift detected "
        f"({len(ordered)} failed check(s)).",
        "drift findings:",
    ]
    for finding in ordered:
        lines.append(f"- {finding.artifact}:{finding.check_id}")
        lines.append(f"  {finding.detail}")
    lines.extend(
        [
            "remediation:",
            "1. Restore M135 packet/docs/CI contract snippets and package-script wiring.",
            "2. Re-run validator:",
            " ".join(rerun_tokens),
        ]
    )
    return "\n".join(lines)


def check_contract(
    *,
    dispatch_plan_path: Path,
    issue_packets_path: Path,
    closeout_packet_path: Path,
    direct_llvm_contract_path: Path,
    planning_lint_workflow_path: Path,
    task_hygiene_workflow_path: Path,
    package_json_path: Path,
) -> int:
    dispatch_plan_content = load_text(dispatch_plan_path, artifact="dispatch_plan")
    issue_packets_content = load_text(issue_packets_path, artifact="issue_packets")
    closeout_packet_content = load_text(closeout_packet_path, artifact="closeout_packet")
    direct_llvm_contract_content = load_text(direct_llvm_contract_path, artifact="direct_llvm_contract")
    planning_lint_workflow_content = load_text(planning_lint_workflow_path, artifact="planning_lint_workflow")
    task_hygiene_workflow_content = load_text(task_hygiene_workflow_path, artifact="task_hygiene_workflow")

    findings = validate_contract(
        dispatch_plan_content=dispatch_plan_content,
        issue_packets_content=issue_packets_content,
        closeout_packet_content=closeout_packet_content,
        direct_llvm_contract_content=direct_llvm_contract_content,
        planning_lint_workflow_content=planning_lint_workflow_content,
        task_hygiene_workflow_content=task_hygiene_workflow_content,
        package_json_path=package_json_path,
    )
    if findings:
        rerun_tokens = [
            "python",
            "scripts/check_m135_direct_llvm_contract.py",
            "--m135-dir",
            display_path(dispatch_plan_path.parent),
            "--direct-llvm-contract",
            display_path(direct_llvm_contract_path),
            "--planning-lint-workflow",
            display_path(planning_lint_workflow_path),
            "--task-hygiene-workflow",
            display_path(task_hygiene_workflow_path),
            "--package-json",
            display_path(package_json_path),
        ]
        print(
            render_drift_report(findings=findings, rerun_tokens=rerun_tokens),
            file=sys.stderr,
        )
        return 1

    checks_passed = sum(len(rules) for rules in REQUIRED_SNIPPETS.values()) + 1 + len(
        PACKAGE_SCRIPT_REQUIRED_TOKENS
    )
    print("m135-direct-llvm-contract: OK")
    print(f"- mode={MODE}")
    print(f"- dispatch_plan={display_path(dispatch_plan_path)}")
    print(f"- issue_packets={display_path(issue_packets_path)}")
    print(f"- closeout_packet={display_path(closeout_packet_path)}")
    print(f"- direct_llvm_contract={display_path(direct_llvm_contract_path)}")
    print(f"- planning_lint_workflow={display_path(planning_lint_workflow_path)}")
    print(f"- task_hygiene_workflow={display_path(task_hygiene_workflow_path)}")
    print(f"- package_json={display_path(package_json_path)}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_m135_direct_llvm_contract.py",
        description=(
            "Fail-closed validator for M135 planning packet docs, direct-LLVM contract docs, "
            "and CI/tooling wiring."
        ),
    )
    parser.add_argument(
        "--m135-dir",
        type=Path,
        default=DEFAULT_M135_DIR,
        help="Path to spec/planning/compiler/m135 directory.",
    )
    parser.add_argument(
        "--direct-llvm-contract",
        type=Path,
        default=DEFAULT_DIRECT_LLVM_CONTRACT_PATH,
        help="Path to docs/contracts/direct_llvm_emission_expectations.md.",
    )
    parser.add_argument(
        "--planning-lint-workflow",
        type=Path,
        default=DEFAULT_PLANNING_LINT_WORKFLOW,
        help="Path to .github/workflows/planning-lint.yml.",
    )
    parser.add_argument(
        "--task-hygiene-workflow",
        type=Path,
        default=DEFAULT_TASK_HYGIENE_WORKFLOW,
        help="Path to .github/workflows/task-hygiene.yml.",
    )
    parser.add_argument(
        "--package-json",
        type=Path,
        default=DEFAULT_PACKAGE_JSON,
        help="Path to package.json.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    m135_dir = resolve_input_path(args.m135_dir)
    direct_llvm_contract_path = resolve_input_path(args.direct_llvm_contract)
    planning_lint_workflow_path = resolve_input_path(args.planning_lint_workflow)
    task_hygiene_workflow_path = resolve_input_path(args.task_hygiene_workflow)
    package_json_path = resolve_input_path(args.package_json)

    dispatch_plan_path = m135_dir / DEFAULT_DISPATCH_PLAN
    issue_packets_path = m135_dir / DEFAULT_ISSUE_PACKETS
    closeout_packet_path = m135_dir / DEFAULT_CLOSEOUT_PACKET

    try:
        return check_contract(
            dispatch_plan_path=dispatch_plan_path,
            issue_packets_path=issue_packets_path,
            closeout_packet_path=closeout_packet_path,
            direct_llvm_contract_path=direct_llvm_contract_path,
            planning_lint_workflow_path=planning_lint_workflow_path,
            task_hygiene_workflow_path=task_hygiene_workflow_path,
            package_json_path=package_json_path,
        )
    except ValueError as exc:
        print(f"m135-direct-llvm-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
