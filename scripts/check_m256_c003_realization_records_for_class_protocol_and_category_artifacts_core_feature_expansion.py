#!/usr/bin/env python3
"""Validate M256-C003 executable realization-record expansion."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "M256-C003"
CONTRACT_ID = "objc3c-executable-realization-records/m256-c003-v1"
CLASS_MODEL = "class-and-metaclass-records-carry-bundle-object-and-super-owner-identities-plus-method-list-refs"
PROTOCOL_MODEL = "protocol-records-carry-owner-inherited-protocol-edges-and-split-instance-class-method-counts"
CATEGORY_MODEL = "category-records-carry-explicit-class-and-category-owner-identities-plus-attachment-and-adopted-protocol-edges"
FAIL_CLOSED_MODEL = "no-identity-edge-elision-no-out-of-band-graph-reconstruction"
SUMMARY_PREFIX = "; executable_realization_records = "

EXPECTATIONS_SNIPPETS = (
    "objc3c-executable-realization-records/m256-c003-v1",
    "class-and-metaclass-records-carry-bundle-object-and-super-owner-identities-plus-method-list-refs",
    "protocol-records-carry-owner-inherited-protocol-edges-and-split-instance-class-method-counts",
    "category-records-carry-explicit-class-and-category-owner-identities-plus-attachment-and-adopted-protocol-edges",
    "tmp/reports/m256/M256-C003/realization_records_summary.json",
)
PACKET_SNIPPETS = (
    "Packet: `M256-C003`",
    "Issue: `#7138`",
    "; executable_realization_records = ...",
    "tests/tooling/fixtures/native/m256_inheritance_override_realization_positive.objc3",
    "tests/tooling/fixtures/native/m256_protocol_conformance_positive.objc3",
    "tests/tooling/fixtures/native/m256_category_merge_positive.objc3",
)
ARCHITECTURE_SNIPPETS = (
    "## M256 executable realization records (C003)",
    "docs/contracts/m256_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion_c003_expectations.md",
    "check:objc3c:m256-c003-realization-records-for-class-protocol-and-category-artifacts",
    "check:objc3c:m256-c003-lane-c-readiness",
)
NATIVE_DOC_SNIPPETS = (
    "## Executable realization records (M256-C003)",
    "objc3c-executable-realization-records/m256-c003-v1",
    "; executable_realization_records = ...",
    "tmp/reports/m256/M256-C003/realization_records_summary.json",
)
LOWERING_SPEC_SNIPPETS = (
    "## M256 executable realization records (C003)",
    "objc3c-executable-realization-records/m256-c003-v1",
    "no-identity-edge-elision-no-out-of-band-graph-reconstruction",
)
METADATA_SPEC_SNIPPETS = (
    "## M256 executable realization-record metadata anchors (C003)",
    "class/metaclass records now carry bundle owner identity, object identity,",
    "protocol records now publish split instance/class method counts alongside",
    "category records now carry explicit class/category owner identities while",
)
LOWERING_HEADER_SNIPPETS = (
    "kObjc3ExecutableRealizationRecordsContractId",
    "kObjc3ExecutableRealizationClassRecordModel",
    "kObjc3ExecutableRealizationProtocolRecordModel",
    "kObjc3ExecutableRealizationCategoryRecordModel",
    "kObjc3ExecutableRealizationFailClosedModel",
    "Objc3ExecutableRealizationRecordsSummary();",
)
LOWERING_CPP_SNIPPETS = (
    "std::string Objc3ExecutableRealizationRecordsSummary()",
    'out << "contract=" << kObjc3ExecutableRealizationRecordsContractId',
    '<< ";class_record_model="',
    '<< ";protocol_record_model="',
    '<< ";category_record_model="',
)
PARSER_SNIPPETS = (
    "M256-C003 executable realization-record expansion anchor",
    "Realization",
)
SEMA_SNIPPETS = (
    "M256-C003 executable realization-record expansion anchor",
    "serializes them into realization-ready records",
)
IR_EMITTER_SNIPPETS = (
    "; executable_realization_records = ",
    '"class_object_identity", i);',
    '"metaclass_object_identity", i);',
    '"super_class_object_identity", i);',
    '"super_metaclass_object_identity", i);',
    '"category_owner_identity", i);',
    "private global { ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i1 }",
    "private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64 }",
)
RUNTIME_CPP_SNIPPETS = (
    "const char *bundle_owner_identity;",
    "const char *object_owner_identity;",
    "const char *super_owner_identity;",
    "std::uint64_t instance_method_count;",
    "std::uint64_t class_method_count;",
    "const char *class_owner_identity;",
    "const char *category_owner_identity;",
)
PACKAGE_SNIPPETS = (
    '"check:objc3c:m256-c003-realization-records-for-class-protocol-and-category-artifacts": "python scripts/check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py"',
    '"test:tooling:m256-c003-realization-records-for-class-protocol-and-category-artifacts": "python -m pytest tests/tooling/test_check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py -q"',
    '"check:objc3c:m256-c003-lane-c-readiness": "python scripts/run_m256_c003_lane_c_readiness.py"',
)

CLASS_RECORD_RE = re.compile(
    r"^@__objc3_meta_class_\d{4} = private global \{ \{ ptr, ptr, ptr, ptr, ptr, ptr \}, \{ ptr, ptr, ptr, ptr, ptr, ptr \} \}",
    re.MULTILINE,
)
PROTOCOL_RECORD_RE = re.compile(
    r"^@__objc3_meta_protocol_\d{4} = private global \{ ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i1 \}",
    re.MULTILINE,
)
CATEGORY_RECORD_RE = re.compile(
    r"^@__objc3_meta_category_\d{4} = private global \{ ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64 \}",
    re.MULTILINE,
)


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


@dataclass(frozen=True)
class CaseDefinition:
    key: str
    fixture: Path
    object_section: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    try:
        return str(path.relative_to(ROOT))
    except ValueError:
        return str(path)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[str], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for index, snippet in enumerate(snippets, start=1):
        passed += require(
            snippet in text,
            display_path(path),
            f"{MODE}-DOC-{index:02d}",
            f"missing snippet: {snippet}",
            failures,
        )
    return passed


def resolve_command(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    if sys.platform == "win32":
        candidate = Path("C:/Program Files/LLVM/bin") / f"{name}.exe"
        if candidate.exists():
            return str(candidate)
    return None


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=ROOT / "docs/contracts/m256_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion_c003_expectations.md")
    parser.add_argument("--packet-doc", type=Path, default=ROOT / "spec/planning/compiler/m256/m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion_packet.md")
    parser.add_argument("--architecture-doc", type=Path, default=ROOT / "native/objc3c/src/ARCHITECTURE.md")
    parser.add_argument("--native-doc", type=Path, default=ROOT / "docs/objc3c-native.md")
    parser.add_argument("--lowering-spec", type=Path, default=ROOT / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md")
    parser.add_argument("--metadata-spec", type=Path, default=ROOT / "spec/MODULE_METADATA_AND_ABI_TABLES.md")
    parser.add_argument("--lowering-header", type=Path, default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.h")
    parser.add_argument("--lowering-cpp", type=Path, default=ROOT / "native/objc3c/src/lower/objc3_lowering_contract.cpp")
    parser.add_argument("--parser-cpp", type=Path, default=ROOT / "native/objc3c/src/parse/objc3_parser.cpp")
    parser.add_argument("--sema-cpp", type=Path, default=ROOT / "native/objc3c/src/sema/objc3_semantic_passes.cpp")
    parser.add_argument("--ir-emitter", type=Path, default=ROOT / "native/objc3c/src/ir/objc3_ir_emitter.cpp")
    parser.add_argument("--runtime-cpp", type=Path, default=ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp")
    parser.add_argument("--package-json", type=Path, default=ROOT / "package.json")
    parser.add_argument("--native-exe", type=Path, default=ROOT / "artifacts/bin/objc3c-native.exe")
    parser.add_argument("--llvm-readobj", default="llvm-readobj")
    parser.add_argument("--probe-root", type=Path, default=ROOT / "tmp/reports/m256/M256-C003/probes")
    parser.add_argument("--summary-out", type=Path, default=ROOT / "tmp/reports/m256/M256-C003/realization_records_summary.json")
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(list(argv))


def parse_section_metrics(readobj_text: str, section_name: str) -> tuple[int | None, int | None]:
    marker = f"Name: {section_name}"
    start = readobj_text.find(marker)
    if start < 0:
        return None, None
    next_marker = readobj_text.find("Name:", start + len(marker))
    block = readobj_text[start: next_marker if next_marker >= 0 else None]
    raw_match = re.search(r"RawDataSize:\s*(\d+)", block)
    reloc_match = re.search(r"RelocationCount:\s*(\d+)", block)
    raw_size = int(raw_match.group(1)) if raw_match else None
    relocations = int(reloc_match.group(1)) if reloc_match else None
    return raw_size, relocations


def check_summary_line(ir_text: str, artifact: str, failures: list[Finding]) -> tuple[int, int, str]:
    checks_total = 0
    checks_passed = 0
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(SUMMARY_PREFIX)), "")
    for check_id, token in (
        ("SUMMARY-01", CONTRACT_ID),
        ("SUMMARY-02", CLASS_MODEL),
        ("SUMMARY-03", PROTOCOL_MODEL),
        ("SUMMARY-04", CATEGORY_MODEL),
        ("SUMMARY-05", FAIL_CLOSED_MODEL),
        ("SUMMARY-06", "class_record_count="),
        ("SUMMARY-07", "protocol_record_count="),
        ("SUMMARY-08", "category_record_count="),
    ):
        checks_total += 1
        checks_passed += require(token in boundary_line, artifact, f"{MODE}-{check_id}", f"summary line missing token: {token}", failures)
    return checks_total, checks_passed, boundary_line


def run_case(args: argparse.Namespace, case: CaseDefinition, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    probe_dir = args.probe_root / case.key.lower()
    probe_dir.mkdir(parents=True, exist_ok=True)
    compile_result = run_command(
        [str(args.native_exe), str(case.fixture), "--out-dir", str(probe_dir), "--emit-prefix", "module"],
        ROOT,
    )
    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, case.key, f"{MODE}-{case.key}-01", f"compile failed: {compile_result.stdout}{compile_result.stderr}", failures)

    ir_path = probe_dir / "module.ll"
    obj_path = probe_dir / "module.obj"
    checks_total += 2
    checks_passed += require(ir_path.exists(), case.key, f"{MODE}-{case.key}-02", f"missing IR: {display_path(ir_path)}", failures)
    checks_passed += require(obj_path.exists(), case.key, f"{MODE}-{case.key}-03", f"missing object: {display_path(obj_path)}", failures)
    if compile_result.returncode != 0 or not ir_path.exists() or not obj_path.exists():
        return checks_total, checks_passed, {"probe_dir": display_path(probe_dir)}

    ir_text = read_text(ir_path)
    summary_total, summary_passed, summary_line = check_summary_line(ir_text, display_path(ir_path), failures)
    checks_total += summary_total
    checks_passed += summary_passed

    if case.key == "CLASS":
        for check_id, token in (
            ("04", "@__objc3_meta_class_class_object_identity_0000 = private constant"),
            ("05", "@__objc3_meta_class_metaclass_object_identity_0000 = private constant"),
            ("06", "@__objc3_meta_class_super_class_object_identity_0001 = private constant"),
            ("07", "@__objc3_meta_class_super_metaclass_object_identity_0001 = private constant"),
        ):
            checks_total += 1
            checks_passed += require(token in ir_text, display_path(ir_path), f"{MODE}-CLASS-{check_id}", f"missing class realization token: {token}", failures)
        checks_total += 1
        checks_passed += require(CLASS_RECORD_RE.search(ir_text) is not None, display_path(ir_path), f"{MODE}-CLASS-08", "missing expanded class realization record layout", failures)
    elif case.key == "PROTOCOL":
        for check_id, token in (
            ("04", "@__objc3_meta_protocol_instance_method_list_ref_0000 = private global { i64, ptr, ptr } { i64 2"),
            ("05", "@__objc3_meta_protocol_class_method_list_ref_0000 = private global { i64, ptr, ptr } { i64 1"),
            ("06", "@__objc3_meta_protocol_inherited_protocol_refs_0000 = private global"),
        ):
            checks_total += 1
            checks_passed += require(token in ir_text, display_path(ir_path), f"{MODE}-PROTOCOL-{check_id}", f"missing protocol realization token: {token}", failures)
        checks_total += 1
        checks_passed += require(PROTOCOL_RECORD_RE.search(ir_text) is not None, display_path(ir_path), f"{MODE}-PROTOCOL-07", "missing expanded protocol realization record layout", failures)
    elif case.key == "CATEGORY":
        for check_id, token in (
            ("04", "@__objc3_meta_category_class_owner_identity_0000 = private constant"),
            ("05", "@__objc3_meta_category_category_owner_identity_0000 = private constant"),
            ("06", "@__objc3_meta_category_attachments_0000 = private global"),
            ("07", "@__objc3_meta_category_adopted_protocol_refs_0000 = private global"),
        ):
            checks_total += 1
            checks_passed += require(token in ir_text, display_path(ir_path), f"{MODE}-CATEGORY-{check_id}", f"missing category realization token: {token}", failures)
        checks_total += 1
        checks_passed += require(CATEGORY_RECORD_RE.search(ir_text) is not None, display_path(ir_path), f"{MODE}-CATEGORY-08", "missing expanded category realization record layout", failures)

    readobj_path = resolve_command(args.llvm_readobj)
    checks_total += 1
    checks_passed += require(readobj_path is not None, args.llvm_readobj, f"{MODE}-{case.key}-09", f"unable to resolve {args.llvm_readobj}", failures)
    raw_size = None
    relocations = None
    if readobj_path is not None:
        readobj_result = run_command([readobj_path, "--sections", str(obj_path)], ROOT)
        checks_total += 1
        checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), f"{MODE}-{case.key}-10", f"llvm-readobj failed: {readobj_result.stderr.strip()}", failures)
        if readobj_result.returncode == 0:
            raw_size, relocations = parse_section_metrics(readobj_result.stdout, case.object_section)
            checks_total += 2
            checks_passed += require(raw_size is not None and raw_size > 64, display_path(obj_path), f"{MODE}-{case.key}-11", f"section {case.object_section} must have nontrivial bytes, saw {raw_size}", failures)
            checks_passed += require(relocations is not None and relocations >= 4, display_path(obj_path), f"{MODE}-{case.key}-12", f"section {case.object_section} must have relocations, saw {relocations}", failures)

    return checks_total, checks_passed, {
        "fixture": display_path(case.fixture),
        "probe_dir": display_path(probe_dir),
        "module_ir": display_path(ir_path),
        "module_obj": display_path(obj_path),
        "summary_line": summary_line,
        "section_name": case.object_section,
        "raw_size": raw_size,
        "relocations": relocations,
    }


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
        (args.runtime_cpp, RUNTIME_CPP_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    if not args.skip_dynamic_probes:
        for case in (
            CaseDefinition("CLASS", ROOT / "tests/tooling/fixtures/native/m256_inheritance_override_realization_positive.objc3", "objc3.runtime.class_descriptors"),
            CaseDefinition("PROTOCOL", ROOT / "tests/tooling/fixtures/native/m256_protocol_conformance_positive.objc3", "objc3.runtime.protocol_descriptors"),
            CaseDefinition("CATEGORY", ROOT / "tests/tooling/fixtures/native/m256_category_merge_positive.objc3", "objc3.runtime.category_descriptors"),
        ):
            case_total, case_passed, case_payload = run_case(args, case, failures)
            checks_total += case_total
            checks_passed += case_passed
            dynamic_cases.append(case_payload)

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        print(f"[fail] {MODE} ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        for finding in failures:
            print(f"- {finding.check_id} [{finding.artifact}] {finding.detail}", file=sys.stderr)
        print(f"[info] summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    print(f"[info] summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
