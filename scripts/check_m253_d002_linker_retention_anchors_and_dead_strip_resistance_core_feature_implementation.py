#!/usr/bin/env python3
"""Fail-closed checker for M253-D002 linker retention and dead-strip resistance."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m253-d002-linker-retention-and-dead-strip-resistance-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-linker-retention-and-dead-strip-resistance/m253-d002-v1"
BOUNDARY_COMMENT_PREFIX = (
    "; runtime_metadata_linker_retention = "
    "contract=objc3c-runtime-linker-retention-and-dead-strip-resistance/m253-d002-v1"
)
NAMED_METADATA_LINE = '!objc3.objc_runtime_linker_retention = !{!62}'
ANCHOR_MODEL = "public-linker-anchor-rooted-in-discovery-table"
DISCOVERY_MODEL = "public-discovery-root-over-retained-metadata-aggregates"
RESPONSE_SUFFIX = ".runtime-metadata-linker-options.rsp"
DISCOVERY_SUFFIX = ".runtime-metadata-discovery.json"
LINKER_ANCHOR_LOGICAL_SECTION = "objc3.runtime.linker_anchor"
DISCOVERY_ROOT_LOGICAL_SECTION = "objc3.runtime.discovery_root"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / (
    "m253_linker_retention_anchors_and_dead_strip_resistance_core_feature_implementation_d002_expectations.md"
)
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / (
    "m253_d002_linker_retention_anchors_and_dead_strip_resistance_core_feature_implementation_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PROCESS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_MANIFEST_ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
DEFAULT_MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_FRONTEND_ANCHOR = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_POSITIVE_SOURCE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / (
    "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
DEFAULT_NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / (
    "m252_b004_missing_interface_property.objc3"
)
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / (
    "d002-linker-retention-and-dead-strip-resistance"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m253/M253-D002/linker_retention_and_dead_strip_resistance_summary.json"
)
DEFAULT_LLVM_READOBJ = "llvm-readobj"
DEFAULT_LLVM_OBJDUMP = "llvm-objdump"
DEFAULT_CLANG = "clang"
DEFAULT_LLVM_LIB = "llvm-lib"
DEFAULT_LLVM_AR = "llvm-ar"


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
    SnippetCheck("M253-D002-DOC-EXP-01", "# M253 Linker Retention Anchors And Dead-Strip Resistance Core Feature Implementation Expectations (D002)"),
    SnippetCheck("M253-D002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-D002-DOC-EXP-03", "one public linker anchor rooted in one public discovery root"),
    SnippetCheck("M253-D002-DOC-EXP-04", f"`module{RESPONSE_SUFFIX}`"),
    SnippetCheck("M253-D002-DOC-EXP-05", "The positive proof must package `module.obj` into one archive/library"),
    SnippetCheck("M253-D002-DOC-EXP-06", "`tmp/reports/m253/M253-D002/linker_retention_and_dead_strip_resistance_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-PKT-01", "# M253-D002 Linker Retention Anchors And Dead-Strip Resistance Core Feature Implementation Packet"),
    SnippetCheck("M253-D002-PKT-02", "Packet: `M253-D002`"),
    SnippetCheck("M253-D002-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-D002-PKT-04", "discovery root, and one deterministic driver-friendly response artifact"),
    SnippetCheck("M253-D002-PKT-05", "Multi-archive fan-in and cross-translation-unit anchor merging remain deferred"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-ARCH-01", "M253 lane-D D002 linker-retention and dead-strip-resistance anchors explicit"),
    SnippetCheck("M253-D002-ARCH-02", "m253_d002_linker_retention_anchors_and_dead_strip_resistance_core_feature_implementation_packet.md"),
    SnippetCheck("M253-D002-ARCH-03", "io/objc3_process.cpp"),
    SnippetCheck("M253-D002-ARCH-04", "libobjc3c_frontend/frontend_anchor.cpp"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-NDOC-01", "## Linker retention anchors and dead-strip resistance (M253-D002)"),
    SnippetCheck("M253-D002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-D002-NDOC-03", f"`{ANCHOR_MODEL}`"),
    SnippetCheck("M253-D002-NDOC-04", f"`module{DISCOVERY_SUFFIX}`"),
    SnippetCheck("M253-D002-NDOC-05", "`tmp/reports/m253/M253-D002/linker_retention_and_dead_strip_resistance_summary.json`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-SPC-01", "## M253 linker retention anchors and dead-strip resistance (D002)"),
    SnippetCheck("M253-D002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-D002-SPC-03", "`objc3_runtime_metadata_link_anchor_<hash>`"),
    SnippetCheck("M253-D002-SPC-04", f"`module{RESPONSE_SUFFIX}`"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-META-01", "## M253 linker retention and dead-strip resistance metadata anchors (D002)"),
    SnippetCheck("M253-D002-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-D002-META-03", "`objc3_runtime_metadata_discovery_root_<hash>`"),
    SnippetCheck("M253-D002-META-04", "single-library retention"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-LHDR-01", "kObjc3RuntimeLinkerRetentionContractId"),
    SnippetCheck("M253-D002-LHDR-02", "kObjc3RuntimeLinkerResponseArtifactSuffix"),
    SnippetCheck("M253-D002-LHDR-03", "Objc3RuntimeMetadataLinkerRetentionSummary()"),
    SnippetCheck("M253-D002-LHDR-04", "Objc3RuntimeMetadataDriverLinkerRetentionFlagForObjectFormat("),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-LCPP-01", "Objc3RuntimeMetadataLinkerRetentionSummary()"),
    SnippetCheck("M253-D002-LCPP-02", "BuildRuntimeMetadataDriverLinkerRetentionFlagForObjectFormat"),
    SnippetCheck("M253-D002-LCPP-03", "no-multi-archive-fan-in-or-cross-translation-unit-anchor-merging"),
)

PROCESS_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-PHDR-01", "TryBuildObjc3RuntimeMetadataLinkerRetentionArtifacts("),
)

PROCESS_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-PCPP-01", "; runtime_metadata_linker_retention = "),
    SnippetCheck("M253-D002-PCPP-02", "Objc3RuntimeMetadataDriverLinkerRetentionFlagForObjectFormat("),
    SnippetCheck("M253-D002-PCPP-03", "runtime metadata linker retention boundary line not found in IR"),
    SnippetCheck("M253-D002-PCPP-04", "driver_linker_flags"),
)

MANIFEST_ARTIFACTS_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-MHDR-01", "BuildRuntimeMetadataLinkerResponseArtifactPath("),
    SnippetCheck("M253-D002-MHDR-02", "BuildRuntimeMetadataDiscoveryArtifactPath("),
    SnippetCheck("M253-D002-MHDR-03", "WriteRuntimeMetadataDiscoveryArtifact("),
)

MANIFEST_ARTIFACTS_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-MCPP-01", "kObjc3RuntimeLinkerResponseArtifactSuffix"),
    SnippetCheck("M253-D002-MCPP-02", "BuildRuntimeMetadataDiscoveryArtifactPath"),
    SnippetCheck("M253-D002-MCPP-03", "WriteRuntimeMetadataLinkerResponseArtifact("),
)

DRIVER_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-DRV-01", "TryBuildObjc3RuntimeMetadataLinkerRetentionArtifacts("),
    SnippetCheck("M253-D002-DRV-02", "WriteRuntimeMetadataLinkerResponseArtifact("),
    SnippetCheck("M253-D002-DRV-03", "WriteRuntimeMetadataDiscoveryArtifact("),
)

FRONTEND_ANCHOR_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-FEA-01", "TryBuildObjc3RuntimeMetadataLinkerRetentionArtifacts("),
    SnippetCheck("M253-D002-FEA-02", "BuildRuntimeMetadataLinkerResponseArtifactPath("),
    SnippetCheck("M253-D002-FEA-03", "BuildRuntimeMetadataDiscoveryArtifactPath("),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-IR-01", "!objc3.objc_runtime_linker_retention = !{!62}"),
    SnippetCheck("M253-D002-IR-02", "; runtime_metadata_linker_retention = "),
    SnippetCheck("M253-D002-IR-03", "objc3_runtime_metadata_link_anchor_"),
    SnippetCheck("M253-D002-IR-04", "objc3_runtime_metadata_discovery_root_"),
    SnippetCheck("M253-D002-IR-05", "kObjc3RuntimeLinkerDiscoveryArtifactSuffix"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-D002-PKG-01", "\"check:objc3c:m253-d002-linker-retention-anchors-and-dead-strip-resistance-core-feature-implementation\""),
    SnippetCheck("M253-D002-PKG-02", "\"test:tooling:m253-d002-linker-retention-anchors-and-dead-strip-resistance-core-feature-implementation\""),
    SnippetCheck("M253-D002-PKG-03", "\"check:objc3c:m253-d002-lane-d-readiness\": \"python scripts/run_m253_d002_lane_d_readiness.py\""),
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
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--process-header", type=Path, default=DEFAULT_PROCESS_HEADER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--manifest-artifacts-header", type=Path, default=DEFAULT_MANIFEST_ARTIFACTS_HEADER)
    parser.add_argument("--manifest-artifacts-cpp", type=Path, default=DEFAULT_MANIFEST_ARTIFACTS_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--frontend-anchor", type=Path, default=DEFAULT_FRONTEND_ANCHOR)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--positive-source-fixture", type=Path, default=DEFAULT_POSITIVE_SOURCE_FIXTURE)
    parser.add_argument("--negative-fixture", type=Path, default=DEFAULT_NEGATIVE_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--llvm-readobj", default=DEFAULT_LLVM_READOBJ)
    parser.add_argument("--llvm-objdump", default=DEFAULT_LLVM_OBJDUMP)
    parser.add_argument("--clang", default=DEFAULT_CLANG)
    parser.add_argument("--llvm-lib", default=DEFAULT_LLVM_LIB)
    parser.add_argument("--llvm-ar", default=DEFAULT_LLVM_AR)
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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def resolve_tool(*names: str) -> str | None:
    for name in names:
        located = shutil.which(name)
        if located:
            return located
        if not name.lower().endswith(".exe"):
            located = shutil.which(name + ".exe")
            if located:
                return located
    return None


def build_library_fixture_source(source_text: str) -> str:
    marker = "\nfn main"
    if marker not in source_text:
        return source_text.rstrip() + "\n"
    return source_text.split(marker, 1)[0].rstrip() + "\n"


def extract_boundary_line(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(BOUNDARY_COMMENT_PREFIX):
            return line
    return ""


def extract_boundary_token(line: str, token_name: str) -> str:
    prefix = token_name + "="
    for part in line.split(";")[1:]:
        if part.startswith(prefix):
            return part[len(prefix) :]
    return ""


def extract_section_names(readobj_stdout: str) -> list[str]:
    names: list[str] = []
    for line in readobj_stdout.splitlines():
        stripped = line.strip()
        if stripped.startswith("Name: "):
            names.append(stripped[len("Name: ") :].split(" (", 1)[0])
    return names


def collect_symbol_lines(objdump_stdout: str) -> list[str]:
    return [line.strip() for line in objdump_stdout.splitlines() if line.strip()]


def run_positive_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[dict[str, Any], int, int]:
    checks_total = 0
    checks_passed = 0

    llvm_readobj = resolve_tool(args.llvm_readobj)
    llvm_objdump = resolve_tool(args.llvm_objdump)
    clang = resolve_tool(args.clang)
    llvm_lib = resolve_tool(args.llvm_lib)
    llvm_ar = resolve_tool(args.llvm_ar)
    positive_out = args.probe_root.resolve() / "positive"
    positive_out.mkdir(parents=True, exist_ok=True)

    source_fixture_text = read_text(args.positive_source_fixture)
    library_fixture = positive_out / "runtime_metadata_library.objc3"
    library_fixture.write_text(build_library_fixture_source(source_fixture_text), encoding="utf-8")

    compile_cmd = [
        str(args.native_exe.resolve()),
        str(library_fixture),
        "--out-dir",
        str(positive_out),
        "--emit-prefix",
        "module",
    ]
    compile_result = run_command(compile_cmd, ROOT)

    manifest_path = positive_out / "module.manifest.json"
    ir_path = positive_out / "module.ll"
    obj_path = positive_out / "module.obj"
    backend_path = positive_out / "module.object-backend.txt"
    response_path = positive_out / f"module{RESPONSE_SUFFIX}"
    discovery_path = positive_out / f"module{DISCOVERY_SUFFIX}"

    backend_text = backend_path.read_text(encoding="utf-8").strip() if backend_path.exists() else ""
    ir_text = read_text(ir_path) if ir_path.exists() else ""
    boundary_line = extract_boundary_line(ir_text)
    discovery_payload = load_json(discovery_path) if discovery_path.exists() else {}
    response_text = response_path.read_text(encoding="utf-8").strip() if response_path.exists() else ""
    manifest_payload = load_json(manifest_path) if manifest_path.exists() else {}
    diagnostics = manifest_payload.get("diagnostics", []) if isinstance(manifest_payload, dict) else []
    discovery_symbol = str(discovery_payload.get("discovery_root_symbol", ""))
    anchor_symbol = str(discovery_payload.get("linker_anchor_symbol", ""))
    object_format = str(discovery_payload.get("object_format", ""))

    case_summary: dict[str, Any] = {
        "case_id": "M253-D002-CASE-POSITIVE-SINGLE-LIBRARY-RETENTION",
        "fixture": display_path(library_fixture),
        "out_dir": display_path(positive_out),
        "compile_exit_code": compile_result.returncode,
        "backend": backend_text,
        "response_file": display_path(response_path),
        "discovery_file": display_path(discovery_path),
        "object_format": object_format,
        "linker_anchor_symbol": anchor_symbol,
        "discovery_root_symbol": discovery_symbol,
        "plain_link_exit_code": None,
        "retained_link_exit_code": None,
    }

    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M253-D002-POS-NATIVE-EXE", "native executable must exist", failures)
    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(library_fixture), "M253-D002-POS-COMPILE", "metadata-only positive fixture must compile", failures)
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(positive_out), "M253-D002-POS-MANIFEST", "module.manifest.json must exist", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(positive_out), "M253-D002-POS-IR", "module.ll must exist", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(positive_out), "M253-D002-POS-OBJECT", "module.obj must exist", failures)
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), "M253-D002-POS-BACKEND", "backend must be llvm-direct", failures)
    checks_total += 1
    checks_passed += require(response_path.exists(), display_path(positive_out), "M253-D002-POS-RESPONSE", f"module{RESPONSE_SUFFIX} must exist", failures)
    checks_total += 1
    checks_passed += require(discovery_path.exists(), display_path(positive_out), "M253-D002-POS-DISCOVERY", f"module{DISCOVERY_SUFFIX} must exist", failures)
    checks_total += 1
    checks_passed += require(not diagnostics, display_path(manifest_path if manifest_path.exists() else positive_out), "M253-D002-POS-DIAGNOSTICS", "manifest diagnostics must be empty", failures)
    checks_total += 1
    checks_passed += require(NAMED_METADATA_LINE in ir_text, display_path(ir_path if ir_path.exists() else positive_out), "M253-D002-POS-NAMED-METADATA", "IR must publish !objc3.objc_runtime_linker_retention", failures)
    checks_total += 1
    checks_passed += require(bool(boundary_line), display_path(ir_path if ir_path.exists() else positive_out), "M253-D002-POS-BOUNDARY", "IR must publish the linker-retention boundary line", failures)

    expected_boundary_tokens = {
        "anchor_model": ANCHOR_MODEL,
        "discovery_model": DISCOVERY_MODEL,
        "linker_anchor_symbol": anchor_symbol,
        "discovery_root_symbol": discovery_symbol,
        "linker_anchor_logical_section": LINKER_ANCHOR_LOGICAL_SECTION,
        "discovery_root_logical_section": DISCOVERY_ROOT_LOGICAL_SECTION,
        "linker_response_artifact_suffix": RESPONSE_SUFFIX,
        "discovery_artifact_suffix": DISCOVERY_SUFFIX,
    }
    for index, (token_name, expected_value) in enumerate(expected_boundary_tokens.items(), start=1):
        actual_value = extract_boundary_token(boundary_line, token_name) if boundary_line else ""
        checks_total += 1
        checks_passed += require(
            actual_value == expected_value,
            display_path(ir_path if ir_path.exists() else positive_out),
            f"M253-D002-POS-BOUNDARY-TOKEN-{index:02d}",
            f"boundary token {token_name} mismatch: expected {expected_value!r}, got {actual_value!r}",
            failures,
        )

    checks_total += 1
    checks_passed += require(
        discovery_payload.get("contract_id") == CONTRACT_ID,
        display_path(discovery_path if discovery_path.exists() else positive_out),
        "M253-D002-POS-DISCOVERY-CONTRACT",
        "discovery payload contract_id mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        discovery_payload.get("object_artifact") == "module.obj",
        display_path(discovery_path if discovery_path.exists() else positive_out),
        "M253-D002-POS-DISCOVERY-OBJECT",
        "discovery payload object_artifact mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        discovery_payload.get("driver_linker_flags") == [response_text],
        display_path(discovery_path if discovery_path.exists() else positive_out),
        "M253-D002-POS-DISCOVERY-FLAGS",
        "discovery payload driver_linker_flags must match response artifact",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        response_text.startswith("-Wl,"),
        display_path(response_path if response_path.exists() else positive_out),
        "M253-D002-POS-RESPONSE-FORMAT",
        "response artifact must contain one driver-friendly linker flag",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        anchor_symbol.startswith("objc3_runtime_metadata_link_anchor_"),
        display_path(discovery_path if discovery_path.exists() else positive_out),
        "M253-D002-POS-ANCHOR-SYMBOL",
        "linker anchor symbol must use the hashed public symbol scheme",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        discovery_symbol.startswith("objc3_runtime_metadata_discovery_root_"),
        display_path(discovery_path if discovery_path.exists() else positive_out),
        "M253-D002-POS-DISCOVERY-SYMBOL",
        "discovery root symbol must use the hashed public symbol scheme",
        failures,
    )

    if llvm_readobj is None or llvm_objdump is None or clang is None:
        failures.append(
            Finding(
                "dynamic",
                "M253-D002-POS-TOOLS",
                "llvm-readobj, llvm-objdump, and clang are required for the positive linker-retention proof",
            )
        )
        return case_summary, checks_total, checks_passed

    readobj_result = run_command([llvm_readobj, "--sections", str(obj_path)], ROOT)
    objdump_result = run_command([llvm_objdump, "--syms", str(obj_path)], ROOT)
    object_sections = extract_section_names(readobj_result.stdout) if readobj_result.returncode == 0 else []
    object_symbol_lines = collect_symbol_lines(objdump_result.stdout) if objdump_result.returncode == 0 else []
    case_summary["object_section_names"] = object_sections

    checks_total += 1
    checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), "M253-D002-POS-OBJECT-READOBJ", f"llvm-readobj failed: {readobj_result.stderr.strip()}", failures)
    checks_total += 1
    checks_passed += require(objdump_result.returncode == 0, display_path(obj_path), "M253-D002-POS-OBJECT-OBJDUMP", f"llvm-objdump failed: {objdump_result.stderr.strip()}", failures)
    for idx, section_name in enumerate(
        (
            LINKER_ANCHOR_LOGICAL_SECTION,
            DISCOVERY_ROOT_LOGICAL_SECTION,
            "objc3.runtime.class_descriptors",
            "objc3.runtime.protocol_descriptors",
            "objc3.runtime.property_descriptors",
            "objc3.runtime.ivar_descriptors",
        ),
        start=1,
    ):
        checks_total += 1
        checks_passed += require(
            section_name in object_sections,
            display_path(obj_path),
            f"M253-D002-POS-OBJECT-SECTION-{idx:02d}",
            f"object must contain section {section_name}",
            failures,
        )
    for idx, symbol_name in enumerate((anchor_symbol, discovery_symbol, "__objc3_sec_class_descriptors"), start=1):
        checks_total += 1
        checks_passed += require(
            any(symbol_name in line for line in object_symbol_lines),
            display_path(obj_path),
            f"M253-D002-POS-OBJECT-SYMBOL-{idx:02d}",
            f"object must contain symbol {symbol_name}",
            failures,
        )

    object_format = object_format or "coff"
    if object_format == "coff":
        if llvm_lib is None:
            failures.append(Finding("dynamic", "M253-D002-POS-ARCHIVER", "llvm-lib is required for COFF archive packaging"))
            return case_summary, checks_total, checks_passed
        archive_path = positive_out / "module.lib"
        archive_cmd = [llvm_lib, "/nologo", f"/out:{archive_path}", str(obj_path)]
    else:
        if llvm_ar is None:
            failures.append(Finding("dynamic", "M253-D002-POS-ARCHIVER", "llvm-ar is required for non-COFF archive packaging"))
            return case_summary, checks_total, checks_passed
        archive_path = positive_out / "module.a"
        archive_cmd = [llvm_ar, "rcs", str(archive_path), str(obj_path)]
    archive_result = run_command(archive_cmd, ROOT)
    checks_total += 1
    checks_passed += require(archive_result.returncode == 0, display_path(archive_path), "M253-D002-POS-ARCHIVE", f"archive creation failed: {archive_result.stderr.strip()}", failures)

    main_c = positive_out / "main.c"
    main_c.write_text("int main(void){return 0;}\n", encoding="utf-8")
    plain_exe = positive_out / ("plain.exe" if object_format == "coff" else "plain.out")
    retained_exe = positive_out / ("retained.exe" if object_format == "coff" else "retained.out")

    plain_link_cmd = [clang, str(main_c), str(archive_path), "-o", str(plain_exe)]
    retained_link_cmd = [clang, str(main_c), str(archive_path), f"@{response_path}", "-o", str(retained_exe)]
    plain_link_result = run_command(plain_link_cmd, ROOT)
    retained_link_result = run_command(retained_link_cmd, ROOT)
    case_summary["plain_link_exit_code"] = plain_link_result.returncode
    case_summary["retained_link_exit_code"] = retained_link_result.returncode

    checks_total += 1
    checks_passed += require(plain_link_result.returncode == 0, display_path(plain_exe), "M253-D002-POS-PLAIN-LINK", f"plain link failed: {plain_link_result.stderr.strip()}", failures)
    checks_total += 1
    checks_passed += require(retained_link_result.returncode == 0, display_path(retained_exe), "M253-D002-POS-RETAINED-LINK", f"retained link failed: {retained_link_result.stderr.strip()}", failures)

    plain_sections_result = run_command([llvm_readobj, "--sections", str(plain_exe)], ROOT)
    retained_sections_result = run_command([llvm_readobj, "--sections", str(retained_exe)], ROOT)
    plain_sections = extract_section_names(plain_sections_result.stdout) if plain_sections_result.returncode == 0 else []
    retained_sections = extract_section_names(retained_sections_result.stdout) if retained_sections_result.returncode == 0 else []
    case_summary["plain_section_names"] = plain_sections
    case_summary["retained_section_names"] = retained_sections

    checks_total += 1
    checks_passed += require(plain_sections_result.returncode == 0, display_path(plain_exe), "M253-D002-POS-PLAIN-READOBJ", f"plain llvm-readobj failed: {plain_sections_result.stderr.strip()}", failures)
    checks_total += 1
    checks_passed += require(retained_sections_result.returncode == 0, display_path(retained_exe), "M253-D002-POS-RETAINED-READOBJ", f"retained llvm-readobj failed: {retained_sections_result.stderr.strip()}", failures)

    if object_format == "coff":
        metadata_prefix = "objc3.ru"
        plain_metadata_sections = [name for name in plain_sections if name.startswith(metadata_prefix)]
        retained_metadata_sections = [name for name in retained_sections if name.startswith(metadata_prefix)]
    else:
        metadata_prefix = "objc3.runtime"
        plain_metadata_sections = [name for name in plain_sections if metadata_prefix in name]
        retained_metadata_sections = [name for name in retained_sections if metadata_prefix in name]
    case_summary["linked_metadata_section_prefix"] = metadata_prefix
    case_summary["plain_metadata_sections"] = plain_metadata_sections
    case_summary["retained_metadata_sections"] = retained_metadata_sections

    checks_total += 1
    checks_passed += require(
        not plain_metadata_sections,
        display_path(plain_exe),
        "M253-D002-POS-PLAIN-NO-METADATA",
        "plain link must not retain metadata sections",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        bool(retained_metadata_sections),
        display_path(retained_exe),
        "M253-D002-POS-RETAINED-METADATA",
        "retained link must preserve metadata sections after dead stripping",
        failures,
    )

    return case_summary, checks_total, checks_passed


def run_negative_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[dict[str, Any], int, int]:
    checks_total = 0
    checks_passed = 0
    negative_out = args.probe_root.resolve() / "negative"
    negative_out.mkdir(parents=True, exist_ok=True)

    compile_cmd = [
        str(args.native_exe.resolve()),
        str(args.negative_fixture.resolve()),
        "--out-dir",
        str(negative_out),
        "--emit-prefix",
        "module",
    ]
    compile_result = run_command(compile_cmd, ROOT)

    manifest_path = negative_out / "module.manifest.json"
    obj_path = negative_out / "module.obj"
    backend_path = negative_out / "module.object-backend.txt"
    response_path = negative_out / f"module{RESPONSE_SUFFIX}"
    discovery_path = negative_out / f"module{DISCOVERY_SUFFIX}"

    case_summary = {
        "case_id": "M253-D002-CASE-NEGATIVE-FAIL-CLOSED",
        "fixture": display_path(args.negative_fixture),
        "out_dir": display_path(negative_out),
        "compile_exit_code": compile_result.returncode,
        "manifest_exists": manifest_path.exists(),
        "object_exists": obj_path.exists(),
        "backend_exists": backend_path.exists(),
        "response_exists": response_path.exists(),
        "discovery_exists": discovery_path.exists(),
    }

    checks_total += 1
    checks_passed += require(compile_result.returncode != 0, display_path(args.negative_fixture), "M253-D002-NEG-STATUS", "negative compile must fail", failures)
    checks_total += 1
    checks_passed += require(not obj_path.exists(), display_path(negative_out), "M253-D002-NEG-NO-OBJECT", "negative compile must not emit module.obj", failures)
    checks_total += 1
    checks_passed += require(not backend_path.exists(), display_path(negative_out), "M253-D002-NEG-NO-BACKEND", "negative compile must not emit module.object-backend.txt", failures)
    checks_total += 1
    checks_passed += require(not response_path.exists(), display_path(negative_out), "M253-D002-NEG-NO-RESPONSE", f"negative compile must not emit module{RESPONSE_SUFFIX}", failures)
    checks_total += 1
    checks_passed += require(not discovery_path.exists(), display_path(negative_out), "M253-D002-NEG-NO-DISCOVERY", f"negative compile must not emit module{DISCOVERY_SUFFIX}", failures)

    return case_summary, checks_total, checks_passed


def serialize_findings(findings: list[Finding]) -> list[dict[str, str]]:
    return [{"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail} for finding in findings]


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_groups: tuple[tuple[Path, Sequence[SnippetCheck]], ...] = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.process_header, PROCESS_HEADER_SNIPPETS),
        (args.process_cpp, PROCESS_CPP_SNIPPETS),
        (args.manifest_artifacts_header, MANIFEST_ARTIFACTS_HEADER_SNIPPETS),
        (args.manifest_artifacts_cpp, MANIFEST_ARTIFACTS_CPP_SNIPPETS),
        (args.driver_cpp, DRIVER_CPP_SNIPPETS),
        (args.frontend_anchor, FRONTEND_ANCHOR_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_groups:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    if not args.skip_dynamic_probes:
        positive_summary, positive_total, positive_passed = run_positive_probe(args, failures)
        checks_total += positive_total
        checks_passed += positive_passed
        dynamic_cases.append(positive_summary)

        negative_summary, negative_total, negative_passed = run_negative_probe(args, failures)
        checks_total += negative_total
        checks_passed += negative_passed
        dynamic_cases.append(negative_summary)

    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "failures": serialize_findings(failures),
    }
    summary_out = (ROOT / args.summary_out) if not args.summary_out.is_absolute() else args.summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        print(
            f"[m253-d002] FAIL {checks_passed}/{checks_total} -> {display_path(summary_out)}",
            file=sys.stderr,
        )
        for failure in failures:
            print(f"  - {failure.check_id} [{failure.artifact}] {failure.detail}", file=sys.stderr)
        return 1

    print(f"[m253-d002] PASS {checks_total}/{checks_total} -> {display_path(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
