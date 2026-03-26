#!/usr/bin/env python3
"""Checker for M315-B003 native-source milestone-marker removal."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SRC_ROOT = ROOT / "native" / "objc3c" / "src"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-B003" / "native_source_marker_removal_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_milestone_marker_removal_from_native_source_core_feature_implementation_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b003_milestone_marker_removal_from_native_source_core_feature_implementation_packet.md"
RESULT_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b003_milestone_marker_removal_from_native_source_core_feature_implementation_result.json"
MATCH_PATTERN = re.compile(r"m[0-9]{3}-[a-z][0-9]{3}", re.IGNORECASE)
COMMENT_PATTERN = re.compile(r"(?m)^\s*//\s*M[0-9]{3}-[A-Z][0-9]{3}(?:/M[0-9]{3}-[A-Z][0-9]{3})*(?=\s)")
CODE_SUFFIXES = {".h", ".hpp", ".cpp", ".cc", ".cxx"}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def measure() -> dict[str, object]:
    rewritten_files: list[str] = []
    total_matches = 0
    leading_comment_markers = 0
    for path in SRC_ROOT.rglob("*"):
        if not path.is_file() or path.suffix.lower() not in CODE_SUFFIXES:
            continue
        text = path.read_text(encoding="utf-8")
        total_matches += len(MATCH_PATTERN.findall(text))
        comment_count = len(COMMENT_PATTERN.findall(text))
        leading_comment_markers += comment_count
        rel = path.relative_to(SRC_ROOT).as_posix()
        if comment_count == 0 and rel in EXPECTED_REWRITTEN_FILES:
            rewritten_files.append(rel)
    return {
        "total_code_matches": total_matches,
        "leading_comment_markers": leading_comment_markers,
        "rewritten_files_without_leading_markers": sorted(rewritten_files),
    }


EXPECTED_REWRITTEN_FILES = [
    "ast/objc3_ast.h",
    "driver/objc3_compilation_driver.cpp",
    "driver/objc3_objc3_path.cpp",
    "driver/objc3_objectivec_path.cpp",
    "io/objc3_manifest_artifacts.cpp",
    "io/objc3_process.cpp",
    "ir/objc3_ir_emitter.cpp",
    "lex/objc3_lexer.cpp",
    "libobjc3c_frontend/frontend_anchor.cpp",
    "lower/objc3_lowering_contract.cpp",
    "lower/objc3_lowering_contract.h",
    "parse/objc3_parser.cpp",
    "pipeline/objc3_frontend_artifacts.cpp",
    "pipeline/objc3_frontend_pipeline.cpp",
    "pipeline/objc3_frontend_types.h",
    "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h",
    "pipeline/objc3_runtime_import_surface.cpp",
    "runtime/objc3_runtime.cpp",
    "runtime/objc3_runtime.h",
    "runtime/objc3_runtime_bootstrap_internal.h",
    "sema/objc3_sema_contract.h",
    "sema/objc3_sema_pass_manager.cpp",
    "sema/objc3_semantic_passes.cpp",
    "token/objc3_token_contract.h",
]


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    result = json.loads(read_text(RESULT_JSON))
    measured = measure()

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-native-source-marker-removal/m315-b003-v1`" in expectations, str(EXPECTATIONS_DOC), "M315-B003-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("comment-marker layer has been removed" in expectations, str(EXPECTATIONS_DOC), "M315-B003-EXP-02", "expectations missing bounded-scope note", failures)
    checks_passed += require("leading `// MNNN-LNNN ...` comment markers" in packet, str(PACKET_DOC), "M315-B003-PKT-01", "packet missing implemented-scope note", failures)
    checks_passed += require("Next issue: `M315-B004`." in packet, str(PACKET_DOC), "M315-B003-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(result.get("mode") == "m315-b003-native-source-marker-removal-v1", str(RESULT_JSON), "M315-B003-RES-01", "mode drifted", failures)
    checks_passed += require(result.get("contract_id") == "objc3c-cleanup-native-source-marker-removal/m315-b003-v1", str(RESULT_JSON), "M315-B003-RES-02", "contract id drifted", failures)
    checks_passed += require(result.get("implemented_scope", {}).get("leading_comment_marker_removal") is True, str(RESULT_JSON), "M315-B003-RES-03", "leading comment marker scope drifted", failures)
    checks_passed += require(result.get("implemented_scope", {}).get("runtime_visible_contract_id_rewrite") is False, str(RESULT_JSON), "M315-B003-RES-04", "runtime-visible contract id scope drifted", failures)
    checks_passed += require(result.get("implemented_scope", {}).get("generated_payload_rewrite") is False, str(RESULT_JSON), "M315-B003-RES-05", "generated payload scope drifted", failures)
    checks_passed += require(result.get("next_issue") == "M315-B004", str(RESULT_JSON), "M315-B003-RES-06", "next issue drifted", failures)

    checks_total += 5
    checks_passed += require(result.get("post_state", {}).get("leading_comment_markers_remaining") == measured["leading_comment_markers"] == 0, str(RESULT_JSON), "M315-B003-POST-01", "leading comment markers remain", failures)
    checks_passed += require(result.get("post_state", {}).get("native_code_match_count_remaining") == measured["total_code_matches"] == 65, str(RESULT_JSON), "M315-B003-POST-02", "remaining code match count drifted", failures)
    checks_passed += require(result.get("post_state", {}).get("rewritten_file_count") == len(EXPECTED_REWRITTEN_FILES) == 24, str(RESULT_JSON), "M315-B003-POST-03", "rewritten file count drifted", failures)
    checks_passed += require(result.get("rewritten_files") == EXPECTED_REWRITTEN_FILES, str(RESULT_JSON), "M315-B003-POST-04", "rewritten file list drifted", failures)
    checks_passed += require(measured["rewritten_files_without_leading_markers"] == EXPECTED_REWRITTEN_FILES, str(RESULT_JSON), "M315-B003-POST-05", "expected rewritten files still contain leading markers", failures)

    checks_total += 4
    checks_passed += require(result.get("downstream_ownership", {}).get("ir_fixture_compatibility_semantics") == "M315-B004", str(RESULT_JSON), "M315-B003-OWN-01", "B004 ownership drifted", failures)
    checks_passed += require(result.get("downstream_ownership", {}).get("remaining_edge_sweep") == "M315-B005", str(RESULT_JSON), "M315-B003-OWN-02", "B005 ownership drifted", failures)
    checks_passed += require(result.get("downstream_ownership", {}).get("source_of_truth_contract") == "M315-C001", str(RESULT_JSON), "M315-B003-OWN-03", "C001 ownership drifted", failures)
    checks_passed += require(result.get("downstream_ownership", {}).get("authenticity_schema") == "M315-C002", str(RESULT_JSON), "M315-B003-OWN-04", "C002 ownership drifted", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": result["mode"],
        "contract_id": result["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "leading_comment_markers_remaining": measured["leading_comment_markers"],
        "native_code_match_count_remaining": measured["total_code_matches"],
        "rewritten_files": EXPECTED_REWRITTEN_FILES,
        "next_issue": "M315-B004",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-B003 native-source marker removal checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
