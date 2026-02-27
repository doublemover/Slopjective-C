#!/usr/bin/env python3
"""Validate fail-closed contract integrity for v0.14 M16 extension registry compatibility."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "extension-registry-compat-contract-v2"
ARTIFACT_ORDER = ("package", "schema", "readme")

DEFAULT_PACKAGE_PATH = ROOT / "spec" / "planning" / "v013_extension_registry_compat_validation_package.md"
DEFAULT_SCHEMA_PATH = ROOT / "registries" / "experimental_extensions" / "index.schema.json"
DEFAULT_README_PATH = ROOT / "tests" / "governance" / "registry_compat" / "README.md"

EXPECTED_VALIDATOR_IDS: tuple[str, ...] = (
    "VAL-RC-01",
    "VAL-RC-02",
    "VAL-RC-03",
    "VAL-RC-04",
    "VAL-RC-05",
    "VAL-RC-06",
)
EXPECTED_FAILURE_TAXONOMY_IDS: tuple[str, ...] = (
    "FTX-RC-M16-01",
    "FTX-RC-M16-02",
    "FTX-RC-M16-03",
    "FTX-RC-M16-04",
    "FTX-RC-M16-05",
    "FTX-RC-M16-06",
    "FTX-RC-M16-07",
)
EXPECTED_PUBLICATION_GATE_IDS: tuple[str, ...] = (
    "PUB-RC-M16-01",
    "PUB-RC-M16-02",
    "PUB-RC-M16-03",
    "PUB-RC-M16-04",
)
SCHEMA_VALIDATOR_ENUM_PATH = (
    "$defs",
    "validator_contract",
    "properties",
    "validator_id",
    "enum",
)
W2_DYNAMIC_CHECK_IDS: tuple[str, ...] = (
    "PKG-26",
    "RDM-14",
    "SCH-19",
    "RDM-15",
    "PKG-27",
    "PKG-28",
)


@dataclass(frozen=True)
class SnippetRule:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class SchemaInvariantRule:
    check_id: str
    path: tuple[str, ...]
    expected: Any


@dataclass(frozen=True)
class DriftFinding:
    artifact: str
    check_id: str
    detail: str


PACKAGE_RULES: tuple[SnippetRule, ...] = (
    SnippetRule(
        "PKG-01",
        "# V013-GOV-02 Package: Extension Registry Compatibility Validation Suite",
    ),
    SnippetRule("PKG-02", "| `seed_id` | `V013-GOV-02` |"),
    SnippetRule("PKG-03", "| `acceptance_gate_id` | `AC-V013-GOV-02` |"),
    SnippetRule(
        "PKG-04",
        "| `CM-RC-07` | `unknown_major_input` | Unknown major from consumer perspective | "
        "Required-field interpretation is undefined | `fail` | `fail` | Hard fail closed "
        "and escalate under `E2`. | `VAL-RC-06` |",
    ),
    SnippetRule(
        "PKG-05",
        "| `VAL-RC-06` | `rg -n \"AC-V013-GOV-02|VAL-RC-|ESC-RC-\" "
        "tests/governance/registry_compat/README.md` | Exit `0`; all required governance "
        "identifiers are present. | Treat test/readme contract as incomplete and block "
        "publish. |",
    ),
    SnippetRule(
        "PKG-06",
        "3. Deterministic validator nondeterminism (same input producing conflicting outcomes).",
    ),
    SnippetRule(
        "PKG-07",
        "| `ESC-RC-04` (`E4`) | Integrity-risk or policy breach with no safe workaround. | "
        "Steering owner + security owner | Immediate | Apply emergency hold and require "
        "superseding corrective artifact set. |",
    ),
    SnippetRule(
        "PKG-08",
        "| `AC-V013-GOV-02-05` | Validation transcript for `python scripts/spec_lint.py` is "
        "recorded. | Section `7` includes command, output, and exit status. | This package "
        "Section `7`. |",
    ),
    SnippetRule("PKG-09", "Exit status: `0` (`PASS`)"),
    SnippetRule(
        "PKG-10",
        "## 9. M16 Extension Registry Compatibility W2 Deterministic Hardening Addendum "
        "(`ERC-DEP-M16-*`)",
    ),
    SnippetRule(
        "PKG-11",
        "| `ERC-DEP-M16-02` | `Hard` | Validator ID set parity is strict: `VAL-RC-01`.."
        "`VAL-RC-06` must remain complete, unique, and stable across W2 authority artifacts. "
        "| Missing, duplicate, renumbered, or ad hoc validator IDs in W2-controlled artifacts. "
        "| `Lane A M16 owner (#899)` | Restore canonical validator ID set and rerun parity "
        "command anchors with exit `0`. | `AC-V014-M16-02` |",
    ),
    SnippetRule(
        "PKG-12",
        "| `ERC-DEP-M16-03` | `Hard` | Validator command/pass-signal parity is strict: "
        "each `VAL-RC-*` command and pass signal in Section `4.1` remains semantically "
        "identical in W2 acceptance/evidence controls. | Command-string drift, "
        "pass-signal drift, or incompatible failure-disposition language for any `VAL-RC-*`. "
        "| `Lane A M16 owner (#899)` | Reconcile command/pass/failure semantics to "
        "Section `4.1` and rerun parity command anchors. | `AC-V014-M16-03` |",
    ),
    SnippetRule(
        "PKG-13",
        "| `ERC-DEP-M16-04` | `Hard` | Failure taxonomy classes `FTX-RC-M16-01`.."
        "`FTX-RC-M16-07` are stable and map deterministically to disposition and "
        "escalation routes. | Missing taxonomy class IDs, ambiguous taxonomy mapping, "
        "or taxonomy-to-disposition drift. | `Lane A M16 owner (#899)` | Restore "
        "taxonomy ID set and deterministic mappings; rerun taxonomy command anchors "
        "with exit `0`. | `AC-V014-M16-04` |",
    ),
    SnippetRule(
        "PKG-14",
        "| `ERC-DEP-M16-07` | `Hard` | Publication gating is deterministic and "
        "fail-closed via `PUB-RC-M16-01`..`PUB-RC-M16-04`; ambiguous or incomplete state "
        "cannot publish. | Missing gate rules, contradictory gate logic, or ambiguous "
        "disposition outcomes in W2 artifacts. | `Lane A M16 owner (#899)` | "
        "Restore deterministic publication-gate rules and rerun gating command anchors "
        "with exit `0`. | `AC-V014-M16-08` |",
    ),
    SnippetRule(
        "PKG-15",
        "| `ERC-DEP-M16-08` | `Hard` | Repository spec lint is the terminal release "
        "gate for lane-A-owned M16 artifacts. | `python scripts/spec_lint.py` exits "
        "non-zero or omits `spec-lint: OK`. | `Lane A M16 owner (#899)` | Resolve "
        "lint findings, rerun validator, and capture clean transcript evidence with "
        "exit code. | `AC-V014-M16-09` |",
    ),
    SnippetRule("PKG-16", "### 9.2 Strict validator parity profile (`VPAR-RC-M16-*`)"),
    SnippetRule(
        "PKG-17",
        "| `VPAR-RC-M16-04` | Validator failure-disposition parity | Failure outcomes "
        "remain fail-closed and preserve escalation route expectations from base governance "
        "policy. | `FTX-RC-M16-03`, `FTX-RC-M16-04` | `AC-V014-M16-03`, `AC-V014-M16-08` |",
    ),
    SnippetRule("PKG-18", "### 9.3 Deterministic failure taxonomy (`FTX-RC-M16-*`)"),
    SnippetRule(
        "PKG-19",
        "| `FTX-RC-M16-07` | `Hard` | Missing evidence anchors, missing exit-code "
        "reporting, or publication-gate ambiguity. | `REJECT` | `ESC-RC-03` (`E3`) "
        "| Not waiverable |",
    ),
    SnippetRule("PKG-20", "### 9.4 Deterministic publication gating (`PUB-RC-M16-*`)"),
    SnippetRule(
        "PKG-21",
        "| `PUB-RC-M16-03` | Acceptance/evidence schema remains complete: stable "
        "`DEP/CMD/EVID/AC` IDs plus explicit exit codes for every replay command. | "
        "`PUBLISH` or valid `HOLD` (if only `PUB-RC-M16-02` applies). | `REJECT`. |",
    ),
    SnippetRule(
        "PKG-22",
        "1. `PUBLISH`: every hard dependency passes and no hard taxonomy class "
        "(`FTX-RC-M16-01`, `FTX-RC-M16-02`, `FTX-RC-M16-03`, `FTX-RC-M16-04`, "
        "`FTX-RC-M16-05`, `FTX-RC-M16-07`) is active.",
    ),
    SnippetRule(
        "PKG-23",
        "2. `HOLD`: only soft class `FTX-RC-M16-06` is active with required owner + ETA "
        "+ replay command, and all hard dependencies remain `PASS`.",
    ),
    SnippetRule(
        "PKG-24",
        "3. `REJECT`: any hard failure taxonomy class is active, required evidence is "
        "missing, or publication state cannot be determined deterministically.",
    ),
    SnippetRule(
        "PKG-25",
        "4. Default fail-closed rule: if there is any uncertainty, publish state is "
        "`REJECT` until deterministic evidence resolves the uncertainty.",
    ),
)

README_RULES: tuple[SnippetRule, ...] = (
    SnippetRule("RDM-01", "# Registry Compatibility Validation Suite (`V013-GOV-02`)"),
    SnippetRule("RDM-02", "- Acceptance gate: `AC-V013-GOV-02`"),
    SnippetRule("RDM-03", "- `CM-RC-07` unknown major input (must fail)"),
    SnippetRule(
        "RDM-04",
        "| `VAL-RC-05` | `python -c 'import json,pathlib,sys;d=json.loads(pathlib.Path(\"registries/"
        "experimental_extensions/index.schema.json\").read_text(encoding=\"utf-8\"));p=d[\"$defs\"]"
        "[\"governance_contract\"][\"properties\"];need={\"compatibility_matrix\","
        "\"required_field_policy\",\"validators\",\"waiver_policy\",\"acceptance_checklist\"};"
        "m=sorted(need-set(p));print(\"contract-keys: OK\" if not m else \"contract-keys: MISSING \""
        "+\",\".join(m));sys.exit(0 if not m else 1)'` | `contract-keys: OK` |",
    ),
    SnippetRule(
        "RDM-05",
        "Validator ordering is fixed (`VAL-RC-01`..`VAL-RC-06`). Any non-zero exit code is "
        "a blocking failure.",
    ),
    SnippetRule("RDM-06", "- Missing `AC-V013-GOV-02` acceptance mapping"),
    SnippetRule(
        "RDM-07",
        "- `ESC-RC-04` (`E4`): integrity or policy breach, immediate emergency hold",
    ),
    SnippetRule(
        "RDM-08",
        "- [x] `AC-V013-GOV-02-05` `python scripts/spec_lint.py` transcript is recorded.",
    ),
    SnippetRule("RDM-09", "spec-lint: OK"),
    SnippetRule("RDM-10", "| `VAL-RC-01` | `python scripts/spec_lint.py` | `spec-lint: OK` |"),
    SnippetRule(
        "RDM-11",
        "| `VAL-RC-06` | `rg -n \"AC-V013-GOV-02|VAL-RC-|ESC-RC-\" tests/governance/"
        "registry_compat/README.md` | exit `0` |",
    ),
    SnippetRule("RDM-12", "- `fail`: incompatible; publish is blocked."),
    SnippetRule(
        "RDM-13",
        "- `ESC-RC-02` (`E2`): repeated/major compatibility failure, response `T+48h`",
    ),
)

SCHEMA_INVARIANT_RULES: tuple[SchemaInvariantRule, ...] = (
    SchemaInvariantRule(
        "SCH-01",
        ("required",),
        [
            "schema_version",
            "registry_id",
            "generated_at_utc",
            "governance_contract",
            "extensions",
        ],
    ),
    SchemaInvariantRule(
        "SCH-02",
        ("$defs", "governance_contract", "required"),
        [
            "contract_version",
            "acceptance_gate_id",
            "compatibility_matrix",
            "required_field_policy",
            "validators",
            "waiver_policy",
            "acceptance_checklist",
        ],
    ),
    SchemaInvariantRule(
        "SCH-03",
        ("$defs", "governance_contract", "properties", "acceptance_gate_id", "const"),
        "AC-V013-GOV-02",
    ),
    SchemaInvariantRule(
        "SCH-04",
        ("$defs", "compatibility_matrix_row", "properties", "change_class", "enum"),
        [
            "patch_nonsemantic",
            "minor_optional_addition",
            "minor_enum_expansion",
            "minor_required_addition",
            "major_required_removal",
            "major_required_rename_without_alias",
            "unknown_major_input",
            "required_field_type_drift",
        ],
    ),
    SchemaInvariantRule(
        "SCH-05",
        (
            "$defs",
            "required_field_policy",
            "properties",
            "missing_required_field_behavior",
            "enum",
        ),
        ["hard_fail"],
    ),
    SchemaInvariantRule(
        "SCH-06",
        (
            "$defs",
            "required_field_policy",
            "properties",
            "added_required_field_policy",
            "enum",
        ),
        ["major_only"],
    ),
    SchemaInvariantRule(
        "SCH-07",
        (
            "$defs",
            "required_field_policy",
            "properties",
            "removed_required_field_policy",
            "enum",
        ),
        ["major_only_with_migration"],
    ),
    SchemaInvariantRule(
        "SCH-08",
        SCHEMA_VALIDATOR_ENUM_PATH,
        list(EXPECTED_VALIDATOR_IDS),
    ),
    SchemaInvariantRule(
        "SCH-09",
        ("$defs", "validator_contract", "properties", "expected_exit_code", "const"),
        0,
    ),
    SchemaInvariantRule(
        "SCH-10",
        (
            "$defs",
            "validator_contract",
            "properties",
            "failure_escalation_level",
            "enum",
        ),
        ["E1", "E2", "E3", "E4"],
    ),
    SchemaInvariantRule(
        "SCH-11",
        ("$defs", "acceptance_checklist_item", "properties", "checklist_id", "enum"),
        [
            "AC-V013-GOV-02-01",
            "AC-V013-GOV-02-02",
            "AC-V013-GOV-02-03",
            "AC-V013-GOV-02-04",
            "AC-V013-GOV-02-05",
        ],
    ),
    SchemaInvariantRule(
        "SCH-12",
        ("$defs", "validator_contract", "required"),
        [
            "validator_id",
            "command",
            "expected_exit_code",
            "deterministic_pass_signal",
            "failure_escalation_level",
        ],
    ),
    SchemaInvariantRule(
        "SCH-13",
        ("$defs", "compatibility_matrix_row", "properties", "backward_result", "enum"),
        ["pass", "conditional", "fail"],
    ),
    SchemaInvariantRule(
        "SCH-14",
        ("$defs", "compatibility_matrix_row", "properties", "forward_result", "enum"),
        ["pass", "conditional", "fail"],
    ),
    SchemaInvariantRule(
        "SCH-15",
        ("$defs", "acceptance_checklist_item", "properties", "status", "enum"),
        ["pass", "fail", "pending"],
    ),
    SchemaInvariantRule(
        "SCH-16",
        (
            "$defs",
            "required_field_policy",
            "properties",
            "incompatible_type_change_behavior",
            "enum",
        ),
        ["hard_fail"],
    ),
    SchemaInvariantRule(
        "SCH-17",
        ("$defs", "waiver_policy", "properties", "requires_owner_and_expiry", "const"),
        True,
    ),
    SchemaInvariantRule(
        "SCH-18",
        ("$defs", "waiver_policy", "properties", "hard_block_on_expired_waiver", "const"),
        True,
    ),
)

ARTIFACT_RANK = {artifact: index for index, artifact in enumerate(ARTIFACT_ORDER)}
_MISSING = object()


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


def load_schema_document(path: Path) -> dict[str, Any]:
    raw = load_text(path, artifact="schema")
    try:
        parsed = json.loads(raw)
    except json.JSONDecodeError as exc:
        raise ValueError(
            "schema file is not valid JSON: "
            f"{display_path(path)} (line {exc.lineno} column {exc.colno}: {exc.msg})"
        ) from exc
    if not isinstance(parsed, dict):
        raise ValueError(f"schema root is not an object: {display_path(path)}")
    return parsed


def validate_rule_configuration() -> None:
    check_sets = {
        "package": PACKAGE_RULES,
        "readme": README_RULES,
    }
    configured_check_ids: set[str] = set()
    for artifact, rules in check_sets.items():
        seen: set[str] = set()
        for rule in rules:
            if rule.check_id in seen:
                raise ValueError(f"rule configuration has duplicate check id: {artifact}:{rule.check_id}")
            seen.add(rule.check_id)
            configured_check_ids.add(rule.check_id)

    schema_seen: set[str] = set()
    for rule in SCHEMA_INVARIANT_RULES:
        if rule.check_id in schema_seen:
            raise ValueError(f"rule configuration has duplicate check id: schema:{rule.check_id}")
        schema_seen.add(rule.check_id)
        configured_check_ids.add(rule.check_id)

    for check_id in W2_DYNAMIC_CHECK_IDS:
        if check_id in configured_check_ids:
            raise ValueError(f"rule configuration has duplicate check id: dynamic:{check_id}")
        configured_check_ids.add(check_id)


def finding_sort_key(finding: DriftFinding) -> tuple[int, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in drift finding: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id)


def collect_snippet_findings(
    *,
    artifact: str,
    content: str,
    rules: tuple[SnippetRule, ...],
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    for rule in rules:
        if rule.snippet not in content:
            findings.append(
                DriftFinding(
                    artifact=artifact,
                    check_id=rule.check_id,
                    detail=f"expected snippet: {rule.snippet}",
                )
            )
    return findings


def resolve_path(document: object, path: tuple[str, ...]) -> object:
    current = document
    for segment in path:
        if not isinstance(current, dict) or segment not in current:
            return _MISSING
        current = current[segment]
    return current


def format_path(path: tuple[str, ...]) -> str:
    return ".".join(path)


def format_value(value: object) -> str:
    if value is _MISSING:
        return "<missing>"
    return json.dumps(value, ensure_ascii=True, sort_keys=True)


def collect_schema_findings(schema_document: dict[str, Any]) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    for rule in SCHEMA_INVARIANT_RULES:
        observed = resolve_path(schema_document, rule.path)
        if observed != rule.expected:
            findings.append(
                DriftFinding(
                    artifact="schema",
                    check_id=rule.check_id,
                    detail=(
                        f"expected invariant: {format_path(rule.path)} == "
                        f"{format_value(rule.expected)} (found {format_value(observed)})"
                    ),
                )
            )
    return findings


def extract_table_row_ids(*, content: str, prefix: str) -> list[str]:
    pattern = re.compile(rf"^\|\s*`({re.escape(prefix)}-[0-9]{{2}})`\s*\|", re.MULTILINE)
    return [match.group(1) for match in pattern.finditer(content)]


def extract_string_list(value: object) -> list[str] | None:
    if not isinstance(value, list):
        return None
    if any(not isinstance(item, str) for item in value):
        return None
    return list(value)


def collect_w2_findings(
    *,
    package_content: str,
    schema_document: dict[str, Any],
    readme_content: str,
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    expected_validators = list(EXPECTED_VALIDATOR_IDS)

    package_validator_ids = extract_table_row_ids(content=package_content, prefix="VAL-RC")
    readme_validator_ids = extract_table_row_ids(content=readme_content, prefix="VAL-RC")

    if package_validator_ids != expected_validators:
        findings.append(
            DriftFinding(
                artifact="package",
                check_id="PKG-26",
                detail=(
                    "strict validator parity: package validator row set must be "
                    f"{format_value(expected_validators)} "
                    f"(found {format_value(package_validator_ids)})"
                ),
            )
        )

    if readme_validator_ids != expected_validators:
        findings.append(
            DriftFinding(
                artifact="readme",
                check_id="RDM-14",
                detail=(
                    "strict validator parity: readme validator row set must be "
                    f"{format_value(expected_validators)} "
                    f"(found {format_value(readme_validator_ids)})"
                ),
            )
        )

    schema_validator_ids = extract_string_list(resolve_path(schema_document, SCHEMA_VALIDATOR_ENUM_PATH))
    if schema_validator_ids is not None and package_validator_ids != schema_validator_ids:
        findings.append(
            DriftFinding(
                artifact="schema",
                check_id="SCH-19",
                detail=(
                    "strict validator parity mismatch: package validator rows "
                    f"{format_value(package_validator_ids)} != schema validator enum "
                    f"{format_value(schema_validator_ids)}"
                ),
            )
        )

    if package_validator_ids != readme_validator_ids:
        findings.append(
            DriftFinding(
                artifact="readme",
                check_id="RDM-15",
                detail=(
                    "strict validator parity mismatch: package validator rows "
                    f"{format_value(package_validator_ids)} != readme validator rows "
                    f"{format_value(readme_validator_ids)}"
                ),
            )
        )

    failure_taxonomy_ids = extract_table_row_ids(content=package_content, prefix="FTX-RC-M16")
    expected_failure_taxonomy_ids = list(EXPECTED_FAILURE_TAXONOMY_IDS)
    if failure_taxonomy_ids != expected_failure_taxonomy_ids:
        findings.append(
            DriftFinding(
                artifact="package",
                check_id="PKG-27",
                detail=(
                    "deterministic failure taxonomy: expected row set "
                    f"{format_value(expected_failure_taxonomy_ids)} "
                    f"(found {format_value(failure_taxonomy_ids)})"
                ),
            )
        )

    publication_gate_ids = extract_table_row_ids(content=package_content, prefix="PUB-RC-M16")
    expected_publication_gate_ids = list(EXPECTED_PUBLICATION_GATE_IDS)
    if publication_gate_ids != expected_publication_gate_ids:
        findings.append(
            DriftFinding(
                artifact="package",
                check_id="PKG-28",
                detail=(
                    "deterministic publication gating: expected gate row set "
                    f"{format_value(expected_publication_gate_ids)} "
                    f"(found {format_value(publication_gate_ids)})"
                ),
            )
        )

    return findings


def validate_contract(
    *,
    package_content: str,
    schema_document: dict[str, Any],
    readme_content: str,
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []
    findings.extend(
        collect_snippet_findings(
            artifact="package",
            content=package_content,
            rules=PACKAGE_RULES,
        )
    )
    findings.extend(collect_schema_findings(schema_document))
    findings.extend(
        collect_snippet_findings(
            artifact="readme",
            content=readme_content,
            rules=README_RULES,
        )
    )
    findings.extend(
        collect_w2_findings(
            package_content=package_content,
            schema_document=schema_document,
            readme_content=readme_content,
        )
    )
    return sorted(findings, key=finding_sort_key)


def render_drift_report(
    *,
    findings: list[DriftFinding],
    package_path: Path,
    schema_path: Path,
    readme_path: Path,
) -> str:
    ordered_findings = sorted(findings, key=finding_sort_key)
    rerun_command = render_command(
        [
            "python",
            "scripts/check_extension_registry_compat_contract.py",
            "--package",
            display_path(package_path),
            "--schema",
            display_path(schema_path),
            "--readme",
            display_path(readme_path),
        ]
    )
    lines = [
        "extension-registry-compat-contract: contract drift detected "
        f"({len(ordered_findings)} failed check(s)).",
        "drift findings:",
    ]
    for finding in ordered_findings:
        lines.append(f"- {finding.artifact}:{finding.check_id}")
        lines.append(f"  {finding.detail}")
    lines.extend(
        [
            "remediation:",
            "1. Restore missing snippet anchors, schema invariants, or W2 parity/taxonomy/"
            "publication-gating controls in the listed artifact(s).",
            "2. Re-run validator:",
            rerun_command,
        ]
    )
    return "\n".join(lines)


def check_contract(*, package_path: Path, schema_path: Path, readme_path: Path) -> int:
    validate_rule_configuration()
    package_content = load_text(package_path, artifact="package")
    schema_document = load_schema_document(schema_path)
    readme_content = load_text(readme_path, artifact="readme")

    findings = validate_contract(
        package_content=package_content,
        schema_document=schema_document,
        readme_content=readme_content,
    )
    if findings:
        print(
            render_drift_report(
                findings=findings,
                package_path=package_path,
                schema_path=schema_path,
                readme_path=readme_path,
            ),
            file=sys.stderr,
        )
        return 1

    checks_passed = (
        len(PACKAGE_RULES)
        + len(SCHEMA_INVARIANT_RULES)
        + len(README_RULES)
        + len(W2_DYNAMIC_CHECK_IDS)
    )
    print("extension-registry-compat-contract: OK")
    print(f"- mode={MODE}")
    print(f"- package={display_path(package_path)}")
    print(f"- schema={display_path(schema_path)}")
    print(f"- readme={display_path(readme_path)}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_extension_registry_compat_contract.py",
        description=(
            "Fail-closed validator for deterministic v0.14 M16 extension registry "
            "compatibility anchors across package/schema/governance-readme artifacts."
        ),
    )
    parser.add_argument(
        "--package",
        type=Path,
        default=DEFAULT_PACKAGE_PATH,
        help="Path to spec/planning/v013_extension_registry_compat_validation_package.md.",
    )
    parser.add_argument(
        "--schema",
        type=Path,
        default=DEFAULT_SCHEMA_PATH,
        help="Path to registries/experimental_extensions/index.schema.json.",
    )
    parser.add_argument(
        "--readme",
        "--governance-readme",
        dest="readme",
        type=Path,
        default=DEFAULT_README_PATH,
        help="Path to tests/governance/registry_compat/README.md (legacy alias: --governance-readme).",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    package_path = resolve_input_path(args.package)
    schema_path = resolve_input_path(args.schema)
    readme_path = resolve_input_path(args.readme)

    try:
        return check_contract(
            package_path=package_path,
            schema_path=schema_path,
            readme_path=readme_path,
        )
    except ValueError as exc:
        print(f"extension-registry-compat-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
