#!/usr/bin/env python3
"""Fail-closed contract checker for M252-C002 typed metadata graph handoff."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-c002-typed-metadata-graph-handoff-and-manifest-schema-v1"
CONTRACT_ID = "objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1"
SCHEMA_ORDERING_MODEL = "contract-header-then-source-graph-payload-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m252_typed_metadata_graph_handoff_and_manifest_schema_c002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m252" / "m252_c002_typed_metadata_graph_handoff_and_manifest_schema_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
DEFAULT_SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_TYPED_SURFACE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_typed_sema_to_lowering_contract_surface.h"
DEFAULT_PARSE_READINESS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "c002-typed-metadata-graph-handoff"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-C002/typed_metadata_graph_handoff_and_manifest_schema_summary.json")


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M252-C002-DOC-EXP-01", "# M252 Typed Metadata Graph Handoff And Manifest Schema Expectations (C002)"),
    SnippetCheck("M252-C002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M252-C002-DOC-EXP-03", "`Objc3ExecutableMetadataTypedLoweringHandoff`"),
    SnippetCheck("M252-C002-DOC-EXP-04", "`frontend.pipeline.semantic_surface.objc_executable_metadata_typed_lowering_handoff`"),
    SnippetCheck("M252-C002-DOC-EXP-05", "`ready_for_lowering == true`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M252-C002-DOC-PKT-01", "# M252-C002 Typed Metadata Graph Handoff And Manifest Schema Packet"),
    SnippetCheck("M252-C002-DOC-PKT-02", "Packet: `M252-C002`"),
    SnippetCheck("M252-C002-DOC-PKT-03", "- `M252-C001`"),
    SnippetCheck("M252-C002-DOC-PKT-04", "- `M252-A003`"),
    SnippetCheck("M252-C002-DOC-PKT-05", "- `M252-B004`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M252-C002-ARCH-01", "M252 lane-C C002 typed metadata graph handoff anchors explicit contract"),
    SnippetCheck("M252-C002-ARCH-02", "lowering-ready metadata graph payload"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M252-C002-NDOC-01", "## Typed metadata graph handoff and manifest schema (M252-C002)"),
    SnippetCheck("M252-C002-NDOC-02", "`frontend.pipeline.semantic_surface.objc_executable_metadata_typed_lowering_handoff`"),
    SnippetCheck("M252-C002-NDOC-03", "`tmp/reports/m252/M252-C002/typed_metadata_graph_handoff_and_manifest_schema_summary.json`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M252-C002-SPC-01", "## M252 typed metadata graph handoff and manifest schema (C002)"),
    SnippetCheck("M252-C002-SPC-02", "`Objc3ExecutableMetadataTypedLoweringHandoff`"),
    SnippetCheck("M252-C002-SPC-03", "`ready_for_lowering == true`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M252-C002-META-01", "## M252 typed metadata graph handoff metadata anchors (C002)"),
    SnippetCheck("M252-C002-META-02", "`frontend.pipeline.semantic_surface.objc_executable_metadata_typed_lowering_handoff`"),
    SnippetCheck("M252-C002-META-03", "`tmp/reports/m252/M252-C002/typed_metadata_graph_handoff_and_manifest_schema_summary.json`"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M252-C002-PARSE-01", "M252-C002 typed-lowering anchor: typed metadata graph handoff packets keep"),
)
SEMA_CONTRACT_SNIPPETS = (
    SnippetCheck("M252-C002-SEMA-01", "M252-C002 typed-lowering anchor: the concrete lowering-ready metadata graph"),
)
SEMA_PASSES_SNIPPETS = (
    SnippetCheck("M252-C002-PASS-01", "M252-C002 typed-lowering anchor: the concrete lowering-ready metadata graph"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M252-C002-TYPES-01", "kObjc3ExecutableMetadataTypedLoweringHandoffContractId"),
    SnippetCheck("M252-C002-TYPES-02", "struct Objc3ExecutableMetadataTypedLoweringHandoff {"),
    SnippetCheck("M252-C002-TYPES-03", "IsReadyObjc3ExecutableMetadataTypedLoweringHandoff("),
    SnippetCheck("M252-C002-TYPES-04", "executable_metadata_typed_lowering_handoff;"),
)
FRONTEND_PIPELINE_SNIPPETS = (
    SnippetCheck("M252-C002-PIPE-01", "BuildExecutableMetadataTypedLoweringHandoffReplayKey("),
    SnippetCheck("M252-C002-PIPE-02", "BuildExecutableMetadataTypedLoweringHandoff("),
    SnippetCheck("M252-C002-PIPE-03", "result.executable_metadata_typed_lowering_handoff ="),
)
TYPED_SURFACE_SNIPPETS = (
    SnippetCheck("M252-C002-TYPED-01", "metadata_graph_typed_lowering_handoff_ready="),
    SnippetCheck("M252-C002-TYPED-02", "surface.executable_metadata_typed_lowering_handoff_ready ="),
    SnippetCheck("M252-C002-TYPED-03", "surface.executable_metadata_typed_lowering_handoff_key ="),
)
PARSE_READINESS_SNIPPETS = (
    SnippetCheck("M252-C002-READINESS-01", "surface.executable_metadata_typed_lowering_handoff_ready ="),
    SnippetCheck("M252-C002-READINESS-02", "surface.executable_metadata_typed_lowering_handoff_key ="),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M252-C002-ART-01", "BuildExecutableMetadataTypedLoweringHandoffJson("),
    SnippetCheck("M252-C002-ART-02", "objc_executable_metadata_typed_lowering_handoff"),
    SnippetCheck("M252-C002-ART-03", "executable_metadata_typed_lowering_handoff_key"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M252-C002-FIX-01", "module runtimeMetadataClassRecords;"),
    SnippetCheck("M252-C002-FIX-02", "module runtimeMetadataCategoryRecords;"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M252-C002-PKG-01", '"check:objc3c:m252-c002-typed-metadata-graph-handoff-and-manifest-schema": "python scripts/check_m252_c002_typed_metadata_graph_handoff_and_manifest_schema.py"'),
    SnippetCheck("M252-C002-PKG-02", '"test:tooling:m252-c002-typed-metadata-graph-handoff-and-manifest-schema": "python -m pytest tests/tooling/test_check_m252_c002_typed_metadata_graph_handoff_and_manifest_schema.py -q"'),
    SnippetCheck("M252-C002-PKG-03", '"check:objc3c:m252-c002-lane-c-readiness": "npm run check:objc3c:m252-c001-lane-c-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-c002-typed-metadata-graph-handoff-and-manifest-schema && npm run test:tooling:m252-c002-typed-metadata-graph-handoff-and-manifest-schema"'),
)


GRAPH_KEYS_TO_MATCH = (
    "contract_id",
    "owner_identity_model",
    "metaclass_node_policy",
    "edge_ordering_model",
    "interface_node_entries",
    "implementation_node_entries",
    "class_node_entries",
    "metaclass_node_entries",
    "protocol_node_entries",
    "category_node_entries",
    "property_node_entries",
    "method_node_entries",
    "ivar_node_entries",
    "owner_edges",
    "source_graph_complete",
    "ready_for_semantic_closure",
    "ready_for_lowering",
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
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-contract", type=Path, default=DEFAULT_SEMA_CONTRACT)
    parser.add_argument("--sema-passes", type=Path, default=DEFAULT_SEMA_PASSES)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-pipeline", type=Path, default=DEFAULT_FRONTEND_PIPELINE)
    parser.add_argument("--typed-surface", type=Path, default=DEFAULT_TYPED_SURFACE)
    parser.add_argument("--parse-readiness", type=Path, default=DEFAULT_PARSE_READINESS)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def run_case(*, runner_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-C002-FIXTURE-EXISTS", "fixture is missing", findings)
    if findings:
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
    diagnostics_path = out_dir / "module.diagnostics.json"
    manifest_path = out_dir / "module.manifest.json"

    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-C002-RUNNER-SUMMARY", "runner summary missing", findings)
    checks_total += require(diagnostics_path.exists(), display_path(diagnostics_path), "M252-C002-RUNNER-DIAGNOSTICS", "runner diagnostics missing", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-C002-RUNNER-MANIFEST", "runner manifest missing", findings)
    if findings:
        return checks_total, findings, None

    summary_payload = load_json(summary_path)
    diagnostics_payload = load_json(diagnostics_path)
    manifest_payload = load_json(manifest_path)
    diagnostics = diagnostics_payload.get("diagnostics")
    checks_total += require(isinstance(diagnostics, list), display_path(diagnostics_path), "M252-C002-DIAGNOSTICS-LIST", "diagnostics payload must contain a diagnostics list", findings)
    if not isinstance(diagnostics, list):
        return checks_total, findings, None
    observed_codes = [entry.get("code") for entry in diagnostics if isinstance(entry, dict)]
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), "M252-C002-RUNNER-SUCCESS", "runner success mismatch", findings)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), "M252-C002-RUNNER-STATUS", "runner status mismatch", findings)
    checks_total += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M252-C002-SEMANTIC-SKIPPED", "semantic stage unexpectedly skipped", findings)
    checks_total += require(completed.returncode == 0, display_path(summary_path), "M252-C002-PROCESS-EXIT", "runner process exit mismatch", findings)
    checks_total += require(observed_codes == [], display_path(diagnostics_path), "M252-C002-DIAGNOSTIC-CODES", f"expected no diagnostics, observed {observed_codes}", findings)

    pipeline = manifest_payload["frontend"]["pipeline"]
    graph = pipeline["semantic_surface"]["objc_executable_metadata_source_graph"]
    freeze = pipeline["semantic_surface"]["objc_executable_metadata_lowering_handoff_surface"]
    typed = pipeline["semantic_surface"]["objc_executable_metadata_typed_lowering_handoff"]
    parse = pipeline["parse_lowering_readiness"]

    checks_total += require(typed.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M252-C002-TYPED-CONTRACT", "typed handoff contract id mismatch", findings)
    checks_total += require(typed.get("ready") is True, display_path(manifest_path), "M252-C002-TYPED-READY", "typed handoff must be ready", findings)
    checks_total += require(typed.get("deterministic") is True, display_path(manifest_path), "M252-C002-TYPED-DETERMINISTIC", "typed handoff must be deterministic", findings)
    checks_total += require(typed.get("manifest_schema_frozen") is True, display_path(manifest_path), "M252-C002-TYPED-SCHEMA-FROZEN", "typed handoff manifest schema must be frozen", findings)
    checks_total += require(typed.get("fail_closed") is True, display_path(manifest_path), "M252-C002-TYPED-FAIL-CLOSED", "typed handoff must be fail-closed", findings)
    checks_total += require(typed.get("ready_for_lowering") is True, display_path(manifest_path), "M252-C002-TYPED-LOWERING-READY", "typed handoff must be lowering-ready", findings)
    checks_total += require(typed.get("manifest_schema_ordering_model") == SCHEMA_ORDERING_MODEL, display_path(manifest_path), "M252-C002-TYPED-SCHEMA-MODEL", "typed handoff schema ordering model mismatch", findings)
    checks_total += require(bool(typed.get("replay_key")), display_path(manifest_path), "M252-C002-TYPED-REPLAY-KEY", "typed handoff replay key must be non-empty", findings)

    checks_total += require(freeze.get("ready") is True, display_path(manifest_path), "M252-C002-C001-READY", "C001 freeze packet must stay ready", findings)
    checks_total += require(freeze.get("ready_for_lowering") is False, display_path(manifest_path), "M252-C002-C001-NOT-LOWERING-READY", "C001 freeze packet must remain not lowering-ready", findings)

    typed_graph = typed.get("source_graph")
    checks_total += require(isinstance(typed_graph, dict), display_path(manifest_path), "M252-C002-TYPED-GRAPH", "typed handoff must publish a source_graph object", findings)
    if not isinstance(typed_graph, dict):
        return checks_total, findings, None

    for key in GRAPH_KEYS_TO_MATCH:
        checks_total += require(typed_graph.get(key) == graph.get(key), display_path(manifest_path), f"M252-C002-GRAPH-{key.upper()}", f"typed handoff source_graph drifted for {key}", findings)

    checks_total += require(typed_graph.get("ready_for_lowering") is False, display_path(manifest_path), "M252-C002-TYPED-GRAPH-NOT-LOWERING-READY", "nested source_graph must remain pre-lowering", findings)

    checks_total += require(parse.get("executable_metadata_typed_lowering_handoff_ready") is True, display_path(manifest_path), "M252-C002-PARSE-TYPED-READY", "parse/lowering projection must publish typed handoff ready", findings)
    checks_total += require(parse.get("executable_metadata_typed_lowering_handoff_deterministic") is True, display_path(manifest_path), "M252-C002-PARSE-TYPED-DETERMINISTIC", "parse/lowering projection must publish typed handoff deterministic", findings)
    checks_total += require(parse.get("executable_metadata_typed_lowering_handoff_key") == typed.get("replay_key"), display_path(manifest_path), "M252-C002-PARSE-TYPED-KEY", "parse/lowering typed handoff key must match typed replay key", findings)

    case_payload = {
        "fixture": display_path(fixture_path),
        "summary_path": display_path(summary_path),
        "manifest_path": display_path(manifest_path),
        "typed_ready": typed.get("ready"),
        "typed_ready_for_lowering": typed.get("ready_for_lowering"),
        "parse_typed_key_matches": parse.get("executable_metadata_typed_lowering_handoff_key") == typed.get("replay_key"),
        "typed_replay_key": typed.get("replay_key"),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M252-C002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-C002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-C002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-C002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-C002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-C002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-C002-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-C002-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-C002-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_types, "M252-C002-TYPES-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_pipeline, "M252-C002-PIPE-EXISTS", FRONTEND_PIPELINE_SNIPPETS),
        (args.typed_surface, "M252-C002-TYPED-EXISTS", TYPED_SURFACE_SNIPPETS),
        (args.parse_readiness, "M252-C002-READINESS-EXISTS", PARSE_READINESS_SNIPPETS),
        (args.frontend_artifacts, "M252-C002-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.class_fixture, "M252-C002-FIX-CLASS-EXISTS", (FIXTURE_SNIPPETS[0],)),
        (args.category_fixture, "M252-C002-FIX-CATEGORY-EXISTS", (FIXTURE_SNIPPETS[1],)),
        (args.package_json, "M252-C002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        added_checks, added_failures = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += added_checks
        failures.extend(added_failures)

    runner_cases: dict[str, object] = {}
    runner_probes_executed = False
    if not args.skip_runner_probes:
        checks_total += require(args.runner_exe.exists(), display_path(args.runner_exe), "M252-C002-RUNNER-EXE", "native frontend C API runner binary is missing", failures)
        if args.runner_exe.exists():
            runner_probes_executed = True
            args.probe_root.mkdir(parents=True, exist_ok=True)
            class_checks, class_failures, class_payload = run_case(
                runner_exe=args.runner_exe,
                fixture_path=args.class_fixture,
                out_dir=args.probe_root / "class_protocol_property_ivar",
            )
            checks_total += class_checks
            failures.extend(class_failures)
            if class_payload is not None:
                runner_cases["class_protocol_property_ivar"] = class_payload

            category_checks, category_failures, category_payload = run_case(
                runner_exe=args.runner_exe,
                fixture_path=args.category_fixture,
                out_dir=args.probe_root / "category_protocol_property",
            )
            checks_total += category_checks
            failures.extend(category_failures)
            if category_payload is not None:
                runner_cases["category_protocol_property"] = category_payload

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [finding.__dict__ for finding in failures],
        "runner_probes_executed": runner_probes_executed,
        "runner_cases": runner_cases,
    }
    summary_path.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        print(f"[m252-c002] FAIL {checks_total - len(failures)}/{checks_total} -> {display_path(summary_path)}", file=sys.stderr)
        for finding in failures:
            print(f" - {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        return 1

    print(f"[m252-c002] PASS {checks_total}/{checks_total} -> {display_path(summary_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
