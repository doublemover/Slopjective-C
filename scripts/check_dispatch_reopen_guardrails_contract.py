#!/usr/bin/env python3
"""Validate fail-closed contract integrity for v0.14 M17 dispatch-reopen guardrails."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "dispatch-reopen-guardrails-contract-v2"
ARTIFACT_ORDER = ("playbook", "dispatch_review", "batch_manifest")

DEFAULT_PLAYBOOK_PATH = ROOT / "spec" / "planning" / "v013_activation_reopen_playbook_20260223.md"
DEFAULT_DISPATCH_REVIEW_PATH = (
    ROOT / "spec" / "planning" / "v014_dispatch_reopen_guardrails_w1_dispatch_review_20260224.md"
)
DEFAULT_BATCH_MANIFEST_PATH = (
    ROOT / "spec" / "planning" / "v014_dispatch_reopen_guardrails_w1_batch_manifest_20260224.md"
)

EXPECTED_PREFLIGHT_RUN_ORDER: tuple[str, ...] = (
    "AR-RUN-01",
    "AR-RUN-08",
    "AR-RUN-09",
    "AR-RUN-02",
    "AR-RUN-03",
    "AR-RUN-04",
    "AR-RUN-05",
    "AR-RUN-06",
    "AR-RUN-07",
)
EXPECTED_FRESHNESS_PROVENANCE_RUN_SEQUENCE: tuple[str, ...] = (
    "AR-RUN-08",
    "AR-RUN-09",
    "AR-RUN-02",
)
EXPECTED_GATE_IDS: tuple[str, ...] = (
    "G0-TRIGGER",
    "G1-PREFLIGHT",
    "G2-OWNERSHIP",
    "G3-DEPENDENCY",
    "G4-SEQUENCER",
    "G5-SEED-PARITY",
    "G6-CANONICAL-PAYLOAD-PUBLICATION",
    "G7-SNAPSHOT-REFRESH",
    "G8-SNAPSHOT-FRESHNESS-EVIDENCE",
)
EXPECTED_W2_GATE_SEQUENCE: tuple[str, ...] = (
    "AR-DRW1-G0-TRIGGER",
    "AR-DRW1-G1-EVIDENCE-LINKAGE",
    "AR-DRW1-G2-ACCEPTANCE-MATRIX",
    "AR-DRW1-G3-LANE-EVIDENCE",
    "AR-DRW1-G4-SPEC-LINT",
    "AR-DRW1-S1-T4-PROVENANCE",
    "AR-DRW1-S2-INTAKE-ORDER",
)
EXPECTED_PROVENANCE_EVIDENCE_IDS: tuple[str, ...] = (
    "AR-CMD-08",
    "AR-CMD-09",
    "AR-SNAPSHOT-01",
    "AR-FRESHNESS-01",
    "AR-DECISION-01",
)
EXPECTED_REVIEW_ISSUE_ROWS: tuple[tuple[str, str], ...] = (
    ("#904", "A"),
    ("#905", "B"),
    ("#906", "C"),
    ("#907", "D"),
    ("#908", "INT"),
)
EXPECTED_REVIEW_CLOSEOUT_ORDER: tuple[str, ...] = ("A", "B", "C", "D", "INT")
EXPECTED_MANIFEST_LANE_ROWS: tuple[tuple[str, str], ...] = (
    ("A", "#904"),
    ("B", "#905"),
    ("C", "#906"),
    ("D", "#907"),
    ("INT", "#908"),
)
EXPECTED_MANIFEST_INTAKE_ORDER: tuple[tuple[str, str], ...] = (
    ("A", "#904"),
    ("B", "#905"),
    ("C", "#906"),
    ("D", "#907"),
    ("INT", "#908"),
)
W2_DYNAMIC_CHECK_IDS: tuple[str, ...] = (
    "PBK-10",
    "PBK-11",
    "PBK-17",
    "PBK-18",
    "PBK-19",
    "REV-09",
    "REV-12",
    "MAN-08",
    "MAN-12",
)


@dataclass(frozen=True)
class SnippetRule:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class DriftFinding:
    artifact: str
    check_id: str
    detail: str


REQUIRED_RULES: dict[str, tuple[SnippetRule, ...]] = {
    "playbook": (
        SnippetRule("PBK-01", "_Deterministic dispatch-reopen contract for issue `#778`._"),
        SnippetRule("PBK-02", "## 3. Gate-Open Criteria (Canonical Reduction)"),
        SnippetRule("PBK-03", "3. `gate_open = tooling_activation OR t4_new_scope_publish`"),
        SnippetRule("PBK-04", "## 4. Runner-Based Preflight Contract (`AR-PREFLIGHT-RUNNER-V1`)"),
        SnippetRule("PBK-05", "1. `AR-RUN-01`"),
        SnippetRule("PBK-06", "9. `AR-RUN-07`"),
        SnippetRule(
            "PBK-07",
            "| `AR-DEP-M03-01` | `Hard` | Required runner chain is `AR-RUN-01`, "
            "`AR-RUN-08`, `AR-RUN-09`, `AR-RUN-02`, `AR-RUN-03`, `AR-RUN-04`, "
            "`AR-RUN-05`, `AR-RUN-06`, `AR-RUN-07` in exactly that order. |",
        ),
        SnippetRule(
            "PBK-08",
            "This playbook does not create tasks, issues, or milestones. It defines only",
        ),
        SnippetRule(
            "PBK-09",
            "- If any `Hard` gate in Section 6 is `FAIL`, reopen state is `BLOCKED`.",
        ),
        SnippetRule(
            "PBK-12",
            "| `AR-DEP-M03-02` | `Hard` | Snapshot refresh (`AR-RUN-08`) and freshness "
            "(`AR-RUN-09`) evidence must be linked to the same reopen decision cycle as "
            "`AR-RUN-02` and `AR-DECISION-01`. |",
        ),
        SnippetRule(
            "PBK-13",
            "| `AR-DRW1-G1-EVIDENCE-LINKAGE` | `Hard` | `AR-DEP-M17-02` |",
        ),
        SnippetRule(
            "PBK-14",
            "| `AR-DRW1-S1-T4-PROVENANCE` | `Soft` | `AR-DEP-M17-04` |",
        ),
        SnippetRule(
            "PBK-15",
            "1. If any `Hard` gate (`AR-DRW1-G0`..`AR-DRW1-G4`) is `FAIL`,",
        ),
        SnippetRule(
            "PBK-16",
            "3. If `AR-DRW1-S1-T4-PROVENANCE` fails in a `T4`-only cycle,",
        ),
    ),
    "dispatch_review": (
        SnippetRule("REV-01", "## 1. Scope"),
        SnippetRule("REV-02", "Reviewed sources:"),
        SnippetRule(
            "REV-03",
            "- `gh issue view 905 --json number,title,body,milestone,url`",
        ),
        SnippetRule("REV-04", "## 3. M17 Task Set"),
        SnippetRule(
            "REV-05",
            "| `#905` | B | Implement tooling and deterministic test coverage |",
        ),
        SnippetRule("REV-06", "Mandatory source inputs for all five issues:"),
        SnippetRule("REV-07", "## 4. Parallelization Decision"),
        SnippetRule(
            "REV-08",
            "1. Execute lanes `A/B/C/D` in parallel with strict file ownership and no "
            "overlapping write paths.",
        ),
        SnippetRule(
            "REV-10",
            "3. Serialize GH closeout at terminal stage as `A -> B -> C -> D -> INT`, "
            "then close milestone.",
        ),
        SnippetRule("REV-11", "## 5. Dispatch Decision"),
        SnippetRule(
            "REV-13",
            "2. Lane `B` (`#905`): `019c90a2-bfc8-7801-ac60-2137d732fd46`",
        ),
    ),
    "batch_manifest": (
        SnippetRule("MAN-01", "- `batch_id`: `BATCH-20260224-M17`"),
        SnippetRule(
            "MAN-02",
            "- Scope class: dispatch-reopen baseline contracts + deterministic "
            "tooling/tests + CI/operator parity + governance/runbook controls",
        ),
        SnippetRule(
            "MAN-03",
            "| `B` | `#905` | `019c90a2-bfc8-7801-ac60-2137d732fd46` | dispatch-reopen "
            "tooling/tests/fixtures + lane-B evidence |",
        ),
        SnippetRule(
            "MAN-04",
            "3. Lane `B` provides executable enforcement consumed by lane `C` entrypoints "
            "and integrator verification.",
        ),
        SnippetRule(
            "MAN-05",
            "2. Lane `B`: `python -m pytest tests/tooling -q`; "
            "`python scripts/spec_lint.py`",
        ),
        SnippetRule("MAN-06", "- Status: `CLOSED`"),
        SnippetRule("MAN-07", "- Lane `B` / issue `#905`:"),
        SnippetRule("MAN-09", "- Source-of-truth inputs:"),
        SnippetRule(
            "MAN-10",
            "- `spec/planning/v014_dispatch_reopen_guardrails_w1_dispatch_review_20260224.md`",
        ),
        SnippetRule(
            "MAN-11",
            "5. `npm run check:dispatch-reopen-guardrails:w1:strict`",
        ),
    ),
}

ARTIFACT_RANK = {artifact: index for index, artifact in enumerate(ARTIFACT_ORDER)}


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def resolve_input_path(raw_path: Path) -> Path:
    if raw_path.is_absolute():
        return raw_path
    return ROOT / raw_path


def shell_quote(token: str) -> str:
    if any(character.isspace() for character in token):
        return f'"{token}"'
    return token


def render_command(tokens: list[str]) -> str:
    return " ".join(shell_quote(token) for token in tokens)


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


def validate_rule_configuration() -> None:
    required = set(ARTIFACT_ORDER)
    provided = set(REQUIRED_RULES)
    missing = sorted(required - provided)
    if missing:
        raise ValueError(f"rule configuration missing artifact set(s): {', '.join(missing)}")
    unexpected = sorted(provided - required)
    if unexpected:
        raise ValueError(
            "rule configuration includes unexpected artifact set(s): "
            f"{', '.join(unexpected)}"
        )

    seen: set[str] = set()
    for artifact in ARTIFACT_ORDER:
        for rule in REQUIRED_RULES[artifact]:
            if rule.check_id in seen:
                raise ValueError(f"rule configuration has duplicate check id: {artifact}:{rule.check_id}")
            seen.add(rule.check_id)
    for check_id in W2_DYNAMIC_CHECK_IDS:
        if check_id in seen:
            raise ValueError(f"rule configuration has duplicate check id: dynamic:{check_id}")
        seen.add(check_id)


def finding_sort_key(finding: DriftFinding) -> tuple[int, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in drift finding: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id)


def format_value(value: object) -> str:
    return json.dumps(value, ensure_ascii=True)


def collect_snippet_findings(*, artifact: str, content: str) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    for rule in REQUIRED_RULES[artifact]:
        if rule.snippet not in content:
            findings.append(
                DriftFinding(
                    artifact=artifact,
                    check_id=rule.check_id,
                    detail=f"expected snippet: {rule.snippet}",
                )
            )
    return findings


def collect_dynamic_findings(
    *,
    playbook_content: str,
    dispatch_review_content: str,
    batch_manifest_content: str,
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []

    run_order = re.findall(r"^\d+\.\s+`(AR-RUN-[0-9]{2})`\s*$", playbook_content, re.MULTILINE)
    expected_run_order = list(EXPECTED_PREFLIGHT_RUN_ORDER)
    if run_order != expected_run_order:
        findings.append(
            DriftFinding(
                artifact="playbook",
                check_id="PBK-10",
                detail=(
                    "deterministic preflight runner order must be "
                    f"{format_value(expected_run_order)} (found {format_value(run_order)})"
                ),
            )
        )

    expected_freshness_sequence = list(EXPECTED_FRESHNESS_PROVENANCE_RUN_SEQUENCE)
    contiguous_freshness_sequence_present = any(
        run_order[index : index + len(expected_freshness_sequence)] == expected_freshness_sequence
        for index in range(max(0, len(run_order) - len(expected_freshness_sequence) + 1))
    )
    if not contiguous_freshness_sequence_present:
        findings.append(
            DriftFinding(
                artifact="playbook",
                check_id="PBK-17",
                detail=(
                    "freshness/provenance gate sequencing requires contiguous runner steps "
                    f"{format_value(expected_freshness_sequence)} "
                    f"(found {format_value(run_order)})"
                ),
            )
        )

    gate_ids = re.findall(r"^\|\s*`(G[0-9]-[A-Z0-9-]+)`\s*\|", playbook_content, re.MULTILINE)
    expected_gate_ids = list(EXPECTED_GATE_IDS)
    if gate_ids != expected_gate_ids:
        findings.append(
            DriftFinding(
                artifact="playbook",
                check_id="PBK-11",
                detail=(
                    "deterministic hard-gate row set must be "
                    f"{format_value(expected_gate_ids)} (found {format_value(gate_ids)})"
                ),
            )
        )

    w2_gate_sequence = re.findall(
        r"^\|\s*`(AR-DRW1-(?:G[0-9]-[A-Z0-9-]+|S[0-9]-[A-Z0-9-]+))`\s*\|",
        playbook_content,
        re.MULTILINE,
    )
    expected_w2_gate_sequence = list(EXPECTED_W2_GATE_SEQUENCE)
    if w2_gate_sequence != expected_w2_gate_sequence:
        findings.append(
            DriftFinding(
                artifact="playbook",
                check_id="PBK-18",
                detail=(
                    "deterministic W2 gate sequencing must be "
                    f"{format_value(expected_w2_gate_sequence)} "
                    f"(found {format_value(w2_gate_sequence)})"
                ),
            )
        )

    evidence_ids = re.findall(r"^\|\s*`(AR-[A-Z0-9-]+)`\s*\|", playbook_content, re.MULTILINE)
    expected_provenance_evidence_ids = list(EXPECTED_PROVENANCE_EVIDENCE_IDS)
    provenance_evidence_ids = [
        evidence_id
        for evidence_id in evidence_ids
        if evidence_id in EXPECTED_PROVENANCE_EVIDENCE_IDS
    ]
    if provenance_evidence_ids != expected_provenance_evidence_ids:
        findings.append(
            DriftFinding(
                artifact="playbook",
                check_id="PBK-19",
                detail=(
                    "freshness/provenance evidence row set must be "
                    f"{format_value(expected_provenance_evidence_ids)} "
                    f"(found {format_value(provenance_evidence_ids)})"
                ),
            )
        )

    issue_rows = re.findall(
        r"^\|\s*`(#\d+)`\s*\|\s*([A-Z]+)\s*\|",
        dispatch_review_content,
        re.MULTILINE,
    )
    expected_issue_rows = [list(row) for row in EXPECTED_REVIEW_ISSUE_ROWS]
    issue_rows_as_lists = [list(row) for row in issue_rows]
    if issue_rows_as_lists != expected_issue_rows:
        findings.append(
            DriftFinding(
                artifact="dispatch_review",
                check_id="REV-09",
                detail=(
                    "deterministic M17 issue/lane row set must be "
                    f"{format_value(expected_issue_rows)} "
                    f"(found {format_value(issue_rows_as_lists)})"
                ),
            )
        )

    closeout_match = re.search(
        r"Serialize GH closeout at terminal stage as "
        r"`([A-Z]+)\s*->\s*([A-Z]+)\s*->\s*([A-Z]+)\s*->\s*([A-Z]+)\s*->\s*([A-Z]+)`",
        dispatch_review_content,
    )
    closeout_order = list(closeout_match.groups()) if closeout_match else []
    expected_closeout_order = list(EXPECTED_REVIEW_CLOSEOUT_ORDER)
    if closeout_order != expected_closeout_order:
        findings.append(
            DriftFinding(
                artifact="dispatch_review",
                check_id="REV-12",
                detail=(
                    "deterministic GH closeout sequence must be "
                    f"{format_value(expected_closeout_order)} "
                    f"(found {format_value(closeout_order)})"
                ),
            )
        )

    lane_rows = re.findall(
        r"^\|\s*`([A-Z]+)`\s*\|\s*`(#\d+)`\s*\|",
        batch_manifest_content,
        re.MULTILINE,
    )
    expected_lane_rows = [list(row) for row in EXPECTED_MANIFEST_LANE_ROWS]
    lane_rows_as_lists = [list(row) for row in lane_rows]
    if lane_rows_as_lists != expected_lane_rows:
        findings.append(
            DriftFinding(
                artifact="batch_manifest",
                check_id="MAN-08",
                detail=(
                    "deterministic lane assignment row set must be "
                    f"{format_value(expected_lane_rows)} "
                    f"(found {format_value(lane_rows_as_lists)})"
                ),
            )
        )

    manifest_intake_rows: list[list[str]] = []
    for line in batch_manifest_content.splitlines():
        stripped = line.strip()
        lane_match = re.match(
            r"^\d+\.\s+Lane\s+`([A-Z]+)`\s+\(`(#\d+)`\)\s*$",
            stripped,
        )
        if lane_match is not None:
            manifest_intake_rows.append([lane_match.group(1), lane_match.group(2)])
            continue
        integrator_match = re.match(
            r"^\d+\.\s+Integrator\s+\(`(#\d+)`\)\s*$",
            stripped,
        )
        if integrator_match is not None:
            manifest_intake_rows.append(["INT", integrator_match.group(1)])

    expected_manifest_intake_rows = [list(row) for row in EXPECTED_MANIFEST_INTAKE_ORDER]
    if manifest_intake_rows != expected_manifest_intake_rows:
        findings.append(
            DriftFinding(
                artifact="batch_manifest",
                check_id="MAN-12",
                detail=(
                    "deterministic intake sequencing rows must be "
                    f"{format_value(expected_manifest_intake_rows)} "
                    f"(found {format_value(manifest_intake_rows)})"
                ),
            )
        )

    return findings


def validate_contract(
    *,
    playbook_content: str,
    dispatch_review_content: str,
    batch_manifest_content: str,
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    findings.extend(collect_snippet_findings(artifact="playbook", content=playbook_content))
    findings.extend(
        collect_snippet_findings(
            artifact="dispatch_review",
            content=dispatch_review_content,
        )
    )
    findings.extend(
        collect_snippet_findings(
            artifact="batch_manifest",
            content=batch_manifest_content,
        )
    )
    findings.extend(
        collect_dynamic_findings(
            playbook_content=playbook_content,
            dispatch_review_content=dispatch_review_content,
            batch_manifest_content=batch_manifest_content,
        )
    )
    return sorted(findings, key=finding_sort_key)


def render_drift_report(
    *,
    findings: list[DriftFinding],
    playbook_path: Path,
    dispatch_review_path: Path,
    batch_manifest_path: Path,
) -> str:
    ordered_findings = sorted(findings, key=finding_sort_key)
    rerun_command = render_command(
        [
            "python",
            "scripts/check_dispatch_reopen_guardrails_contract.py",
            "--playbook",
            display_path(playbook_path),
            "--dispatch-review",
            display_path(dispatch_review_path),
            "--batch-manifest",
            display_path(batch_manifest_path),
        ]
    )
    lines = [
        "dispatch-reopen-guardrails-contract: contract drift detected "
        f"({len(ordered_findings)} failed check(s)).",
        "drift findings:",
    ]
    for finding in ordered_findings:
        lines.append(f"- {finding.artifact}:{finding.check_id}")
        lines.append(f"  {finding.detail}")
    lines.extend(
        [
            "remediation:",
            "1. Restore missing baseline-control snippet anchors, freshness/provenance "
            "invariants, and deterministic gate-sequencing parity in the listed artifact(s).",
            "2. Re-run validator:",
            rerun_command,
        ]
    )
    return "\n".join(lines)


def check_contract(
    *,
    playbook_path: Path,
    dispatch_review_path: Path,
    batch_manifest_path: Path,
) -> int:
    validate_rule_configuration()
    playbook_content = load_text(playbook_path, artifact="playbook")
    dispatch_review_content = load_text(dispatch_review_path, artifact="dispatch_review")
    batch_manifest_content = load_text(batch_manifest_path, artifact="batch_manifest")

    findings = validate_contract(
        playbook_content=playbook_content,
        dispatch_review_content=dispatch_review_content,
        batch_manifest_content=batch_manifest_content,
    )
    if findings:
        print(
            render_drift_report(
                findings=findings,
                playbook_path=playbook_path,
                dispatch_review_path=dispatch_review_path,
                batch_manifest_path=batch_manifest_path,
            ),
            file=sys.stderr,
        )
        return 1

    checks_passed = sum(len(rules) for rules in REQUIRED_RULES.values()) + len(W2_DYNAMIC_CHECK_IDS)
    print("dispatch-reopen-guardrails-contract: OK")
    print(f"- mode={MODE}")
    print(f"- playbook={display_path(playbook_path)}")
    print(f"- dispatch_review={display_path(dispatch_review_path)}")
    print(f"- batch_manifest={display_path(batch_manifest_path)}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_dispatch_reopen_guardrails_contract.py",
        description=(
            "Fail-closed validator for deterministic v0.14 M17 dispatch-reopen "
            "W2 controls across playbook/review/batch-manifest artifacts."
        ),
    )
    parser.add_argument(
        "--playbook",
        type=Path,
        default=DEFAULT_PLAYBOOK_PATH,
        help="Path to spec/planning/v013_activation_reopen_playbook_20260223.md.",
    )
    parser.add_argument(
        "--dispatch-review",
        "--review",
        dest="dispatch_review",
        type=Path,
        default=DEFAULT_DISPATCH_REVIEW_PATH,
        help=(
            "Path to spec/planning/v014_dispatch_reopen_guardrails_w1_dispatch_review_20260224.md "
            "(legacy alias: --review)."
        ),
    )
    parser.add_argument(
        "--batch-manifest",
        "--manifest",
        dest="batch_manifest",
        type=Path,
        default=DEFAULT_BATCH_MANIFEST_PATH,
        help="Path to spec/planning/v014_dispatch_reopen_guardrails_w1_batch_manifest_20260224.md.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    playbook_path = resolve_input_path(args.playbook)
    dispatch_review_path = resolve_input_path(args.dispatch_review)
    batch_manifest_path = resolve_input_path(args.batch_manifest)

    try:
        return check_contract(
            playbook_path=playbook_path,
            dispatch_review_path=dispatch_review_path,
            batch_manifest_path=batch_manifest_path,
        )
    except ValueError as exc:
        print(f"dispatch-reopen-guardrails-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
