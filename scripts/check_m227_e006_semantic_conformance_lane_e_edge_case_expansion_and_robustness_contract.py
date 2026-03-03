#!/usr/bin/env python3
"""Fail-closed validator for M227-E006 semantic conformance lane-E edge-case expansion and robustness."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-e006-semantic-conformance-lane-e-edge-case-expansion-and-robustness-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m227_lane_e_semantic_conformance_edge_case_expansion_and_robustness_e006_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_e006_semantic_conformance_lane_e_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m227/M227-E006/semantic_conformance_lane_e_edge_case_expansion_and_robustness_contract_summary.json"
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
class PackageScriptKeyCheck:
    check_id: str
    script_key: str


@dataclass(frozen=True)
class PackageScriptCheck:
    check_id: str
    script_key: str
    expected_value: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M227-E006-E005-01",
        "M227-E005",
        Path("docs/contracts/m227_lane_e_semantic_conformance_edge_case_compatibility_completion_e005_expectations.md"),
    ),
    AssetCheck(
        "M227-E006-E005-02",
        "M227-E005",
        Path("scripts/check_m227_e005_semantic_conformance_lane_e_edge_case_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M227-E006-E005-03",
        "M227-E005",
        Path("tests/tooling/test_check_m227_e005_semantic_conformance_lane_e_edge_case_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M227-E006-E005-04",
        "M227-E005",
        Path("spec/planning/compiler/m227/m227_e005_semantic_conformance_lane_e_edge_case_compatibility_completion_packet.md"),
    ),
    AssetCheck(
        "M227-E006-A006-01",
        "M227-A006",
        Path("docs/contracts/m227_semantic_pass_edge_robustness_expectations.md"),
    ),
    AssetCheck(
        "M227-E006-A006-02",
        "M227-A006",
        Path("scripts/check_m227_a006_semantic_pass_edge_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-E006-A006-03",
        "M227-A006",
        Path("tests/tooling/test_check_m227_a006_semantic_pass_edge_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-E006-A006-04",
        "M227-A006",
        Path("spec/planning/compiler/m227/m227_a006_semantic_pass_edge_robustness_packet.md"),
    ),
    AssetCheck(
        "M227-E006-B006-01",
        "M227-B006",
        Path("docs/contracts/m227_type_system_objc3_forms_edge_robustness_b006_expectations.md"),
    ),
    AssetCheck(
        "M227-E006-B006-02",
        "M227-B006",
        Path("scripts/check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-E006-B006-03",
        "M227-B006",
        Path("tests/tooling/test_check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-E006-B006-04",
        "M227-B006",
        Path("spec/planning/compiler/m227/m227_b006_type_system_objc3_forms_edge_robustness_packet.md"),
    ),
    AssetCheck(
        "M227-E006-C006-01",
        "M227-C006",
        Path("docs/contracts/m227_typed_sema_to_lowering_edge_case_expansion_and_robustness_c006_expectations.md"),
    ),
    AssetCheck(
        "M227-E006-C006-02",
        "M227-C006",
        Path("scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-E006-C006-03",
        "M227-C006",
        Path("tests/tooling/test_check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-E006-C006-04",
        "M227-C006",
        Path("spec/planning/compiler/m227/m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M227-E006-D006-01",
        "M227-D006",
        Path("docs/contracts/m227_runtime_facing_type_metadata_edge_case_expansion_and_robustness_d006_expectations.md"),
    ),
    AssetCheck(
        "M227-E006-D006-02",
        "M227-D006",
        Path("scripts/check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-E006-D006-03",
        "M227-D006",
        Path("tests/tooling/test_check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-E006-D006-04",
        "M227-D006",
        Path("spec/planning/compiler/m227/m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_packet.md"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E006-EXP-03",
        "# M227 Lane E Semantic Conformance Edge-Case Expansion and Robustness Expectations (E006)",
    ),
    SnippetCheck(
        "M227-E006-EXP-04",
        "Contract ID: `objc3c-lane-e-semantic-conformance-edge-case-expansion-and-robustness-contract/m227-e006-v1`",
    ),
    SnippetCheck(
        "M227-E006-EXP-05",
        "Dependencies: `M227-E005`, `M227-A006`, `M227-B006`, `M227-C006`, `M227-D006`",
    ),
    SnippetCheck(
        "M227-E006-EXP-06",
        "Issue `#5164` governs lane-E edge-case expansion and robustness scope and dependency-token/reference continuity.",
    ),
    SnippetCheck("M227-E006-EXP-07", "## Prerequisite Dependency Matrix"),
    SnippetCheck(
        "M227-E006-EXP-08",
        "checker `scripts/check_m227_e005_semantic_conformance_lane_e_edge_case_compatibility_completion_contract.py`",
    ),
    SnippetCheck(
        "M227-E006-EXP-09",
        "checker `scripts/check_m227_a006_semantic_pass_edge_robustness_contract.py`",
    ),
    SnippetCheck(
        "M227-E006-EXP-10",
        "checker `scripts/check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py`",
    ),
    SnippetCheck(
        "M227-E006-EXP-11",
        "checker `scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`",
    ),
    SnippetCheck(
        "M227-E006-EXP-12",
        "checker `scripts/check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py`",
    ),
    SnippetCheck(
        "M227-E006-EXP-13",
        "`check:objc3c:m227-e006-lane-e-readiness`",
    ),
    SnippetCheck(
        "M227-E006-EXP-14",
        "`python scripts/check_m227_e006_semantic_conformance_lane_e_edge_case_expansion_and_robustness_contract.py --emit-json`",
    ),
    SnippetCheck(
        "M227-E006-EXP-15",
        "`tmp/reports/m227/M227-E006/semantic_conformance_lane_e_edge_case_expansion_and_robustness_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E006-PKT-03",
        "# M227-E006 Semantic Conformance Lane-E Edge-Case Expansion and Robustness Packet",
    ),
    SnippetCheck("M227-E006-PKT-04", "Packet: `M227-E006`"),
    SnippetCheck("M227-E006-PKT-05", "Issue: `#5164`"),
    SnippetCheck("M227-E006-PKT-06", "Scaffold date: `2026-03-03`"),
    SnippetCheck(
        "M227-E006-PKT-07",
        "Dependencies: `M227-E005`, `M227-A006`, `M227-B006`, `M227-C006`, `M227-D006`",
    ),
    SnippetCheck("M227-E006-PKT-08", "## Frozen Dependency Tokens and References"),
    SnippetCheck(
        "M227-E006-PKT-09",
        "scripts/check_m227_e006_semantic_conformance_lane_e_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck(
        "M227-E006-PKT-10",
        "tests/tooling/test_check_m227_e006_semantic_conformance_lane_e_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck(
        "M227-E006-PKT-11",
        "Dependency matrices in the expectations contract and this packet must remain",
    ),
    SnippetCheck(
        "M227-E006-PKT-12",
        "token drift,",
    ),
    SnippetCheck(
        "M227-E006-PKT-13",
        "`tmp/reports/m227/M227-E006/semantic_conformance_lane_e_edge_case_expansion_and_robustness_contract_summary.json`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E006-ARCH-01",
        "M227 lane-E E006 semantic conformance edge-case expansion and robustness anchors dependency references",
    ),
    SnippetCheck(
        "M227-E006-ARCH-02",
        "(`M227-E005`, `M227-A006`, `M227-B006`, `M227-C006`, and `M227-D006`)",
    ),
    SnippetCheck(
        "M227-E006-ARCH-03",
        "check:objc3c:m227-e006-lane-e-readiness",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E006-SPC-01",
        "semantic conformance lane-E edge-case expansion and robustness wiring shall preserve explicit lane-E dependency anchors (`M227-E005`, `M227-A006`, `M227-B006`, `M227-C006`, and `M227-D006`)",
    ),
    SnippetCheck(
        "M227-E006-SPC-02",
        "preserve readiness continuity across direct `M227-E005` and `M227-A006` checker/test commands plus `check:objc3c:m227-b006-lane-b-readiness`, `check:objc3c:m227-c006-lane-c-readiness`, and `check:objc3c:m227-d006-lane-d-readiness`",
    ),
    SnippetCheck(
        "M227-E006-SPC-03",
        "lane-E edge-case expansion/robustness evidence drift.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E006-META-01",
        "deterministic lane-E semantic conformance edge-case expansion and robustness dependency anchors for `M227-E005`, `M227-A006`, `M227-B006`, `M227-C006`, and `M227-D006`",
    ),
    SnippetCheck(
        "M227-E006-META-02",
        "with fail-closed readiness continuity (`check:objc3c:m227-e006-lane-e-readiness`)",
    ),
    SnippetCheck(
        "M227-E006-META-03",
        "semantic conformance lane-E edge-case expansion/robustness metadata governance drift fails closed.",
    ),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M227-E006-CFG-01",
        "check:objc3c:m227-e006-semantic-conformance-lane-e-edge-case-expansion-and-robustness-contract",
    ),
    PackageScriptKeyCheck(
        "M227-E006-CFG-02",
        "test:tooling:m227-e006-semantic-conformance-lane-e-edge-case-expansion-and-robustness-contract",
    ),
    PackageScriptKeyCheck("M227-E006-CFG-03", "check:objc3c:m227-e006-lane-e-readiness"),
    PackageScriptKeyCheck("M227-E006-CFG-07", "compile:objc3c"),
    PackageScriptKeyCheck("M227-E006-CFG-08", "proof:objc3c"),
    PackageScriptKeyCheck("M227-E006-CFG-09", "test:objc3c:execution-replay-proof"),
    PackageScriptKeyCheck("M227-E006-CFG-10", "test:objc3c:perf-budget"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M227-E006-CFG-04",
        "check:objc3c:m227-e006-semantic-conformance-lane-e-edge-case-expansion-and-robustness-contract",
        "python scripts/check_m227_e006_semantic_conformance_lane_e_edge_case_expansion_and_robustness_contract.py",
    ),
    PackageScriptCheck(
        "M227-E006-CFG-05",
        "test:tooling:m227-e006-semantic-conformance-lane-e-edge-case-expansion-and-robustness-contract",
        "python -m pytest tests/tooling/test_check_m227_e006_semantic_conformance_lane_e_edge_case_expansion_and_robustness_contract.py -q",
    ),
    PackageScriptCheck(
        "M227-E006-CFG-06",
        "check:objc3c:m227-e006-lane-e-readiness",
        "python scripts/check_m227_e005_semantic_conformance_lane_e_edge_case_compatibility_completion_contract.py && python -m pytest tests/tooling/test_check_m227_e005_semantic_conformance_lane_e_edge_case_compatibility_completion_contract.py -q && python scripts/check_m227_a006_semantic_pass_edge_robustness_contract.py && python -m pytest tests/tooling/test_check_m227_a006_semantic_pass_edge_robustness_contract.py -q && npm run check:objc3c:m227-b006-lane-b-readiness && npm run check:objc3c:m227-c006-lane-c-readiness && npm run check:objc3c:m227-d006-lane-d-readiness && npm run check:objc3c:m227-e006-semantic-conformance-lane-e-edge-case-expansion-and-robustness-contract && npm run test:tooling:m227-e006-semantic-conformance-lane-e-edge-case-expansion-and-robustness-contract",
    ),
)

DEPENDENCY_ORDER: tuple[str, ...] = (
    "M227-E005",
    "M227-A006",
    "M227-B006",
    "M227-C006",
    "M227-D006",
)

DEPENDENCY_REFERENCE_TOKENS: dict[str, str] = {
    "M227-E005": "scripts/check_m227_e005_semantic_conformance_lane_e_edge_case_compatibility_completion_contract.py",
    "M227-A006": "scripts/check_m227_a006_semantic_pass_edge_robustness_contract.py",
    "M227-B006": "scripts/check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py",
    "M227-C006": "scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py",
    "M227-D006": "scripts/check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py",
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
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
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

    def check_document(
        *,
        path: Path,
        artifact: str,
        exists_check_id: str,
        utf8_check_id: str,
        snippets: tuple[SnippetCheck, ...],
    ) -> str | None:
        try:
            text = load_text(path, artifact=artifact)
            check(
                artifact=artifact,
                check_id=exists_check_id,
                condition=True,
                detail="",
            )
            check(
                artifact=artifact,
                check_id=utf8_check_id,
                condition=True,
                detail="",
            )
        except ValueError as exc:
            text = None
            check(
                artifact=artifact,
                check_id=exists_check_id,
                condition=False,
                detail=str(exc),
            )
            check(
                artifact=artifact,
                check_id=utf8_check_id,
                condition=False,
                detail=f"{artifact} could not be loaded as UTF-8 text",
            )

        if text is not None:
            for snippet in snippets:
                check(
                    artifact=artifact,
                    check_id=snippet.check_id,
                    condition=snippet.snippet in text,
                    detail=f"expected snippet missing: {snippet.snippet}",
                )
        else:
            for snippet in snippets:
                check(
                    artifact=artifact,
                    check_id=snippet.check_id,
                    condition=False,
                    detail=f"{artifact} unavailable for snippet checks",
                )
        return text

    expectations_text = check_document(
        path=args.expectations_doc,
        artifact="expectations_doc",
        exists_check_id="M227-E006-EXP-01",
        utf8_check_id="M227-E006-EXP-02",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    packet_text = check_document(
        path=args.packet_doc,
        artifact="packet_doc",
        exists_check_id="M227-E006-PKT-01",
        utf8_check_id="M227-E006-PKT-02",
        snippets=PACKET_SNIPPETS,
    )
    check_document(
        path=args.architecture_doc,
        artifact="architecture_doc",
        exists_check_id="M227-E006-ARCH-00",
        utf8_check_id="M227-E006-ARCH-UTF8",
        snippets=ARCHITECTURE_SNIPPETS,
    )
    check_document(
        path=args.lowering_spec,
        artifact="lowering_spec",
        exists_check_id="M227-E006-SPC-00",
        utf8_check_id="M227-E006-SPC-UTF8",
        snippets=LOWERING_SPEC_SNIPPETS,
    )
    check_document(
        path=args.metadata_spec,
        artifact="metadata_spec",
        exists_check_id="M227-E006-META-00",
        utf8_check_id="M227-E006-META-UTF8",
        snippets=METADATA_SPEC_SNIPPETS,
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
        prefix="M227-E006-EXP",
        text=expectations_text,
        rows=expectations_rows,
    )
    packet_row_map = evaluate_matrix_semantics(
        artifact="packet_doc",
        prefix="M227-E006-PKT",
        text=packet_text,
        rows=packet_rows,
    )

    for index, task in enumerate(DEPENDENCY_ORDER, start=1):
        expectations_row = expectations_row_map.get(task)
        packet_row = packet_row_map.get(task)
        check(
            artifact="dependency_matrix",
            check_id=f"M227-E006-SEM-MATCH-{index:02d}",
            condition=(
                expectations_row is not None
                and packet_row is not None
                and expectations_row.reference == packet_row.reference
            ),
            detail=f"dependency matrix expectation/packet mismatch for {task}",
        )

    try:
        package_text = load_text(args.package_json, artifact="package_json")
        check(
            artifact="package_json",
            check_id="M227-E006-CFG-00",
            condition=True,
            detail="",
        )
        package_payload = json.loads(package_text)
        check(
            artifact="package_json",
            check_id="M227-E006-CFG-JSON",
            condition=True,
            detail="",
        )
    except ValueError as exc:
        package_payload = None
        check(
            artifact="package_json",
            check_id="M227-E006-CFG-00",
            condition=False,
            detail=str(exc),
        )
        check(
            artifact="package_json",
            check_id="M227-E006-CFG-JSON",
            condition=False,
            detail="package_json unavailable for JSON checks",
        )
    except json.JSONDecodeError as exc:
        package_payload = None
        check(
            artifact="package_json",
            check_id="M227-E006-CFG-00",
            condition=True,
            detail="",
        )
        check(
            artifact="package_json",
            check_id="M227-E006-CFG-JSON",
            condition=False,
            detail=f"unable to parse package.json: {exc}",
        )

    if isinstance(package_payload, dict):
        scripts = package_payload.get("scripts")
        check(
            artifact="package_json",
            check_id="M227-E006-CFG-SCRIPTS",
            condition=isinstance(scripts, dict),
            detail='expected top-level "scripts" object in package.json',
        )
    else:
        scripts = None
        check(
            artifact="package_json",
            check_id="M227-E006-CFG-SCRIPTS",
            condition=False,
            detail="package_json payload unavailable for script checks",
        )

    if isinstance(scripts, dict):
        for key_check in PACKAGE_SCRIPT_KEY_CHECKS:
            check(
                artifact="package_json",
                check_id=key_check.check_id,
                condition=key_check.script_key in scripts,
                detail=f'expected scripts["{key_check.script_key}"] to exist',
            )
        for script_check in PACKAGE_SCRIPT_CHECKS:
            actual = scripts.get(script_check.script_key)
            check(
                artifact="package_json",
                check_id=script_check.check_id,
                condition=actual == script_check.expected_value,
                detail=(
                    f'expected scripts["{script_check.script_key}"] to equal '
                    f'"{script_check.expected_value}"'
                ),
            )
    else:
        for key_check in PACKAGE_SCRIPT_KEY_CHECKS:
            check(
                artifact="package_json",
                check_id=key_check.check_id,
                condition=False,
                detail="scripts object unavailable",
            )
        for script_check in PACKAGE_SCRIPT_CHECKS:
            check(
                artifact="package_json",
                check_id=script_check.check_id,
                condition=False,
                detail="scripts object unavailable",
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

    if args.emit_json:
        print(canonical_json(summary), end="")

    if findings:
        if not args.emit_json:
            print(
                "m227-e006-semantic-conformance-lane-e-edge-case-expansion-and-robustness: "
                f"contract drift detected ({len(findings)} finding(s)).",
                file=sys.stderr,
            )
            for finding in findings:
                print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
            print(f"wrote summary: {display_path(summary_out)}", file=sys.stderr)
        return 1

    if not args.emit_json:
        print("m227-e006-semantic-conformance-lane-e-edge-case-expansion-and-robustness: OK")
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
            "m227-e006-semantic-conformance-lane-e-edge-case-expansion-and-robustness: "
            f"error: {exc}",
            file=sys.stderr,
        )
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
