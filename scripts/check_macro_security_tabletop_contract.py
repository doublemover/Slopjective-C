#!/usr/bin/env python3
"""Validate fail-closed contract integrity for v0.13 macro-security tabletop artifacts."""

from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "macro-security-tabletop-contract-v2"
ARTIFACT_ORDER = ("package", "playbook", "report")

DEFAULT_PACKAGE_PATH = ROOT / "spec" / "planning" / "v013_macro_security_tabletop_package.md"
DEFAULT_PLAYBOOK_PATH = ROOT / "spec" / "governance" / "macro_security_incident_playbook_v1.md"
DEFAULT_REPORT_PATH = ROOT / "reports" / "security" / "v013_macro_tabletop.md"


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
        SnippetRule("PKG-01", "`seed_id` | `V013-GOV-04` |"),
        SnippetRule("PKG-02", "`wave_id` | `W1` |"),
        SnippetRule("PKG-03", "`batch_id` | `BATCH-20260223-11S` |"),
        SnippetRule("PKG-04", "`acceptance_gate_id` | `AC-V013-GOV-04` |"),
        SnippetRule("PKG-05", "| `SMT-V013-05` |"),
        SnippetRule("PKG-06", "| `RT-V013-T4` (Low) |"),
        SnippetRule("PKG-07", "| `FRL-V013-05` | `SMT-V013-05` |"),
        SnippetRule("PKG-08", "| `AC-V013-GOV-04-06` |"),
        SnippetRule("PKG-09", "## 10. M11 Macro Security Tabletop Ops W1 Addendum (`MSTP-DEP-M11-*`)"),
        SnippetRule(
            "PKG-10",
            "| `MSTP-DEP-M11-06` | `Soft` | Open remediation drift is advisory "
            "`HOLD` only when rows retain explicit owner, due date, and evidence "
            "hook while all hard controls remain intact. |",
        ),
        SnippetRule(
            "PKG-11",
            "| `MSTP-DEP-M11-07` | `Hard` | M11 disposition semantics are "
            "fail-closed with explicit `PASS`/`HOLD`/`FAIL` rules and no waiver "
            "path for hard failures. |",
        ),
        SnippetRule(
            "PKG-12",
            "4. Fail-closed default: if evidence is ambiguous, missing, or "
            "contradictory, disposition is `FAIL` until deterministic alignment is "
            "restored.",
        ),
    ),
    "playbook": (
        SnippetRule("PBK-01", "### 10.1 Scenario matrix contract (`SMT-V013-*`)"),
        SnippetRule("PBK-02", "| `SMT-V013-05` | `INC-COORD` |"),
        SnippetRule("PBK-03", "### 11.1 Tier definitions"),
        SnippetRule("PBK-04", "| `RT-V013-T4` | `SEV-4` |"),
        SnippetRule("PBK-05", "### 12.1 Remediation ledger contract (`FRL-V013-*`)"),
        SnippetRule("PBK-06", "| `FRL-V013-05` | `SMT-V013-05` |"),
        SnippetRule("PBK-07", "### 12.4 Reseed metadata binding (`#790`, `BATCH-20260223-11S`)"),
        SnippetRule("PBK-08", "| `acceptance_gate_id` | `AC-V013-GOV-04` |"),
        SnippetRule("PBK-09", "### 11.2 Tier transition and override rules"),
        SnippetRule(
            "PBK-10",
            "3. tier downgrade MUST NOT occur before `CER-C` completion criteria are "
            "met,",
        ),
    ),
    "report": (
        SnippetRule("RPT-01", "`seed_id` | `V013-GOV-04` |"),
        SnippetRule("RPT-02", "`wave_id` | `W1` |"),
        SnippetRule("RPT-03", "`batch_id` | `BATCH-20260223-11S` |"),
        SnippetRule("RPT-04", "`acceptance_gate_id` | `AC-V013-GOV-04` |"),
        SnippetRule(
            "RPT-05",
            "Scenario matrix summary: `5/5` scenarios passed severity/tier determinism checks.",
        ),
        SnippetRule("RPT-06", "| `FRL-V013-05` | `SMT-V013-05` |"),
        SnippetRule(
            "RPT-07",
            "| `PBK-V013-03` | Added remediation ledger, metadata binding, and "
            "`AC-V013-GOV-04` mapping to playbook Section `12`. |",
        ),
        SnippetRule(
            "RPT-08",
            "| `AC-V013-GOV-04-06` | Validation transcript for "
            "`python scripts/spec_lint.py` is recorded. |",
        ),
        SnippetRule(
            "RPT-09",
            "| `TTX-EV-04` | `SMT-V013-04` | Recovery hold until replay checks "
            "passed; `G-R1` deferred per policy. | `PASS` |",
        ),
        SnippetRule("RPT-10", "Ledger completeness check:"),
        SnippetRule("RPT-11", "- rows with evidence hook: `5/5`"),
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
    playbook_path: Path,
    report_path: Path,
) -> str:
    ordered_findings = sorted(findings, key=finding_sort_key)
    rerun_command = render_command(
        [
            "python",
            "scripts/check_macro_security_tabletop_contract.py",
            "--package",
            display_path(package_path),
            "--playbook",
            display_path(playbook_path),
            "--report",
            display_path(report_path),
        ]
    )
    lines = [
        "macro-security-tabletop-contract: contract drift detected "
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


def check_contract(*, package_path: Path, playbook_path: Path, report_path: Path) -> int:
    validate_rule_configuration()
    sources = {
        "package": load_text(package_path, artifact="package"),
        "playbook": load_text(playbook_path, artifact="playbook"),
        "report": load_text(report_path, artifact="report"),
    }
    findings = validate_contract(sources)
    if findings:
        print(
            render_drift_report(
                findings=findings,
                package_path=package_path,
                playbook_path=playbook_path,
                report_path=report_path,
            ),
            file=sys.stderr,
        )
        return 1

    checks_passed = sum(len(rules) for rules in REQUIRED_RULES.values())
    print("macro-security-tabletop-contract: OK")
    print(f"- mode={MODE}")
    print(f"- package={display_path(package_path)}")
    print(f"- playbook={display_path(playbook_path)}")
    print(f"- report={display_path(report_path)}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_macro_security_tabletop_contract.py",
        description=(
            "Fail-closed validator for deterministic v0.13 macro-security tabletop "
            "contract anchors across planning, governance, and report artifacts."
        ),
    )
    parser.add_argument(
        "--package",
        type=Path,
        default=DEFAULT_PACKAGE_PATH,
        help="Path to spec/planning/v013_macro_security_tabletop_package.md.",
    )
    parser.add_argument(
        "--playbook",
        type=Path,
        default=DEFAULT_PLAYBOOK_PATH,
        help="Path to spec/governance/macro_security_incident_playbook_v1.md.",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=DEFAULT_REPORT_PATH,
        help="Path to reports/security/v013_macro_tabletop.md.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    package_path = resolve_input_path(args.package)
    playbook_path = resolve_input_path(args.playbook)
    report_path = resolve_input_path(args.report)

    try:
        return check_contract(
            package_path=package_path,
            playbook_path=playbook_path,
            report_path=report_path,
        )
    except ValueError as exc:
        print(f"macro-security-tabletop-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
