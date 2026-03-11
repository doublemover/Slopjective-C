#!/usr/bin/env python3
"""Validate M260-A002 runtime-backed ownership attribute surface emission."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m260-a002-runtime-backed-object-ownership-attribute-surface-v1"
CONTRACT_ID = "objc3c-runtime-backed-object-ownership-attribute-surface/m260-a002-v1"
SOURCE_MODEL = "runtime-backed-property-source-surface-publishes-attribute-lifetime-hook-and-accessor-ownership-profiles"
DESCRIPTOR_MODEL = "emitted-property-descriptor-records-carry-attribute-lifetime-hook-and-accessor-ownership-strings"
RUNTIME_MODEL = "runtime-backed-property-metadata-consumes-emitted-ownership-strings-without-source-rediscovery"
FAILURE_MODEL = "no-manifest-only-ownership-proof-no-source-recovery-no-live-arc-hook-emission-yet"
NEXT_ISSUE = "M260-B001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m260" / "M260-A002" / "runtime_backed_object_ownership_attribute_surface_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m260_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m260" / "m260_a002_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PACKAGE_JSON = ROOT / "package.json"
COMPILE_SCRIPT = ROOT / "scripts" / "objc3c_native_compile.ps1"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m260_runtime_backed_object_ownership_attribute_surface_positive.objc3"
PROBE_OUT_DIR = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m260" / "a002" / "positive"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--doc-source", type=Path, default=DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=DOC_NATIVE)
    parser.add_argument("--architecture-doc", type=Path, default=ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--sema-source", type=Path, default=SEMA_SOURCE)
    parser.add_argument("--ir-source", type=Path, default=IR_SOURCE)
    parser.add_argument("--lowering-header", type=Path, default=LOWERING_HEADER)
    parser.add_argument("--lowering-source", type=Path, default=LOWERING_SOURCE)
    parser.add_argument("--runtime-header", type=Path, default=RUNTIME_HEADER)
    parser.add_argument("--runtime-source", type=Path, default=RUNTIME_SOURCE)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
    parser.add_argument("--compile-script", type=Path, default=COMPILE_SCRIPT)
    parser.add_argument("--fixture", type=Path, default=FIXTURE)
    parser.add_argument("--probe-out-dir", type=Path, default=PROBE_OUT_DIR)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_compile(compile_script: Path, fixture: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    cmd = [
        "pwsh",
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(compile_script),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    return subprocess.run(
        cmd,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        check=False,
    )


def property_record_by_key(records: list[dict[str, Any]], owner_kind: str, property_name: str) -> dict[str, Any] | None:
    for record in records:
        if record.get("owner_kind") == owner_kind and record.get("property_name") == property_name:
            return record
    return None


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    dynamic_probe_summary: dict[str, Any] = {"executed": False}

    checks_total += 5
    checks_passed += ensure_snippets(
        args.expectations_doc,
        (
            SnippetCheck("M260-A002-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M260-A002-DOC-02", "Issue: `#7169`"),
            SnippetCheck("M260-A002-DOC-03", "`tests/tooling/fixtures/native/m260_runtime_backed_object_ownership_attribute_surface_positive.objc3`"),
            SnippetCheck("M260-A002-DOC-04", "`property_attribute_profile`"),
            SnippetCheck("M260-A002-DOC-05", "`M260-B001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M260-A002-PKT-01", "Packet: `M260-A002`"),
            SnippetCheck("M260-A002-PKT-02", "Issue: `#7169`"),
            SnippetCheck("M260-A002-PKT-03", "Dependencies: `M260-A001`"),
            SnippetCheck("M260-A002-PKT-04", "`ownership_runtime_hook_profile`"),
            SnippetCheck("M260-A002-PKT-05", "`M260-B001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M260-A002-SRC-01", "## M260 runtime-backed object ownership attribute surface (A002)"),
            SnippetCheck("M260-A002-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-A002-SRC-03", "`ownership_runtime_hook_profile`"),
            SnippetCheck("M260-A002-SRC-04", "runtime-backed properties and members"),
            SnippetCheck("M260-A002-SRC-05", "`M260-B001`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M260-A002-NDOC-01", "## M260 runtime-backed object ownership attribute surface (A002)"),
            SnippetCheck("M260-A002-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-A002-NDOC-03", "property descriptors now carry the ownership surface"),
            SnippetCheck("M260-A002-NDOC-04", "`M260-B001`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.architecture_doc,
        (
            SnippetCheck("M260-A002-ARCH-01", "## M260 Runtime-Backed Object Ownership Attribute Surface (A002)"),
            SnippetCheck("M260-A002-ARCH-02", "property/member ownership attribute"),
            SnippetCheck("M260-A002-ARCH-03", "manifest-only"),
            SnippetCheck("M260-A002-ARCH-04", "`M260-B001`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M260-A002-SPC-01", "## M260 runtime-backed object ownership attribute surface (A002)"),
            SnippetCheck("M260-A002-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-A002-SPC-03", f"`{SOURCE_MODEL}`"),
            SnippetCheck("M260-A002-SPC-04", f"`{DESCRIPTOR_MODEL}`"),
            SnippetCheck("M260-A002-SPC-05", "`M260-B001`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M260-A002-META-01", f"`{CONTRACT_ID}`"),
            SnippetCheck("M260-A002-META-02", "`property_attribute_profile`"),
            SnippetCheck("M260-A002-META-03", "tmp/artifacts/compilation/objc3c-native/m260/a002/positive/module.obj"),
            SnippetCheck("M260-A002-META-04", "`M260-B001`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.sema_source,
        (
            SnippetCheck("M260-A002-SEMA-01", "M260-A002 runtime-backed ownership attribute surface anchor:"),
            SnippetCheck("M260-A002-SEMA-02", "property_metadata.property_attribute_profile"),
            SnippetCheck("M260-A002-SEMA-03", "property_metadata.accessor_ownership_profile"),
            SnippetCheck("M260-A002-SEMA-04", "runtime-backed descriptors can stay synchronized"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.ir_source,
        (
            SnippetCheck("M260-A002-IR-01", "M260-A002 runtime-backed object ownership attribute surface anchor:"),
            SnippetCheck("M260-A002-IR-02", "runtime_backed_object_ownership_attribute_surface"),
            SnippetCheck("M260-A002-IR-03", '"property_attribute_profile"'),
            SnippetCheck("M260-A002-IR-04", '"ownership_runtime_hook_profile"'),
            SnippetCheck("M260-A002-IR-05", '"accessor_ownership_profile"'),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_header,
        (
            SnippetCheck("M260-A002-LHDR-01", "kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId"),
            SnippetCheck("M260-A002-LHDR-02", SOURCE_MODEL),
            SnippetCheck("M260-A002-LHDR-03", DESCRIPTOR_MODEL),
            SnippetCheck("M260-A002-LHDR-04", RUNTIME_MODEL),
            SnippetCheck("M260-A002-LHDR-05", "Objc3RuntimeBackedObjectOwnershipAttributeSurfaceSummary"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.lowering_source,
        (
            SnippetCheck("M260-A002-LOW-01", "M260-A002 runtime-backed object ownership attribute surface anchor:"),
            SnippetCheck("M260-A002-LOW-02", "property/member facts stop being manifest-only evidence"),
            SnippetCheck("M260-A002-LOW-03", "kObjc3RuntimeBackedObjectOwnershipAttributeDescriptorModel"),
            SnippetCheck("M260-A002-LOW-04", "no-live-arc-hook-emission-no-source-recovery"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.runtime_header,
        (
            SnippetCheck("M260-A002-RHDR-01", "const char *property_attribute_profile;"),
            SnippetCheck("M260-A002-RHDR-02", "const char *ownership_lifetime_profile;"),
            SnippetCheck("M260-A002-RHDR-03", "const char *ownership_runtime_hook_profile;"),
            SnippetCheck("M260-A002-RHDR-04", "const char *accessor_ownership_profile;"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.runtime_source,
        (
            SnippetCheck("M260-A002-RT-01", "const char *property_attribute_profile;"),
            SnippetCheck("M260-A002-RT-02", "snapshot->property_attribute_profile ="),
            SnippetCheck("M260-A002-RT-03", "snapshot->ownership_runtime_hook_profile ="),
            SnippetCheck("M260-A002-RT-04", "snapshot->accessor_ownership_profile ="),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M260-A002-PKG-01", '"check:objc3c:m260-a002-ownership-attribute-surface-for-runtime-backed-objects-and-members":'),
            SnippetCheck("M260-A002-PKG-02", '"test:tooling:m260-a002-ownership-attribute-surface-for-runtime-backed-objects-and-members":'),
            SnippetCheck("M260-A002-PKG-03", '"check:objc3c:m260-a002-lane-a-readiness":'),
            SnippetCheck("M260-A002-PKG-04", 'python scripts/run_m260_a002_lane_a_readiness.py'),
            SnippetCheck("M260-A002-PKG-05", '"compile:objc3c":'),
        ),
        failures,
    )

    if not args.skip_dynamic_probes:
        dynamic_probe_summary["executed"] = True
        result = run_compile(args.compile_script, args.fixture, args.probe_out_dir)
        dynamic_probe_summary["returncode"] = result.returncode
        dynamic_probe_summary["stdout"] = result.stdout.strip()
        dynamic_probe_summary["stderr"] = result.stderr.strip()

        manifest_path = args.probe_out_dir / "module.manifest.json"
        ir_path = args.probe_out_dir / "module.ll"
        obj_path = args.probe_out_dir / "module.obj"

        checks_total += 20
        checks_passed += require(result.returncode == 0, display_path(args.compile_script), "M260-A002-DYN-01", "ownership-surface fixture compile failed", failures)
        checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M260-A002-DYN-02", "manifest artifact missing", failures)
        checks_passed += require(ir_path.exists(), display_path(ir_path), "M260-A002-DYN-03", "IR artifact missing", failures)
        checks_passed += require(obj_path.exists(), display_path(obj_path), "M260-A002-DYN-04", "object artifact missing", failures)

        manifest_text = read_text(manifest_path) if manifest_path.exists() else ""
        checks_passed += require(manifest_text.count('\"property_attribute_profile\":\"') >= 10, display_path(manifest_path), "M260-A002-DYN-05", "expected serialized property attribute profiles in the manifest surfaces", failures)
        checks_passed += require(manifest_text.count('\"accessor_ownership_profile\":\"') >= 10, display_path(manifest_path), "M260-A002-DYN-06", "expected serialized accessor ownership profiles in the manifest surfaces", failures)
        checks_passed += require('\"property_name\":\"weakValue\"' in manifest_text and '\"ownership_lifetime_profile\":\"weak\"' in manifest_text, display_path(manifest_path), "M260-A002-DYN-07", "weak property must publish weak lifetime profile", failures)
        checks_passed += require('\"property_name\":\"weakValue\"' in manifest_text and '\"ownership_runtime_hook_profile\":\"objc-weak-side-table\"' in manifest_text, display_path(manifest_path), "M260-A002-DYN-08", "weak property must publish weak runtime hook profile", failures)
        checks_passed += require('\"property_name\":\"weakValue\"' in manifest_text and '\"accessor_ownership_profile\":\"getter=weakValue;setter_available=1;setter=setWeakValue:;ownership_lifetime=weak;runtime_hook=objc-weak-side-table\"' in manifest_text, display_path(manifest_path), "M260-A002-DYN-09", "weak accessor ownership profile drifted", failures)
        checks_passed += require('\"property_name\":\"borrowedValue\"' in manifest_text and '\"ownership_lifetime_profile\":\"unowned-unsafe\"' in manifest_text, display_path(manifest_path), "M260-A002-DYN-10", "assign object property must publish unowned-unsafe lifetime profile", failures)
        checks_passed += require('\"property_name\":\"borrowedValue\"' in manifest_text and '\"ownership_runtime_hook_profile\":\"objc-unowned-unsafe-direct\"' in manifest_text, display_path(manifest_path), "M260-A002-DYN-11", "assign object property must publish direct unowned runtime hook", failures)
        checks_passed += require('\"property_name\":\"token\"' in manifest_text and '\"property_attribute_profile\":\"readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=1;strong=0;weak=0;unowned=0;assign=0;attributes=copy,getter=tokenValue,nonatomic,setter=setTokenValue:\"' in manifest_text, display_path(manifest_path), "M260-A002-DYN-12", "copy property attribute profile drifted", failures)
        checks_passed += require('\"property_name\":\"value\"' in manifest_text and '\"property_attribute_profile\":\"readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;strong=1;weak=0;unowned=0;assign=0;attributes=getter=currentValue,nonatomic,setter=setCurrentValue:,strong\"' in manifest_text, display_path(manifest_path), "M260-A002-DYN-13", "strong property attribute profile drifted", failures)
        checks_passed += require('\"property_name\":\"count\"' in manifest_text and '\"accessor_ownership_profile\":\"getter=countValue;setter_available=1;setter=setCountValue:;ownership_lifetime=unowned-unsafe;runtime_hook=objc-unowned-unsafe-direct\"' in manifest_text, display_path(manifest_path), "M260-A002-DYN-14", "assign scalar accessor ownership profile drifted", failures)

        ir_text = read_text(ir_path) if ir_path.exists() else ""
        checks_passed += require(f"; runtime_backed_object_ownership_attribute_surface = contract={CONTRACT_ID}" in ir_text, display_path(ir_path), "M260-A002-DYN-15", "IR ownership-surface summary missing", failures)
        checks_passed += require("@__objc3_meta_property_property_attribute_profile_0004" in ir_text, display_path(ir_path), "M260-A002-DYN-16", "weak property attribute profile symbol missing from IR", failures)
        checks_passed += require("@__objc3_meta_property_ownership_lifetime_profile_0004" in ir_text and 'c"weak\\00"' in ir_text, display_path(ir_path), "M260-A002-DYN-17", "weak lifetime profile not emitted into property descriptor section", failures)
        checks_passed += require("@__objc3_meta_property_ownership_runtime_hook_profile_0004" in ir_text and 'c"objc-weak-side-table\\00"' in ir_text, display_path(ir_path), "M260-A002-DYN-18", "weak runtime hook profile not emitted into property descriptor section", failures)
        checks_passed += require("@__objc3_meta_property_property_attribute_profile_0000" in ir_text and 'c"readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;strong=0;weak=0;unowned=0;assign=1;attributes=assign,getter=borrowedValue,setter=setBorrowedValue:\\00"' in ir_text, display_path(ir_path), "M260-A002-DYN-19", "assign-object attribute profile not emitted into property descriptor section", failures)
        checks_passed += require("ptr @__objc3_meta_property_property_attribute_profile_0004, ptr @__objc3_meta_property_ownership_lifetime_profile_0004, ptr @__objc3_meta_property_ownership_runtime_hook_profile_0004, ptr @__objc3_meta_property_accessor_ownership_profile_0004" in ir_text, display_path(ir_path), "M260-A002-DYN-20", "weak property descriptor must reference emitted ownership profile strings", failures)

        dynamic_probe_summary["manifest"] = display_path(manifest_path)
        dynamic_probe_summary["ir"] = display_path(ir_path)
        dynamic_probe_summary["obj"] = display_path(obj_path)
    else:
        dynamic_probe_summary["skipped"] = True

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "source_model": SOURCE_MODEL,
        "descriptor_model": DESCRIPTOR_MODEL,
        "runtime_model": RUNTIME_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "dynamic_probe": dynamic_probe_summary,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
