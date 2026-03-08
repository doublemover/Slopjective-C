#!/usr/bin/env python3
"""Fail-closed contract checker for M252-C001 metadata graph lowering handoff."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-c001-metadata-graph-lowering-handoff-contract-v1"
CONTRACT_ID = "objc3c-executable-metadata-lowering-handoff-freeze/m252-c001-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m252_metadata_graph_lowering_handoff_contract_and_architecture_freeze_c001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m252" / "m252_c001_metadata_graph_lowering_handoff_contract_and_architecture_freeze_packet.md"
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
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "c001-metadata-graph-lowering-handoff"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-C001/metadata_graph_lowering_handoff_contract_summary.json")


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
    SnippetCheck("M252-C001-DOC-EXP-01", "# M252 Metadata Graph Lowering Handoff Contract And Architecture Freeze Expectations (C001)"),
    SnippetCheck("M252-C001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M252-C001-DOC-EXP-03", "Objc3ExecutableMetadataLoweringHandoffSurface"),
    SnippetCheck("M252-C001-DOC-EXP-04", "objc_executable_metadata_lowering_handoff_surface"),
    SnippetCheck("M252-C001-DOC-EXP-05", "ready_for_lowering == false"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M252-C001-DOC-PKT-01", "# M252-C001 Metadata Graph Lowering Handoff Contract And Architecture Freeze Packet"),
    SnippetCheck("M252-C001-DOC-PKT-02", "Packet: `M252-C001`"),
    SnippetCheck("M252-C001-DOC-PKT-03", "- `M252-B004`"),
    SnippetCheck("M252-C001-DOC-PKT-04", "The packet remains fail-closed and `ready_for_lowering == false`."),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M252-C001-ARCH-01", "M252 lane-C C001 metadata graph lowering handoff"),
    SnippetCheck("M252-C001-ARCH-02", "typed and parse/lowering surfaces project one canonical metadata handoff schema"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M252-C001-NDOC-01", "## Metadata graph lowering handoff freeze (M252-C001)"),
    SnippetCheck("M252-C001-NDOC-02", "`frontend.pipeline.semantic_surface.objc_executable_metadata_lowering_handoff_surface`"),
    SnippetCheck("M252-C001-NDOC-03", "`tmp/reports/m252/M252-C001/metadata_graph_lowering_handoff_contract_summary.json`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M252-C001-SPC-01", "## M252 metadata graph lowering handoff freeze (C001)"),
    SnippetCheck("M252-C001-SPC-02", "Objc3ExecutableMetadataLoweringHandoffSurface"),
    SnippetCheck("M252-C001-SPC-03", "ready_for_lowering == false"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M252-C001-META-01", "## M252 metadata graph lowering handoff metadata anchors (C001)"),
    SnippetCheck("M252-C001-META-02", "objc_executable_metadata_lowering_handoff_surface"),
    SnippetCheck("M252-C001-META-03", "metadata_graph_lowering_handoff_contract_summary.json"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M252-C001-PARSE-01", "M252-C001 lowering-handoff anchor: category semantic-link symbols remain"),
)
SEMA_CONTRACT_SNIPPETS = (
    SnippetCheck("M252-C001-SEMA-01", "M252-C001 lowering-handoff anchor: this typed metadata handoff is the"),
)
SEMA_PASSES_SNIPPETS = (
    SnippetCheck("M252-C001-PASS-01", "M252-C001 lowering-handoff anchor: semantic integration plus type metadata"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M252-C001-TYPES-01", "kObjc3ExecutableMetadataLoweringHandoffContractId"),
    SnippetCheck("M252-C001-TYPES-02", "struct Objc3ExecutableMetadataLoweringHandoffSurface"),
    SnippetCheck("M252-C001-TYPES-03", "IsReadyObjc3ExecutableMetadataLoweringHandoffSurface"),
)
FRONTEND_PIPELINE_SNIPPETS = (
    SnippetCheck("M252-C001-PIPE-01", "BuildExecutableMetadataLoweringHandoffReplayKey"),
    SnippetCheck("M252-C001-PIPE-02", "BuildExecutableMetadataLoweringHandoffSurface("),
    SnippetCheck("M252-C001-PIPE-03", "result.executable_metadata_lowering_handoff_surface ="),
)
TYPED_SURFACE_SNIPPETS = (
    SnippetCheck("M252-C001-TYPED-01", "metadata_graph_lowering_handoff_ready="),
    SnippetCheck("M252-C001-TYPED-02", "surface.executable_metadata_lowering_handoff_ready ="),
    SnippetCheck("M252-C001-TYPED-03", "surface.executable_metadata_lowering_handoff_key ="),
)
PARSE_READINESS_SNIPPETS = (
    SnippetCheck("M252-C001-READINESS-01", "surface.executable_metadata_lowering_handoff_ready ="),
    SnippetCheck("M252-C001-READINESS-02", "surface.executable_metadata_lowering_handoff_key ="),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M252-C001-ART-01", "BuildExecutableMetadataLoweringHandoffSurfaceJson"),
    SnippetCheck("M252-C001-ART-02", "objc_executable_metadata_lowering_handoff_surface"),
    SnippetCheck("M252-C001-ART-03", "executable_metadata_lowering_handoff_key"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M252-C001-FIX-01", "module runtimeMetadataClassRecords;"),
    SnippetCheck("M252-C001-FIX-02", "module runtimeMetadataCategoryRecords;"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M252-C001-PKG-01", '"check:objc3c:m252-c001-metadata-graph-lowering-handoff-contract": "python scripts/check_m252_c001_metadata_graph_lowering_handoff_contract.py"'),
    SnippetCheck("M252-C001-PKG-02", '"test:tooling:m252-c001-metadata-graph-lowering-handoff-contract": "python -m pytest tests/tooling/test_check_m252_c001_metadata_graph_lowering_handoff_contract.py -q"'),
    SnippetCheck("M252-C001-PKG-03", '"check:objc3c:m252-c001-lane-c-readiness": "npm run check:objc3c:m252-b004-lane-b-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-c001-metadata-graph-lowering-handoff-contract && npm run test:tooling:m252-c001-metadata-graph-lowering-handoff-contract"'),
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
    parser.add_argument("--sema-contract", dest="sema_contract", type=Path, default=DEFAULT_SEMA_CONTRACT)
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


def entry_count(value: object) -> int:
    if isinstance(value, list):
        return len(value)
    if isinstance(value, int):
        return value
    raise TypeError(f"expected list-or-int entry payload, observed {type(value).__name__}")


def run_case(*, runner_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-C001-FIXTURE-EXISTS", "fixture is missing", findings)
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
    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-C001-RUNNER-SUMMARY", "runner summary missing", findings)
    checks_total += require(diagnostics_path.exists(), display_path(diagnostics_path), "M252-C001-RUNNER-DIAGNOSTICS", "runner diagnostics missing", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-C001-RUNNER-MANIFEST", "runner manifest missing", findings)
    if findings:
        return checks_total, findings, None

    summary_payload = load_json(summary_path)
    diagnostics_payload = load_json(diagnostics_path)
    manifest_payload = load_json(manifest_path)
    diagnostics = diagnostics_payload.get("diagnostics")
    checks_total += require(isinstance(diagnostics, list), display_path(diagnostics_path), "M252-C001-DIAGNOSTICS-LIST", "diagnostics payload must contain a diagnostics list", findings)
    if not isinstance(diagnostics, list):
        return checks_total, findings, None
    observed_codes = [entry.get("code") for entry in diagnostics if isinstance(entry, dict)]
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), "M252-C001-RUNNER-SUCCESS", "runner success mismatch", findings)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), "M252-C001-RUNNER-STATUS", "runner status mismatch", findings)
    checks_total += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M252-C001-SEMANTIC-SKIPPED", "semantic stage unexpectedly skipped", findings)
    checks_total += require(completed.returncode == 0, display_path(summary_path), "M252-C001-PROCESS-EXIT", "runner process exit mismatch", findings)
    checks_total += require(observed_codes == [], display_path(diagnostics_path), "M252-C001-DIAGNOSTIC-CODES", f"expected no diagnostics, observed {observed_codes}", findings)

    pipeline = manifest_payload["frontend"]["pipeline"]
    graph = pipeline["semantic_surface"]["objc_executable_metadata_source_graph"]
    handoff = pipeline["semantic_surface"]["objc_executable_metadata_lowering_handoff_surface"]
    parse = pipeline["parse_lowering_readiness"]

    checks_total += require(handoff.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M252-C001-HANDOFF-CONTRACT", "handoff contract id mismatch", findings)
    checks_total += require(handoff.get("ready") is True, display_path(manifest_path), "M252-C001-HANDOFF-READY", "handoff surface must be ready", findings)
    checks_total += require(handoff.get("fail_closed") is True, display_path(manifest_path), "M252-C001-HANDOFF-FAIL-CLOSED", "handoff surface must be fail-closed", findings)
    checks_total += require(handoff.get("ready_for_lowering") is False, display_path(manifest_path), "M252-C001-HANDOFF-NOT-LOWERING-READY", "handoff surface must remain not lowering-ready", findings)
    checks_total += require(handoff.get("semantic_validation_ready") is True, display_path(manifest_path), "M252-C001-HANDOFF-VALIDATION-READY", "semantic validation readiness mismatch", findings)
    checks_total += require(bool(handoff.get("replay_key")), display_path(manifest_path), "M252-C001-HANDOFF-REPLAY-KEY", "handoff replay key must be non-empty", findings)
    checks_total += require(parse.get("executable_metadata_lowering_handoff_ready") is True, display_path(manifest_path), "M252-C001-PARSE-HANDOFF-READY", "parse/lowering projection must publish handoff ready", findings)
    checks_total += require(parse.get("executable_metadata_lowering_handoff_deterministic") is True, display_path(manifest_path), "M252-C001-PARSE-HANDOFF-DETERMINISTIC", "parse/lowering projection must publish handoff deterministic", findings)
    checks_total += require(parse.get("executable_metadata_lowering_handoff_key") == handoff.get("replay_key"), display_path(manifest_path), "M252-C001-PARSE-HANDOFF-KEY", "parse/lowering handoff key must match semantic surface replay key", findings)

    expected_counts = {
        "interface_node_count": entry_count(graph["interface_node_entries"]),
        "implementation_node_count": entry_count(
            graph["implementation_node_entries"]
        ),
        "class_node_count": entry_count(graph["class_node_entries"]),
        "metaclass_node_count": entry_count(graph["metaclass_node_entries"]),
        "protocol_node_count": entry_count(graph["protocol_node_entries"]),
        "category_node_count": entry_count(graph["category_node_entries"]),
        "property_node_count": entry_count(graph["property_node_entries"]),
        "method_node_count": entry_count(graph["method_node_entries"]),
        "ivar_node_count": entry_count(graph["ivar_node_entries"]),
        "owner_edge_count": entry_count(graph["owner_edges"]),
    }
    for key, expected_value in expected_counts.items():
        checks_total += require(handoff.get(key) == expected_value, display_path(manifest_path), f"M252-C001-{key.upper()}", f"handoff {key} mismatch: expected {expected_value}, observed {handoff.get(key)}", findings)

    case_payload = {
        "fixture": display_path(fixture_path),
        "summary_path": display_path(summary_path),
        "manifest_path": display_path(manifest_path),
        "handoff_ready": handoff.get("ready"),
        "parse_handoff_key_matches": parse.get("executable_metadata_lowering_handoff_key") == handoff.get("replay_key"),
        "replay_key": handoff.get("replay_key"),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M252-C001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-C001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-C001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-C001-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-C001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-C001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-C001-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-C001-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-C001-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_types, "M252-C001-TYPES-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_pipeline, "M252-C001-PIPE-EXISTS", FRONTEND_PIPELINE_SNIPPETS),
        (args.typed_surface, "M252-C001-TYPED-EXISTS", TYPED_SURFACE_SNIPPETS),
        (args.parse_readiness, "M252-C001-READINESS-EXISTS", PARSE_READINESS_SNIPPETS),
        (args.frontend_artifacts, "M252-C001-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.class_fixture, "M252-C001-FIX-CLASS-EXISTS", (FIXTURE_SNIPPETS[0],)),
        (args.category_fixture, "M252-C001-FIX-CATEGORY-EXISTS", (FIXTURE_SNIPPETS[1],)),
        (args.package_json, "M252-C001-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        added_checks, added_failures = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += added_checks
        failures.extend(added_failures)

    runner_cases: dict[str, object] = {}
    runner_probes_executed = False
    if not args.skip_runner_probes:
        checks_total += require(args.runner_exe.exists(), display_path(args.runner_exe), "M252-C001-RUNNER-EXE", "native frontend C API runner binary is missing", failures)
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

    ok = not failures
    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [finding.__dict__ for finding in failures],
        "runner_probes_executed": runner_probes_executed,
        "runner_cases": runner_cases,
    }
    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if ok:
        print(f"[m252-c001] PASS {checks_total}/{checks_total} -> {display_path(summary_path)}")
        return 0

    print(f"[m252-c001] FAIL {checks_total - len(failures)}/{checks_total} -> {display_path(summary_path)}", file=sys.stderr)
    for finding in failures:
        print(f" - {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
