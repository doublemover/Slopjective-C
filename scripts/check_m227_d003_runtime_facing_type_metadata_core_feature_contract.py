#!/usr/bin/env python3
"""Fail-closed validator for M227-D003 runtime-facing type metadata core feature."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-d003-runtime-facing-type-metadata-core-feature-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "typed_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_typed_sema_to_lowering_contract_surface.h",
    "parse_readiness_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_runtime_facing_type_metadata_core_feature_d003_expectations.md",
    "planning_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_d003_runtime_facing_type_metadata_core_feature_packet.md",
}

ARTIFACT_ORDER: tuple[str, ...] = tuple(ARTIFACTS.keys())
ARTIFACT_RANK = {name: index for index, name in enumerate(ARTIFACT_ORDER)}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types_header": (
        ("M227-D003-TYP-01", "bool protocol_category_handoff_deterministic = false;"),
        ("M227-D003-TYP-02", "bool class_protocol_category_linking_handoff_deterministic = false;"),
        ("M227-D003-TYP-03", "bool selector_normalization_handoff_deterministic = false;"),
        ("M227-D003-TYP-04", "bool property_attribute_handoff_deterministic = false;"),
        ("M227-D003-TYP-05", "bool typed_core_feature_expansion_consistent = false;"),
        ("M227-D003-TYP-06", "std::size_t typed_core_feature_expansion_case_count = 0;"),
        ("M227-D003-TYP-07", "std::size_t typed_core_feature_expansion_passed_case_count = 0;"),
        ("M227-D003-TYP-08", "std::size_t typed_core_feature_expansion_failed_case_count = 0;"),
        ("M227-D003-TYP-09", "std::string typed_core_feature_expansion_key;"),
        ("M227-D003-TYP-10", "bool protocol_category_deterministic = false;"),
        ("M227-D003-TYP-11", "bool class_protocol_category_linking_deterministic = false;"),
        ("M227-D003-TYP-12", "bool selector_normalization_deterministic = false;"),
        ("M227-D003-TYP-13", "bool property_attribute_deterministic = false;"),
        ("M227-D003-TYP-14", "bool typed_sema_core_feature_expansion_consistent = false;"),
        ("M227-D003-TYP-15", "std::size_t typed_sema_core_feature_expansion_case_count = 0;"),
        ("M227-D003-TYP-16", "std::size_t typed_sema_core_feature_expansion_passed_case_count = 0;"),
        ("M227-D003-TYP-17", "std::size_t typed_sema_core_feature_expansion_failed_case_count = 0;"),
        ("M227-D003-TYP-18", "std::string typed_sema_core_feature_expansion_key;"),
    ),
    "typed_surface_header": (
        (
            "M227-D003-SUR-01",
            "inline constexpr std::size_t kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount = 4u;",
        ),
        ("M227-D003-SUR-02", "BuildObjc3TypedSemaToLoweringCoreFeatureExpansionKey("),
        ("M227-D003-SUR-03", "surface.protocol_category_handoff_deterministic ="),
        ("M227-D003-SUR-04", "surface.class_protocol_category_linking_handoff_deterministic ="),
        ("M227-D003-SUR-05", "surface.selector_normalization_handoff_deterministic ="),
        ("M227-D003-SUR-06", "surface.property_attribute_handoff_deterministic ="),
        ("M227-D003-SUR-07", "surface.typed_core_feature_expansion_case_count ="),
        ("M227-D003-SUR-08", "surface.typed_core_feature_expansion_passed_case_count ="),
        ("M227-D003-SUR-09", "surface.typed_core_feature_expansion_failed_case_count ="),
        ("M227-D003-SUR-10", "surface.typed_core_feature_expansion_consistent ="),
        ("M227-D003-SUR-11", "surface.typed_core_feature_expansion_key ="),
        ("M227-D003-SUR-12", "const bool typed_core_feature_expansion_key_ready ="),
        ("M227-D003-SUR-13", "surface.typed_core_feature_consistent ="),
        ("M227-D003-SUR-14", "surface.typed_core_feature_expansion_consistent &&"),
        ("M227-D003-SUR-15", "typed_core_feature_expansion_key_ready;"),
        (
            "M227-D003-SUR-16",
            "surface.failure_reason = \"typed sema-to-lowering core feature expansion is inconsistent\";",
        ),
        ("M227-D003-SUR-17", "surface.failure_reason = \"typed core feature expansion key is empty\";"),
    ),
    "parse_readiness_header": (
        ("M227-D003-REA-01", "surface.protocol_category_deterministic ="),
        ("M227-D003-REA-02", "surface.class_protocol_category_linking_deterministic ="),
        ("M227-D003-REA-03", "surface.selector_normalization_deterministic ="),
        ("M227-D003-REA-04", "surface.property_attribute_deterministic ="),
        ("M227-D003-REA-05", "surface.typed_sema_core_feature_expansion_consistent ="),
        ("M227-D003-REA-06", "surface.typed_sema_core_feature_expansion_case_count ="),
        ("M227-D003-REA-07", "surface.typed_sema_core_feature_expansion_passed_case_count ="),
        ("M227-D003-REA-08", "surface.typed_sema_core_feature_expansion_failed_case_count ="),
        ("M227-D003-REA-09", "surface.typed_sema_core_feature_expansion_key ="),
        (
            "M227-D003-REA-10",
            "surface.typed_sema_core_feature_expansion_case_count ==\n          kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount &&",
        ),
        ("M227-D003-REA-11", "const bool typed_core_feature_expansion_ready ="),
        ("M227-D003-REA-12", "typed_core_feature_expansion_ready &&"),
        ("M227-D003-REA-13", "surface.protocol_category_deterministic &&"),
        ("M227-D003-REA-14", "surface.class_protocol_category_linking_deterministic &&"),
        ("M227-D003-REA-15", "surface.selector_normalization_deterministic &&"),
        ("M227-D003-REA-16", "surface.property_attribute_deterministic &&"),
        ("M227-D003-REA-17", "!surface.typed_sema_core_feature_expansion_key.empty() &&"),
        ("M227-D003-REA-18", "surface.failure_reason = \"protocol/category handoff is not deterministic\";"),
        (
            "M227-D003-REA-19",
            "surface.failure_reason = \"class/protocol/category linking handoff is not deterministic\";",
        ),
        ("M227-D003-REA-20", "surface.failure_reason = \"selector normalization handoff is not deterministic\";"),
        ("M227-D003-REA-21", "surface.failure_reason = \"property attribute handoff is not deterministic\";"),
        (
            "M227-D003-REA-22",
            "surface.failure_reason = \"typed sema-to-lowering core feature expansion is inconsistent\";",
        ),
        ("M227-D003-REA-23", "surface.failure_reason = \"typed sema-to-lowering core feature expansion key is empty\";"),
    ),
    "contract_doc": (
        (
            "M227-D003-DOC-01",
            "Contract ID: `objc3c-runtime-facing-type-metadata-core-feature/m227-d003-v1`",
        ),
        (
            "M227-D003-DOC-02",
            "`python scripts/check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py`",
        ),
        (
            "M227-D003-DOC-03",
            "`python -m pytest tests/tooling/test_check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py -q`",
        ),
        ("M227-D003-DOC-04", "`npm run build:objc3c-native`"),
        (
            "M227-D003-DOC-05",
            "`tmp/reports/m227/M227-D003/runtime_facing_type_metadata_core_feature_contract_summary.json`",
        ),
    ),
    "planning_doc": (
        ("M227-D003-PLN-01", "# M227-D003 Runtime-Facing Type Metadata Core Feature Packet"),
        ("M227-D003-PLN-02", "Packet: `M227-D003`"),
        (
            "M227-D003-PLN-03",
            "scripts/check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py",
        ),
        (
            "M227-D003-PLN-04",
            "tmp/reports/m227/M227-D003/runtime_facing_type_metadata_core_feature_contract_summary.json",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parse_readiness_header": (
        (
            "M227-D003-REA-FORBID-01",
            "const bool typed_core_feature_ready =\n      surface.typed_handoff_key_deterministic &&\n      surface.typed_sema_core_feature_consistent &&\n      !surface.typed_sema_core_feature_key.empty();",
        ),
    ),
}

SEMANTIC_CHECK_TOTAL = 5


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


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


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m227/M227-D003/runtime_facing_type_metadata_core_feature_contract_summary.json"
        ),
    )
    return parser.parse_args(argv)


def collect_required_findings(*, artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
        if snippet not in text:
            findings.append(
                Finding(
                    artifact=artifact,
                    check_id=check_id,
                    detail=f"expected snippet missing: {snippet}",
                )
            )
    return findings


def collect_forbidden_findings(*, artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
        if snippet in text:
            findings.append(
                Finding(
                    artifact=artifact,
                    check_id=check_id,
                    detail=f"forbidden snippet present: {snippet}",
                )
            )
    return findings


def find_semantic_handoff_block(text: str) -> str | None:
    match = re.search(
        r"const bool semantic_handoff_deterministic =\s*(.*?);",
        text,
        re.S,
    )
    if match is None:
        return None
    return match.group(1)


def find_assignment_block(text: str, lhs: str) -> str | None:
    match = re.search(rf"{re.escape(lhs)}\s*=\s*(.*?);", text, re.S)
    if match is None:
        return None
    return match.group(1)


def collect_semantic_findings(texts: dict[str, str]) -> list[Finding]:
    findings: list[Finding] = []

    typed_surface = texts["typed_surface_header"]
    case_count_match = re.search(
        r"kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount\s*=\s*(\d+)u;",
        typed_surface,
    )
    if case_count_match is None:
        findings.append(
            Finding(
                artifact="typed_surface_header",
                check_id="M227-D003-SEM-01",
                detail="unable to parse kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount",
            )
        )
    else:
        case_count = int(case_count_match.group(1))
        if case_count != 4:
            findings.append(
                Finding(
                    artifact="typed_surface_header",
                    check_id="M227-D003-SEM-01",
                    detail=(
                        "typed core feature expansion case-count constant drifted: "
                        f"expected 4, got {case_count}"
                    ),
                )
            )

    typed_order_markers = (
        "surface.typed_core_feature_expansion_case_count =",
        "surface.typed_core_feature_expansion_passed_case_count =",
        "surface.typed_core_feature_expansion_consistent =",
        "surface.typed_core_feature_expansion_key =",
        "surface.typed_core_feature_consistent =",
    )
    typed_positions = [typed_surface.find(marker) for marker in typed_order_markers]
    if any(pos < 0 for pos in typed_positions):
        findings.append(
            Finding(
                artifact="typed_surface_header",
                check_id="M227-D003-SEM-02",
                detail="unable to locate typed core feature expansion ordering anchors",
            )
        )
    elif typed_positions != sorted(typed_positions):
        findings.append(
            Finding(
                artifact="typed_surface_header",
                check_id="M227-D003-SEM-02",
                detail="typed core feature expansion accounting/key ordering drifted",
            )
        )

    parse_readiness = texts["parse_readiness_header"]
    readiness_order_markers = (
        "surface.typed_sema_core_feature_expansion_consistent =",
        "const bool typed_core_feature_expansion_case_accounting_consistent =",
        "const bool typed_core_feature_expansion_ready =",
        "const bool typed_core_feature_ready =",
        "!surface.typed_sema_core_feature_expansion_key.empty() &&",
    )
    readiness_positions = [parse_readiness.find(marker) for marker in readiness_order_markers]
    if any(pos < 0 for pos in readiness_positions):
        findings.append(
            Finding(
                artifact="parse_readiness_header",
                check_id="M227-D003-SEM-03",
                detail="unable to locate parse/lowering expansion gating ordering anchors",
            )
        )
    elif readiness_positions != sorted(readiness_positions):
        findings.append(
            Finding(
                artifact="parse_readiness_header",
                check_id="M227-D003-SEM-03",
                detail="parse/lowering expansion gating ordering drifted",
            )
        )

    semantic_handoff_block = find_semantic_handoff_block(parse_readiness)
    required_semantic_tokens = (
        "surface.protocol_category_deterministic",
        "surface.class_protocol_category_linking_deterministic",
        "surface.selector_normalization_deterministic",
        "surface.property_attribute_deterministic",
    )
    if semantic_handoff_block is None:
        findings.append(
            Finding(
                artifact="parse_readiness_header",
                check_id="M227-D003-SEM-04",
                detail="unable to parse semantic_handoff_deterministic definition",
            )
        )
    else:
        for token in required_semantic_tokens:
            if token not in semantic_handoff_block:
                findings.append(
                    Finding(
                        artifact="parse_readiness_header",
                        check_id="M227-D003-SEM-04",
                        detail=f"semantic_handoff_deterministic missing runtime-facing token: {token}",
                    )
                )

    typed_semantic_handoff_block = find_assignment_block(
        typed_surface,
        "surface.semantic_handoff_deterministic",
    )
    if typed_semantic_handoff_block is None:
        findings.append(
            Finding(
                artifact="typed_surface_header",
                check_id="M227-D003-SEM-05",
                detail="unable to parse typed surface semantic_handoff_deterministic definition",
            )
        )
    else:
        for token in required_semantic_tokens:
            typed_token = token.replace("_deterministic", "_handoff_deterministic")
            if typed_token not in typed_semantic_handoff_block:
                findings.append(
                    Finding(
                        artifact="typed_surface_header",
                        check_id="M227-D003-SEM-05",
                        detail=f"typed semantic_handoff_deterministic missing runtime-facing token: {typed_token}",
                    )
                )

    return findings


def total_checks() -> int:
    required_total = sum(len(snippets) for snippets in REQUIRED_SNIPPETS.values())
    forbidden_total = sum(len(snippets) for snippets in FORBIDDEN_SNIPPETS.values())
    return required_total + forbidden_total + SEMANTIC_CHECK_TOTAL


def finding_sort_key(finding: Finding) -> tuple[int, str, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in findings: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    try:
        texts = {artifact: load_text(path, artifact=artifact) for artifact, path in ARTIFACTS.items()}
        findings: list[Finding] = []
        for artifact, text in texts.items():
            findings.extend(collect_required_findings(artifact=artifact, text=text))
            findings.extend(collect_forbidden_findings(artifact=artifact, text=text))
        findings.extend(collect_semantic_findings(texts))
        findings = sorted(findings, key=finding_sort_key)
    except ValueError as exc:
        print(
            "m227-d003-runtime-facing-type-metadata-core-feature: "
            f"error: {exc}",
            file=sys.stderr,
        )
        return 2

    checks_total = total_checks()
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        print(
            "m227-d003-runtime-facing-type-metadata-core-feature: "
            f"contract drift detected ({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("m227-d003-runtime-facing-type-metadata-core-feature: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={summary['checks_passed']}")
    print("- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> int:
    try:
        return run(sys.argv[1:])
    except Exception as exc:  # noqa: BLE001
        print(
            "m227-d003-runtime-facing-type-metadata-core-feature: "
            f"error: {exc}",
            file=sys.stderr,
        )
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
