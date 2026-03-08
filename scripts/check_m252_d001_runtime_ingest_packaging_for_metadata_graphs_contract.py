#!/usr/bin/env python3
"""Fail-closed contract checker for M252-D001 runtime-ingest packaging."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-d001-runtime-ingest-packaging-for-metadata-graphs-contract-v1"
CONTRACT_ID = "objc3c-executable-metadata-runtime-ingest-packaging-boundary/m252-d001-v1"
TYPED_HANDOFF_CONTRACT_ID = "objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1"
DEBUG_PROJECTION_CONTRACT_ID = "objc3c-executable-metadata-debug-projection/m252-c003-v1"
PACKAGING_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_packaging_contract"
TYPED_HANDOFF_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_executable_metadata_typed_lowering_handoff"
DEBUG_PROJECTION_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_executable_metadata_debug_projection"
PAYLOAD_MODEL = "typed-handoff-plus-debug-projection-manifest-v1"
TRANSPORT_ARTIFACT = "module.manifest.json"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m252_runtime_ingest_packaging_for_metadata_graphs_contract_and_architecture_freeze_d001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m252" / "m252_d001_runtime_ingest_packaging_for_metadata_graphs_contract_and_architecture_freeze_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
DEFAULT_SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "d001-runtime-ingest-packaging"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-D001/runtime_ingest_packaging_for_metadata_graphs_contract_summary.json")


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
    SnippetCheck("M252-D001-DOC-EXP-01", "# M252 Runtime Ingest Packaging For Metadata Graphs Contract And Architecture Freeze Expectations (D001)"),
    SnippetCheck("M252-D001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M252-D001-DOC-EXP-03", "`Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary`"),
    SnippetCheck("M252-D001-DOC-EXP-04", f"`{PAYLOAD_MODEL}`"),
    SnippetCheck("M252-D001-DOC-EXP-05", f"`{TRANSPORT_ARTIFACT}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M252-D001-DOC-PKT-01", "# M252-D001 Runtime Ingest Packaging For Metadata Graphs Contract And Architecture Freeze Packet"),
    SnippetCheck("M252-D001-DOC-PKT-02", "Packet: `M252-D001`"),
    SnippetCheck("M252-D001-DOC-PKT-03", f"- Contract id `{CONTRACT_ID}`"),
    SnippetCheck("M252-D001-DOC-PKT-04", f"- Frozen payload model `{PAYLOAD_MODEL}`"),
    SnippetCheck("M252-D001-DOC-PKT-05", f"- Frozen transport artifact `{TRANSPORT_ARTIFACT}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M252-D001-ARCH-01", "M252 lane-D D001 runtime-ingest packaging freeze anchors explicit contract"),
    SnippetCheck("M252-D001-ARCH-02", "m252_runtime_ingest_packaging_for_metadata_graphs_contract_and_architecture_freeze_d001_expectations.md"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M252-D001-NDOC-01", "## Runtime ingest packaging boundary (M252-D001)"),
    SnippetCheck("M252-D001-NDOC-02", f"`{PACKAGING_SURFACE_PATH}`"),
    SnippetCheck("M252-D001-NDOC-03", f"`{PAYLOAD_MODEL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M252-D001-SPC-01", "## M252 runtime ingest packaging for metadata graphs (D001)"),
    SnippetCheck("M252-D001-SPC-02", "`Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary`"),
    SnippetCheck("M252-D001-SPC-03", f"`{CONTRACT_ID}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M252-D001-META-01", "## M252 runtime ingest packaging metadata anchors (D001)"),
    SnippetCheck("M252-D001-META-02", f"`{PACKAGING_SURFACE_PATH}`"),
    SnippetCheck("M252-D001-META-03", f"`{TRANSPORT_ARTIFACT}`"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M252-D001-PARSE-01", "M252-D001 runtime-ingest packaging anchor: lane-D packages this same"),
)
SEMA_CONTRACT_SNIPPETS = (
    SnippetCheck("M252-D001-SEMA-01", "M252-D001 runtime-ingest packaging anchor: the manifest packaging boundary"),
)
SEMA_PASSES_SNIPPETS = (
    SnippetCheck("M252-D001-PASS-01", "M252-D001 runtime-ingest packaging anchor: lane-D freezes one manifest"),
)
AST_SNIPPETS = (
    SnippetCheck("M252-D001-AST-01", "kObjc3ExecutableMetadataRuntimeIngestPackagingContractId"),
    SnippetCheck("M252-D001-AST-02", "kObjc3ExecutableMetadataRuntimeIngestPackagingSurfacePath"),
    SnippetCheck("M252-D001-AST-03", "kObjc3ExecutableMetadataRuntimeIngestPackagingPayloadModel"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M252-D001-TYPES-01", "struct Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary {"),
    SnippetCheck("M252-D001-TYPES-02", "std::string typed_lowering_handoff_replay_key;"),
    SnippetCheck("M252-D001-TYPES-03", "std::string debug_projection_replay_key;"),
    SnippetCheck("M252-D001-TYPES-04", "inline bool IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary("),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M252-D001-ART-01", "BuildExecutableMetadataRuntimeIngestPackagingContractSummary("),
    SnippetCheck("M252-D001-ART-02", "BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson("),
    SnippetCheck("M252-D001-ART-03", "objc_executable_metadata_runtime_ingest_packaging_contract"),
    SnippetCheck("M252-D001-ART-04", "executable_metadata_runtime_ingest_packaging_contract_id"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M252-D001-FIX-01", "module runtimeMetadataClassRecords;"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M252-D001-PKG-01", '"check:objc3c:m252-d001-runtime-ingest-packaging-for-metadata-graphs-contract": "python scripts/check_m252_d001_runtime_ingest_packaging_for_metadata_graphs_contract.py"'),
    SnippetCheck("M252-D001-PKG-02", '"test:tooling:m252-d001-runtime-ingest-packaging-for-metadata-graphs-contract": "python -m pytest tests/tooling/test_check_m252_d001_runtime_ingest_packaging_for_metadata_graphs_contract.py -q"'),
    SnippetCheck("M252-D001-PKG-03", '"check:objc3c:m252-d001-lane-d-readiness": "npm run check:objc3c:m252-c003-lane-c-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-d001-runtime-ingest-packaging-for-metadata-graphs-contract && npm run test:tooling:m252-d001-runtime-ingest-packaging-for-metadata-graphs-contract"'),
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
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
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


def run_runner(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, capture_output=True, text=True, check=False)


def run_manifest_case(*, runner_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-D001-FIXTURE-EXISTS", "fixture is missing", findings)
    checks_total += require(runner_exe.exists(), display_path(runner_exe), "M252-D001-RUNNER-EXISTS", "runner executable is missing", findings)
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
    completed = run_runner(command)
    summary_path = out_dir / "module.c_api_summary.json"
    manifest_path = out_dir / "module.manifest.json"

    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-D001-MANIFEST-SUMMARY", "runner summary missing", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-D001-MANIFEST-FILE", "runner manifest missing", findings)
    if findings:
        return checks_total, findings, None

    summary_payload = load_json(summary_path)
    manifest_payload = load_json(manifest_path)
    checks_total += require(completed.returncode == 0, display_path(summary_path), "M252-D001-MANIFEST-EXIT", "runner process exit mismatch", findings)
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), "M252-D001-MANIFEST-SUCCESS", "runner success mismatch", findings)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), "M252-D001-MANIFEST-STATUS", "runner status mismatch", findings)

    frontend = manifest_payload.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    sema_pass_manager = pipeline.get("sema_pass_manager") if isinstance(pipeline, dict) else None
    packaging = semantic_surface.get("objc_executable_metadata_runtime_ingest_packaging_contract") if isinstance(semantic_surface, dict) else None
    typed = semantic_surface.get("objc_executable_metadata_typed_lowering_handoff") if isinstance(semantic_surface, dict) else None
    debug = semantic_surface.get("objc_executable_metadata_debug_projection") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(packaging, dict), display_path(manifest_path), "M252-D001-PACKET-EXISTS", "runtime-ingest packaging packet missing from semantic surface", findings)
    checks_total += require(isinstance(typed, dict), display_path(manifest_path), "M252-D001-TYPED-EXISTS", "typed handoff packet missing from semantic surface", findings)
    checks_total += require(isinstance(debug, dict), display_path(manifest_path), "M252-D001-DEBUG-EXISTS", "debug projection packet missing from semantic surface", findings)
    checks_total += require(isinstance(sema_pass_manager, dict), display_path(manifest_path), "M252-D001-SPM-EXISTS", "sema pass manager summary missing", findings)
    if findings:
        return checks_total, findings, None

    checks_total += require(packaging.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M252-D001-CONTRACT", "contract id mismatch", findings)
    checks_total += require(packaging.get("typed_lowering_handoff_contract_id") == TYPED_HANDOFF_CONTRACT_ID, display_path(manifest_path), "M252-D001-TYPED-CONTRACT", "typed handoff contract id mismatch", findings)
    checks_total += require(packaging.get("debug_projection_contract_id") == DEBUG_PROJECTION_CONTRACT_ID, display_path(manifest_path), "M252-D001-DEBUG-CONTRACT", "debug projection contract id mismatch", findings)
    checks_total += require(packaging.get("packaging_surface_path") == PACKAGING_SURFACE_PATH, display_path(manifest_path), "M252-D001-PACKAGING-PATH", "packaging surface path mismatch", findings)
    checks_total += require(packaging.get("typed_handoff_surface_path") == TYPED_HANDOFF_SURFACE_PATH, display_path(manifest_path), "M252-D001-TYPED-PATH", "typed handoff surface path mismatch", findings)
    checks_total += require(packaging.get("debug_projection_surface_path") == DEBUG_PROJECTION_SURFACE_PATH, display_path(manifest_path), "M252-D001-DEBUG-PATH", "debug projection surface path mismatch", findings)
    checks_total += require(packaging.get("packaging_payload_model") == PAYLOAD_MODEL, display_path(manifest_path), "M252-D001-PAYLOAD-MODEL", "payload model mismatch", findings)
    checks_total += require(packaging.get("transport_artifact_relative_path") == TRANSPORT_ARTIFACT, display_path(manifest_path), "M252-D001-TRANSPORT-ARTIFACT", "transport artifact mismatch", findings)
    checks_total += require(packaging.get("ready") is True, display_path(manifest_path), "M252-D001-READY", "packaging summary must be ready", findings)
    checks_total += require(packaging.get("boundary_frozen") is True, display_path(manifest_path), "M252-D001-BOUNDARY-FROZEN", "boundary must be frozen", findings)
    checks_total += require(packaging.get("fail_closed") is True, display_path(manifest_path), "M252-D001-FAIL-CLOSED", "packaging summary must be fail-closed", findings)
    checks_total += require(packaging.get("typed_lowering_handoff_ready") is True, display_path(manifest_path), "M252-D001-TYPED-READY", "typed handoff must be ready", findings)
    checks_total += require(packaging.get("debug_projection_ready") is True, display_path(manifest_path), "M252-D001-DEBUG-READY", "debug projection must be ready", findings)
    checks_total += require(packaging.get("manifest_transport_frozen") is True, display_path(manifest_path), "M252-D001-MANIFEST-FROZEN", "manifest transport must be frozen", findings)
    checks_total += require(packaging.get("runtime_section_emission_not_yet_landed") is True, display_path(manifest_path), "M252-D001-NONGOAL-SECTION", "runtime section emission non-goal must remain explicit", findings)
    checks_total += require(packaging.get("startup_registration_not_yet_landed") is True, display_path(manifest_path), "M252-D001-NONGOAL-STARTUP", "startup registration non-goal must remain explicit", findings)
    checks_total += require(packaging.get("runtime_loader_registration_not_yet_landed") is True, display_path(manifest_path), "M252-D001-NONGOAL-LOADER", "runtime loader registration non-goal must remain explicit", findings)
    checks_total += require(packaging.get("explicit_non_goals_published") is True, display_path(manifest_path), "M252-D001-NONGOALS-PUBLISHED", "non-goals must be published", findings)
    checks_total += require(packaging.get("ready_for_packaging_implementation") is True, display_path(manifest_path), "M252-D001-READY-FOR-IMPL", "ready-for-packaging-implementation must be true", findings)
    checks_total += require(packaging.get("typed_lowering_handoff_replay_key") == typed.get("replay_key"), display_path(manifest_path), "M252-D001-TYPED-REPLAY", "typed handoff replay key mismatch", findings)
    checks_total += require(packaging.get("debug_projection_replay_key") == debug.get("replay_key"), display_path(manifest_path), "M252-D001-DEBUG-REPLAY", "debug projection replay key mismatch", findings)
    checks_total += require(bool(packaging.get("replay_key")), display_path(manifest_path), "M252-D001-REPLAY-KEY", "packaging replay key must be non-empty", findings)
    checks_total += require(packaging.get("failure_reason") == "", display_path(manifest_path), "M252-D001-FAILURE-REASON", "failure reason must be empty", findings)

    checks_total += require(sema_pass_manager.get("executable_metadata_runtime_ingest_packaging_contract_id") == CONTRACT_ID, display_path(manifest_path), "M252-D001-FLAT-CONTRACT", "flattened contract id mismatch", findings)
    checks_total += require(sema_pass_manager.get("executable_metadata_runtime_ingest_packaging_payload_model") == PAYLOAD_MODEL, display_path(manifest_path), "M252-D001-FLAT-PAYLOAD", "flattened payload model mismatch", findings)
    checks_total += require(sema_pass_manager.get("executable_metadata_runtime_ingest_packaging_transport_artifact_relative_path") == TRANSPORT_ARTIFACT, display_path(manifest_path), "M252-D001-FLAT-TRANSPORT", "flattened transport artifact mismatch", findings)
    checks_total += require(sema_pass_manager.get("executable_metadata_runtime_ingest_packaging_ready_for_packaging_implementation") is True, display_path(manifest_path), "M252-D001-FLAT-READY", "flattened ready-for-packaging-implementation mismatch", findings)

    case_payload: dict[str, object] = {
        "fixture": display_path(fixture_path),
        "manifest_path": display_path(manifest_path),
        "typed_handoff_replay_key": packaging.get("typed_lowering_handoff_replay_key"),
        "debug_projection_replay_key": packaging.get("debug_projection_replay_key"),
        "replay_key": packaging.get("replay_key"),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(list(argv) if argv is not None else sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    dynamic_cases: list[dict[str, object]] = []

    doc_checks: tuple[tuple[Path, str, tuple[SnippetCheck, ...]], ...] = (
        (args.expectations_doc, "M252-D001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-D001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-D001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-D001-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-D001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-D001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-D001-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-D001-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-D001-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.ast_header, "M252-D001-AST-EXISTS", AST_SNIPPETS),
        (args.frontend_types, "M252-D001-TYPES-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, "M252-D001-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.class_fixture, "M252-D001-FIX-EXISTS", FIXTURE_SNIPPETS),
        (args.package_json, "M252-D001-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in doc_checks:
        added_checks, added_failures = check_doc_contract(
            path=path, exists_check_id=exists_check_id, snippets=snippets
        )
        checks_total += added_checks
        failures.extend(added_failures)

    dynamic_probes_executed = not args.skip_runner_probes
    if dynamic_probes_executed:
        probe_root = args.probe_root.resolve()
        case_checks, case_failures, case_payload = run_manifest_case(
            runner_exe=args.runner_exe.resolve(),
            fixture_path=args.class_fixture.resolve(),
            out_dir=probe_root / "class-protocol-property-ivar",
        )
        checks_total += case_checks
        failures.extend(case_failures)
        if case_payload is not None:
            dynamic_cases.append(case_payload)

    checks_passed = checks_total - len(failures)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "dynamic_cases": dynamic_cases,
        "failures": [failure.__dict__ for failure in failures],
    }

    summary_out = args.summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        print(f"[m252-d001] FAIL {checks_passed}/{checks_total} -> {display_path(summary_out)}")
        for failure in failures:
            print(f"  - {failure.check_id} @ {failure.artifact}: {failure.detail}")
        return 1

    print(f"[m252-d001] PASS {checks_passed}/{checks_total} -> {display_path(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
