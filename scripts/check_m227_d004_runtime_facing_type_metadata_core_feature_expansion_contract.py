#!/usr/bin/env python3
"""Fail-closed validator for M227-D004 runtime-facing type metadata core feature expansion."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-d004-runtime-facing-type-metadata-core-feature-expansion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m227_runtime_facing_type_metadata_core_feature_expansion_d004_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_d004_runtime_facing_type_metadata_core_feature_expansion_packet.md"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m227/M227-D004/runtime_facing_type_metadata_core_feature_expansion_contract_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    lane_task: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class DependencyRow:
    task: str
    token: str
    reference: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M227-D004-D003-01",
        "M227-D003",
        Path("docs/contracts/m227_runtime_facing_type_metadata_core_feature_d003_expectations.md"),
    ),
    AssetCheck(
        "M227-D004-D003-02",
        "M227-D003",
        Path("scripts/check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py"),
    ),
    AssetCheck(
        "M227-D004-D003-03",
        "M227-D003",
        Path("tests/tooling/test_check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py"),
    ),
    AssetCheck(
        "M227-D004-D003-04",
        "M227-D003",
        Path("spec/planning/compiler/m227/m227_d003_runtime_facing_type_metadata_core_feature_packet.md"),
    ),
    AssetCheck(
        "M227-D004-A004-01",
        "M227-A004",
        Path("docs/contracts/m227_semantic_pass_core_feature_expansion_expectations.md"),
    ),
    AssetCheck(
        "M227-D004-A004-02",
        "M227-A004",
        Path("scripts/check_m227_a004_semantic_pass_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M227-D004-A004-03",
        "M227-A004",
        Path("tests/tooling/test_check_m227_a004_semantic_pass_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M227-D004-A004-04",
        "M227-A004",
        Path("spec/planning/compiler/m227/m227_a004_semantic_pass_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M227-D004-B004-01",
        "M227-B004",
        Path("docs/contracts/m227_type_system_objc3_forms_core_feature_expansion_b004_expectations.md"),
    ),
    AssetCheck(
        "M227-D004-B004-02",
        "M227-B004",
        Path("scripts/check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M227-D004-B004-03",
        "M227-B004",
        Path("tests/tooling/test_check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M227-D004-B004-04",
        "M227-B004",
        Path("spec/planning/compiler/m227/m227_b004_type_system_objc3_forms_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M227-D004-C004-01",
        "M227-C004",
        Path("docs/contracts/m227_typed_sema_to_lowering_core_feature_expansion_c004_expectations.md"),
    ),
    AssetCheck(
        "M227-D004-C004-02",
        "M227-C004",
        Path("scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M227-D004-C004-03",
        "M227-C004",
        Path("tests/tooling/test_check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M227-D004-C004-04",
        "M227-C004",
        Path("spec/planning/compiler/m227/m227_c004_typed_sema_to_lowering_core_feature_expansion_packet.md"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-D004-EXP-03",
        "# Runtime-Facing Type Metadata Core Feature Expansion Expectations (M227-D004)",
    ),
    SnippetCheck(
        "M227-D004-EXP-04",
        "Contract ID: `objc3c-runtime-facing-type-metadata-core-feature-expansion/m227-d004-v1`",
    ),
    SnippetCheck(
        "M227-D004-EXP-05",
        "Dependencies: `M227-D003`, `M227-A004`, `M227-B004`, `M227-C004`",
    ),
    SnippetCheck(
        "M227-D004-EXP-06",
        "Issue `#5150` governs lane-D core feature expansion scope and dependency-token/reference continuity.",
    ),
    SnippetCheck("M227-D004-EXP-07", "## Prerequisite Dependency Matrix"),
    SnippetCheck(
        "M227-D004-EXP-08",
        "checker `scripts/check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py`",
    ),
    SnippetCheck(
        "M227-D004-EXP-09",
        "checker `scripts/check_m227_a004_semantic_pass_core_feature_expansion_contract.py`",
    ),
    SnippetCheck(
        "M227-D004-EXP-10",
        "checker `scripts/check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py`",
    ),
    SnippetCheck(
        "M227-D004-EXP-11",
        "checker `scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`",
    ),
    SnippetCheck(
        "M227-D004-EXP-12",
        "`python scripts/check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py`",
    ),
    SnippetCheck(
        "M227-D004-EXP-13",
        "`python -m pytest tests/tooling/test_check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py -q`",
    ),
    SnippetCheck(
        "M227-D004-EXP-14",
        "`tmp/reports/m227/M227-D004/runtime_facing_type_metadata_core_feature_expansion_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-D004-PKT-03",
        "# M227-D004 Runtime-Facing Type Metadata Core Feature Expansion Packet",
    ),
    SnippetCheck("M227-D004-PKT-04", "Packet: `M227-D004`"),
    SnippetCheck("M227-D004-PKT-05", "Issue: `#5150`"),
    SnippetCheck("M227-D004-PKT-06", "Scaffold date: `2026-03-03`"),
    SnippetCheck(
        "M227-D004-PKT-07",
        "Dependencies: `M227-D003`, `M227-A004`, `M227-B004`, `M227-C004`",
    ),
    SnippetCheck("M227-D004-PKT-08", "## Frozen Dependency Tokens and References"),
    SnippetCheck(
        "M227-D004-PKT-09",
        "scripts/check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py",
    ),
    SnippetCheck(
        "M227-D004-PKT-10",
        "tests/tooling/test_check_m227_d004_runtime_facing_type_metadata_core_feature_expansion_contract.py",
    ),
    SnippetCheck(
        "M227-D004-PKT-11",
        "Dependency matrices in the expectations contract and this packet must remain",
    ),
    SnippetCheck(
        "M227-D004-PKT-12",
        "reference drift, row-order drift, or row-count drift.",
    ),
    SnippetCheck(
        "M227-D004-PKT-13",
        "`tmp/reports/m227/M227-D004/runtime_facing_type_metadata_core_feature_expansion_contract_summary.json`",
    ),
)

DEPENDENCY_ORDER: tuple[str, ...] = (
    "M227-D003",
    "M227-A004",
    "M227-B004",
    "M227-C004",
)

DEPENDENCY_REFERENCE_TOKENS: dict[str, str] = {
    "M227-D003": "scripts/check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py",
    "M227-A004": "scripts/check_m227_a004_semantic_pass_core_feature_expansion_contract.py",
    "M227-B004": "scripts/check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py",
    "M227-C004": "scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py",
}

MATRIX_ROW_PATTERN = re.compile(
    r"^\| `(?P<task>M227-[A-Z]\d{3})` \| `(?P<token>M227-[A-Z]\d{3})` \| (?P<reference>.+) \|$",
    re.MULTILINE,
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


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


def extract_dependency_rows(text: str) -> list[DependencyRow]:
    rows: list[DependencyRow] = []
    for match in MATRIX_ROW_PATTERN.finditer(text):
        task = match.group("task")
        if task not in DEPENDENCY_ORDER:
            continue
        rows.append(
            DependencyRow(
                task=task,
                token=match.group("token"),
                reference=match.group("reference").strip(),
            )
        )
    return rows


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    checks_passed = 0
    findings: list[Finding] = []

    def check(*, artifact: str, check_id: str, condition: bool, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        if condition:
            checks_passed += 1
            return
        findings.append(Finding(artifact=artifact, check_id=check_id, detail=detail))

    for asset in PREREQUISITE_ASSETS:
        absolute = ROOT / asset.relative_path
        exists_and_file = absolute.exists() and absolute.is_file()
        if exists_and_file:
            detail = ""
        elif absolute.exists():
            detail = f"{asset.lane_task} prerequisite path is not a file: {asset.relative_path.as_posix()}"
        else:
            detail = f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}"
        check(
            artifact=asset.relative_path.as_posix(),
            check_id=asset.check_id,
            condition=exists_and_file,
            detail=detail,
        )

    try:
        expectations_text = load_text(args.expectations_doc, artifact="expectations_doc")
        check(
            artifact="expectations_doc",
            check_id="M227-D004-EXP-01",
            condition=True,
            detail="",
        )
        check(
            artifact="expectations_doc",
            check_id="M227-D004-EXP-02",
            condition=True,
            detail="",
        )
    except ValueError as exc:
        expectations_text = None
        check(
            artifact="expectations_doc",
            check_id="M227-D004-EXP-01",
            condition=False,
            detail=str(exc),
        )
        check(
            artifact="expectations_doc",
            check_id="M227-D004-EXP-02",
            condition=False,
            detail="expectations_doc could not be loaded as UTF-8 text",
        )

    if expectations_text is not None:
        for snippet in EXPECTATIONS_SNIPPETS:
            check(
                artifact="expectations_doc",
                check_id=snippet.check_id,
                condition=snippet.snippet in expectations_text,
                detail=f"expected snippet missing: {snippet.snippet}",
            )
    else:
        for snippet in EXPECTATIONS_SNIPPETS:
            check(
                artifact="expectations_doc",
                check_id=snippet.check_id,
                condition=False,
                detail="expectations_doc unavailable for snippet checks",
            )

    try:
        packet_text = load_text(args.packet_doc, artifact="packet_doc")
        check(
            artifact="packet_doc",
            check_id="M227-D004-PKT-01",
            condition=True,
            detail="",
        )
        check(
            artifact="packet_doc",
            check_id="M227-D004-PKT-02",
            condition=True,
            detail="",
        )
    except ValueError as exc:
        packet_text = None
        check(
            artifact="packet_doc",
            check_id="M227-D004-PKT-01",
            condition=False,
            detail=str(exc),
        )
        check(
            artifact="packet_doc",
            check_id="M227-D004-PKT-02",
            condition=False,
            detail="packet_doc could not be loaded as UTF-8 text",
        )

    if packet_text is not None:
        for snippet in PACKET_SNIPPETS:
            check(
                artifact="packet_doc",
                check_id=snippet.check_id,
                condition=snippet.snippet in packet_text,
                detail=f"expected snippet missing: {snippet.snippet}",
            )
    else:
        for snippet in PACKET_SNIPPETS:
            check(
                artifact="packet_doc",
                check_id=snippet.check_id,
                condition=False,
                detail="packet_doc unavailable for snippet checks",
            )

    expectations_rows = extract_dependency_rows(expectations_text) if expectations_text is not None else []
    packet_rows = extract_dependency_rows(packet_text) if packet_text is not None else []

    def evaluate_matrix_semantics(
        *,
        artifact: str,
        prefix: str,
        text: str | None,
        rows: list[DependencyRow],
    ) -> dict[str, DependencyRow]:
        expected_size = len(DEPENDENCY_ORDER)
        check(
            artifact=artifact,
            check_id=f"{prefix}-SEM-01",
            condition=text is not None and len(rows) == expected_size,
            detail=(
                "dependency matrix row-count drifted: "
                f"expected {expected_size} lane rows, got {len(rows)}"
            ),
        )
        row_order = tuple(row.task for row in rows)
        check(
            artifact=artifact,
            check_id=f"{prefix}-SEM-02",
            condition=text is not None and row_order == DEPENDENCY_ORDER,
            detail=(
                "dependency matrix order drifted: expected "
                f"{', '.join(DEPENDENCY_ORDER)} got {', '.join(row_order) or '<none>'}"
            ),
        )

        row_by_task: dict[str, DependencyRow] = {}
        for row in rows:
            row_by_task.setdefault(row.task, row)

        for index, task in enumerate(DEPENDENCY_ORDER, start=1):
            row = row_by_task.get(task)
            check(
                artifact=artifact,
                check_id=f"{prefix}-SEM-TOK-{index:02d}",
                condition=row is not None and row.token == task,
                detail=(
                    f"dependency matrix token mismatch for {task}: "
                    f"expected {task}, got {row.token if row is not None else '<missing row>'}"
                ),
            )
            expected_reference = DEPENDENCY_REFERENCE_TOKENS[task]
            check(
                artifact=artifact,
                check_id=f"{prefix}-SEM-REF-{index:02d}",
                condition=row is not None and expected_reference in row.reference,
                detail=(
                    f"dependency matrix reference drift for {task}: "
                    f"missing required token {expected_reference}"
                ),
            )
        return row_by_task

    expectations_row_map = evaluate_matrix_semantics(
        artifact="expectations_doc",
        prefix="M227-D004-EXP",
        text=expectations_text,
        rows=expectations_rows,
    )
    packet_row_map = evaluate_matrix_semantics(
        artifact="packet_doc",
        prefix="M227-D004-PKT",
        text=packet_text,
        rows=packet_rows,
    )

    for index, task in enumerate(DEPENDENCY_ORDER, start=1):
        expectations_row = expectations_row_map.get(task)
        packet_row = packet_row_map.get(task)
        check(
            artifact="dependency_matrix",
            check_id=f"M227-D004-SEM-MATCH-{index:02d}",
            condition=(
                expectations_row is not None
                and packet_row is not None
                and expectations_row.reference == packet_row.reference
            ),
            detail=f"dependency matrix expectation/packet mismatch for {task}",
        )

    findings = sorted(findings, key=lambda finding: (finding.check_id, finding.artifact, finding.detail))
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    summary_out = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        print(
            "m227-d004-runtime-facing-type-metadata-core-feature-expansion: "
            f"contract drift detected ({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        print(f"wrote summary: {display_path(summary_out)}", file=sys.stderr)
        return 1

    print("m227-d004-runtime-facing-type-metadata-core-feature-expansion: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    print(f"- summary={display_path(summary_out)}")
    return 0


def main() -> int:
    try:
        return run(sys.argv[1:])
    except Exception as exc:  # noqa: BLE001
        print(
            "m227-d004-runtime-facing-type-metadata-core-feature-expansion: "
            f"error: {exc}",
            file=sys.stderr,
        )
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
