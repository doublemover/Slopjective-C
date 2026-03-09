#!/usr/bin/env python3
"""Fail-closed checker for M263-C003 archive/static-link bootstrap replay corpus."""

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
MODE = "m263-c003-archive-static-link-bootstrap-replay-corpus-v1"
CONTRACT_ID = "objc3c-runtime-bootstrap-archive-static-link-replay-corpus/m263-c003-v1"
SOURCE_DISCOVERY_CONTRACT_ID = "objc3c-runtime-linker-retention-and-dead-strip-resistance/m253-d002-v1"
ARCHIVE_DISCOVERY_CONTRACT_ID = "objc3c-runtime-metadata-archive-and-static-link-discovery/m253-d003-v1"
BOOTSTRAP_FAILURE_RESTART_CONTRACT_ID = "objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1"
REGISTRATION_DESCRIPTOR_LOWERING_CONTRACT_ID = "objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1"
CORPUS_MODEL = "merged-archive-static-link-discovery-artifacts-drive-live-bootstrap-replay-probes"
BINARY_PROOF_MODEL = "plain-link-omits-bootstrap-images-retained-link-replays-them"
MERGED_RESPONSE_SUFFIX = ".merged.runtime-metadata-linker-options.rsp"
MERGED_DISCOVERY_SUFFIX = ".merged.runtime-metadata-discovery.json"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_archive_and_static_link_bootstrap_replay_corpus_conformance_corpus_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_c003_archive_and_static_link_bootstrap_replay_corpus_conformance_corpus_expansion_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
MERGE_SCRIPT = ROOT / "scripts" / "merge_objc3_runtime_metadata_linker_artifacts.py"
AUTO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_c003_archive_bootstrap_replay_auto.objc3"
EXPLICIT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_c003_archive_bootstrap_replay_explicit.objc3"
PROBE_SOURCE = ROOT / "tests" / "tooling" / "runtime" / "m263_c003_archive_static_link_bootstrap_replay_probe.cpp"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m263" / "M263-C003" / "archive_static_link_bootstrap_replay_corpus_summary.json"
PROBE_ROOT = ROOT / "tmp" / "reports" / "m263" / "M263-C003" / "probe"


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
    SnippetCheck("M263-C003-DOC-01", "# M263 Archive And Static-Link Bootstrap Replay Corpus Conformance Corpus Expansion Expectations (C003)"),
    SnippetCheck("M263-C003-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M263-C003-DOC-03", "plain archive link with no retention flags"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M263-C003-PKT-01", "Packet: `M263-C003`"),
    SnippetCheck("M263-C003-PKT-02", "Dependencies: `M263-C002`, `M263-B003`"),
    SnippetCheck("M263-C003-PKT-03", "module.merged.runtime-metadata-discovery.json"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M263-C003-NDOC-01", "## Archive and static-link bootstrap replay corpus (M263-C003)"),
    SnippetCheck("M263-C003-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-C003-NDOC-03", "`module.merged.runtime-metadata-linker-options.rsp`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M263-C003-SPC-01", "## M263 archive/static-link bootstrap replay corpus (C003)"),
    SnippetCheck("M263-C003-SPC-02", f"`{CORPUS_MODEL}`"),
    SnippetCheck("M263-C003-SPC-03", f"`{BINARY_PROOF_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M263-C003-META-01", "## M263 archive/static-link bootstrap replay corpus anchors (C003)"),
    SnippetCheck("M263-C003-META-02", "`module.merged.runtime-metadata-discovery.json`"),
    SnippetCheck("M263-C003-META-03", "`objc3_runtime_replay_registered_images_for_testing`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M263-C003-ARCH-01", "## M263 archive/static-link bootstrap replay corpus (C003)"),
    SnippetCheck("M263-C003-ARCH-02", "plain archive links omit bootstrap images"),
    SnippetCheck("M263-C003-ARCH-03", "retained merged-archive links register and replay multiple images"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M263-C003-LHDR-01", "kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId"),
    SnippetCheck("M263-C003-LHDR-02", CONTRACT_ID),
    SnippetCheck("M263-C003-LHDR-03", "kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M263-C003-LCPP-01", "Objc3RuntimeBootstrapArchiveStaticLinkReplayCorpusSummary()"),
    SnippetCheck("M263-C003-LCPP-02", "M263-C003 archive/static-link bootstrap replay corpus anchor"),
    SnippetCheck("M263-C003-LCPP-03", "kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel"),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M263-C003-IR-01", "runtime_bootstrap_archive_static_link_replay_corpus = "),
    SnippetCheck("M263-C003-IR-02", "M263-C003 archive/static-link bootstrap replay corpus anchor"),
    SnippetCheck("M263-C003-IR-03", "reset_replay_state_snapshot_symbol="),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M263-C003-ART-01", "objc_runtime_bootstrap_archive_static_link_replay_corpus"),
    SnippetCheck("M263-C003-ART-02", "archive_static_link_discovery_contract_id"),
    SnippetCheck("M263-C003-ART-03", "replay_registered_images_symbol"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M263-C003-PKG-01", '"check:objc3c:m263-c003-archive-static-link-bootstrap-replay-corpus"'),
    SnippetCheck("M263-C003-PKG-02", '"test:tooling:m263-c003-archive-static-link-bootstrap-replay-corpus"'),
    SnippetCheck("M263-C003-PKG-03", '"check:objc3c:m263-c003-lane-c-readiness"'),
)
PROBE_SNIPPETS = (
    SnippetCheck("M263-C003-PRB-01", "objc3_runtime_replay_registered_images_for_testing"),
    SnippetCheck("M263-C003-PRB-02", "objc3_runtime_copy_reset_replay_state_for_testing"),
    SnippetCheck("M263-C003-PRB-03", "startup_registered_image_count"),
)
FIXTURE_AUTO_SNIPPETS = (
    SnippetCheck("M263-C003-FIX-A-01", "module ArchiveReplayAuto;"),
    SnippetCheck("M263-C003-FIX-A-02", "#pragma objc_language_version(3)"),
)
FIXTURE_EXPLICIT_SNIPPETS = (
    SnippetCheck("M263-C003-FIX-B-01", "module ArchiveReplayExplicit;"),
    SnippetCheck("M263-C003-FIX-B-02", "#pragma objc_registration_descriptor(ArchiveReplayExplicitDescriptor)"),
    SnippetCheck("M263-C003-FIX-B-03", "#pragma objc_image_root(ArchiveReplayExplicitImageRoot)"),
)


def canonical_json(value: Any) -> str:
    return json.dumps(value, indent=2, sort_keys=True) + "\n"


def display_path(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return path.resolve().as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> Any:
    return json.loads(read_text(path))


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str], cwd: Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=str(cwd), capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def resolve_tool(name: str) -> str | None:
    direct = shutil.which(name)
    if direct:
        return direct
    if not name.endswith(".exe"):
        return shutil.which(name + ".exe")
    return None


def build_library_fixture_source(source_text: str) -> str:
    marker = "\nfn main"
    if marker not in source_text:
        return source_text.rstrip() + "\n"
    return source_text.split(marker, 1)[0].rstrip() + "\n"


def parse_section_records(readobj_stdout: str) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    current: dict[str, Any] | None = None
    for raw_line in readobj_stdout.splitlines():
        line = raw_line.strip()
        if line == "Section {":
            current = {}
            continue
        if line == "}" and current is not None:
            if "Name" in current:
                records.append(current)
            current = None
            continue
        if current is None or ": " not in line:
            continue
        key, value = line.split(": ", 1)
        if key == "Name":
            current[key] = value.split(" (", 1)[0]
        elif key == "RawDataSize":
            try:
                current[key] = int(value, 0)
            except ValueError:
                current[key] = 0
    return records


def metadata_sections(section_records: list[dict[str, Any]], object_format: str) -> list[dict[str, Any]]:
    if object_format == "coff":
        return [record for record in section_records if str(record.get("Name", "")).startswith("objc3.ru")]
    return [record for record in section_records if "objc3.runtime" in str(record.get("Name", ""))]


def total_raw_size(section_records: list[dict[str, Any]]) -> int:
    return sum(int(record.get("RawDataSize", 0)) for record in section_records)


def compile_source(
    args: argparse.Namespace,
    source: Path,
    out_dir: Path,
    registration_order_ordinal: int,
) -> subprocess.CompletedProcess[str]:
    command = [
        str(args.native_exe.resolve()),
        str(source.resolve()),
        "--out-dir",
        str(out_dir.resolve()),
        "--emit-prefix",
        "module",
        "--objc3-bootstrap-registration-order-ordinal",
        str(registration_order_ordinal),
    ]
    llc = resolve_tool("llc")
    if llc is not None:
        command.extend(["--llc", llc])
    return run_command(command)


def archive_command(archive_tool: str, object_format: str, archive_path: Path, object_path: Path) -> list[str]:
    if object_format == "coff":
        return [archive_tool, "/nologo", f"/out:{archive_path}", str(object_path)]
    return [archive_tool, "rcs", str(archive_path), str(object_path)]


def run_dynamic_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    summary: dict[str, Any] = {"case_id": "M263-C003-CORPUS"}

    clangxx = resolve_tool(args.clangxx) or args.clangxx
    llvm_readobj = resolve_tool(args.llvm_readobj) or args.llvm_readobj
    archive_tool = resolve_tool(args.llvm_lib if sys.platform.startswith("win") else args.llvm_ar)
    if archive_tool is None:
        failures.append(Finding("toolchain", "M263-C003-TOOL-ARCHIVE", "required archive tool is unavailable"))
        return 1, 0, summary

    out_root = args.probe_root.resolve() / "corpus"
    a_dir = out_root / "a"
    b_dir = out_root / "b"
    merged_dir = out_root / "merged"
    plain_dir = out_root / "plain"
    single_dir = out_root / "single"
    for path in (a_dir, b_dir, merged_dir, plain_dir, single_dir):
        path.mkdir(parents=True, exist_ok=True)

    src_a = a_dir / "lib.objc3"
    src_b = b_dir / "lib.objc3"
    src_a.write_text(build_library_fixture_source(read_text(args.auto_fixture)), encoding="utf-8")
    src_b.write_text(build_library_fixture_source(read_text(args.explicit_fixture)), encoding="utf-8")

    compile_a = compile_source(args, src_a, a_dir, registration_order_ordinal=1)
    compile_b = compile_source(args, src_b, b_dir, registration_order_ordinal=2)
    checks_total += 2
    checks_passed += require(compile_a.returncode == 0, display_path(src_a), "M263-C003-COMPILE-A", f"compile exited with {compile_a.returncode}", failures)
    checks_passed += require(compile_b.returncode == 0, display_path(src_b), "M263-C003-COMPILE-B", f"compile exited with {compile_b.returncode}", failures)

    discovery_a_path = a_dir / "module.runtime-metadata-discovery.json"
    discovery_b_path = b_dir / "module.runtime-metadata-discovery.json"
    response_a_path = a_dir / "module.runtime-metadata-linker-options.rsp"
    backend_a_path = a_dir / "module.object-backend.txt"
    backend_b_path = b_dir / "module.object-backend.txt"
    checks_total += 4
    checks_passed += require(discovery_a_path.exists(), display_path(discovery_a_path), "M263-C003-DISCOVERY-A", "discovery artifact missing", failures)
    checks_passed += require(discovery_b_path.exists(), display_path(discovery_b_path), "M263-C003-DISCOVERY-B", "discovery artifact missing", failures)
    checks_passed += require(response_a_path.exists(), display_path(response_a_path), "M263-C003-RSP-A", "single-archive response artifact missing", failures)
    checks_passed += require(backend_a_path.exists() and backend_b_path.exists(), display_path(out_root), "M263-C003-BACKEND-TXT", "object backend markers missing", failures)
    if failures:
        return checks_total, checks_passed, summary

    discovery_a = load_json(discovery_a_path)
    discovery_b = load_json(discovery_b_path)
    object_format = str(discovery_a.get("object_format", ""))
    key_a = str(discovery_a.get("translation_unit_identity_key", ""))
    key_b = str(discovery_b.get("translation_unit_identity_key", ""))
    checks_total += 5
    checks_passed += require(discovery_a.get("contract_id") == SOURCE_DISCOVERY_CONTRACT_ID, display_path(discovery_a_path), "M263-C003-DISCOVERY-A-CONTRACT", "discovery A contract mismatch", failures)
    checks_passed += require(discovery_b.get("contract_id") == SOURCE_DISCOVERY_CONTRACT_ID, display_path(discovery_b_path), "M263-C003-DISCOVERY-B-CONTRACT", "discovery B contract mismatch", failures)
    checks_passed += require(object_format == str(discovery_b.get("object_format", "")), display_path(out_root), "M263-C003-OBJECT-FORMAT", "object format mismatch across corpus inputs", failures)
    checks_passed += require(key_a and key_b and key_a != key_b, display_path(out_root), "M263-C003-TU-KEYS", "corpus inputs must publish distinct translation_unit_identity_key values", failures)
    checks_passed += require(backend_a_path.read_text(encoding="utf-8").strip() == "llvm-direct" and backend_b_path.read_text(encoding="utf-8").strip() == "llvm-direct", display_path(out_root), "M263-C003-BACKEND", "corpus inputs must remain llvm-direct", failures)
    if failures:
        return checks_total, checks_passed, summary

    archive_a = a_dir / ("bootstrap_a.lib" if object_format == "coff" else "bootstrap_a.a")
    archive_b = b_dir / ("bootstrap_b.lib" if object_format == "coff" else "bootstrap_b.a")
    archive_a_result = run_command(archive_command(archive_tool, object_format, archive_a, a_dir / "module.obj"))
    archive_b_result = run_command(archive_command(archive_tool, object_format, archive_b, b_dir / "module.obj"))
    checks_total += 2
    checks_passed += require(archive_a_result.returncode == 0, display_path(archive_a), "M263-C003-ARCHIVE-A", f"archive A creation failed: {archive_a_result.stderr.strip()}", failures)
    checks_passed += require(archive_b_result.returncode == 0, display_path(archive_b), "M263-C003-ARCHIVE-B", f"archive B creation failed: {archive_b_result.stderr.strip()}", failures)

    merge_result = run_command([sys.executable, str(args.merge_script.resolve()), str(discovery_a_path.resolve()), str(discovery_b_path.resolve()), "--out-dir", str(merged_dir.resolve()), "--emit-prefix", "module"])
    merged_response = merged_dir / f"module{MERGED_RESPONSE_SUFFIX}"
    merged_discovery = merged_dir / f"module{MERGED_DISCOVERY_SUFFIX}"
    checks_total += 3
    checks_passed += require(merge_result.returncode == 0, display_path(merged_dir), "M263-C003-MERGE", f"merge failed: {merge_result.stderr.strip()}", failures)
    checks_passed += require(merged_response.exists(), display_path(merged_response), "M263-C003-MERGED-RSP", "merged response artifact missing", failures)
    checks_passed += require(merged_discovery.exists(), display_path(merged_discovery), "M263-C003-MERGED-DISCOVERY", "merged discovery artifact missing", failures)
    if failures:
        return checks_total, checks_passed, summary

    merged_payload = load_json(merged_discovery)
    merged_keys = [str(item) for item in merged_payload.get("translation_unit_identity_keys", [])]
    checks_total += 4
    checks_passed += require(merged_payload.get("contract_id") == ARCHIVE_DISCOVERY_CONTRACT_ID, display_path(merged_discovery), "M263-C003-MERGED-CONTRACT", "merged discovery contract mismatch", failures)
    checks_passed += require(merged_payload.get("merge_model") == "deduplicated-driver-flag-fan-in", display_path(merged_discovery), "M263-C003-MERGED-MODEL", "merged discovery merge model mismatch", failures)
    checks_passed += require(len(merged_payload.get("driver_linker_flags", [])) == 2, display_path(merged_discovery), "M263-C003-MERGED-FLAGS", "merged discovery must retain two driver linker flags", failures)
    checks_passed += require(merged_keys == [key_a, key_b], display_path(merged_discovery), "M263-C003-MERGED-KEYS", "merged discovery must preserve deterministic translation_unit_identity_key ordering", failures)
    if failures:
        return checks_total, checks_passed, summary

    probe_source = args.probe_source.resolve()
    runtime_include_root = args.runtime_include_root.resolve()
    runtime_library = args.runtime_library.resolve()
    plain_exe = plain_dir / ("plain.exe" if object_format == "coff" else "plain.out")
    single_exe = single_dir / ("single.exe" if object_format == "coff" else "single.out")
    merged_exe = merged_dir / ("merged.exe" if object_format == "coff" else "merged.out")

    plain_link = run_command([clangxx, "-std=c++20", "-Wall", "-Wextra", "-pedantic", f"-I{runtime_include_root}", str(probe_source), str(archive_a), str(archive_b), str(runtime_library), "-o", str(plain_exe)])
    single_link = run_command([clangxx, "-std=c++20", "-Wall", "-Wextra", "-pedantic", f"-I{runtime_include_root}", str(probe_source), str(archive_a), str(runtime_library), f"@{response_a_path}", "-o", str(single_exe)])
    merged_link = run_command([clangxx, "-std=c++20", "-Wall", "-Wextra", "-pedantic", f"-I{runtime_include_root}", str(probe_source), str(archive_a), str(archive_b), str(runtime_library), f"@{merged_response}", "-o", str(merged_exe)])
    checks_total += 3
    checks_passed += require(plain_link.returncode == 0, display_path(plain_exe), "M263-C003-LINK-PLAIN", f"plain link failed: {plain_link.stderr.strip()}", failures)
    checks_passed += require(single_link.returncode == 0, display_path(single_exe), "M263-C003-LINK-SINGLE", f"single retained link failed: {single_link.stderr.strip()}", failures)
    checks_passed += require(merged_link.returncode == 0, display_path(merged_exe), "M263-C003-LINK-MERGED", f"merged retained link failed: {merged_link.stderr.strip()}", failures)
    if failures:
        return checks_total, checks_passed, summary

    plain_sections_result = run_command([llvm_readobj, "--sections", str(plain_exe)])
    single_sections_result = run_command([llvm_readobj, "--sections", str(single_exe)])
    merged_sections_result = run_command([llvm_readobj, "--sections", str(merged_exe)])
    plain_sections = metadata_sections(parse_section_records(plain_sections_result.stdout), object_format)
    single_sections = metadata_sections(parse_section_records(single_sections_result.stdout), object_format)
    merged_sections = metadata_sections(parse_section_records(merged_sections_result.stdout), object_format)
    single_size = total_raw_size(single_sections)
    merged_size = total_raw_size(merged_sections)
    checks_total += 6
    checks_passed += require(plain_sections_result.returncode == 0, display_path(plain_exe), "M263-C003-READOBJ-PLAIN", "plain executable must be inspectable", failures)
    checks_passed += require(single_sections_result.returncode == 0, display_path(single_exe), "M263-C003-READOBJ-SINGLE", "single executable must be inspectable", failures)
    checks_passed += require(merged_sections_result.returncode == 0, display_path(merged_exe), "M263-C003-READOBJ-MERGED", "merged executable must be inspectable", failures)
    checks_passed += require(not plain_sections, display_path(plain_exe), "M263-C003-SECTIONS-PLAIN", "plain executable must not retain objc3 runtime sections", failures)
    checks_passed += require(bool(single_sections), display_path(single_exe), "M263-C003-SECTIONS-SINGLE", "single retained executable must keep objc3 runtime sections", failures)
    checks_passed += require(bool(merged_sections), display_path(merged_exe), "M263-C003-SECTIONS-MERGED", "merged retained executable must keep objc3 runtime sections", failures)
    if failures:
        return checks_total, checks_passed, summary

    plain_run = run_command([str(plain_exe)])
    single_run = run_command([str(single_exe)])
    merged_run = run_command([str(merged_exe)])
    checks_total += 3
    checks_passed += require(plain_run.returncode == 0, display_path(plain_exe), "M263-C003-RUN-PLAIN", f"plain probe exited with {plain_run.returncode}", failures)
    checks_passed += require(single_run.returncode == 0, display_path(single_exe), "M263-C003-RUN-SINGLE", f"single probe exited with {single_run.returncode}", failures)
    checks_passed += require(merged_run.returncode == 0, display_path(merged_exe), "M263-C003-RUN-MERGED", f"merged probe exited with {merged_run.returncode}", failures)
    if failures:
        return checks_total, checks_passed, summary

    plain_payload = json.loads(plain_run.stdout)
    single_payload = json.loads(single_run.stdout)
    merged_payload_runtime = json.loads(merged_run.stdout)

    checks_total += 17
    checks_passed += require(plain_payload.get("startup_registered_image_count") == 0, display_path(plain_exe), "M263-C003-PLAIN-COUNT", "plain link must start with zero registered images", failures)
    checks_passed += require(plain_payload.get("startup_walked_image_count") == 0, display_path(plain_exe), "M263-C003-PLAIN-WALK", "plain link must walk zero images", failures)
    checks_passed += require(single_payload.get("startup_registration_copy_status") == 0 and single_payload.get("startup_image_walk_copy_status") == 0 and single_payload.get("startup_reset_replay_copy_status") == 0, display_path(single_exe), "M263-C003-SINGLE-STARTUP-COPIES", "single retained probe snapshot copies must succeed", failures)
    checks_passed += require(single_payload.get("startup_registered_image_count") == 1, display_path(single_exe), "M263-C003-SINGLE-STARTUP-COUNT", "single retained link must register one image at startup", failures)
    checks_passed += require(single_payload.get("startup_walked_image_count") == 1, display_path(single_exe), "M263-C003-SINGLE-STARTUP-WALK", "single retained link must walk one image at startup", failures)
    checks_passed += require(single_payload.get("startup_next_expected_registration_order_ordinal") == 2, display_path(single_exe), "M263-C003-SINGLE-ORDINAL", "single retained link must advance the next ordinal to 2", failures)
    checks_passed += require(single_payload.get("post_reset_registered_image_count") == 0, display_path(single_exe), "M263-C003-SINGLE-POST-RESET-COUNT", "single retained link must clear live images on reset", failures)
    checks_passed += require(single_payload.get("post_reset_retained_bootstrap_image_count") == 1, display_path(single_exe), "M263-C003-SINGLE-POST-RESET-RETAINED", "single retained link must retain one bootstrap image across reset", failures)
    checks_passed += require(single_payload.get("replay_status") == 0, display_path(single_exe), "M263-C003-SINGLE-REPLAY-STATUS", "single retained replay must succeed", failures)
    checks_passed += require(single_payload.get("post_replay_registered_image_count") == 1 and single_payload.get("post_replay_walked_image_count") == 1, display_path(single_exe), "M263-C003-SINGLE-POST-REPLAY-COUNT", "single retained replay must restore one image", failures)
    checks_passed += require(single_payload.get("post_replay_last_replayed_image_count") == 1, display_path(single_exe), "M263-C003-SINGLE-POST-REPLAY-LAST-COUNT", "single retained replay must report one replayed image", failures)
    checks_passed += require(single_payload.get("post_replay_last_replayed_translation_unit_identity_key") == key_a, display_path(single_exe), "M263-C003-SINGLE-POST-REPLAY-TU", "single retained replay must preserve the retained translation unit identity", failures)
    checks_passed += require(merged_payload_runtime.get("startup_registration_copy_status") == 0 and merged_payload_runtime.get("startup_image_walk_copy_status") == 0 and merged_payload_runtime.get("startup_reset_replay_copy_status") == 0, display_path(merged_exe), "M263-C003-MERGED-STARTUP-COPIES", "merged retained probe snapshot copies must succeed", failures)
    checks_passed += require(merged_payload_runtime.get("startup_registered_image_count") == 2, display_path(merged_exe), "M263-C003-MERGED-STARTUP-COUNT", "merged retained link must register two images at startup", failures)
    checks_passed += require(merged_payload_runtime.get("startup_walked_image_count") == 2, display_path(merged_exe), "M263-C003-MERGED-STARTUP-WALK", "merged retained link must walk two images at startup", failures)
    checks_passed += require(merged_payload_runtime.get("startup_next_expected_registration_order_ordinal") == 3, display_path(merged_exe), "M263-C003-MERGED-ORDINAL", "merged retained link must advance the next ordinal to 3", failures)
    checks_passed += require(merged_payload_runtime.get("post_reset_retained_bootstrap_image_count") == 2, display_path(merged_exe), "M263-C003-MERGED-POST-RESET-RETAINED", "merged retained link must retain two bootstrap images across reset", failures)
    checks_passed += require(merged_payload_runtime.get("replay_status") == 0, display_path(merged_exe), "M263-C003-MERGED-REPLAY-STATUS", "merged retained replay must succeed", failures)
    checks_total += 6
    checks_passed += require(merged_payload_runtime.get("post_replay_registered_image_count") == 2 and merged_payload_runtime.get("post_replay_walked_image_count") == 2, display_path(merged_exe), "M263-C003-MERGED-POST-REPLAY-COUNT", "merged retained replay must restore two images", failures)
    checks_passed += require(merged_payload_runtime.get("post_replay_next_expected_registration_order_ordinal") == 3, display_path(merged_exe), "M263-C003-MERGED-POST-REPLAY-ORDINAL", "merged retained replay must preserve the next ordinal", failures)
    checks_passed += require(merged_payload_runtime.get("post_replay_last_replayed_image_count") == 2, display_path(merged_exe), "M263-C003-MERGED-POST-REPLAY-LAST-COUNT", "merged retained replay must report two replayed images", failures)
    checks_passed += require(merged_payload_runtime.get("post_replay_last_replayed_translation_unit_identity_key") == key_b, display_path(merged_exe), "M263-C003-MERGED-POST-REPLAY-TU", "merged retained replay must preserve deterministic replay order", failures)
    checks_passed += require(merged_payload_runtime.get("post_replay_last_walked_translation_unit_identity_key") == key_b, display_path(merged_exe), "M263-C003-MERGED-POST-REPLAY-WALK-TU", "merged retained replay must walk the final retained image deterministically", failures)
    checks_passed += require(merged_payload_runtime.get("post_replay_last_registered_translation_unit_identity_key") == key_b, display_path(merged_exe), "M263-C003-MERGED-POST-REPLAY-REG-TU", "merged retained replay must finish on the final retained translation unit identity", failures)

    summary.update(
        {
            "object_format": object_format,
            "translation_unit_identity_keys": [key_a, key_b],
            "plain_sections": plain_sections,
            "single_sections": single_sections,
            "merged_sections": merged_sections,
            "single_retained_metadata_raw_size": single_size,
            "merged_retained_metadata_raw_size": merged_size,
            "plain_probe": plain_payload,
            "single_probe": single_payload,
            "merged_probe": merged_payload_runtime,
            "merged_driver_linker_flags": merged_payload.get("driver_linker_flags", []),
            "commands": {
                "merge": [sys.executable, str(args.merge_script.resolve()), str(discovery_a_path.resolve()), str(discovery_b_path.resolve()), "--out-dir", str(merged_dir.resolve()), "--emit-prefix", "module"],
                "plain_link": [clangxx, "-std=c++20", "-Wall", "-Wextra", "-pedantic", f"-I{runtime_include_root}", str(probe_source), str(archive_a), str(archive_b), str(runtime_library), "-o", str(plain_exe)],
                "single_link": [clangxx, "-std=c++20", "-Wall", "-Wextra", "-pedantic", f"-I{runtime_include_root}", str(probe_source), str(archive_a), str(runtime_library), f"@{response_a_path}", "-o", str(single_exe)],
                "merged_link": [clangxx, "-std=c++20", "-Wall", "-Wextra", "-pedantic", f"-I{runtime_include_root}", str(probe_source), str(archive_a), str(archive_b), str(runtime_library), f"@{merged_response}", "-o", str(merged_exe)],
            },
        }
    )
    return checks_total, checks_passed, summary


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=ARCHITECTURE_DOC)
    parser.add_argument("--lowering-header", type=Path, default=LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=LOWERING_CPP)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=IR_EMITTER_CPP)
    parser.add_argument("--frontend-artifacts-cpp", type=Path, default=FRONTEND_ARTIFACTS_CPP)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    parser.add_argument("--probe-source", type=Path, default=PROBE_SOURCE)
    parser.add_argument("--auto-fixture", type=Path, default=AUTO_FIXTURE)
    parser.add_argument("--explicit-fixture", type=Path, default=EXPLICIT_FIXTURE)
    parser.add_argument("--merge-script", type=Path, default=MERGE_SCRIPT)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--runtime-include-root", type=Path, default=RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--runtime-library", type=Path, default=RUNTIME_LIB)
    parser.add_argument("--clangxx", default=resolve_tool("clang++") or resolve_tool("clang++.exe") or "clang++")
    parser.add_argument("--llvm-readobj", default=resolve_tool("llvm-readobj") or "llvm-readobj")
    parser.add_argument("--llvm-lib", default=resolve_tool("llvm-lib") or "llvm-lib")
    parser.add_argument("--llvm-ar", default=resolve_tool("llvm-ar") or "llvm-ar")
    parser.add_argument("--probe-root", type=Path, default=PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_groups: tuple[tuple[Path, Sequence[SnippetCheck]], ...] = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.ir_emitter_cpp, IR_EMITTER_SNIPPETS),
        (args.frontend_artifacts_cpp, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
        (args.probe_source, PROBE_SNIPPETS),
        (args.auto_fixture, FIXTURE_AUTO_SNIPPETS),
        (args.explicit_fixture, FIXTURE_EXPLICIT_SNIPPETS),
    )
    for path, snippets in static_groups:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_summary: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dynamic_total, dynamic_passed, dynamic_summary = run_dynamic_probe(args, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "source_discovery_contract_id": SOURCE_DISCOVERY_CONTRACT_ID,
        "archive_static_link_discovery_contract_id": ARCHIVE_DISCOVERY_CONTRACT_ID,
        "bootstrap_failure_restart_contract_id": BOOTSTRAP_FAILURE_RESTART_CONTRACT_ID,
        "registration_descriptor_lowering_contract_id": REGISTRATION_DESCRIPTOR_LOWERING_CONTRACT_ID,
        "corpus_model": CORPUS_MODEL,
        "binary_proof_model": BINARY_PROOF_MODEL,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "ok": not failures,
        "dynamic_summary": dynamic_summary,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.check_id} @ {finding.artifact}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M263-C003 contract validated; summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
