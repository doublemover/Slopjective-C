#!/usr/bin/env python3
"""Fail-closed contract checker for M251-A003 runtime-record manifest handoff."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-a003-runtime-record-manifest-handoff-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_core_feature_expansion_a003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_core_feature_expansion_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_FRONTEND_ANCHOR = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_CLASS_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
DEFAULT_CATEGORY_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
)
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runtime-record-manifest-handoff"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-A003/runtime_record_manifest_handoff_contract_summary.json"
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
    SnippetCheck(
        "M251-A003-DOC-EXP-01",
        "# M251 Frontend Handoff Normalization and Manifest Projection for Runtime Records Expectations (A003)",
    ),
    SnippetCheck(
        "M251-A003-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-record-manifest-handoff/m251-a003-v1`",
    ),
    SnippetCheck(
        "M251-A003-DOC-EXP-03",
        "`objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object` succeeds",
    ),
    SnippetCheck(
        "M251-A003-DOC-EXP-04",
        "`objc3c-native` full compile still fails closed",
    ),
    SnippetCheck(
        "M251-A003-DOC-EXP-05",
        "`tmp/reports/m251/M251-A003/runtime_record_manifest_handoff_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A003-DOC-PKT-01",
        "# M251-A003 Frontend Handoff Normalization and Manifest Projection for Runtime Records Packet",
    ),
    SnippetCheck("M251-A003-DOC-PKT-02", "Packet: `M251-A003`"),
    SnippetCheck("M251-A003-DOC-PKT-03", "Dependencies: `M251-A002`"),
    SnippetCheck("M251-A003-DOC-PKT-04", "Manifest projection is built before downstream emit/readiness failures are"),
    SnippetCheck("M251-A003-DOC-PKT-05", "Emit-stage stage summaries report `attempted=false` and `skipped=true`")
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A003-ARCH-01",
        "M251 lane-A A003 runtime record manifest handoff normalization anchors explicit",
    ),
    SnippetCheck(
        "M251-A003-ARCH-02",
        "m251_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_core_feature_expansion_a003_expectations.md",
    ),
    SnippetCheck(
        "M251-A003-ARCH-03",
        "full CLI fail-closed runs preserve the manifest handoff artifact",
    ),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A003-NDOC-01",
        "## Runtime record manifest handoff normalization (M251-A003)",
    ),
    SnippetCheck(
        "M251-A003-NDOC-02",
        "`objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object` now",
    ),
    SnippetCheck(
        "M251-A003-NDOC-03",
        "`objc3c-native` full compile still fails closed",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A003-SPC-01",
        "## M251 runtime record manifest handoff normalization (A003)",
    ),
    SnippetCheck(
        "M251-A003-SPC-02",
        "manifest-only frontend runs to succeed without reporting downstream emit",
    ),
    SnippetCheck(
        "M251-A003-SPC-03",
        "full compile/emit workflows to remain fail-closed while still preserving the",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A003-META-01",
        "## M251 runtime record manifest handoff normalization metadata anchors (A003)",
    ),
    SnippetCheck(
        "M251-A003-META-02",
        "emit-stage summary state showing `attempted=false` and `skipped=true`",
    ),
    SnippetCheck(
        "M251-A003-META-03",
        "preserved runtime-record manifest artifacts for fail-closed CLI/native full",
    ),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-A003-TYP-01", "bool emit_manifest = true;"),
    SnippetCheck("M251-A003-TYP-02", "bool emit_ir = true;"),
    SnippetCheck("M251-A003-TYP-03", "bool emit_object = true;"),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A003-ART-01",
        "const auto record_post_pipeline_failure = [&](const char *code, std::string message) {",
    ),
    SnippetCheck(
        "M251-A003-ART-02",
        "bundle.manifest_json = manifest.str();",
    ),
    SnippetCheck(
        "M251-A003-ART-03",
        "if (!options.emit_ir && !options.emit_object) {",
    ),
    SnippetCheck(
        "M251-A003-ART-04",
        "MakeDiag(1, 1, post_pipeline_failure_code, post_pipeline_failure_message)",
    ),
)

FRONTEND_ANCHOR_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-A003-ANCHOR-01", "frontend_options.emit_manifest = options.emit_manifest != 0;"),
    SnippetCheck("M251-A003-ANCHOR-02", "frontend_options.emit_ir = options.emit_ir != 0;"),
    SnippetCheck("M251-A003-ANCHOR-03", "frontend_options.emit_object = options.emit_object != 0;"),
    SnippetCheck(
        "M251-A003-ANCHOR-04",
        "if (options->emit_manifest != 0 && has_out_dir && !product.artifact_bundle.manifest_json.empty()) {",
    ),
    SnippetCheck(
        "M251-A003-ANCHOR-05",
        "const bool wants_emit_stage = options->emit_ir != 0 || options->emit_object != 0;",
    ),
)

DRIVER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-A003-DRV-01", "if (!artifacts.manifest_json.empty()) {"),
    SnippetCheck(
        "M251-A003-DRV-02",
        "M251-A003 expands the handoff so manifest projection survives fail-closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A003-PKG-01",
        '"check:objc3c:m251-a003-frontend-handoff-normalization-and-manifest-projection-for-runtime-records-contract": "python scripts/check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract.py"',
    ),
    SnippetCheck(
        "M251-A003-PKG-02",
        '"test:tooling:m251-a003-frontend-handoff-normalization-and-manifest-projection-for-runtime-records-contract": "python -m pytest tests/tooling/test_check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract.py -q"',
    ),
    SnippetCheck(
        "M251-A003-PKG-03",
        '"check:objc3c:m251-a003-lane-a-readiness": "npm run check:objc3c:m251-a002-lane-a-readiness && npm run build:objc3c-native && npm run check:objc3c:m251-a003-frontend-handoff-normalization-and-manifest-projection-for-runtime-records-contract && npm run test:tooling:m251-a003-frontend-handoff-normalization-and-manifest-projection-for-runtime-records-contract"',
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
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--frontend-anchor", type=Path, default=DEFAULT_FRONTEND_ANCHOR)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)



def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required artifact is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings



def load_json(path: Path, *, artifact: str) -> tuple[dict[str, object] | None, list[Finding]]:
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), f"{artifact}-JSON-EXISTS", f"missing JSON artifact: {display_path(path)}"))
        return None, findings
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        findings.append(Finding(display_path(path), f"{artifact}-JSON-PARSE", f"failed to parse JSON: {exc}"))
        return None, findings
    if not isinstance(payload, dict):
        findings.append(Finding(display_path(path), f"{artifact}-JSON-SHAPE", "expected top-level JSON object"))
        return None, findings
    return payload, findings



def run_runner_probe(*, runner_exe: Path, fixture_path: Path, out_dir: Path, case_id: str) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0

    checks_total += 1
    if not fixture_path.exists():
        findings.append(Finding(display_path(fixture_path), f"{case_id}-FIXTURE-EXISTS", f"fixture is missing: {display_path(fixture_path)}"))
        return checks_total, findings, None

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(runner_exe),
        str(fixture_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ]
    completed = subprocess.run(command, capture_output=True, text=True, check=False)
    summary_path = out_dir / "module.c_api_summary.json"
    checks_total += 1
    if not summary_path.exists():
        findings.append(Finding(display_path(summary_path), f"{case_id}-SUMMARY", "frontend C API runner did not write module.c_api_summary.json"))
        return checks_total, findings, None

    payload, summary_findings = load_json(summary_path, artifact=f"{case_id}-SUMMARY")
    checks_total += 1
    findings.extend(summary_findings)
    if payload is None:
        return checks_total, findings, None

    checks_total += 1
    if completed.returncode != 0:
        findings.append(Finding(display_path(summary_path), f"{case_id}-EXIT", f"expected runner exit code 0, got {completed.returncode}"))

    checks_total += 1
    if payload.get("status") != 0 or payload.get("success") is not True:
        findings.append(Finding(display_path(summary_path), f"{case_id}-SUCCESS", "expected manifest-only runner probe to succeed"))

    for stage_name in ("lex", "parse", "sema", "lower"):
        checks_total += 1
        stage_payload = payload.get("stages", {}).get(stage_name)
        if not isinstance(stage_payload, dict) or stage_payload.get("diagnostics_errors") != 0:
            findings.append(Finding(display_path(summary_path), f"{case_id}-{stage_name.upper()}-CLEAN", f"expected {stage_name} diagnostics_errors == 0"))

    checks_total += 1
    emit_stage = payload.get("stages", {}).get("emit")
    if not isinstance(emit_stage, dict) or emit_stage.get("attempted") is not False or emit_stage.get("skipped") is not True:
        findings.append(Finding(display_path(summary_path), f"{case_id}-EMIT-STAGE", "expected emit stage attempted=false and skipped=true"))

    manifest_path_text = str(payload.get("paths", {}).get("manifest", ""))
    checks_total += 1
    if not manifest_path_text:
        findings.append(Finding(display_path(summary_path), f"{case_id}-MANIFEST-PATH", "expected non-empty manifest path in runner summary"))
        return checks_total, findings, None
    manifest_path = ROOT / Path(manifest_path_text)

    checks_total += 1
    if str(payload.get("paths", {}).get("ir", "")):
        findings.append(Finding(display_path(summary_path), f"{case_id}-IR-PATH", "expected empty IR path for manifest-only probe"))

    checks_total += 1
    if str(payload.get("paths", {}).get("object", "")):
        findings.append(Finding(display_path(summary_path), f"{case_id}-OBJECT-PATH", "expected empty object path for manifest-only probe"))

    manifest_payload, manifest_findings = load_json(manifest_path, artifact=f"{case_id}-MANIFEST")
    checks_total += 1
    findings.extend(manifest_findings)
    if manifest_payload is None:
        return checks_total, findings, None

    checks_total += 1
    if "runtime_metadata_source_records" not in manifest_payload:
        findings.append(Finding(display_path(manifest_path), f"{case_id}-RUNTIME-RECORDS", "manifest missing runtime_metadata_source_records block"))
    else:
        runtime_records = manifest_payload["runtime_metadata_source_records"]
        if not isinstance(runtime_records, dict) or runtime_records.get("deterministic") is not True:
            findings.append(Finding(display_path(manifest_path), f"{case_id}-RUNTIME-RECORDS-DETERMINISTIC", "runtime_metadata_source_records must be deterministic"))

    if case_id.endswith("CLASS"):
        checks_total += 1
        interfaces = manifest_payload.get("interfaces")
        if not isinstance(interfaces, list) or not any(entry.get("name") == "Widget" for entry in interfaces if isinstance(entry, dict)):
            findings.append(Finding(display_path(manifest_path), f"{case_id}-INTERFACES", "expected Widget interface in manifest interfaces"))
    else:
        checks_total += 1
        categories = manifest_payload.get("categories")
        if not isinstance(categories, list) or not any(entry.get("category_name") == "Debug" for entry in categories if isinstance(entry, dict)):
            findings.append(Finding(display_path(manifest_path), f"{case_id}-CATEGORIES", "expected Debug category in manifest categories"))

    case_payload = {
        "case_id": case_id,
        "fixture": display_path(fixture_path),
        "out_dir": display_path(out_dir),
        "summary_path": display_path(summary_path),
        "manifest_path": display_path(manifest_path),
        "status": payload.get("status"),
        "process_exit_code": payload.get("process_exit_code"),
        "success": payload.get("success"),
        "emit_stage": payload.get("stages", {}).get("emit", {}),
    }
    return checks_total, findings, case_payload



def run_cli_probe(*, native_exe: Path, fixture_path: Path, out_dir: Path, case_id: str) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0

    checks_total += 1
    if not fixture_path.exists():
        findings.append(Finding(display_path(fixture_path), f"{case_id}-FIXTURE-EXISTS", f"fixture is missing: {display_path(fixture_path)}"))
        return checks_total, findings, None

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(native_exe),
        str(fixture_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    completed = subprocess.run(command, capture_output=True, text=True, check=False)
    manifest_path = out_dir / "module.manifest.json"
    diagnostics_path = out_dir / "module.diagnostics.json"
    ir_path = out_dir / "module.ll"
    object_path = out_dir / "module.obj"

    checks_total += 1
    if completed.returncode == 0:
        findings.append(Finding(display_path(out_dir), f"{case_id}-EXIT", "expected full CLI probe to remain fail-closed with non-zero exit"))

    checks_total += 1
    if not manifest_path.exists():
        findings.append(Finding(display_path(manifest_path), f"{case_id}-MANIFEST", "expected fail-closed CLI probe to preserve module.manifest.json"))
        return checks_total, findings, None

    checks_total += 1
    if not diagnostics_path.exists():
        findings.append(Finding(display_path(diagnostics_path), f"{case_id}-DIAGNOSTICS", "expected diagnostics artifact for fail-closed CLI probe"))

    checks_total += 1
    if ir_path.exists() or object_path.exists():
        findings.append(Finding(display_path(out_dir), f"{case_id}-IR-OBJECT", "expected fail-closed CLI probe to avoid IR/object outputs"))

    manifest_payload, manifest_findings = load_json(manifest_path, artifact=f"{case_id}-MANIFEST")
    checks_total += 1
    findings.extend(manifest_findings)
    if manifest_payload is None:
        return checks_total, findings, None

    checks_total += 1
    if "runtime_metadata_source_records" not in manifest_payload:
        findings.append(Finding(display_path(manifest_path), f"{case_id}-RUNTIME-RECORDS", "manifest missing runtime_metadata_source_records block"))

    checks_total += 1
    interfaces = manifest_payload.get("interfaces")
    if not isinstance(interfaces, list) or not any(entry.get("name") == "Widget" for entry in interfaces if isinstance(entry, dict)):
        findings.append(Finding(display_path(manifest_path), f"{case_id}-INTERFACES", "expected Widget interface in fail-closed CLI manifest"))

    case_payload = {
        "case_id": case_id,
        "fixture": display_path(fixture_path),
        "out_dir": display_path(out_dir),
        "manifest_path": display_path(manifest_path),
        "diagnostics_path": display_path(diagnostics_path),
        "process_exit_code": completed.returncode,
        "manifest_exists": manifest_path.exists(),
        "ir_exists": ir_path.exists(),
        "object_exists": object_path.exists(),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload



def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    static_checks = (
        (args.expectations_doc, "M251-A003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M251-A003-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M251-A003-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M251-A003-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M251-A003-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M251-A003-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.frontend_types, "M251-A003-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, "M251-A003-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.frontend_anchor, "M251-A003-ANCHOR-EXISTS", FRONTEND_ANCHOR_SNIPPETS),
        (args.driver_cpp, "M251-A003-DRV-EXISTS", DRIVER_SNIPPETS),
        (args.package_json, "M251-A003-PKG-EXISTS", PACKAGE_SNIPPETS),
    )

    for path, exists_check_id, snippets in static_checks:
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    dynamic_cases: list[dict[str, object]] = []
    if not args.skip_dynamic_probes:
        checks_total += 1
        if not args.runner_exe.exists():
            failures.append(Finding(display_path(args.runner_exe), "M251-A003-RUNNER-EXISTS", "frontend C API runner binary is missing; run npm run build:objc3c-native"))
        else:
            for case_id, fixture_path, out_dir in (
                ("M251-A003-CASE-CLASS", args.class_fixture, args.probe_root / "c_api_manifest_only_class"),
                ("M251-A003-CASE-CATEGORY", args.category_fixture, args.probe_root / "c_api_manifest_only_category"),
            ):
                count, findings, case_payload = run_runner_probe(
                    runner_exe=args.runner_exe,
                    fixture_path=fixture_path,
                    out_dir=out_dir,
                    case_id=case_id,
                )
                checks_total += count
                failures.extend(findings)
                if case_payload is not None:
                    dynamic_cases.append(case_payload)

        checks_total += 1
        if not args.native_exe.exists():
            failures.append(Finding(display_path(args.native_exe), "M251-A003-NATIVE-EXISTS", "native objc3c binary is missing; run npm run build:objc3c-native"))
        else:
            count, findings, case_payload = run_cli_probe(
                native_exe=args.native_exe,
                fixture_path=args.class_fixture,
                out_dir=args.probe_root / "cli_fail_closed_manifest_preserved",
                case_id="M251-A003-CASE-CLI",
            )
            checks_total += count
            failures.extend(findings)
            if case_payload is not None:
                dynamic_cases.append(case_payload)

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
