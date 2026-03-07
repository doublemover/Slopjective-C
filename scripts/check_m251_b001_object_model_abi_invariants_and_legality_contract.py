#!/usr/bin/env python3
"""Fail-closed contract checker for M251-B001 runtime export legality freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-b001-runtime-export-legality-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_object_model_abi_invariants_and_legality_contract_and_architecture_freeze_b001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_b001_object_model_abi_invariants_and_legality_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_CLASS_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
DEFAULT_CATEGORY_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
)
DEFAULT_HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runtime-export-legality"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-B001/object_model_abi_invariants_and_legality_contract_summary.json"
)


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
    SnippetCheck("M251-B001-DOC-EXP-01", "# M251 Object-Model ABI Invariants and Legality Expectations (B001)"),
    SnippetCheck("M251-B001-DOC-EXP-02", "Contract ID: `objc3c-runtime-export-legality-freeze/m251-b001-v1`"),
    SnippetCheck("M251-B001-DOC-EXP-03", "`Objc3RuntimeExportLegalityBoundary` as the canonical lane-B freeze packet"),
    SnippetCheck("M251-B001-DOC-EXP-04", "does not yet reject duplicate runtime identities"),
    SnippetCheck("M251-B001-DOC-EXP-05", "`tmp/reports/m251/M251-B001/object_model_abi_invariants_and_legality_contract_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-DOC-PKT-01", "# M251-B001 Object-Model ABI Invariants and Legality Packet"),
    SnippetCheck("M251-B001-DOC-PKT-02", "Packet: `M251-B001`"),
    SnippetCheck("M251-B001-DOC-PKT-03", "Dependencies: none"),
    SnippetCheck("M251-B001-DOC-PKT-04", "The checker runs three deterministic probes:"),
    SnippetCheck("M251-B001-DOC-PKT-05", "Duplicate-identity blocking, incomplete-declaration blocking, and illegal"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-ARCH-01", "M251 lane-B B001 runtime export legality freeze anchors explicit"),
    SnippetCheck("M251-B001-ARCH-02", "m251_object_model_abi_invariants_and_legality_contract_and_architecture_freeze_b001_expectations.md"),
    SnippetCheck("M251-B001-ARCH-03", "metadata export enforcement still pending for B002"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-NDOC-01", "## Runtime export legality freeze (M251-B001)"),
    SnippetCheck("M251-B001-NDOC-02", "`Objc3RuntimeExportLegalityBoundary`"),
    SnippetCheck("M251-B001-NDOC-03", "blocking remain pending for `M251-B002`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-SPC-01", "## M251 runtime export legality freeze (B001)"),
    SnippetCheck("M251-B001-SPC-02", "packet to be ready while metadata export enforcement remains pending"),
    SnippetCheck("M251-B001-SPC-03", "pending enforcement bits for"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-META-01", "## M251 runtime export legality metadata anchors (B001)"),
    SnippetCheck("M251-B001-META-02", "`!objc3.objc_runtime_export_legality`"),
    SnippetCheck("M251-B001-META-03", "category, property, method, and ivar record inventory"),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-TYP-01", "kObjc3RuntimeExportLegalityContractId"),
    SnippetCheck("M251-B001-TYP-02", "struct Objc3RuntimeExportLegalityBoundary {"),
    SnippetCheck("M251-B001-TYP-03", "duplicate_runtime_identity_enforcement_pending = true;"),
    SnippetCheck("M251-B001-TYP-04", "inline bool IsReadyObjc3RuntimeExportLegalityBoundary("),
    SnippetCheck("M251-B001-TYP-05", "Objc3RuntimeExportLegalityBoundary runtime_export_legality_boundary;"),
)

FRONTEND_PIPELINE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-PIPE-01", "Objc3RuntimeExportLegalityBoundary BuildRuntimeExportLegalityBoundary("),
    SnippetCheck("M251-B001-PIPE-02", "boundary.runtime_metadata_source_boundary_ready ="),
    SnippetCheck("M251-B001-PIPE-03", "boundary.metadata_export_enforcement_ready = false;"),
    SnippetCheck("M251-B001-PIPE-04", "runtime export legality freeze is not fail-closed"),
    SnippetCheck("M251-B001-PIPE-05", "result.runtime_export_legality_boundary = BuildRuntimeExportLegalityBoundary("),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-ART-01", "const Objc3RuntimeExportLegalityBoundary &runtime_export_legality ="),
    SnippetCheck("M251-B001-ART-02", "runtime_export_legality_contract_id"),
    SnippetCheck("M251-B001-ART-03", "runtime_export_boundary_ready"),
    SnippetCheck("M251-B001-ART-04", "ir_frontend_metadata.runtime_export_legality_contract_id ="),
    SnippetCheck("M251-B001-ART-05", "ir_frontend_metadata.runtime_export_boundary_ready ="),
)

IR_EMITTER_H_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-IRH-01", "std::string runtime_export_legality_contract_id;"),
    SnippetCheck("M251-B001-IRH-02", "bool runtime_export_semantic_boundary_frozen = false;"),
    SnippetCheck("M251-B001-IRH-03", "bool runtime_export_boundary_ready = false;"),
)

IR_EMITTER_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-IRC-01", "; runtime_export_legality = "),
    SnippetCheck("M251-B001-IRC-02", "!objc3.objc_runtime_export_legality = !{!46}"),
    SnippetCheck("M251-B001-IRC-03", 'out << "!46 = !{!\\\"" << EscapeCStringLiteral(frontend_metadata_.runtime_export_legality_contract_id)'),
)

DRIVER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-DRV-01", "WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, artifacts.manifest_json);"),
)

RUNTIME_SHIM_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B001-SHIM-01", "this shim remains test-only evidence and is not the native"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-B001-PKG-01",
        '"check:objc3c:m251-b001-object-model-abi-invariants-and-legality-contract": "python scripts/check_m251_b001_object_model_abi_invariants_and_legality_contract.py"',
    ),
    SnippetCheck(
        "M251-B001-PKG-02",
        '"test:tooling:m251-b001-object-model-abi-invariants-and-legality-contract": "python -m pytest tests/tooling/test_check_m251_b001_object_model_abi_invariants_and_legality_contract.py -q"',
    ),
    SnippetCheck(
        "M251-B001-PKG-03",
        '"check:objc3c:m251-b001-lane-b-readiness": "npm run build:objc3c-native && npm run check:objc3c:m251-b001-object-model-abi-invariants-and-legality-contract && npm run test:tooling:m251-b001-object-model-abi-invariants-and-legality-contract"',
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
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-pipeline", type=Path, default=DEFAULT_FRONTEND_PIPELINE)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--ir-emitter-h", type=Path, default=DEFAULT_IR_EMITTER_H)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--hello-fixture", type=Path, default=DEFAULT_HELLO_FIXTURE)
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
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing snippet: {snippet.snippet}",
                )
            )
    return passed


def run_command(command: Sequence[str], cwd: Path) -> subprocess.run:
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[list[dict[str, object]], int, int]:
    cases: list[dict[str, object]] = []
    checks_total = 0
    checks_passed = 0
    probe_root = args.probe_root.resolve()
    probe_root.mkdir(parents=True, exist_ok=True)

    class_out = probe_root / "class_manifest_only"
    class_out.mkdir(parents=True, exist_ok=True)
    class_cmd = [
        str(args.runner_exe.resolve()),
        str(args.class_fixture.resolve()),
        "--out-dir",
        str(class_out),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ]
    class_result = run_command(class_cmd, ROOT)
    class_summary_path = class_out / "module.c_api_summary.json"
    class_manifest_path = class_out / "module.manifest.json"
    class_case: dict[str, object] = {
        "case_id": "M251-B001-CASE-CLASS",
        "command": class_cmd,
        "process_exit_code": class_result.returncode,
        "summary_path": display_path(class_summary_path),
        "manifest_path": display_path(class_manifest_path),
    }
    checks_total += 4
    if class_result.returncode == 0 and class_summary_path.exists() and class_manifest_path.exists():
        class_summary = load_json(class_summary_path)
        class_manifest = load_json(class_manifest_path)
        sema = class_manifest["frontend"]["pipeline"]["sema_pass_manager"]
        class_case["status"] = class_summary["status"]
        class_case["success"] = class_summary["success"]
        class_case["emit_stage"] = class_summary["stages"]["emit"]
        class_case["runtime_export_semantic_boundary_frozen"] = sema["runtime_export_semantic_boundary_frozen"]
        class_case["runtime_export_fail_closed"] = sema["runtime_export_fail_closed"]
        class_case["runtime_export_boundary_ready"] = sema["runtime_export_boundary_ready"]
        if class_summary["status"] == 0 and class_summary["success"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B001-CASE-CLASS-STATUS", "class probe did not succeed manifest-only"))
        if not class_summary["stages"]["emit"]["attempted"] and class_summary["stages"]["emit"]["skipped"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B001-CASE-CLASS-EMIT", "class probe emit stage did not remain skipped"))
        if sema["runtime_export_legality_contract_id"] == "objc3c-runtime-export-legality-freeze/m251-b001-v1":
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B001-CASE-CLASS-CONTRACT", "class probe manifest contract id mismatch"))
        if sema["runtime_export_semantic_boundary_frozen"] and sema["runtime_export_fail_closed"] and sema["runtime_export_boundary_ready"] and sema["runtime_export_failure_reason"] == "":
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B001-CASE-CLASS-READY", "class probe legality packet was not frozen/fail-closed/ready"))
    else:
        failures.append(Finding("dynamic", "M251-B001-CASE-CLASS-EXIT", f"runner exited {class_result.returncode} or artifacts were missing"))
    cases.append(class_case)

    category_out = probe_root / "category_manifest_only"
    category_out.mkdir(parents=True, exist_ok=True)
    category_cmd = [
        str(args.runner_exe.resolve()),
        str(args.category_fixture.resolve()),
        "--out-dir",
        str(category_out),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ]
    category_result = run_command(category_cmd, ROOT)
    category_summary_path = category_out / "module.c_api_summary.json"
    category_manifest_path = category_out / "module.manifest.json"
    category_case: dict[str, object] = {
        "case_id": "M251-B001-CASE-CATEGORY",
        "command": category_cmd,
        "process_exit_code": category_result.returncode,
        "summary_path": display_path(category_summary_path),
        "manifest_path": display_path(category_manifest_path),
    }
    checks_total += 2
    if category_result.returncode == 0 and category_summary_path.exists() and category_manifest_path.exists():
        category_summary = load_json(category_summary_path)
        category_manifest = load_json(category_manifest_path)
        sema = category_manifest["frontend"]["pipeline"]["sema_pass_manager"]
        category_case["status"] = category_summary["status"]
        category_case["success"] = category_summary["success"]
        category_case["runtime_export_category_record_count"] = sema["runtime_export_category_record_count"]
        if category_summary["status"] == 0 and category_summary["success"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B001-CASE-CATEGORY-STATUS", "category probe did not succeed manifest-only"))
        if sema["runtime_export_category_record_count"] > 0 and sema["runtime_export_boundary_ready"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B001-CASE-CATEGORY-COUNT", "category probe did not report ready category export records"))
    else:
        failures.append(Finding("dynamic", "M251-B001-CASE-CATEGORY-EXIT", f"runner exited {category_result.returncode} or artifacts were missing"))
    cases.append(category_case)

    hello_out = probe_root / "hello_ir"
    hello_out.mkdir(parents=True, exist_ok=True)
    hello_cmd = [
        str(args.native_exe.resolve()),
        str(args.hello_fixture.resolve()),
        "--out-dir",
        str(hello_out),
        "--emit-prefix",
        "module",
    ]
    hello_result = run_command(hello_cmd, ROOT)
    hello_ir_path = hello_out / "module.ll"
    hello_case: dict[str, object] = {
        "case_id": "M251-B001-CASE-IR",
        "command": hello_cmd,
        "process_exit_code": hello_result.returncode,
        "ir_path": display_path(hello_ir_path),
    }
    checks_total += 3
    if hello_result.returncode == 0 and hello_ir_path.exists():
        ir_text = read_text(hello_ir_path)
        hello_case["ir_contains_contract_comment"] = "; runtime_export_legality = objc3c-runtime-export-legality-freeze/m251-b001-v1" in ir_text
        hello_case["ir_contains_named_metadata"] = "!objc3.objc_runtime_export_legality = !{" in ir_text
        if hello_result.returncode == 0:
            checks_passed += 1
        if hello_case["ir_contains_contract_comment"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B001-CASE-IR-COMMENT", "hello IR did not preserve the legality contract comment"))
        if hello_case["ir_contains_named_metadata"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B001-CASE-IR-META", "hello IR did not preserve the legality named metadata node"))
    else:
        failures.append(Finding("dynamic", "M251-B001-CASE-IR-EXIT", f"native driver exited {hello_result.returncode} or IR was missing"))
    cases.append(hello_case)

    return cases, checks_total, checks_passed


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_passed = 0
    checks_total = 0

    static_targets = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        (args.frontend_pipeline, FRONTEND_PIPELINE_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_emitter_h, IR_EMITTER_H_SNIPPETS),
        (args.ir_emitter_cpp, IR_EMITTER_CPP_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.runtime_shim, RUNTIME_SHIM_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    if not args.skip_dynamic_probes:
        dynamic_cases, dynamic_total, dynamic_passed = run_dynamic_probes(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    ok = not failures
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [failure.__dict__ for failure in failures],
    }
    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
        return 0

    for failure in failures:
        print(f"[fail] {failure.check_id} ({failure.artifact}): {failure.detail}")
    return 1


if __name__ == "__main__":
    raise SystemExit(run())
