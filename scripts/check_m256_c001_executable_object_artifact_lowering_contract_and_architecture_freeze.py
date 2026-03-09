#!/usr/bin/env python3
"""Fail-closed contract checker for M256-C001 executable object artifact lowering freeze."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m256-c001-executable-object-artifact-lowering-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-executable-object-artifact-lowering/m256-c001-v1"
BOUNDARY_COMMENT_PREFIX = "; executable_object_artifact_lowering = contract=objc3c-executable-object-artifact-lowering/m256-c001-v1"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m256_executable_object_artifact_lowering_contract_and_architecture_freeze_c001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m256" / "m256_c001_executable_object_artifact_lowering_contract_and_architecture_freeze_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_inheritance_override_realization_positive.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_category_merge_positive.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m256" / "executable-object-artifact-lowering"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m256/M256-C001/executable_object_artifact_lowering_contract_summary.json")


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-DOC-EXP-01", "# M256 Executable Object Artifact Lowering Contract and Architecture Freeze Expectations (C001)"),
    SnippetCheck("M256-C001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M256-C001-DOC-EXP-03", "`implementation-owner-identity-to-llvm-definition-symbol`"),
    SnippetCheck("M256-C001-DOC-EXP-04", "`class-metaclass-and-category-descriptor-bundles-point-to-owner-scoped-method-list-ref-records`"),
    SnippetCheck("M256-C001-DOC-EXP-05", "`tmp/reports/m256/M256-C001/executable_object_artifact_lowering_contract_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-DOC-PKT-01", "# M256-C001 Executable Object Artifact Lowering Contract and Architecture Freeze Packet"),
    SnippetCheck("M256-C001-DOC-PKT-02", "Packet: `M256-C001`"),
    SnippetCheck("M256-C001-DOC-PKT-03", "Issue: `#7136`"),
    SnippetCheck("M256-C001-DOC-PKT-04", "`m256_inheritance_override_realization_positive.objc3`"),
    SnippetCheck("M256-C001-DOC-PKT-05", "`m256_category_merge_positive.objc3`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-ARCH-01", "## M256 executable object artifact lowering (C001)"),
    SnippetCheck("M256-C001-ARCH-02", "implementation-owned method-list entries may point at concrete"),
    SnippetCheck("M256-C001-ARCH-03", "check:objc3c:m256-c001-lane-c-readiness"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-NDOC-01", "## Executable object artifact lowering (M256-C001)"),
    SnippetCheck("M256-C001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-C001-NDOC-03", "implementation-owned method-list entries bind by owner identity"),
    SnippetCheck("M256-C001-NDOC-04", "`tmp/reports/m256/M256-C001/executable_object_artifact_lowering_contract_summary.json`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-SPC-01", "## M256 executable object artifact lowering (C001)"),
    SnippetCheck("M256-C001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-C001-SPC-03", "implementation-owned method entries to LLVM definition symbols by owner"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-META-01", "## M256 executable object artifact lowering metadata anchors (C001)"),
    SnippetCheck("M256-C001-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M256-C001-META-03", "`selector-owner-return-arity-implementation-symbol-has-body`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-LHDR-01", "kObjc3ExecutableObjectArtifactLoweringContractId"),
    SnippetCheck("M256-C001-LHDR-02", "kObjc3ExecutableObjectArtifactLoweringMethodBodyBindingModel"),
    SnippetCheck("M256-C001-LHDR-03", "kObjc3ExecutableObjectArtifactLoweringRealizationRecordModel"),
    SnippetCheck("M256-C001-LHDR-04", "Objc3ExecutableObjectArtifactLoweringSummary()"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-LCPP-01", "Objc3ExecutableObjectArtifactLoweringSummary()"),
    SnippetCheck("M256-C001-LCPP-02", "M256-C001 executable object artifact lowering freeze anchor"),
    SnippetCheck("M256-C001-LCPP-03", "non_goals=no-new-descriptor-families-no-bootstrap-rebinding-no-protocol-executable-realization"),
)

PARSER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-PARSE-01", "M256-C001 executable object artifact lowering freeze anchor: parser"),
)

SEMA_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-SEMA-01", "M256-C001 executable object artifact lowering freeze anchor: sema owns the"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M256-C001-IR-01", 'out << "; executable_object_artifact_lowering = "'),
    SnippetCheck("M256-C001-IR-02", "M256-C001 executable object artifact lowering freeze anchor:"),
    SnippetCheck("M256-C001-IR-03", "bound_method_entry_count"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M256-C001-PKG-01",
        '"check:objc3c:m256-c001-executable-object-artifact-lowering-contract": "python scripts/check_m256_c001_executable_object_artifact_lowering_contract_and_architecture_freeze.py"',
    ),
    SnippetCheck(
        "M256-C001-PKG-02",
        '"test:tooling:m256-c001-executable-object-artifact-lowering-contract": "python -m pytest tests/tooling/test_check_m256_c001_executable_object_artifact_lowering_contract_and_architecture_freeze.py -q"',
    ),
    SnippetCheck(
        "M256-C001-PKG-03",
        '"check:objc3c:m256-c001-lane-c-readiness": "python scripts/run_m256_c001_lane_c_readiness.py"',
    ),
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-cpp", type=Path, default=DEFAULT_SEMA_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_native_probe(
    *,
    args: argparse.Namespace,
    fixture: Path,
    case_id: str,
    out_leaf: str,
    minimum_method_definitions: int,
    minimum_bound_method_entries: int,
    expected_aggregate_symbol: str,
    expected_ref_token: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, Any]]:
    probe_dir = args.probe_root.resolve() / out_leaf
    command = [
        str(args.native_exe.resolve()),
        str(fixture.resolve()),
        "--out-dir",
        str(probe_dir),
        "--emit-prefix",
        "module",
    ]
    result = run_command(command, ROOT)

    ir_path = probe_dir / "module.ll"
    obj_path = probe_dir / "module.obj"
    case: dict[str, Any] = {
        "case_id": case_id,
        "fixture": display_path(fixture),
        "command": command,
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
    }

    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), f"{case_id}-NATIVE-EXISTS", "objc3c-native.exe is missing", failures)
    checks_total += 1
    checks_passed += require(fixture.exists(), display_path(fixture), f"{case_id}-FIXTURE", "fixture is missing", failures)
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(ir_path), f"{case_id}-STATUS", "native probe must exit 0", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), f"{case_id}-IR", "native probe must emit module.ll", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(obj_path), f"{case_id}-OBJ", "native probe must emit module.obj", failures)

    if not ir_path.exists():
        case["stdout"] = result.stdout
        case["stderr"] = result.stderr
        return checks_total, checks_passed, case

    ir_text = read_text(ir_path)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    method_definition_count = len(re.findall(r"^define .*@objc3_method_", ir_text, flags=re.MULTILINE))
    bound_method_entry_count = len(re.findall(r"ptr @objc3_method_", ir_text))
    case["boundary_line"] = boundary_line
    case["method_definition_count"] = method_definition_count
    case["bound_method_entry_count"] = bound_method_entry_count

    checks_total += 1
    checks_passed += require(bool(boundary_line), display_path(ir_path), f"{case_id}-BOUNDARY", "IR must publish the executable object artifact lowering boundary line", failures)
    checks_total += 1
    checks_passed += require("method_body_binding_model=implementation-owner-identity-to-llvm-definition-symbol" in boundary_line, display_path(ir_path), f"{case_id}-METHOD-BODY-MODEL", "boundary line must carry the method-body binding model", failures)
    checks_total += 1
    checks_passed += require("realization_record_model=class-metaclass-and-category-descriptor-bundles-point-to-owner-scoped-method-list-ref-records" in boundary_line, display_path(ir_path), f"{case_id}-REALIZATION-MODEL", "boundary line must carry the realization-record model", failures)
    checks_total += 1
    checks_passed += require("method_entry_payload_model=selector-owner-return-arity-implementation-symbol-has-body" in boundary_line, display_path(ir_path), f"{case_id}-ENTRY-MODEL", "boundary line must carry the method-entry payload model", failures)
    checks_total += 1
    checks_passed += require("fail_closed_model=no-synthetic-implementation-symbols-no-rebound-legality-no-new-section-families" in boundary_line, display_path(ir_path), f"{case_id}-FAIL-CLOSED", "boundary line must carry the fail-closed model", failures)
    checks_total += 1
    checks_passed += require(method_definition_count >= minimum_method_definitions, display_path(ir_path), f"{case_id}-METHOD-DEFINITIONS", f"expected at least {minimum_method_definitions} @objc3_method_ definitions", failures)
    checks_total += 1
    checks_passed += require(bound_method_entry_count >= minimum_bound_method_entries, display_path(ir_path), f"{case_id}-BOUND-ENTRIES", f"expected at least {minimum_bound_method_entries} bound method-list entries", failures)
    checks_total += 1
    checks_passed += require(expected_aggregate_symbol in ir_text, display_path(ir_path), f"{case_id}-AGGREGATE", f"IR must retain {expected_aggregate_symbol}", failures)
    checks_total += 1
    checks_passed += require(expected_ref_token in ir_text, display_path(ir_path), f"{case_id}-REF-TOKEN", f"IR must retain {expected_ref_token}", failures)
    checks_total += 1
    checks_passed += require(obj_path.stat().st_size > 0 if obj_path.exists() else False, display_path(obj_path), f"{case_id}-OBJ-SIZE", "module.obj must be non-empty", failures)

    return checks_total, checks_passed, case


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.sema_cpp, SEMA_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    dynamic_probes_executed = False
    if not args.skip_dynamic_probes:
        dynamic_probes_executed = True
        class_total, class_passed, class_case = run_native_probe(
            args=args,
            fixture=args.class_fixture,
            case_id="M256-C001-CASE-CLASS",
            out_leaf="class-positive",
            minimum_method_definitions=4,
            minimum_bound_method_entries=4,
            expected_aggregate_symbol="@__objc3_sec_class_descriptors",
            expected_ref_token="instance_method_list_ref",
            failures=failures,
        )
        checks_total += class_total
        checks_passed += class_passed
        dynamic_cases.append(class_case)

        category_total, category_passed, category_case = run_native_probe(
            args=args,
            fixture=args.category_fixture,
            case_id="M256-C001-CASE-CATEGORY",
            out_leaf="category-positive",
            minimum_method_definitions=3,
            minimum_bound_method_entries=3,
            expected_aggregate_symbol="@__objc3_sec_category_descriptors",
            expected_ref_token="class_method_list_ref",
            failures=failures,
        )
        checks_total += category_total
        checks_passed += category_passed
        dynamic_cases.append(category_case)

    payload = {
        "mode": MODE,
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "evidence_path": str(args.summary_out).replace("\\", "/"),
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
