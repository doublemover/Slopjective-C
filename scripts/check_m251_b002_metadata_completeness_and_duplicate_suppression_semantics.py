#!/usr/bin/env python3
"""Fail-closed contract checker for M251-B002 runtime export enforcement."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-b002-runtime-export-enforcement-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_metadata_completeness_and_duplicate_suppression_semantics_core_feature_implementation_b002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_b002_metadata_completeness_and_duplicate_suppression_semantics_core_feature_implementation_packet.md"
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
DEFAULT_HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_DUPLICATE_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_export_enforcement_duplicate_interface.objc3"
)
DEFAULT_INCOMPLETE_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_export_enforcement_incomplete_interface.objc3"
)
DEFAULT_ILLEGAL_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_export_enforcement_illegal_property_signature.objc3"
)
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runtime-export-enforcement"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-B002/metadata_completeness_and_duplicate_suppression_semantics_summary.json"
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
    SnippetCheck("M251-B002-DOC-EXP-01", "# M251 Metadata Completeness and Duplicate Suppression Expectations (B002)"),
    SnippetCheck("M251-B002-DOC-EXP-02", "Contract ID: `objc3c-runtime-export-enforcement/m251-b002-v1`"),
    SnippetCheck("M251-B002-DOC-EXP-03", "`Objc3RuntimeExportEnforcementSummary` as the canonical lane-B enforcement"),
    SnippetCheck("M251-B002-DOC-EXP-04", "Forward `@protocol` declarations remain legal dependency hints rather than"),
    SnippetCheck("M251-B002-DOC-EXP-05", "`tmp/reports/m251/M251-B002/metadata_completeness_and_duplicate_suppression_semantics_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-DOC-PKT-01", "# M251-B002 Metadata Completeness and Duplicate Suppression Semantics Packet"),
    SnippetCheck("M251-B002-DOC-PKT-02", "Packet: `M251-B002`"),
    SnippetCheck("M251-B002-DOC-PKT-03", "Dependencies: `M251-B001`, `M251-A002`"),
    SnippetCheck("M251-B002-DOC-PKT-04", "The checker runs five deterministic probes:"),
    SnippetCheck("M251-B002-DOC-PKT-05", "Forward `@protocol` declarations are not exportable runtime metadata units"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-ARCH-01", "M251 lane-B B002 runtime export enforcement anchors explicit"),
    SnippetCheck("M251-B002-ARCH-02", "m251_b002_metadata_completeness_and_duplicate_suppression_semantics_core_feature_implementation_packet.md"),
    SnippetCheck("M251-B002-ARCH-03", "duplicate, incomplete, redeclaration, and shape-drift hazards while forward"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-NDOC-01", "## Runtime export enforcement semantics (M251-B002)"),
    SnippetCheck("M251-B002-NDOC-02", "`Objc3RuntimeExportEnforcementSummary`"),
    SnippetCheck("M251-B002-NDOC-03", "legal dependency hints and do not block export readiness."),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-SPC-01", "## M251 runtime export enforcement semantics (B002)"),
    SnippetCheck("M251-B002-SPC-02", "mixes, and metadata-shape drift to block lowering when present,"),
    SnippetCheck("M251-B002-SPC-03", "incomplete export candidates."),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-META-01", "## M251 runtime export enforcement metadata anchors (B002)"),
    SnippetCheck("M251-B002-META-02", "`!objc3.objc_runtime_export_enforcement`"),
    SnippetCheck("M251-B002-META-03", "illegal-redeclaration, and metadata-shape-drift counters,"),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-TYP-01", "kObjc3RuntimeExportEnforcementContractId"),
    SnippetCheck("M251-B002-TYP-02", "struct Objc3RuntimeExportEnforcementSummary {"),
    SnippetCheck("M251-B002-TYP-03", "bool metadata_completeness_enforced = false;"),
    SnippetCheck("M251-B002-TYP-04", "inline bool IsReadyObjc3RuntimeExportEnforcementSummary("),
    SnippetCheck("M251-B002-TYP-05", "Objc3RuntimeExportEnforcementSummary runtime_export_enforcement_summary;"),
)

FRONTEND_PIPELINE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-PIPE-01", "Objc3RuntimeExportEnforcementSummary BuildRuntimeExportEnforcementSummary("),
    SnippetCheck("M251-B002-PIPE-02", "Forward protocol declarations are dependency hints for later complete"),
    SnippetCheck("M251-B002-PIPE-03", "AreCompatibleRuntimePropertyRedeclarations("),
    SnippetCheck("M251-B002-PIPE-04", "AreCompatibleRuntimeMethodRedeclarations("),
    SnippetCheck("M251-B002-PIPE-05", '"O3S260"'),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-ART-01", "const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement ="),
    SnippetCheck("M251-B002-ART-02", "runtime_export_enforcement_contract_id"),
    SnippetCheck("M251-B002-ART-03", "runtime_export_metadata_completeness_enforced"),
    SnippetCheck("M251-B002-ART-04", "runtime_export_ready_for_runtime_export"),
    SnippetCheck("M251-B002-ART-05", "runtime_export_enforcement_failure_reason"),
)

IR_EMITTER_H_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-IRH-01", "std::string runtime_export_enforcement_contract_id;"),
    SnippetCheck("M251-B002-IRH-02", "bool runtime_export_enforcement_fail_closed = false;"),
    SnippetCheck("M251-B002-IRH-03", "std::size_t runtime_export_illegal_redeclaration_mix_sites = 0;"),
)

IR_EMITTER_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-IRC-01", "; runtime_export_enforcement = "),
    SnippetCheck("M251-B002-IRC-02", "!objc3.objc_runtime_export_enforcement = !{!47}"),
    SnippetCheck("M251-B002-IRC-03", 'out << "!47 = !{!\\\"" << EscapeCStringLiteral(frontend_metadata_.runtime_export_enforcement_contract_id)'),
)

DRIVER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-DRV-01", "WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, artifacts.manifest_json);"),
)

RUNTIME_SHIM_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B002-SHIM-01", "this shim remains test-only evidence and is not the native"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-B002-PKG-01",
        '"check:objc3c:m251-b002-metadata-completeness-and-duplicate-suppression-semantics": "python scripts/check_m251_b002_metadata_completeness_and_duplicate_suppression_semantics.py"',
    ),
    SnippetCheck(
        "M251-B002-PKG-02",
        '"test:tooling:m251-b002-metadata-completeness-and-duplicate-suppression-semantics": "python -m pytest tests/tooling/test_check_m251_b002_metadata_completeness_and_duplicate_suppression_semantics.py -q"',
    ),
    SnippetCheck(
        "M251-B002-PKG-03",
        '"check:objc3c:m251-b002-lane-b-readiness": "npm run check:objc3c:m251-b001-lane-b-readiness && npm run check:objc3c:m251-b002-metadata-completeness-and-duplicate-suppression-semantics && npm run test:tooling:m251-b002-metadata-completeness-and-duplicate-suppression-semantics"',
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
    parser.add_argument("--hello-fixture", type=Path, default=DEFAULT_HELLO_FIXTURE)
    parser.add_argument("--duplicate-fixture", type=Path, default=DEFAULT_DUPLICATE_FIXTURE)
    parser.add_argument("--incomplete-fixture", type=Path, default=DEFAULT_INCOMPLETE_FIXTURE)
    parser.add_argument("--illegal-fixture", type=Path, default=DEFAULT_ILLEGAL_FIXTURE)
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


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def load_diagnostics(path: Path) -> list[dict[str, object]]:
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    if not isinstance(diagnostics, list):
        raise ValueError(f"diagnostics payload missing list at {path}")
    return diagnostics


def has_diagnostic(
    diagnostics: Sequence[dict[str, object]], code: str, message_substring: str | None = None
) -> bool:
    for diagnostic in diagnostics:
        if diagnostic.get("code") != code:
            continue
        if message_substring is None:
            return True
        message = diagnostic.get("message", "")
        if isinstance(message, str) and message_substring in message:
            return True
    return False


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
        "case_id": "M251-B002-CASE-CLASS",
        "command": class_cmd,
        "process_exit_code": class_result.returncode,
        "summary_path": display_path(class_summary_path),
        "manifest_path": display_path(class_manifest_path),
    }
    checks_total += 5
    if class_result.returncode == 0 and class_summary_path.exists() and class_manifest_path.exists():
        class_summary = load_json(class_summary_path)
        class_manifest = load_json(class_manifest_path)
        sema = class_manifest["frontend"]["pipeline"]["sema_pass_manager"]
        class_case["status"] = class_summary["status"]
        class_case["success"] = class_summary["success"]
        class_case["emit_stage"] = class_summary["stages"]["emit"]
        class_case["runtime_export_enforcement_contract_id"] = sema["runtime_export_enforcement_contract_id"]
        class_case["runtime_export_ready_for_runtime_export"] = sema["runtime_export_ready_for_runtime_export"]
        class_case["runtime_export_duplicate_runtime_identity_sites"] = sema["runtime_export_duplicate_runtime_identity_sites"]
        class_case["runtime_export_incomplete_declaration_sites"] = sema["runtime_export_incomplete_declaration_sites"]
        class_case["runtime_export_illegal_redeclaration_mix_sites"] = sema["runtime_export_illegal_redeclaration_mix_sites"]
        class_case["runtime_export_metadata_shape_drift_sites"] = sema["runtime_export_metadata_shape_drift_sites"]
        class_case["runtime_export_enforcement_failure_reason"] = sema["runtime_export_enforcement_failure_reason"]
        if class_summary["status"] == 0 and class_summary["success"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B002-CASE-CLASS-STATUS", "class probe did not succeed manifest-only"))
        if not class_summary["stages"]["emit"]["attempted"] and class_summary["stages"]["emit"]["skipped"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B002-CASE-CLASS-EMIT", "class probe emit stage did not remain skipped"))
        if sema["runtime_export_enforcement_contract_id"] == "objc3c-runtime-export-enforcement/m251-b002-v1":
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B002-CASE-CLASS-CONTRACT", "class probe enforcement contract id mismatch"))
        if (
            sema["runtime_export_metadata_completeness_enforced"]
            and sema["runtime_export_duplicate_runtime_identity_suppression_enforced"]
            and sema["runtime_export_illegal_redeclaration_mix_blocking_enforced"]
            and sema["runtime_export_metadata_shape_drift_blocking_enforced"]
            and sema["runtime_export_enforcement_fail_closed"]
        ):
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B002-CASE-CLASS-ENFORCED", "class probe enforcement booleans were not all enabled"))
        if (
            sema["runtime_export_ready_for_runtime_export"]
            and sema["runtime_export_duplicate_runtime_identity_sites"] == 0
            and sema["runtime_export_incomplete_declaration_sites"] == 0
            and sema["runtime_export_illegal_redeclaration_mix_sites"] == 0
            and sema["runtime_export_metadata_shape_drift_sites"] == 0
            and sema["runtime_export_enforcement_failure_reason"] == ""
        ):
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B002-CASE-CLASS-READY", "class probe enforcement packet was not ready and zeroed"))
    else:
        failures.append(Finding("dynamic", "M251-B002-CASE-CLASS-EXIT", f"runner exited {class_result.returncode} or artifacts were missing"))
    cases.append(class_case)

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
        "case_id": "M251-B002-CASE-IR",
        "command": hello_cmd,
        "process_exit_code": hello_result.returncode,
        "ir_path": display_path(hello_ir_path),
    }
    checks_total += 3
    if hello_result.returncode == 0 and hello_ir_path.exists():
        ir_text = read_text(hello_ir_path)
        hello_case["ir_contains_contract_comment"] = "; runtime_export_enforcement = objc3c-runtime-export-enforcement/m251-b002-v1" in ir_text
        hello_case["ir_contains_named_metadata"] = "!objc3.objc_runtime_export_enforcement = !{" in ir_text
        if hello_result.returncode == 0:
            checks_passed += 1
        if hello_case["ir_contains_contract_comment"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B002-CASE-IR-COMMENT", "hello IR did not preserve the enforcement contract comment"))
        if hello_case["ir_contains_named_metadata"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B002-CASE-IR-META", "hello IR did not preserve the enforcement named metadata node"))
    else:
        failures.append(Finding("dynamic", "M251-B002-CASE-IR-EXIT", f"native driver exited {hello_result.returncode} or IR was missing"))
    cases.append(hello_case)

    def run_negative_case(case_id: str, fixture: Path, out_leaf: str, expected_code: str, expected_message: str) -> tuple[dict[str, object], int, int, list[Finding]]:
        local_failures: list[Finding] = []
        local_total = 3
        local_passed = 0
        out_dir = probe_root / out_leaf
        out_dir.mkdir(parents=True, exist_ok=True)
        command = [
            str(args.native_exe.resolve()),
            str(fixture.resolve()),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ]
        result = run_command(command, ROOT)
        diagnostics_path = out_dir / "module.diagnostics.json"
        case: dict[str, object] = {
            "case_id": case_id,
            "command": command,
            "process_exit_code": result.returncode,
            "diagnostics_path": display_path(diagnostics_path),
        }
        if result.returncode != 0 and diagnostics_path.exists():
            diagnostics = load_diagnostics(diagnostics_path)
            case["diagnostic_codes"] = [diag.get("code") for diag in diagnostics]
            case["diagnostic_messages"] = [diag.get("message") for diag in diagnostics]
            local_passed += 1
            if has_diagnostic(diagnostics, expected_code):
                local_passed += 1
            else:
                local_failures.append(Finding("dynamic", case_id + "-CODE", f"expected diagnostic code {expected_code}"))
            if has_diagnostic(diagnostics, expected_code, expected_message):
                local_passed += 1
            else:
                local_failures.append(Finding("dynamic", case_id + "-MESSAGE", f"expected diagnostic message containing: {expected_message}"))
        else:
            local_failures.append(Finding("dynamic", case_id + "-EXIT", f"native driver exited {result.returncode} or diagnostics were missing"))
        return case, local_total, local_passed, local_failures

    duplicate_case, duplicate_total, duplicate_passed, duplicate_failures = run_negative_case(
        "M251-B002-CASE-DUPLICATE",
        args.duplicate_fixture,
        "duplicate_identity",
        "O3S200",
        "duplicate interface 'Widget'",
    )
    cases.append(duplicate_case)
    checks_total += duplicate_total
    checks_passed += duplicate_passed
    failures.extend(duplicate_failures)

    incomplete_case, incomplete_total, incomplete_passed, incomplete_failures = run_negative_case(
        "M251-B002-CASE-INCOMPLETE",
        args.incomplete_fixture,
        "incomplete_declaration",
        "O3S260",
        "incomplete runtime metadata declarations are not exportable",
    )
    cases.append(incomplete_case)
    checks_total += incomplete_total
    checks_passed += incomplete_passed
    failures.extend(incomplete_failures)

    illegal_case, illegal_total, illegal_passed, illegal_failures = run_negative_case(
        "M251-B002-CASE-ILLEGAL",
        args.illegal_fixture,
        "illegal_redeclaration",
        "O3S206",
        "incompatible property signature for 'value' in implementation 'Widget'",
    )
    cases.append(illegal_case)
    checks_total += illegal_total
    checks_passed += illegal_passed
    failures.extend(illegal_failures)

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
