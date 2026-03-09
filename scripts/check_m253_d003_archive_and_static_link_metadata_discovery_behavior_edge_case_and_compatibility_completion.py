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
MODE = "m253-d003"
CONTRACT_ID = "objc3c-runtime-metadata-archive-and-static-link-discovery/m253-d003-v1"
SOURCE_CONTRACT_ID = "objc3c-runtime-linker-retention-and-dead-strip-resistance/m253-d002-v1"
TRANSLATION_UNIT_IDENTITY_MODEL = "input-path-plus-parse-and-lowering-replay"
MERGE_MODEL = "deduplicated-driver-flag-fan-in"
BOUNDARY_COMMENT_PREFIX = "; runtime_metadata_archive_static_link_discovery = "
NAMED_METADATA_LINE = '!objc3.objc_runtime_archive_static_link_discovery = !{!63}'
MERGED_RESPONSE_SUFFIX = ".merged.runtime-metadata-linker-options.rsp"
MERGED_DISCOVERY_SUFFIX = ".merged.runtime-metadata-discovery.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m253" / "M253-D003" / "archive_and_static_link_metadata_discovery_behavior_summary.json"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_archive_and_static_link_metadata_discovery_behavior_edge_case_and_compatibility_completion_d003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_d003_archive_and_static_link_metadata_discovery_behavior_edge_case_and_compatibility_completion_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_MERGE_SCRIPT = ROOT / "scripts" / "merge_objc3_runtime_metadata_linker_artifacts.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_POSITIVE_SOURCE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "reports" / "m253" / "M253-D003" / "probe"


@dataclass
class Finding:
    artifact: str
    check_id: str
    detail: str


@dataclass
class SnippetCheck:
    check_id: str
    snippet: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M253-D003-DOC-01", "module.merged.runtime-metadata-linker-options.rsp"),
    SnippetCheck("M253-D003-DOC-02", "translation_unit_identity_key"),
    SnippetCheck("M253-D003-DOC-03", "duplicate public `objc3c_entry`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M253-D003-PKT-01", "cross-translation-unit public-anchor collisions"),
    SnippetCheck("M253-D003-PKT-02", "multi-archive response/discovery fan-in"),
    SnippetCheck("M253-D003-PKT-03", "duplicate `objc3c_entry` collisions"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M253-D003-ARC-01", "M253 lane-D D003 archive/static-link discovery behavior"),
    SnippetCheck("M253-D003-ARC-02", "scripts/merge_objc3_runtime_metadata_linker_artifacts.py"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M253-D003-NDOC-01", "Archive and static-link metadata discovery behavior (M253-D003)"),
    SnippetCheck("M253-D003-NDOC-02", "module.merged.runtime-metadata-discovery.json"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M253-D003-SPC-01", "## M253 archive/static-link metadata discovery behavior (D003)"),
    SnippetCheck("M253-D003-SPC-02", "deduplicated-driver-flag-fan-in"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M253-D003-MSPC-01", "translation-unit identity model"),
    SnippetCheck("M253-D003-MSPC-02", "module.merged.runtime-metadata-linker-options.rsp"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M253-D003-LHDR-01", "kObjc3RuntimeArchiveStaticLinkDiscoveryContractId"),
    SnippetCheck("M253-D003-LHDR-02", "kObjc3RuntimeMergedLinkerResponseArtifactSuffix"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M253-D003-LCPP-01", "Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary"),
    SnippetCheck("M253-D003-LCPP-02", "translation-unit-stable public anchors"),
)
IR_EMITTER_HEADER_SNIPPETS = (
    SnippetCheck("M253-D003-IHDR-01", "runtime_metadata_archive_static_link_discovery_contract_id"),
    SnippetCheck("M253-D003-IHDR-02", "runtime_metadata_archive_static_link_translation_unit_identity_key"),
)
IR_EMITTER_CPP_SNIPPETS = (
    SnippetCheck("M253-D003-ICPP-01", "!objc3.objc_runtime_archive_static_link_discovery = !{!63}"),
    SnippetCheck("M253-D003-ICPP-02", "translation_unit_identity_key="),
    SnippetCheck("M253-D003-ICPP-03", "define internal i32 @objc3c_entry()"),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck("M253-D003-PCPP-01", '\\"translation_unit_identity_model\\"'),
    SnippetCheck("M253-D003-PCPP-02", '\\"translation_unit_identity_key\\"'),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M253-D003-FE-01", "runtime_metadata_archive_static_link_translation_unit_identity_key"),
    SnippetCheck("M253-D003-FE-02", "input_path.generic_string() + \"|\""),
)
MERGE_SCRIPT_SNIPPETS = (
    SnippetCheck("M253-D003-MRG-01", "MERGED_CONTRACT_ID"),
    SnippetCheck("M253-D003-MRG-02", "linker-anchor collision detected"),
    SnippetCheck("M253-D003-MRG-03", "merged.runtime-metadata-linker-options.rsp"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M253-D003-PKG-01", "tool:objc3c:merge-runtime-metadata-linker-artifacts"),
    SnippetCheck("M253-D003-PKG-02", "check:objc3c:m253-d003-lane-d-readiness"),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter-header", type=Path, default=DEFAULT_IR_EMITTER_HEADER)
    parser.add_argument("--ir-emitter-cpp", type=Path, default=DEFAULT_IR_EMITTER_CPP)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--frontend-artifacts-cpp", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS_CPP)
    parser.add_argument("--merge-script", type=Path, default=DEFAULT_MERGE_SCRIPT)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--positive-source-fixture", type=Path, default=DEFAULT_POSITIVE_SOURCE_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--clang", default=shutil.which("clang") or shutil.which("clang.exe") or "clang")
    parser.add_argument("--llvm-readobj", default=shutil.which("llvm-readobj") or shutil.which("llvm-readobj.exe") or "llvm-readobj")
    parser.add_argument("--llvm-lib", default=shutil.which("llvm-lib") or shutil.which("llvm-lib.exe") or "llvm-lib")
    parser.add_argument("--llvm-ar", default=shutil.which("llvm-ar") or shutil.which("llvm-ar.exe") or "llvm-ar")
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)



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



def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if not condition:
        failures.append(Finding(artifact, check_id, detail))
        return 0
    return 1



def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        passed += require(snippet.snippet in text, display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}", failures)
    return passed



def run_command(cmd: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(cmd), cwd=cwd, capture_output=True, text=True, encoding="utf-8")



def build_library_fixture_source(source_text: str) -> str:
    marker = "\nfn main"
    if marker not in source_text:
        return source_text.rstrip() + "\n"
    return source_text.split(marker, 1)[0].rstrip() + "\n"



def build_alternate_library_fixture_source(source_text: str) -> str:
    base = build_library_fixture_source(source_text)
    base = base.replace("@protocol Base\n@end", "@protocol AltBase\n@end")
    return (
        base.replace("module runtimeMetadataClassRecords;", "module runtimeMetadataClassRecordsAlt;")
        .replace("@protocol Base;", "@protocol AltBase;")
        .replace("@protocol Worker<Base>", "@protocol AltWorker<AltBase>")
        .replace("@interface Widget<Worker>", "@interface AltWidget<AltWorker>")
        .replace("@implementation Widget", "@implementation AltWidget")
        .replace("Widget", "AltWidget")
    )



def extract_boundary_line(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(BOUNDARY_COMMENT_PREFIX):
            return line
    return ""



def extract_section_records(readobj_stdout: str) -> list[dict[str, Any]]:
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



def merge_script_command(args: argparse.Namespace, out_dir: Path, emit_prefix: str, discovery_files: Sequence[Path]) -> list[str]:
    return [
        sys.executable,
        str(args.merge_script.resolve()),
        *[str(path.resolve()) for path in discovery_files],
        "--out-dir",
        str(out_dir.resolve()),
        "--emit-prefix",
        emit_prefix,
    ]



def run_collision_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[dict[str, Any], int, int]:
    checks_total = 0
    checks_passed = 0
    out_root = args.probe_root.resolve() / "collision"
    (out_root / "a").mkdir(parents=True, exist_ok=True)
    (out_root / "b").mkdir(parents=True, exist_ok=True)
    source_text = build_library_fixture_source(read_text(args.positive_source_fixture))
    src_a = out_root / "a" / "collision.objc3"
    src_b = out_root / "b" / "collision.objc3"
    src_a.write_text(source_text, encoding="utf-8")
    src_b.write_text(source_text, encoding="utf-8")

    compile_a = run_command([str(args.native_exe.resolve()), str(src_a), "--out-dir", str(src_a.parent), "--emit-prefix", "module"], ROOT)
    compile_b = run_command([str(args.native_exe.resolve()), str(src_b), "--out-dir", str(src_b.parent), "--emit-prefix", "module"], ROOT)
    disc_a = load_json(src_a.parent / "module.runtime-metadata-discovery.json")
    disc_b = load_json(src_b.parent / "module.runtime-metadata-discovery.json")
    ir_a = read_text(src_a.parent / "module.ll")
    ir_b = read_text(src_b.parent / "module.ll")

    summary = {
        "case_id": "M253-D003-CASE-COLLISION-PROVENANCE",
        "source_a": display_path(src_a),
        "source_b": display_path(src_b),
        "compile_exit_codes": [compile_a.returncode, compile_b.returncode],
        "anchor_symbols": [disc_a.get("linker_anchor_symbol"), disc_b.get("linker_anchor_symbol")],
        "discovery_symbols": [disc_a.get("discovery_root_symbol"), disc_b.get("discovery_root_symbol")],
        "translation_unit_identity_keys": [disc_a.get("translation_unit_identity_key"), disc_b.get("translation_unit_identity_key")],
    }

    for idx, result in enumerate((compile_a, compile_b), start=1):
        checks_total += 1
        checks_passed += require(result.returncode == 0, display_path(src_a if idx == 1 else src_b), f"M253-D003-COLL-COMPILE-{idx:02d}", "collision probe compile must succeed", failures)
    checks_total += 1
    checks_passed += require(disc_a.get("translation_unit_identity_model") == TRANSLATION_UNIT_IDENTITY_MODEL, display_path(src_a.parent), "M253-D003-COLL-MODEL-A", "discovery A must publish the D003 translation-unit identity model", failures)
    checks_total += 1
    checks_passed += require(disc_b.get("translation_unit_identity_model") == TRANSLATION_UNIT_IDENTITY_MODEL, display_path(src_b.parent), "M253-D003-COLL-MODEL-B", "discovery B must publish the D003 translation-unit identity model", failures)
    checks_total += 1
    checks_passed += require(disc_a.get("translation_unit_identity_key") != disc_b.get("translation_unit_identity_key"), display_path(out_root), "M253-D003-COLL-TU-KEY", "distinct translation units must produce distinct translation_unit_identity_key values", failures)
    checks_total += 1
    checks_passed += require(disc_a.get("linker_anchor_symbol") != disc_b.get("linker_anchor_symbol"), display_path(out_root), "M253-D003-COLL-ANCHOR", "distinct translation units must produce distinct public linker anchor symbols", failures)
    checks_total += 1
    checks_passed += require(disc_a.get("discovery_root_symbol") != disc_b.get("discovery_root_symbol"), display_path(out_root), "M253-D003-COLL-DISCOVERY", "distinct translation units must produce distinct public discovery-root symbols", failures)
    checks_total += 1
    checks_passed += require("define internal i32 @objc3c_entry()" in ir_a, display_path(src_a.parent / "module.ll"), "M253-D003-COLL-INTERNAL-ENTRY-A", "metadata-only TU A must keep objc3c_entry internal", failures)
    checks_total += 1
    checks_passed += require("define internal i32 @objc3c_entry()" in ir_b, display_path(src_b.parent / "module.ll"), "M253-D003-COLL-INTERNAL-ENTRY-B", "metadata-only TU B must keep objc3c_entry internal", failures)
    checks_total += 1
    checks_passed += require(bool(extract_boundary_line(ir_a)), display_path(src_a.parent / "module.ll"), "M253-D003-COLL-BOUNDARY-A", "IR A must publish the archive/static-link boundary comment", failures)
    checks_total += 1
    checks_passed += require(bool(extract_boundary_line(ir_b)), display_path(src_b.parent / "module.ll"), "M253-D003-COLL-BOUNDARY-B", "IR B must publish the archive/static-link boundary comment", failures)

    return summary, checks_total, checks_passed



def run_multi_archive_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[dict[str, Any], int, int]:
    checks_total = 0
    checks_passed = 0
    clang = shutil.which(args.clang) or args.clang
    llvm_readobj = shutil.which(args.llvm_readobj) or args.llvm_readobj
    llvm_lib = shutil.which(args.llvm_lib) or args.llvm_lib
    llvm_ar = shutil.which(args.llvm_ar) or args.llvm_ar
    out_root = args.probe_root.resolve() / "fanin"
    (out_root / "a").mkdir(parents=True, exist_ok=True)
    (out_root / "b").mkdir(parents=True, exist_ok=True)
    (out_root / "merged").mkdir(parents=True, exist_ok=True)

    source_text = read_text(args.positive_source_fixture)
    src_a = out_root / "a" / "lib.objc3"
    src_b = out_root / "b" / "lib.objc3"
    src_a.write_text(build_library_fixture_source(source_text), encoding="utf-8")
    src_b.write_text(build_alternate_library_fixture_source(source_text), encoding="utf-8")

    compile_a = run_command([str(args.native_exe.resolve()), str(src_a), "--out-dir", str(src_a.parent), "--emit-prefix", "module"], ROOT)
    compile_b = run_command([str(args.native_exe.resolve()), str(src_b), "--out-dir", str(src_b.parent), "--emit-prefix", "module"], ROOT)
    discovery_a_path = src_a.parent / "module.runtime-metadata-discovery.json"
    discovery_b_path = src_b.parent / "module.runtime-metadata-discovery.json"
    discovery_a = load_json(discovery_a_path)
    discovery_b = load_json(discovery_b_path)
    object_format = str(discovery_a.get("object_format", ""))

    if object_format == "coff":
        archive_a = out_root / "a" / "a.lib"
        archive_b = out_root / "b" / "b.lib"
        archive_result_a = run_command([llvm_lib, "/nologo", f"/out:{archive_a}", str(src_a.parent / "module.obj")], ROOT)
        archive_result_b = run_command([llvm_lib, "/nologo", f"/out:{archive_b}", str(src_b.parent / "module.obj")], ROOT)
    else:
        archive_a = out_root / "a" / "a.a"
        archive_b = out_root / "b" / "b.a"
        archive_result_a = run_command([llvm_ar, "rcs", str(archive_a), str(src_a.parent / "module.obj")], ROOT)
        archive_result_b = run_command([llvm_ar, "rcs", str(archive_b), str(src_b.parent / "module.obj")], ROOT)

    merge_result = run_command(
        merge_script_command(args, out_root / "merged", "module", [discovery_a_path, discovery_b_path]),
        ROOT,
    )
    merged_response = out_root / "merged" / f"module{MERGED_RESPONSE_SUFFIX}"
    merged_discovery = out_root / "merged" / f"module{MERGED_DISCOVERY_SUFFIX}"
    merged_payload = load_json(merged_discovery) if merged_discovery.exists() else {}

    main_c = out_root / "main.c"
    main_c.write_text("int main(void){return 0;}\n", encoding="utf-8")
    plain_exe = out_root / ("plain.exe" if object_format == "coff" else "plain.out")
    single_exe = out_root / ("single.exe" if object_format == "coff" else "single.out")
    merged_exe = out_root / ("merged.exe" if object_format == "coff" else "merged.out")

    plain_link = run_command([clang, str(main_c), str(archive_a), str(archive_b), "-o", str(plain_exe)], ROOT)
    single_link = run_command([clang, str(main_c), str(archive_a), f"@{src_a.parent / 'module.runtime-metadata-linker-options.rsp'}", "-o", str(single_exe)], ROOT)
    merged_link = run_command([clang, str(main_c), str(archive_a), str(archive_b), f"@{merged_response}", "-o", str(merged_exe)], ROOT)

    plain_sections_result = run_command([llvm_readobj, "--sections", str(plain_exe)], ROOT)
    single_sections_result = run_command([llvm_readobj, "--sections", str(single_exe)], ROOT)
    merged_sections_result = run_command([llvm_readobj, "--sections", str(merged_exe)], ROOT)
    plain_metadata = metadata_sections(extract_section_records(plain_sections_result.stdout), object_format)
    single_metadata = metadata_sections(extract_section_records(single_sections_result.stdout), object_format)
    merged_metadata = metadata_sections(extract_section_records(merged_sections_result.stdout), object_format)
    single_size = total_raw_size(single_metadata)
    merged_size = total_raw_size(merged_metadata)

    summary = {
        "case_id": "M253-D003-CASE-MULTI-ARCHIVE-FANIN",
        "object_format": object_format,
        "compile_exit_codes": [compile_a.returncode, compile_b.returncode],
        "archive_exit_codes": [archive_result_a.returncode, archive_result_b.returncode],
        "merge_exit_code": merge_result.returncode,
        "plain_link_exit_code": plain_link.returncode,
        "single_link_exit_code": single_link.returncode,
        "merged_link_exit_code": merged_link.returncode,
        "single_retained_metadata_raw_size": single_size,
        "merged_retained_metadata_raw_size": merged_size,
        "merged_driver_linker_flags": merged_payload.get("driver_linker_flags", []),
    }

    for idx, result in enumerate((compile_a, compile_b), start=1):
        checks_total += 1
        checks_passed += require(result.returncode == 0, display_path(src_a if idx == 1 else src_b), f"M253-D003-FANIN-COMPILE-{idx:02d}", "multi-archive fixture compile must succeed", failures)
    for idx, result in enumerate((archive_result_a, archive_result_b), start=1):
        checks_total += 1
        checks_passed += require(result.returncode == 0, display_path(archive_a if idx == 1 else archive_b), f"M253-D003-FANIN-ARCHIVE-{idx:02d}", "archive creation must succeed", failures)
    checks_total += 1
    checks_passed += require(merge_result.returncode == 0, display_path(merged_discovery), "M253-D003-FANIN-MERGE", f"merge utility failed: {merge_result.stderr.strip()}", failures)
    checks_total += 1
    checks_passed += require(merged_payload.get("contract_id") == CONTRACT_ID, display_path(merged_discovery), "M253-D003-FANIN-MERGED-CONTRACT", "merged discovery payload must use the D003 contract id", failures)
    checks_total += 1
    checks_passed += require(merged_payload.get("source_contract_id") == SOURCE_CONTRACT_ID, display_path(merged_discovery), "M253-D003-FANIN-MERGED-SOURCE-CONTRACT", "merged discovery payload must record the D002 source contract id", failures)
    checks_total += 1
    checks_passed += require(merged_payload.get("merge_model") == MERGE_MODEL, display_path(merged_discovery), "M253-D003-FANIN-MERGED-MODEL", "merged discovery payload must publish the D003 merge model", failures)
    checks_total += 1
    checks_passed += require(len(merged_payload.get("driver_linker_flags", [])) == 2, display_path(merged_discovery), "M253-D003-FANIN-MERGED-FLAGS", "merged discovery payload must retain one driver flag per archive input", failures)
    checks_total += 1
    checks_passed += require(plain_link.returncode == 0, display_path(plain_exe), "M253-D003-FANIN-PLAIN-LINK", f"plain multi-archive link failed: {plain_link.stderr.strip()}", failures)
    checks_total += 1
    checks_passed += require(single_link.returncode == 0, display_path(single_exe), "M253-D003-FANIN-SINGLE-LINK", f"single retained link failed: {single_link.stderr.strip()}", failures)
    checks_total += 1
    checks_passed += require(merged_link.returncode == 0, display_path(merged_exe), "M253-D003-FANIN-MERGED-LINK", f"merged retained link failed: {merged_link.stderr.strip()}", failures)
    checks_total += 1
    checks_passed += require(plain_sections_result.returncode == 0, display_path(plain_exe), "M253-D003-FANIN-PLAIN-READOBJ", "plain linked image must be inspectable", failures)
    checks_total += 1
    checks_passed += require(single_sections_result.returncode == 0, display_path(single_exe), "M253-D003-FANIN-SINGLE-READOBJ", "single retained image must be inspectable", failures)
    checks_total += 1
    checks_passed += require(merged_sections_result.returncode == 0, display_path(merged_exe), "M253-D003-FANIN-MERGED-READOBJ", "merged retained image must be inspectable", failures)
    checks_total += 1
    checks_passed += require(not plain_metadata, display_path(plain_exe), "M253-D003-FANIN-PLAIN-NO-METADATA", "plain multi-archive link must not retain metadata sections", failures)
    checks_total += 1
    checks_passed += require(bool(single_metadata), display_path(single_exe), "M253-D003-FANIN-SINGLE-METADATA", "single retained link must preserve metadata sections", failures)
    checks_total += 1
    checks_passed += require(bool(merged_metadata), display_path(merged_exe), "M253-D003-FANIN-MERGED-METADATA", "merged retained link must preserve metadata sections", failures)
    checks_total += 1
    checks_passed += require(merged_size > single_size, display_path(merged_exe), "M253-D003-FANIN-MERGED-SIZE", "merged retained metadata must exceed the single-archive retained baseline", failures)

    return summary, checks_total, checks_passed



def run_negative_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[dict[str, Any], int, int]:
    checks_total = 0
    checks_passed = 0
    out_root = args.probe_root.resolve() / "negative"
    out_root.mkdir(parents=True, exist_ok=True)
    source_text = build_library_fixture_source(read_text(args.positive_source_fixture))
    src = out_root / "duplicate.objc3"
    src.write_text(source_text, encoding="utf-8")
    compile_result = run_command([str(args.native_exe.resolve()), str(src), "--out-dir", str(out_root), "--emit-prefix", "module"], ROOT)
    discovery = out_root / "module.runtime-metadata-discovery.json"
    merge_out = out_root / "merged"
    merge_out.mkdir(parents=True, exist_ok=True)
    merge_result = run_command(merge_script_command(args, merge_out, "module", [discovery, discovery]), ROOT)
    merged_response = merge_out / f"module{MERGED_RESPONSE_SUFFIX}"
    merged_discovery = merge_out / f"module{MERGED_DISCOVERY_SUFFIX}"

    summary = {
        "case_id": "M253-D003-CASE-NEGATIVE-COLLIDING-DISCOVERY-INPUTS",
        "compile_exit_code": compile_result.returncode,
        "merge_exit_code": merge_result.returncode,
        "merged_response_exists": merged_response.exists(),
        "merged_discovery_exists": merged_discovery.exists(),
    }

    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(src), "M253-D003-NEG-COMPILE", "negative merge probe source must compile so the collision is real", failures)
    checks_total += 1
    checks_passed += require(merge_result.returncode != 0, display_path(discovery), "M253-D003-NEG-MERGE-STATUS", "merge utility must fail on colliding discovery inputs", failures)
    checks_total += 1
    checks_passed += require(not merged_response.exists(), display_path(merge_out), "M253-D003-NEG-NO-RSP", "collision failure must not emit a merged response artifact", failures)
    checks_total += 1
    checks_passed += require(not merged_discovery.exists(), display_path(merge_out), "M253-D003-NEG-NO-DISCOVERY", "collision failure must not emit a merged discovery artifact", failures)

    return summary, checks_total, checks_passed



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
        (args.ir_emitter_header, IR_EMITTER_HEADER_SNIPPETS),
        (args.ir_emitter_cpp, IR_EMITTER_CPP_SNIPPETS),
        (args.process_cpp, PROCESS_CPP_SNIPPETS),
        (args.frontend_artifacts_cpp, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.merge_script, MERGE_SCRIPT_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_groups:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    if not args.skip_dynamic_probes:
        collision_summary, collision_total, collision_passed = run_collision_probe(args, failures)
        checks_total += collision_total
        checks_passed += collision_passed
        dynamic_cases.append(collision_summary)

        fanin_summary, fanin_total, fanin_passed = run_multi_archive_probe(args, failures)
        checks_total += fanin_total
        checks_passed += fanin_passed
        dynamic_cases.append(fanin_summary)

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
    summary_out = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        print(f"[m253-d003] FAIL {checks_passed}/{checks_total} -> {display_path(summary_out)}", file=sys.stderr)
        for failure in failures:
            print(f"  - {failure.check_id} [{failure.artifact}] {failure.detail}", file=sys.stderr)
        return 1

    print(f"[m253-d003] PASS {checks_passed}/{checks_total} -> {display_path(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
