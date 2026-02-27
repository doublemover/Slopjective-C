#!/usr/bin/env python3
"""Validate fail-closed contract integrity for v0.14 M14 abstract-machine sync audit."""

from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "abstract-machine-sync-audit-contract-v2"
ARTIFACT_ORDER = ("package", "report", "signoff")

DEFAULT_PACKAGE_PATH = ROOT / "spec" / "planning" / "v013_abstract_machine_sync_audit_2026q2_package.md"
DEFAULT_REPORT_PATH = ROOT / "reports" / "spec_sync" / "abstract_machine_audit_2026Q2.md"
DEFAULT_SIGNOFF_PATH = ROOT / "spec" / "planning" / "v013_rel03_signoff_consolidation_package.md"


@dataclass(frozen=True)
class SnippetRule:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class DriftFinding:
    artifact: str
    check_id: str
    snippet: str


REQUIRED_RULES: dict[str, tuple[SnippetRule, ...]] = {
    "package": (
        SnippetRule(
            "PKG-01",
            "## 8. M13 Abstract Machine Sync Audit W1 Hardening Addendum (`AMSA-DEP-M13-*`)",
        ),
        SnippetRule(
            "PKG-02",
            "| Dependency ID | Type | Deterministic semantic rule | Fail criteria | Escalation owner | Unblock condition | Linked acceptance row |",
        ),
        SnippetRule(
            "PKG-03",
            "| `AMSA-DEP-M13-01` | `Hard` | This package remains canonical lane-A authority for M13 abstract-machine sync audit dependency semantics and fail-closed baseline controls. |",
        ),
        SnippetRule(
            "PKG-04",
            "| `AMSA-DEP-M13-02` | `Hard` | `reports/spec_sync/abstract_machine_audit_2026Q2.md` remains deterministic source authority for drift IDs/classes/status, REL-03 impact, and remediation-priority semantics. |",
        ),
        SnippetRule(
            "PKG-05",
            "| `AMSA-DEP-M13-03` | `Hard` | `spec/planning/v013_seed_source_reconciliation_package.md` remains deterministic source authority for reconciled contract ID, Part 0/3/10 closure status, conflict decisions, and `V013-SPEC-03` consumer binding fields. |",
        ),
        SnippetRule(
            "PKG-06",
            "| `AMSA-DEP-M13-04` | `Hard` | M13 matrix/evidence schema must publish stable `DEP/CMD/EVID/AC` IDs with deterministic command and evidence-anchor mapping. |",
        ),
        SnippetRule(
            "PKG-07",
            "| `AMSA-DEP-M13-05` | `Hard` | Every M13 acceptance row must declare dependency type, fail criteria, escalation owner, and unblock condition. |",
        ),
        SnippetRule(
            "PKG-08",
            "| `AMSA-DEP-M13-06` | `Soft` | Advisory drift handling is `HOLD` only when drift IDs/status/owner/ETA/evidence metadata remain explicit while all hard controls remain intact. |",
        ),
        SnippetRule(
            "PKG-09",
            "| `AMSA-DEP-M13-07` | `Hard` | M13 disposition semantics are fail-closed with explicit `PASS`/`HOLD`/`FAIL` rules and no waiver path for hard failures. |",
        ),
        SnippetRule(
            "PKG-10",
            "| `AMSA-DEP-M13-08` | `Hard` | Repository lint validator (`python scripts/spec_lint.py`) is blocking for lane-A-owned M13 artifacts. |",
        ),
        SnippetRule(
            "PKG-11",
            "1. `PASS`: hard rows (`AMSA-DEP-M13-01`, `AMSA-DEP-M13-02`, `AMSA-DEP-M13-03`, `AMSA-DEP-M13-04`, `AMSA-DEP-M13-05`, `AMSA-DEP-M13-07`, `AMSA-DEP-M13-08`) pass with linked evidence.",
        ),
        SnippetRule(
            "PKG-12",
            "2. `HOLD`: only soft row `AMSA-DEP-M13-06` remains open with explicit owner, ETA, and bounded-risk note while all hard rows remain `PASS`.",
        ),
        SnippetRule(
            "PKG-13",
            "3. `FAIL`: any hard row fails, required evidence mapping is missing, or soft-row drift is used to bypass hard controls.",
        ),
        SnippetRule(
            "PKG-14",
            "4. Fail-closed default: if evidence is ambiguous, missing, or contradictory, disposition is `FAIL` until deterministic alignment is restored.",
        ),
    ),
    "report": (
        SnippetRule("RPT-01", "## Drift findings table"),
        SnippetRule(
            "RPT-02",
            "| `AM-AUDIT-2026Q2-01` | `normative conflict` | Part 6 (`#part-6-6`) |",
        ),
        SnippetRule(
            "RPT-03",
            "| `AM-AUDIT-2026Q2-02` | `missing example` | Part 7 (`#part-7-6-5`) |",
        ),
        SnippetRule(
            "RPT-04",
            "| `AM-AUDIT-2026Q2-03` | `editorial mismatch` | Part 0 (`#part-0-4-1`) |",
        ),
        SnippetRule(
            "RPT-05",
            "| `EDGE-V013-017` (`V013-SPEC-03 -> V013-REL-03`) | Kickoff packet must cite current abstract-machine sync status. |",
        ),
        SnippetRule(
            "RPT-06",
            "- `class_weight`: `normative conflict=3`, `missing example=2`, `editorial mismatch=1`",
        ),
        SnippetRule("RPT-07", "- `rel03_weight`: `blocking=2`, `advisory=1`"),
        SnippetRule(
            "RPT-08",
            "- `am_row_count`: number of AM matrix rows directly touched by the finding",
        ),
        SnippetRule(
            "RPT-09",
            "- `priority_score = (100 * class_weight) + (10 * rel03_weight) + am_row_count`",
        ),
        SnippetRule("RPT-10", "Tie-break rule: if scores are equal, sort by lexical `drift_id`."),
    ),
    "signoff": (
        SnippetRule(
            "SGN-01",
            "# v0.13 REL-03 Signoff and Evidence Consolidation Support Package (`V013-REL-03`) {#v013-rel03-signoff-consolidation-package}",
        ),
        SnippetRule(
            "SGN-02",
            "| `EDGE-V013-017` | `V013-SPEC-03` | `V013-REL-03` | `reports/spec_sync/abstract_machine_audit_2026Q2.md` | Consume latest audit drift state and unresolved IDs verbatim; no reinterpretation in REL package. | `blocking` |",
        ),
        SnippetRule(
            "SGN-03",
            "| `REL03-EV-03` | `EDGE-V013-017` | `reports/spec_sync/abstract_machine_audit_2026Q2.md` | SPEC-03 audit baseline and drift-status payload. | Import unresolved drift IDs/status into kickoff risk table unchanged. | `Test-Path reports/spec_sync/abstract_machine_audit_2026Q2.md` | `present (True)` |",
        ),
        SnippetRule("SGN-04", "2. AM sync lane input (`EDGE-V013-017`): `REL03-EV-03`."),
        SnippetRule(
            "SGN-05",
            "| `REL03-EX-02` | `reports/spec_sync/abstract_machine_audit_2026Q2.md` missing or unreadable. | `blocking` | Freeze REL-03 signoff and request SPEC-03 republish. | Escalate to SPEC lane owner and release chair. | `<=24h` from detection. |",
        ),
        SnippetRule(
            "SGN-06",
            "| `AC-729-02` | Dependency register explicitly covers `EDGE-V013-016..019`. | Section `2` lists each edge with producer/consumer and required artifacts. | `PASS` |",
        ),
        SnippetRule(
            "SGN-07",
            '| `REL03-CMD-03` | `rg -n "spec/planning/future_work_v011_carryover.md|reports/spec_sync/abstract_machine_audit_2026Q2.md|spec/planning/v013_profile_gate_delta.md|spec/CONFORMANCE_PROFILE_CHECKLIST.md|spec/planning/v013_review_board_cadence_quorum_package.md|reports/reviews/v013_review_board_calendar.md" spec/planning/v013_rel03_signoff_consolidation_package.md` | `matched all required upstream artifact references` | `0` | `PASS` |',
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
    for artifact in ARTIFACT_ORDER:
        seen: set[str] = set()
        for rule in REQUIRED_RULES[artifact]:
            if rule.check_id in seen:
                raise ValueError(f"rule configuration has duplicate check id: {artifact}:{rule.check_id}")
            seen.add(rule.check_id)


def finding_sort_key(finding: DriftFinding) -> tuple[int, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in drift finding: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id)


def validate_contract(sources: dict[str, str]) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    for artifact in ARTIFACT_ORDER:
        content = sources[artifact]
        for rule in REQUIRED_RULES[artifact]:
            if rule.snippet not in content:
                findings.append(
                    DriftFinding(
                        artifact=artifact,
                        check_id=rule.check_id,
                        snippet=rule.snippet,
                    )
                )
    return sorted(findings, key=finding_sort_key)


def render_drift_report(
    *,
    findings: list[DriftFinding],
    package_path: Path,
    report_path: Path,
    signoff_path: Path,
) -> str:
    ordered_findings = sorted(findings, key=finding_sort_key)
    rerun_command = render_command(
        [
            "python",
            "scripts/check_abstract_machine_sync_audit_contract.py",
            "--package",
            display_path(package_path),
            "--report",
            display_path(report_path),
            "--signoff",
            display_path(signoff_path),
        ]
    )
    lines = [
        "abstract-machine-sync-audit-contract: contract drift detected "
        f"({len(ordered_findings)} failed check(s)).",
        "drift findings:",
    ]
    for finding in ordered_findings:
        lines.append(f"- {finding.artifact}:{finding.check_id}")
        lines.append(f"  expected snippet: {finding.snippet}")
    lines.extend(
        [
            "remediation:",
            "1. Restore missing contract snippet(s) in the listed artifact(s).",
            "2. Re-run validator:",
            rerun_command,
        ]
    )
    return "\n".join(lines)


def check_contract(*, package_path: Path, report_path: Path, signoff_path: Path) -> int:
    validate_rule_configuration()
    sources = {
        "package": load_text(package_path, artifact="package"),
        "report": load_text(report_path, artifact="report"),
        "signoff": load_text(signoff_path, artifact="signoff"),
    }
    findings = validate_contract(sources)
    if findings:
        print(
            render_drift_report(
                findings=findings,
                package_path=package_path,
                report_path=report_path,
                signoff_path=signoff_path,
            ),
            file=sys.stderr,
        )
        return 1

    checks_passed = sum(len(rules) for rules in REQUIRED_RULES.values())
    print("abstract-machine-sync-audit-contract: OK")
    print(f"- mode={MODE}")
    print(f"- package={display_path(package_path)}")
    print(f"- report={display_path(report_path)}")
    print(f"- signoff={display_path(signoff_path)}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_abstract_machine_sync_audit_contract.py",
        description=(
            "Fail-closed validator for deterministic v0.14 M14 abstract-machine sync "
            "audit contract anchors across package/report/signoff artifacts."
        ),
    )
    parser.add_argument(
        "--package",
        type=Path,
        default=DEFAULT_PACKAGE_PATH,
        help="Path to spec/planning/v013_abstract_machine_sync_audit_2026q2_package.md.",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=DEFAULT_REPORT_PATH,
        help="Path to reports/spec_sync/abstract_machine_audit_2026Q2.md.",
    )
    parser.add_argument(
        "--signoff",
        "--reconciliation",
        dest="signoff",
        type=Path,
        default=DEFAULT_SIGNOFF_PATH,
        help=(
            "Path to spec/planning/v013_rel03_signoff_consolidation_package.md "
            "(legacy alias: --reconciliation)."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    package_path = resolve_input_path(args.package)
    report_path = resolve_input_path(args.report)
    signoff_path = resolve_input_path(args.signoff)

    try:
        return check_contract(
            package_path=package_path,
            report_path=report_path,
            signoff_path=signoff_path,
        )
    except ValueError as exc:
        print(f"abstract-machine-sync-audit-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
