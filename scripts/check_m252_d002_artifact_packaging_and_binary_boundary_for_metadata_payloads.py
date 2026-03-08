#!/usr/bin/env python3
"""Fail-closed checker for M252-D002 runtime-ingest binary packaging."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-d002-artifact-packaging-and-binary-boundary-for-metadata-payloads-v1"
CONTRACT_ID = "objc3c-executable-metadata-runtime-ingest-binary-boundary/m252-d002-v1"
PACKAGING_CONTRACT_ID = "objc3c-executable-metadata-runtime-ingest-packaging-boundary/m252-d001-v1"
TYPED_HANDOFF_CONTRACT_ID = "objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1"
DEBUG_PROJECTION_CONTRACT_ID = "objc3c-executable-metadata-debug-projection/m252-c003-v1"
PACKAGING_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_packaging_contract"
BINARY_BOUNDARY_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_binary_boundary"
PAYLOAD_MODEL = "typed-handoff-plus-debug-projection-manifest-v1"
ENVELOPE_FORMAT = "objc3-runtime-metadata-envelope-v1"
BINARY_ARTIFACT = "module.runtime-metadata.bin"
BINARY_MAGIC = b"OBJC3RM1"
CHUNK_NAMES = (
    "runtime_ingest_packaging_contract",
    "typed_lowering_handoff",
    "debug_projection",
)

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m252_artifact_packaging_and_binary_boundary_for_metadata_payloads_d002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m252" / "m252_d002_artifact_packaging_and_binary_boundary_for_metadata_payloads_packet.md"
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
DEFAULT_FRONTEND_ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
DEFAULT_FILE_IO_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_file_io.h"
DEFAULT_FILE_IO_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_file_io.cpp"
DEFAULT_MANIFEST_ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
DEFAULT_MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_FRONTEND_ANCHOR = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_RUNNER_CPP = ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "d002-runtime-ingest-binary-boundary"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-D002/artifact_packaging_and_binary_boundary_for_metadata_payloads_summary.json")


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
    SnippetCheck("M252-D002-DOC-EXP-01", "# M252 Artifact Packaging And Binary Boundary For Metadata Payloads Expectations (D002)"),
    SnippetCheck("M252-D002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M252-D002-DOC-EXP-03", "`Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary`"),
    SnippetCheck("M252-D002-DOC-EXP-04", f"`{BINARY_ARTIFACT}`"),
    SnippetCheck("M252-D002-DOC-EXP-05", f"`{ENVELOPE_FORMAT}`"),
    SnippetCheck("M252-D002-DOC-EXP-06", f"`{BINARY_MAGIC.decode('ascii')}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M252-D002-DOC-PKT-01", "# M252-D002 Artifact Packaging And Binary Boundary For Metadata Payloads Packet"),
    SnippetCheck("M252-D002-DOC-PKT-02", "Packet: `M252-D002`"),
    SnippetCheck("M252-D002-DOC-PKT-03", f"- Contract id `{CONTRACT_ID}`"),
    SnippetCheck("M252-D002-DOC-PKT-04", f"- Emitted artifact `{BINARY_ARTIFACT}`"),
    SnippetCheck("M252-D002-DOC-PKT-05", f"- Envelope format `{ENVELOPE_FORMAT}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M252-D002-ARCH-01", "M252 lane-D D002 runtime-ingest binary boundary anchors explicit artifact"),
    SnippetCheck("M252-D002-ARCH-02", "m252_artifact_packaging_and_binary_boundary_for_metadata_payloads_d002_expectations.md"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M252-D002-NDOC-01", "## Runtime ingest binary boundary (M252-D002)"),
    SnippetCheck("M252-D002-NDOC-02", f"`{BINARY_BOUNDARY_SURFACE_PATH}`"),
    SnippetCheck("M252-D002-NDOC-03", f"`{BINARY_ARTIFACT}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M252-D002-SPC-01", "## M252 artifact packaging and binary boundary for metadata payloads (D002)"),
    SnippetCheck("M252-D002-SPC-02", "`Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary`"),
    SnippetCheck("M252-D002-SPC-03", f"`{CONTRACT_ID}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M252-D002-META-01", "## M252 runtime ingest binary boundary metadata anchors (D002)"),
    SnippetCheck("M252-D002-META-02", f"`{BINARY_BOUNDARY_SURFACE_PATH}`"),
    SnippetCheck("M252-D002-META-03", f"`{BINARY_ARTIFACT}`"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M252-D002-PARSE-01", "M252-D002 binary-boundary anchor: lane-D packages this same semantic-link"),
)
SEMA_CONTRACT_SNIPPETS = (
    SnippetCheck("M252-D002-SEMA-01", "M252-D002 binary-boundary anchor: the executable metadata binary envelope"),
)
SEMA_PASSES_SNIPPETS = (
    SnippetCheck("M252-D002-PASS-01", "M252-D002 binary-boundary anchor: lane-D packages this exact sema-owned"),
)
AST_SNIPPETS = (
    SnippetCheck("M252-D002-AST-01", "kObjc3ExecutableMetadataRuntimeIngestBinaryBoundaryContractId"),
    SnippetCheck("M252-D002-AST-02", "kObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySurfacePath"),
    SnippetCheck("M252-D002-AST-03", "kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeFormat"),
    SnippetCheck("M252-D002-AST-04", "kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactSuffix"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M252-D002-TYPES-01", "struct Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary {"),
    SnippetCheck("M252-D002-TYPES-02", "std::array<std::string, 3u> chunk_names = {"),
    SnippetCheck("M252-D002-TYPES-03", "bool ready_for_section_emission_handoff = false;"),
    SnippetCheck("M252-D002-TYPES-04", "inline bool IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary("),
)
FRONTEND_ARTIFACTS_HEADER_SNIPPETS = (
    SnippetCheck("M252-D002-ARTH-01", "std::string runtime_metadata_binary;"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M252-D002-ART-01", "BuildExecutableMetadataRuntimeIngestBinaryEnvelope("),
    SnippetCheck("M252-D002-ART-02", "BuildExecutableMetadataRuntimeIngestBinaryBoundarySummary("),
    SnippetCheck("M252-D002-ART-03", "BuildExecutableMetadataRuntimeIngestBinaryBoundarySummaryJson("),
    SnippetCheck("M252-D002-ART-04", "objc_executable_metadata_runtime_ingest_binary_boundary"),
    SnippetCheck("M252-D002-ART-05", "bundle.runtime_metadata_binary = executable_metadata_runtime_ingest_binary_payload;"),
)
FILE_IO_HEADER_SNIPPETS = (
    SnippetCheck("M252-D002-IOH-01", "void WriteBytes(const std::filesystem::path &path, const std::string &contents);"),
)
FILE_IO_CPP_SNIPPETS = (
    SnippetCheck("M252-D002-IOC-01", "void WriteBytes(const std::filesystem::path &path, const std::string &contents) {"),
    SnippetCheck("M252-D002-IOC-02", "out.write(contents.data(), static_cast<std::streamsize>(contents.size()));"),
)
MANIFEST_ARTIFACTS_HEADER_SNIPPETS = (
    SnippetCheck("M252-D002-MAH-01", "BuildRuntimeMetadataBinaryArtifactPath("),
    SnippetCheck("M252-D002-MAH-02", "WriteRuntimeMetadataBinaryArtifact("),
)
MANIFEST_ARTIFACTS_CPP_SNIPPETS = (
    SnippetCheck("M252-D002-MAC-01", "BuildRuntimeMetadataBinaryArtifactPath("),
    SnippetCheck("M252-D002-MAC-02", "WriteRuntimeMetadataBinaryArtifact("),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M252-D002-DRV-01", "if (!artifacts.runtime_metadata_binary.empty()) {"),
    SnippetCheck("M252-D002-DRV-02", "WriteRuntimeMetadataBinaryArtifact(cli_options.out_dir,"),
)
FRONTEND_ANCHOR_SNIPPETS = (
    SnippetCheck("M252-D002-FEA-01", "std::string runtime_metadata_binary_path;"),
    SnippetCheck("M252-D002-FEA-02", "static bool WriteBinaryFile("),
    SnippetCheck("M252-D002-FEA-03", "BuildRuntimeMetadataBinaryArtifactPath(out_dir, emit_prefix);"),
)
RUNNER_SNIPPETS = (
    SnippetCheck("M252-D002-RUN-01", "BuildRuntimeMetadataBinaryArtifactPath(options.out_dir, options.emit_prefix);"),
    SnippetCheck("M252-D002-RUN-02", "runtime_metadata_binary"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M252-D002-FIX-01", "module runtimeMetadataClassRecords;"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M252-D002-PKG-01", '"check:objc3c:m252-d002-artifact-packaging-and-binary-boundary-for-metadata-payloads": "python scripts/check_m252_d002_artifact_packaging_and_binary_boundary_for_metadata_payloads.py"'),
    SnippetCheck("M252-D002-PKG-02", '"test:tooling:m252-d002-artifact-packaging-and-binary-boundary-for-metadata-payloads": "python -m pytest tests/tooling/test_check_m252_d002_artifact_packaging_and_binary_boundary_for_metadata_payloads.py -q"'),
    SnippetCheck("M252-D002-PKG-03", '"check:objc3c:m252-d002-lane-d-readiness": "npm run check:objc3c:m252-d001-lane-d-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-d002-artifact-packaging-and-binary-boundary-for-metadata-payloads && npm run test:tooling:m252-d002-artifact-packaging-and-binary-boundary-for-metadata-payloads"'),
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
    parser.add_argument("--frontend-artifacts-header", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS_HEADER)
    parser.add_argument("--file-io-header", type=Path, default=DEFAULT_FILE_IO_HEADER)
    parser.add_argument("--file-io-cpp", type=Path, default=DEFAULT_FILE_IO_CPP)
    parser.add_argument("--manifest-artifacts-header", type=Path, default=DEFAULT_MANIFEST_ARTIFACTS_HEADER)
    parser.add_argument("--manifest-artifacts-cpp", type=Path, default=DEFAULT_MANIFEST_ARTIFACTS_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--frontend-anchor", type=Path, default=DEFAULT_FRONTEND_ANCHOR)
    parser.add_argument("--runner-cpp", type=Path, default=DEFAULT_RUNNER_CPP)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
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


def parse_binary_envelope(binary_path: Path) -> tuple[int, list[Finding], dict[str, Any] | None]:
    findings: list[Finding] = []
    checks_total = 0
    data = binary_path.read_bytes()
    checks_total += require(len(data) >= len(BINARY_MAGIC) + 8, display_path(binary_path), "M252-D002-BIN-LEN", "binary envelope is shorter than the header", findings)
    if findings:
        return checks_total, findings, None

    cursor = 0
    magic = data[: len(BINARY_MAGIC)]
    cursor += len(BINARY_MAGIC)
    version = int.from_bytes(data[cursor : cursor + 4], "little")
    cursor += 4
    chunk_count = int.from_bytes(data[cursor : cursor + 4], "little")
    cursor += 4

    checks_total += require(magic == BINARY_MAGIC, display_path(binary_path), "M252-D002-BIN-MAGIC", "binary magic mismatch", findings)
    checks_total += require(version == 1, display_path(binary_path), "M252-D002-BIN-VERSION", "binary version mismatch", findings)
    checks_total += require(chunk_count == len(CHUNK_NAMES), display_path(binary_path), "M252-D002-BIN-COUNT", "binary chunk count mismatch", findings)
    if findings:
        return checks_total, findings, None

    chunks: list[dict[str, Any]] = []
    for index in range(chunk_count):
        checks_total += require(cursor + 4 <= len(data), display_path(binary_path), f"M252-D002-BIN-NAME-LEN-{index}", "binary truncated before chunk-name length", findings)
        if findings:
            return checks_total, findings, None
        name_len = int.from_bytes(data[cursor : cursor + 4], "little")
        cursor += 4
        checks_total += require(cursor + name_len <= len(data), display_path(binary_path), f"M252-D002-BIN-NAME-{index}", "binary truncated before chunk name", findings)
        if findings:
            return checks_total, findings, None
        name = data[cursor : cursor + name_len].decode("utf-8")
        cursor += name_len
        checks_total += require(cursor + 4 <= len(data), display_path(binary_path), f"M252-D002-BIN-PAYLOAD-LEN-{index}", "binary truncated before chunk payload length", findings)
        if findings:
            return checks_total, findings, None
        payload_len = int.from_bytes(data[cursor : cursor + 4], "little")
        cursor += 4
        checks_total += require(cursor + payload_len <= len(data), display_path(binary_path), f"M252-D002-BIN-PAYLOAD-{index}", "binary truncated before chunk payload", findings)
        if findings:
            return checks_total, findings, None
        payload_text = data[cursor : cursor + payload_len].decode("utf-8")
        cursor += payload_len
        chunks.append(
            {
                "name": name,
                "payload_len": payload_len,
                "payload": json.loads(payload_text),
            }
        )

    checks_total += require(cursor == len(data), display_path(binary_path), "M252-D002-BIN-TRAILING", "binary envelope has trailing bytes", findings)
    checks_total += require([chunk["name"] for chunk in chunks] == list(CHUNK_NAMES), display_path(binary_path), "M252-D002-BIN-NAMES", "binary chunk ordering mismatch", findings)
    if findings:
        return checks_total, findings, None

    return checks_total, findings, {
        "magic": magic.decode("ascii"),
        "version": version,
        "chunk_count": chunk_count,
        "chunks": chunks,
        "payload_bytes": len(data),
    }


def run_binary_case(*, runner_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-D002-FIXTURE-EXISTS", "fixture is missing", findings)
    checks_total += require(runner_exe.exists(), display_path(runner_exe), "M252-D002-RUNNER-EXISTS", "runner executable is missing", findings)
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
    binary_path = out_dir / BINARY_ARTIFACT

    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-D002-SUMMARY", "runner summary missing", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-D002-MANIFEST", "runner manifest missing", findings)
    checks_total += require(binary_path.exists(), display_path(binary_path), "M252-D002-BINARY", "runtime metadata binary missing", findings)
    if findings:
        return checks_total, findings, None

    summary_payload = load_json(summary_path)
    manifest_payload = load_json(manifest_path)

    checks_total += require(completed.returncode == 0, display_path(summary_path), "M252-D002-EXIT", "runner process exit mismatch", findings)
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), "M252-D002-SUCCESS", "runner success mismatch", findings)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), "M252-D002-STATUS", "runner status mismatch", findings)

    summary_paths = summary_payload.get("paths", {})
    runtime_metadata_binary_path = str(summary_paths.get("runtime_metadata_binary", ""))
    checks_total += require(runtime_metadata_binary_path.endswith(BINARY_ARTIFACT), display_path(summary_path), "M252-D002-SUMMARY-PATH", "summary must publish runtime metadata binary path", findings)

    frontend = manifest_payload.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    packaging = semantic_surface.get("objc_executable_metadata_runtime_ingest_packaging_contract") if isinstance(semantic_surface, dict) else None
    typed = semantic_surface.get("objc_executable_metadata_typed_lowering_handoff") if isinstance(semantic_surface, dict) else None
    debug = semantic_surface.get("objc_executable_metadata_debug_projection") if isinstance(semantic_surface, dict) else None
    binary_boundary = semantic_surface.get("objc_executable_metadata_runtime_ingest_binary_boundary") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(packaging, dict), display_path(manifest_path), "M252-D002-PACKAGING-EXISTS", "packaging packet missing", findings)
    checks_total += require(isinstance(typed, dict), display_path(manifest_path), "M252-D002-TYPED-EXISTS", "typed handoff packet missing", findings)
    checks_total += require(isinstance(debug, dict), display_path(manifest_path), "M252-D002-DEBUG-EXISTS", "debug projection packet missing", findings)
    checks_total += require(isinstance(binary_boundary, dict), display_path(manifest_path), "M252-D002-BOUNDARY-EXISTS", "binary boundary packet missing", findings)
    if findings:
        return checks_total, findings, None

    checks_total += require(binary_boundary.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M252-D002-CONTRACT", "binary boundary contract id mismatch", findings)
    checks_total += require(binary_boundary.get("packaging_contract_id") == PACKAGING_CONTRACT_ID, display_path(manifest_path), "M252-D002-PACKAGING-CONTRACT", "packaging contract id mismatch", findings)
    checks_total += require(binary_boundary.get("typed_lowering_handoff_contract_id") == TYPED_HANDOFF_CONTRACT_ID, display_path(manifest_path), "M252-D002-TYPED-CONTRACT", "typed contract id mismatch", findings)
    checks_total += require(binary_boundary.get("debug_projection_contract_id") == DEBUG_PROJECTION_CONTRACT_ID, display_path(manifest_path), "M252-D002-DEBUG-CONTRACT", "debug contract id mismatch", findings)
    checks_total += require(binary_boundary.get("binary_boundary_surface_path") == BINARY_BOUNDARY_SURFACE_PATH, display_path(manifest_path), "M252-D002-SURFACE", "binary boundary surface path mismatch", findings)
    checks_total += require(binary_boundary.get("packaging_surface_path") == PACKAGING_SURFACE_PATH, display_path(manifest_path), "M252-D002-PACKAGING-SURFACE", "packaging surface path mismatch", findings)
    checks_total += require(binary_boundary.get("payload_model") == PAYLOAD_MODEL, display_path(manifest_path), "M252-D002-PAYLOAD-MODEL", "payload model mismatch", findings)
    checks_total += require(binary_boundary.get("envelope_format") == ENVELOPE_FORMAT, display_path(manifest_path), "M252-D002-ENVELOPE", "envelope format mismatch", findings)
    checks_total += require(binary_boundary.get("artifact_relative_path") == BINARY_ARTIFACT, display_path(manifest_path), "M252-D002-ARTIFACT", "artifact relative path mismatch", findings)
    checks_total += require(binary_boundary.get("binary_magic") == BINARY_MAGIC.decode("ascii"), display_path(manifest_path), "M252-D002-MAGIC", "binary magic mismatch", findings)
    checks_total += require(binary_boundary.get("chunk_names") == list(CHUNK_NAMES), display_path(manifest_path), "M252-D002-CHUNK-NAMES", "chunk names mismatch", findings)
    checks_total += require(binary_boundary.get("ready") is True, display_path(manifest_path), "M252-D002-READY", "binary boundary must be ready", findings)
    checks_total += require(binary_boundary.get("binary_boundary_emitted") is True, display_path(manifest_path), "M252-D002-EMITTED", "binary boundary must report emitted payload", findings)
    checks_total += require(binary_boundary.get("ready_for_section_emission_handoff") is True, display_path(manifest_path), "M252-D002-HANDOFF", "binary boundary must be ready for section emission handoff", findings)

    bin_checks, bin_findings, envelope = parse_binary_envelope(binary_path)
    checks_total += bin_checks
    findings.extend(bin_findings)
    if envelope is None:
        return checks_total, findings, None

    chunk_payloads = {chunk["name"]: chunk["payload"] for chunk in envelope["chunks"]}
    checks_total += require(chunk_payloads[CHUNK_NAMES[0]].get("contract_id") == PACKAGING_CONTRACT_ID, display_path(binary_path), "M252-D002-CHUNK-PACKAGING", "packaging chunk contract mismatch", findings)
    checks_total += require(chunk_payloads[CHUNK_NAMES[1]].get("contract_id") == TYPED_HANDOFF_CONTRACT_ID, display_path(binary_path), "M252-D002-CHUNK-TYPED", "typed chunk contract mismatch", findings)
    checks_total += require(chunk_payloads[CHUNK_NAMES[2]].get("contract_id") == DEBUG_PROJECTION_CONTRACT_ID, display_path(binary_path), "M252-D002-CHUNK-DEBUG", "debug chunk contract mismatch", findings)
    checks_total += require(chunk_payloads[CHUNK_NAMES[0]] == packaging, display_path(binary_path), "M252-D002-CHUNK-PACKAGING-EQ", "packaging chunk payload drifted from manifest packet", findings)
    checks_total += require(chunk_payloads[CHUNK_NAMES[1]] == typed, display_path(binary_path), "M252-D002-CHUNK-TYPED-EQ", "typed chunk payload drifted from manifest packet", findings)
    checks_total += require(chunk_payloads[CHUNK_NAMES[2]] == debug, display_path(binary_path), "M252-D002-CHUNK-DEBUG-EQ", "debug chunk payload drifted from manifest packet", findings)
    checks_total += require(binary_boundary.get("payload_bytes") == envelope["payload_bytes"], display_path(binary_path), "M252-D002-PAYLOAD-BYTES", "manifest payload_bytes does not match emitted binary length", findings)

    return checks_total, findings, {
        "fixture": display_path(fixture_path),
        "summary_path": display_path(summary_path),
        "manifest_path": display_path(manifest_path),
        "runtime_metadata_binary_path": display_path(binary_path),
        "payload_bytes": envelope["payload_bytes"],
        "chunk_names": list(CHUNK_NAMES),
        "packaging_contract_replay_key": binary_boundary.get("packaging_contract_replay_key", ""),
        "typed_handoff_replay_key": binary_boundary.get("typed_lowering_handoff_replay_key", ""),
        "debug_projection_replay_key": binary_boundary.get("debug_projection_replay_key", ""),
        "replay_key": binary_boundary.get("replay_key", ""),
    }


def serialize_findings(findings: list[Finding]) -> list[dict[str, str]]:
    return [{"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail} for finding in findings]


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []
    dynamic_cases: list[dict[str, object]] = []

    for path, exists_id, snippets in (
        (args.expectations_doc, "M252-D002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-D002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-D002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-D002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-D002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-D002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-D002-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-D002-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-D002-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.ast_header, "M252-D002-AST-EXISTS", AST_SNIPPETS),
        (args.frontend_types, "M252-D002-TYPES-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts_header, "M252-D002-ARTH-EXISTS", FRONTEND_ARTIFACTS_HEADER_SNIPPETS),
        (args.frontend_artifacts, "M252-D002-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.file_io_header, "M252-D002-IOH-EXISTS", FILE_IO_HEADER_SNIPPETS),
        (args.file_io_cpp, "M252-D002-IOC-EXISTS", FILE_IO_CPP_SNIPPETS),
        (args.manifest_artifacts_header, "M252-D002-MAH-EXISTS", MANIFEST_ARTIFACTS_HEADER_SNIPPETS),
        (args.manifest_artifacts_cpp, "M252-D002-MAC-EXISTS", MANIFEST_ARTIFACTS_CPP_SNIPPETS),
        (args.driver_cpp, "M252-D002-DRV-EXISTS", DRIVER_SNIPPETS),
        (args.frontend_anchor, "M252-D002-FEA-EXISTS", FRONTEND_ANCHOR_SNIPPETS),
        (args.runner_cpp, "M252-D002-RUN-EXISTS", RUNNER_SNIPPETS),
        (args.fixture, "M252-D002-FIX-EXISTS", FIXTURE_SNIPPETS),
        (args.package_json, "M252-D002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        local_checks, local_failures = check_doc_contract(path=path, exists_check_id=exists_id, snippets=snippets)
        checks_total += local_checks
        failures.extend(local_failures)

    if not args.skip_runner_probes:
        probe_root = args.probe_root.resolve()
        case_checks, case_failures, case_payload = run_binary_case(
            runner_exe=args.runner_exe.resolve(),
            fixture_path=args.fixture.resolve(),
            out_dir=probe_root,
        )
        checks_total += case_checks
        failures.extend(case_failures)
        if case_payload is not None:
            dynamic_cases.append(case_payload)

    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "dynamic_probes_executed": not args.skip_runner_probes,
        "dynamic_cases": dynamic_cases,
        "failures": serialize_findings(failures),
    }
    summary_out = (ROOT / args.summary_out) if not args.summary_out.is_absolute() else args.summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        print(f"[m252-d002] FAIL {summary_payload['checks_passed']}/{checks_total} -> {display_path(summary_out)}", file=sys.stderr)
        for failure in failures:
            print(f"  - {failure.check_id} [{failure.artifact}] {failure.detail}", file=sys.stderr)
        return 1

    print(f"[m252-d002] PASS {checks_total}/{checks_total} -> {display_path(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
