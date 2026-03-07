#!/usr/bin/env python3
"""Fail-closed contract checker for M251-A002 runtime metadata source-record extraction."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m251-a002-class-protocol-category-property-ivar-source-record-extraction-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m251_class_protocol_category_property_ivar_source_record_extraction_core_feature_implementation_a002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m251"
    / "m251_a002_class_protocol_category_property_ivar_source_record_extraction_core_feature_implementation_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_CLASS_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
DEFAULT_CATEGORY_FIXTURE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
)
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m251" / "runtime-metadata-source-record-extraction"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m251/M251-A002/runtime_metadata_source_record_extraction_contract_summary.json"
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
        "M251-A002-DOC-EXP-01",
        "# M251 Class Protocol Category Property Ivar Source-Record Extraction Expectations (A002)",
    ),
    SnippetCheck(
        "M251-A002-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-metadata-source-record-extraction/m251-a002-v1`",
    ),
    SnippetCheck(
        "M251-A002-DOC-EXP-03",
        "`Objc3RuntimeMetadataSourceRecordSet`",
    ),
    SnippetCheck(
        "M251-A002-DOC-EXP-04",
        "`objc3c-frontend-c-api-runner.exe`",
    ),
    SnippetCheck(
        "M251-A002-DOC-EXP-05",
        "Current emit-only `O3L300` failure is acceptable",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A002-DOC-PKT-01",
        "# M251-A002 Class Protocol Category Property Ivar Source-Record Extraction Packet",
    ),
    SnippetCheck("M251-A002-DOC-PKT-02", "Packet: `M251-A002`"),
    SnippetCheck("M251-A002-DOC-PKT-03", "Dependencies: `M251-A001`"),
    SnippetCheck("M251-A002-DOC-PKT-04", "`Objc3RuntimeMetadataSourceRecordSet`"),
    SnippetCheck("M251-A002-DOC-PKT-05", "`Class(Category)`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A002-ARCH-01",
        "M251 lane-A A002 runtime metadata source record extraction anchors explicit",
    ),
    SnippetCheck(
        "M251-A002-ARCH-02",
        "m251_class_protocol_category_property_ivar_source_record_extraction_core_feature_implementation_a002_expectations.md",
    ),
    SnippetCheck(
        "M251-A002-ARCH-03",
        "frontend C API runner fixture probes fail closed",
    ),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A002-NDOC-01",
        "## Runtime metadata source-record extraction (M251-A002)",
    ),
    SnippetCheck(
        "M251-A002-NDOC-02",
        "`Objc3RuntimeMetadataSourceRecordSet`",
    ),
    SnippetCheck(
        "M251-A002-NDOC-03",
        "Current fixture proof uses `objc3c-frontend-c-api-runner.exe`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A002-SPC-01",
        "## M251 runtime metadata source record extraction (A002)",
    ),
    SnippetCheck(
        "M251-A002-SPC-02",
        "`Objc3RuntimeMetadataSourceRecordSet`",
    ),
    SnippetCheck(
        "M251-A002-SPC-03",
        "Current emit-only downstream `O3L300` remains acceptable during `M251-A002`",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A002-META-01",
        "## M251 runtime metadata source record set metadata anchors (A002)",
    ),
    SnippetCheck(
        "M251-A002-META-02",
        "`runtime_metadata_source_records` object",
    ),
    SnippetCheck(
        "M251-A002-META-03",
        "`classes_lexicographic` filtered by `record_kind`",
    ),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A002-TYP-01",
        "struct Objc3RuntimeMetadataClassSourceRecord {",
    ),
    SnippetCheck(
        "M251-A002-TYP-02",
        "struct Objc3RuntimeMetadataCategorySourceRecord {",
    ),
    SnippetCheck(
        "M251-A002-TYP-03",
        "struct Objc3RuntimeMetadataSourceRecordSet {",
    ),
    SnippetCheck(
        "M251-A002-TYP-04",
        "inline bool IsReadyObjc3RuntimeMetadataSourceRecordSet(",
    ),
    SnippetCheck(
        "M251-A002-TYP-05",
        "Objc3RuntimeMetadataSourceRecordSet runtime_metadata_source_records;",
    ),
)

FRONTEND_PIPELINE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A002-PIP-01",
        "std::string BuildCategoryOwnerName(const std::string &class_name,",
    ),
    SnippetCheck(
        "M251-A002-PIP-02",
        '  return class_name + "(" + category_name + ")";',
    ),
    SnippetCheck(
        "M251-A002-PIP-03",
        "Objc3RuntimeMetadataSourceRecordSet BuildRuntimeMetadataSourceRecordSet(",
    ),
    SnippetCheck(
        "M251-A002-PIP-04",
        'append_property_records(interface_decl.properties, "category-interface", owner_name);',
    ),
    SnippetCheck(
        "M251-A002-PIP-05",
        'append_method_records(implementation.methods, "category-implementation", owner_name);',
    ),
    SnippetCheck(
        "M251-A002-PIP-06",
        "records.deterministic = std::is_sorted(records.classes_lexicographic.begin(),",
    ),
)

FRONTEND_ARTIFACTS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A002-ART-01",
        "const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records =",
    ),
    SnippetCheck(
        "M251-A002-ART-02",
        'manifest << "  \\\"interfaces\\\": [\\n";',
    ),
    SnippetCheck(
        "M251-A002-ART-03",
        'manifest << "  \\\"protocols\\\": [\\n";',
    ),
    SnippetCheck(
        "M251-A002-ART-04",
        'manifest << "  \\\"categories\\\": [\\n";',
    ),
    SnippetCheck(
        "M251-A002-ART-05",
        'manifest << "  \\\"runtime_metadata_source_records\\\": {\\n";',
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M251-A002-PKG-01",
        '"check:objc3c:m251-a002-class-protocol-category-property-ivar-source-record-extraction-contract": "python scripts/check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract.py"',
    ),
    SnippetCheck(
        "M251-A002-PKG-02",
        '"test:tooling:m251-a002-class-protocol-category-property-ivar-source-record-extraction-contract": "python -m pytest tests/tooling/test_check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract.py -q"',
    ),
    SnippetCheck(
        "M251-A002-PKG-03",
        '"check:objc3c:m251-a002-lane-a-readiness": "npm run check:objc3c:m251-a001-lane-a-readiness && npm run build:objc3c-native && npm run check:objc3c:m251-a002-class-protocol-category-property-ivar-source-record-extraction-contract && npm run test:tooling:m251-a002-class-protocol-category-property-ivar-source-record-extraction-contract"',
    ),
)

CLASS_FIXTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-A002-FXT-CLS-01", "@protocol Worker<Base>"),
    SnippetCheck("M251-A002-FXT-CLS-02", "@interface Widget<Worker>"),
    SnippetCheck("M251-A002-FXT-CLS-03", "@implementation Widget"),
    SnippetCheck("M251-A002-FXT-CLS-04", "setter=setCurrentValue:"),
)

CATEGORY_FIXTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M251-A002-FXT-CAT-01", "@protocol Worker<Base>"),
    SnippetCheck("M251-A002-FXT-CAT-02", "@interface Widget (Debug)<Worker>"),
    SnippetCheck("M251-A002-FXT-CAT-03", "@implementation Widget (Debug)"),
    SnippetCheck("M251-A002-FXT-CAT-04", "@property (readonly) id shadow;"),
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
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-runner-probes", action="store_true")
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
        findings.append(Finding(display_path(summary_path), f"{case_id}-RUNNER-SUMMARY", "frontend C API runner did not write module.c_api_summary.json"))
        return checks_total, findings, None

    try:
        payload = json.loads(summary_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        findings.append(Finding(display_path(summary_path), f"{case_id}-RUNNER-SUMMARY-JSON", f"failed to parse runner summary JSON: {exc}"))
        return checks_total, findings, None

    expected_zero_error_stages = ("lex", "parse", "sema", "lower")
    for stage_name in expected_zero_error_stages:
        checks_total += 1
        stage_payload = payload.get("stages", {}).get(stage_name)
        if not isinstance(stage_payload, dict) or stage_payload.get("diagnostics_errors") != 0:
            findings.append(
                Finding(
                    display_path(summary_path),
                    f"{case_id}-STAGE-{stage_name.upper()}-CLEAN",
                    f"expected {stage_name} stage diagnostics_errors == 0",
                )
            )

    checks_total += 1
    if payload.get("semantic_skipped"):
        findings.append(Finding(display_path(summary_path), f"{case_id}-SEMANTIC-SKIPPED", "semantic stage unexpectedly skipped"))

    checks_total += 1
    if completed.returncode != payload.get("process_exit_code"):
        findings.append(
            Finding(
                display_path(summary_path),
                f"{case_id}-PROCESS-EXIT-CODE",
                "runner process exit code does not match summary process_exit_code",
            )
        )

    checks_total += 1
    status = payload.get("status")
    success = bool(payload.get("success"))
    emit_errors = payload.get("stages", {}).get("emit", {}).get("diagnostics_errors")
    last_error = str(payload.get("last_error", ""))
    if success:
        pass
    elif status == 1 and emit_errors and emit_errors >= 1:
        if last_error and "O3L300" not in last_error and "semantic parity surface is not ready" not in last_error:
            findings.append(
                Finding(
                    display_path(summary_path),
                    f"{case_id}-EMIT-ONLY-FAILURE",
                    "runner failed outside the known downstream emit/readiness gate",
                )
            )
    else:
        findings.append(
            Finding(
                display_path(summary_path),
                f"{case_id}-FRONTEND-ACCEPTANCE",
                "expected frontend acceptance with either full success or emit-only downstream failure",
            )
        )

    case_payload = {
        "case_id": case_id,
        "fixture": display_path(fixture_path),
        "out_dir": display_path(out_dir),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
        "summary_path": display_path(summary_path),
        "status": status,
        "process_exit_code": payload.get("process_exit_code"),
        "success": success,
        "semantic_skipped": payload.get("semantic_skipped"),
        "manifest_path": payload.get("paths", {}).get("manifest", ""),
        "last_error": last_error,
        "stages": payload.get("stages", {}),
    }
    return checks_total, findings, case_payload


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    static_checks = (
        (args.expectations_doc, "M251-A002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M251-A002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M251-A002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M251-A002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M251-A002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M251-A002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.frontend_types, "M251-A002-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_pipeline, "M251-A002-PIP-EXISTS", FRONTEND_PIPELINE_SNIPPETS),
        (args.frontend_artifacts, "M251-A002-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.package_json, "M251-A002-PKG-EXISTS", PACKAGE_SNIPPETS),
        (args.class_fixture, "M251-A002-FXT-CLS-EXISTS", CLASS_FIXTURE_SNIPPETS),
        (args.category_fixture, "M251-A002-FXT-CAT-EXISTS", CATEGORY_FIXTURE_SNIPPETS),
    )

    for path, exists_check_id, snippets in static_checks:
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    runner_cases: list[dict[str, object]] = []
    if not args.skip_runner_probes:
        checks_total += 1
        if not args.runner_exe.exists():
            failures.append(
                Finding(
                    display_path(args.runner_exe),
                    "M251-A002-RUNNER-EXISTS",
                    "frontend C API runner binary is missing; run npm run build:objc3c-native",
                )
            )
        else:
            probes = (
                ("M251-A002-CASE-CLASS", args.class_fixture, args.probe_root / "class-protocol-property-ivar"),
                ("M251-A002-CASE-CATEGORY", args.category_fixture, args.probe_root / "category-protocol-property"),
            )
            for case_id, fixture_path, out_dir in probes:
                count, findings, case_payload = run_runner_probe(
                    runner_exe=args.runner_exe,
                    fixture_path=fixture_path,
                    out_dir=out_dir,
                    case_id=case_id,
                )
                checks_total += count
                failures.extend(findings)
                if case_payload is not None:
                    runner_cases.append(case_payload)

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "runner_probes_executed": not args.skip_runner_probes,
        "runner_cases": runner_cases,
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
