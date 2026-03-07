#!/usr/bin/env python3
"""Fail-closed contract checker for M251-B003 runtime export diagnostics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-b003-runtime-export-diagnostics-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_illegal_runtime_exposed_declaration_diagnostics_core_feature_expansion_b003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_b003_illegal_runtime_exposed_declaration_diagnostics_core_feature_expansion_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_CLASS_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
DEFAULT_INCOMPLETE_INTERFACE_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_export_diagnostics_incomplete_interface.objc3"
)
DEFAULT_CATEGORY_MISSING_IMPLEMENTATION_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_export_diagnostics_category_missing_implementation.objc3"
)
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runtime-export-diagnostics"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-B003/illegal_runtime_exposed_declaration_diagnostics_summary.json"
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
    SnippetCheck("M251-B003-DOC-EXP-01", "# M251 Illegal Runtime-Exposed Declaration Diagnostics Expectations (B003)"),
    SnippetCheck("M251-B003-DOC-EXP-02", "Contract ID: `objc3c-runtime-export-diagnostics/m251-b003-v1`"),
    SnippetCheck("M251-B003-DOC-EXP-03", "Interface-only runtime export units report a precise reason naming the class"),
    SnippetCheck("M251-B003-DOC-EXP-04", "Category-interface-only runtime export units report a precise reason naming"),
    SnippetCheck("M251-B003-DOC-EXP-05", "`tmp/reports/m251/M251-B003/illegal_runtime_exposed_declaration_diagnostics_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B003-DOC-PKT-01", "# M251-B003 Illegal Runtime-Exposed Declaration Diagnostics Packet"),
    SnippetCheck("M251-B003-DOC-PKT-02", "Packet: `M251-B003`"),
    SnippetCheck("M251-B003-DOC-PKT-03", "Dependencies: `M251-B002`"),
    SnippetCheck("M251-B003-DOC-PKT-04", "The checker runs three deterministic probes:"),
    SnippetCheck("M251-B003-DOC-PKT-05", "B003 deliberately reuses `O3S260` so the B002 readiness chain remains stable."),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B003-ARCH-01", "M251 lane-B B003 runtime export diagnostic precision anchors explicit"),
    SnippetCheck("M251-B003-ARCH-02", "m251_b003_illegal_runtime_exposed_declaration_diagnostics_core_feature_expansion_packet.md"),
    SnippetCheck("M251-B003-ARCH-03", "category declarations that parse successfully but still cannot participate in runtime export"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B003-NDOC-01", "## Runtime export diagnostic precision (M251-B003)"),
    SnippetCheck("M251-B003-NDOC-02", "interface 'Widget' is missing a matching @implementation"),
    SnippetCheck("M251-B003-NDOC-03", "category 'Widget(Tracing)' is missing a matching @implementation"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B003-SPC-01", "## M251 runtime export diagnostic precision (B003)"),
    SnippetCheck("M251-B003-SPC-02", "B003 shall keep the B002 fail-closed blocker code stable while making incomplete runtime export messages precise"),
    SnippetCheck("M251-B003-SPC-03", "class and category interface declarations that parse successfully but still cannot participate in runtime export"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B003-META-01", "## M251 runtime export diagnostic precision metadata anchors (B003)"),
    SnippetCheck("M251-B003-META-02", "No new metadata node is introduced for B003; the precision change is diagnostic-surface only."),
    SnippetCheck("M251-B003-META-03", "runtime export diagnostic precision must remain source-anchored and deterministic"),
)

FRONTEND_PIPELINE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B003-PIPE-01", "struct Objc3RuntimeExportBlockingDiagnostic {"),
    SnippetCheck("M251-B003-PIPE-02", "BuildRuntimeExportBlockingDiagnostics("),
    SnippetCheck("M251-B003-PIPE-03", "interface '"),
    SnippetCheck("M251-B003-PIPE-04", "category '"),
    SnippetCheck("M251-B003-PIPE-05", "runtime_export_blocking_diagnostics"),
)

DRIVER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B003-DRV-01", "WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, artifacts.manifest_json);"),
)

RUNTIME_SHIM_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-B003-SHIM-01", "this shim remains test-only evidence and is not the native"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-B003-PKG-01",
        '"check:objc3c:m251-b003-illegal-runtime-exposed-declaration-diagnostics": "python scripts/check_m251_b003_illegal_runtime_exposed_declaration_diagnostics.py"',
    ),
    SnippetCheck(
        "M251-B003-PKG-02",
        '"test:tooling:m251-b003-illegal-runtime-exposed-declaration-diagnostics": "python -m pytest tests/tooling/test_check_m251_b003_illegal_runtime_exposed_declaration_diagnostics.py -q"',
    ),
    SnippetCheck(
        "M251-B003-PKG-03",
        '"check:objc3c:m251-b003-lane-b-readiness": "npm run check:objc3c:m251-b002-lane-b-readiness && npm run check:objc3c:m251-b003-illegal-runtime-exposed-declaration-diagnostics && npm run test:tooling:m251-b003-illegal-runtime-exposed-declaration-diagnostics"',
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
    parser.add_argument("--frontend-pipeline", type=Path, default=DEFAULT_FRONTEND_PIPELINE)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--incomplete-interface-fixture", type=Path, default=DEFAULT_INCOMPLETE_INTERFACE_FIXTURE)
    parser.add_argument("--category-missing-implementation-fixture", type=Path, default=DEFAULT_CATEGORY_MISSING_IMPLEMENTATION_FIXTURE)
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


def has_diagnostic(diagnostics: Sequence[dict[str, object]], code: str, message_substring: str) -> bool:
    for diagnostic in diagnostics:
        if diagnostic.get("code") != code:
            continue
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
        "case_id": "M251-B003-CASE-CLASS",
        "command": class_cmd,
        "process_exit_code": class_result.returncode,
        "summary_path": display_path(class_summary_path),
        "manifest_path": display_path(class_manifest_path),
    }
    checks_total += 3
    if class_result.returncode == 0 and class_summary_path.exists() and class_manifest_path.exists():
        class_summary = load_json(class_summary_path)
        class_manifest = load_json(class_manifest_path)
        sema = class_manifest["frontend"]["pipeline"]["sema_pass_manager"]
        class_case["status"] = class_summary["status"]
        class_case["success"] = class_summary["success"]
        class_case["runtime_export_ready_for_runtime_export"] = sema["runtime_export_ready_for_runtime_export"]
        if class_summary["status"] == 0 and class_summary["success"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B003-CASE-CLASS-STATUS", "class probe did not succeed manifest-only"))
        if sema["runtime_export_ready_for_runtime_export"]:
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B003-CASE-CLASS-READY", "class probe did not remain runtime-export ready"))
        if sema["runtime_export_enforcement_failure_reason"] == "":
            checks_passed += 1
        else:
            failures.append(Finding("dynamic", "M251-B003-CASE-CLASS-FAILURE-REASON", "class probe gained an unexpected enforcement failure reason"))
    else:
        failures.append(Finding("dynamic", "M251-B003-CASE-CLASS-EXIT", f"runner exited {class_result.returncode} or artifacts were missing"))
    cases.append(class_case)

    def run_negative(case_id: str, fixture: Path, out_leaf: str, expected_line: int, expected_message: str) -> tuple[dict[str, object], int, int, list[Finding]]:
        local_failures: list[Finding] = []
        local_total = 4
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
            case["diagnostic_lines"] = [diag.get("line") for diag in diagnostics]
            local_passed += 1
            if has_diagnostic(diagnostics, "O3S260", expected_message):
                local_passed += 1
            else:
                local_failures.append(Finding("dynamic", case_id + "-MESSAGE", f"expected diagnostic message containing: {expected_message}"))
            if any(diag.get("code") == "O3S260" for diag in diagnostics):
                local_passed += 1
            else:
                local_failures.append(Finding("dynamic", case_id + "-CODE", "expected O3S260 diagnostic"))
            if any(diag.get("line") == expected_line for diag in diagnostics):
                local_passed += 1
            else:
                local_failures.append(Finding("dynamic", case_id + "-LINE", f"expected diagnostic line {expected_line}"))
        else:
            local_failures.append(Finding("dynamic", case_id + "-EXIT", f"native driver exited {result.returncode} or diagnostics were missing"))
        return case, local_total, local_passed, local_failures

    interface_case, interface_total, interface_passed, interface_failures = run_negative(
        "M251-B003-CASE-INCOMPLETE-INTERFACE",
        args.incomplete_interface_fixture,
        "incomplete_interface",
        4,
        "interface 'Widget' is missing a matching @implementation",
    )
    cases.append(interface_case)
    checks_total += interface_total
    checks_passed += interface_passed
    failures.extend(interface_failures)

    category_case, category_total, category_passed, category_failures = run_negative(
        "M251-B003-CASE-CATEGORY-MISSING-IMPLEMENTATION",
        args.category_missing_implementation_fixture,
        "category_missing_implementation",
        4,
        "category 'Widget(Tracing)' is missing a matching @implementation",
    )
    cases.append(category_case)
    checks_total += category_total
    checks_passed += category_passed
    failures.extend(category_failures)

    return cases, checks_total, checks_passed


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_targets = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.frontend_pipeline, FRONTEND_PIPELINE_SNIPPETS),
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
