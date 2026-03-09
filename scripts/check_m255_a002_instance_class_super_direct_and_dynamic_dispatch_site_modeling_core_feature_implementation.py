#!/usr/bin/env python3
"""Fail-closed contract checker for M255-A002 dispatch-site modeling."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-a002-dispatch-site-modeling-core-feature-implementation-v1"
IMPLEMENTATION_CONTRACT_ID = "objc3c-dispatch-site-modeling/m255-a002-v1"
LOWERING_CONTRACT_ID = "objc3c-dispatch-surface-classification/m255-a001-v1"
SURFACE_PATH = (
    "frontend.pipeline.semantic_surface."
    "objc_dispatch_surface_classification_surface"
)
LOWERING_PATH = "lowering_dispatch_surface_classification"
LIVE_RUNTIME_ENTRYPOINT_FAMILY = "objc3_runtime_dispatch_i32-objc3_msgsend_i32-compat"
DIRECT_DISPATCH_BINDING = "reserved-non-goal"
EXPECTED_COUNTS = {
    "instance_dispatch_sites": 2,
    "class_dispatch_sites": 2,
    "super_dispatch_sites": 1,
    "direct_dispatch_sites": 0,
    "dynamic_dispatch_sites": 1,
}
EXPECTED_ENTRYPOINTS = {
    "instance_entrypoint_family": LIVE_RUNTIME_ENTRYPOINT_FAMILY,
    "class_entrypoint_family": LIVE_RUNTIME_ENTRYPOINT_FAMILY,
    "super_entrypoint_family": LIVE_RUNTIME_ENTRYPOINT_FAMILY,
    "direct_entrypoint_family": DIRECT_DISPATCH_BINDING,
    "dynamic_entrypoint_family": LIVE_RUNTIME_ENTRYPOINT_FAMILY,
}

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m255_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation_a002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m255"
    / "m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation_packet.md"
)
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_PARSER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_SEMA = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
DEFAULT_IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_RUNTIME_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_dispatch_surface_modeling.objc3"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m255_a002_lane_a_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "a002-dispatch-site-modeling"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m255/M255-A002/dispatch_site_modeling_summary.json")


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
    SnippetCheck("M255-A002-DOC-EXP-01", "# M255 Instance, Class, Super, Direct, and Dynamic Dispatch-Site Modeling Core Feature Implementation Expectations (A002)"),
    SnippetCheck("M255-A002-DOC-EXP-02", f"Implementation Contract ID: `{IMPLEMENTATION_CONTRACT_ID}`"),
    SnippetCheck("M255-A002-DOC-EXP-03", f"Live lowering handoff contract: `{LOWERING_CONTRACT_ID}`"),
    SnippetCheck("M255-A002-DOC-EXP-04", f"`{SURFACE_PATH}`"),
    SnippetCheck("M255-A002-DOC-EXP-05", f"`{LOWERING_PATH}`"),
    SnippetCheck("M255-A002-DOC-EXP-06", "instance dispatch sites `2`"),
    SnippetCheck("M255-A002-DOC-EXP-07", "class dispatch sites `2`"),
    SnippetCheck("M255-A002-DOC-EXP-08", "super dispatch sites `1`"),
    SnippetCheck("M255-A002-DOC-EXP-09", "dynamic dispatch sites `1`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-A002-DOC-PKT-01", "# M255-A002 Instance, Class, Super, Direct, and Dynamic Dispatch-Site Modeling Core Feature Implementation Packet"),
    SnippetCheck("M255-A002-DOC-PKT-02", "Packet: `M255-A002`"),
    SnippetCheck("M255-A002-DOC-PKT-03", "Dependencies: `M255-A001`"),
    SnippetCheck("M255-A002-DOC-PKT-04", f"Implementation Contract ID: `{IMPLEMENTATION_CONTRACT_ID}`"),
    SnippetCheck("M255-A002-DOC-PKT-05", "- class `2`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-A002-NDOC-01", "## Dispatch-site modeling (M255-A002)"),
    SnippetCheck("M255-A002-NDOC-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M255-A002-NDOC-03", f"`{LOWERING_PATH}`"),
    SnippetCheck("M255-A002-NDOC-04", "`tests/tooling/fixtures/native/m255_dispatch_surface_modeling.objc3`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-A002-SPC-01", "## M255 dispatch-site modeling implementation (A002)"),
    SnippetCheck("M255-A002-SPC-02", f"`{LOWERING_CONTRACT_ID}`"),
    SnippetCheck("M255-A002-SPC-03", "known-class identifiers plus implicit `self`/`super`"),
    SnippetCheck("M255-A002-SPC-04", "`!objc3.objc_dispatch_surface_classification`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-A002-META-01", "## M255 dispatch/runtime metadata anchors (A002)"),
    SnippetCheck("M255-A002-META-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M255-A002-META-03", f"`{LOWERING_PATH}`"),
    SnippetCheck("M255-A002-META-04", "`frontend_objc_dispatch_surface_classification_profile`"),
)
AST_SNIPPETS = (
    SnippetCheck("M255-A002-AST-01", "enum class DispatchSurfaceKind {"),
    SnippetCheck("M255-A002-AST-02", "Unclassified,"),
    SnippetCheck("M255-A002-AST-03", "DispatchSurfaceKind dispatch_surface_kind ="),
    SnippetCheck("M255-A002-AST-04", "std::string dispatch_surface_family_symbol;"),
    SnippetCheck("M255-A002-AST-05", "bool dispatch_surface_is_normalized = false;"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-A002-PARSE-01", "M255-A002 dispatch-site modeling anchor"),
    SnippetCheck("M255-A002-PARSE-02", "dispatch_surface_kind = Expr::DispatchSurfaceKind::Unclassified;"),
    SnippetCheck("M255-A002-PARSE-03", "dispatch_surface_is_normalized = false;"),
)
FRONTEND_PIPELINE_SNIPPETS = (
    SnippetCheck("M255-A002-PIPE-01", "NormalizeProgramDispatchSurfaceClassification("),
    SnippetCheck("M255-A002-PIPE-02", "CollectKnownClassNames("),
    SnippetCheck("M255-A002-PIPE-03", "ClassifyDispatchSurfaceKind("),
    SnippetCheck("M255-A002-PIPE-04", "DispatchSurfaceFamilySymbol("),
    SnippetCheck("M255-A002-PIPE-05", "MutableObjc3ParsedProgramAst(result.program)"),
)
SEMA_SNIPPETS = (
    SnippetCheck("M255-A002-SEMA-01", 'body_globals.try_emplace(interface_decl.name, ValueType::ObjCClass);'),
    SnippetCheck("M255-A002-SEMA-02", 'scopes.back().emplace("self",'),
    SnippetCheck("M255-A002-SEMA-03", 'scopes.back().emplace("super",'),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-A002-HDR-01", "bool IsValidObjc3DispatchSurfaceClassificationContract("),
    SnippetCheck("M255-A002-HDR-02", "std::string Objc3DispatchSurfaceClassificationReplayKey("),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M255-A002-LCPP-01", "bool IsValidObjc3DispatchSurfaceClassificationContract("),
    SnippetCheck("M255-A002-LCPP-02", "Objc3DispatchSurfaceClassificationReplayKey("),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M255-A002-ART-01", '\\"objc_dispatch_surface_classification_surface\\"'),
    SnippetCheck("M255-A002-ART-02", '\\"lowering_dispatch_surface_classification\\"'),
    SnippetCheck("M255-A002-ART-03", "BuildDispatchSurfaceClassificationContract("),
    SnippetCheck("M255-A002-ART-04", "dispatch_surface_classification_replay_key"),
)
IR_HEADER_SNIPPETS = (
    SnippetCheck("M255-A002-IRH-01", "std::string lowering_dispatch_surface_classification_replay_key;"),
    SnippetCheck("M255-A002-IRH-02", "std::size_t dispatch_surface_classification_class_sites = 0;"),
    SnippetCheck("M255-A002-IRH-03", "bool deterministic_dispatch_surface_classification_handoff = false;"),
)
IR_CPP_SNIPPETS = (
    SnippetCheck("M255-A002-IRC-01", "frontend_objc_dispatch_surface_classification_profile"),
    SnippetCheck("M255-A002-IRC-02", "!objc3.objc_dispatch_surface_classification = !{!66}"),
    SnippetCheck("M255-A002-IRC-03", 'ctx.immediate_identifiers["self"] = self_identity;'),
    SnippetCheck("M255-A002-IRC-04", 'ctx.immediate_identifiers["super"] = super_identity;'),
    SnippetCheck("M255-A002-IRC-05", "SeedKnownClassReceiverBindings(ctx);"),
)
RUNTIME_SHIM_SNIPPETS = (
    SnippetCheck("M255-A002-SHIM-01", "M255-A002 dispatch-site modeling"),
    SnippetCheck("M255-A002-SHIM-02", "non-zero receiver identities for implicit self/super and known class-name sites"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M255-A002-FIX-01", "module dispatchSurfaceModeling;"),
    SnippetCheck("M255-A002-FIX-02", "+ (id) sharedFromSelf;"),
    SnippetCheck("M255-A002-FIX-03", "return [self shared];"),
    SnippetCheck("M255-A002-FIX-04", "[super ping];"),
    SnippetCheck("M255-A002-FIX-05", "[Widget shared];"),
    SnippetCheck("M255-A002-FIX-06", "return [(toggle ? local : 0) ping];"),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M255-A002-RUN-01", "check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py"),
    SnippetCheck("M255-A002-RUN-02", "test_check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M255-A002-PKG-01", '"check:objc3c:m255-a002-instance-class-super-direct-and-dynamic-dispatch-site-modeling-core-feature-implementation": "python scripts/check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py"'),
    SnippetCheck("M255-A002-PKG-02", '"test:tooling:m255-a002-instance-class-super-direct-and-dynamic-dispatch-site-modeling-core-feature-implementation": "python -m pytest tests/tooling/test_check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py -q"'),
    SnippetCheck("M255-A002-PKG-03", '"check:objc3c:m255-a002-lane-a-readiness": "python scripts/run_m255_a002_lane_a_readiness.py"'),
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
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--parser", type=Path, default=DEFAULT_PARSER)
    parser.add_argument("--frontend-pipeline", type=Path, default=DEFAULT_FRONTEND_PIPELINE)
    parser.add_argument("--sema", type=Path, default=DEFAULT_SEMA)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--ir-header", type=Path, default=DEFAULT_IR_HEADER)
    parser.add_argument("--ir-cpp", type=Path, default=DEFAULT_IR_CPP)
    parser.add_argument("--runtime-shim", type=Path, default=DEFAULT_RUNTIME_SHIM)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
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


def run_process(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, capture_output=True, text=True, check=False)


def check_dispatch_surface_payload(payload: dict[str, Any], artifact: str, prefix: str, failures: list[Finding]) -> int:
    checks_total = 0
    for key, expected in EXPECTED_COUNTS.items():
        checks_total += require(payload.get(key) == expected, artifact, f"{prefix}-{key.upper()}", f"expected {key}={expected}", failures)
    for key, expected in EXPECTED_ENTRYPOINTS.items():
        checks_total += require(payload.get(key) == expected, artifact, f"{prefix}-{key.upper()}", f"expected {key}={expected}", failures)
    checks_total += require(payload.get("deterministic_handoff") is True, artifact, f"{prefix}-DETERMINISTIC", "deterministic_handoff must be true", failures)
    replay_key = payload.get("replay_key")
    checks_total += require(isinstance(replay_key, str) and LOWERING_CONTRACT_ID in replay_key, artifact, f"{prefix}-REPLAY", "replay key must contain the lowering contract id", failures)
    return checks_total


def run_runner_probe(*, runner_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(runner_exe.exists(), display_path(runner_exe), "M255-A002-RUNNER-EXISTS", "frontend runner binary is missing", failures)
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M255-A002-FIXTURE-EXISTS", "fixture is missing", failures)
    if failures:
        return checks_total, failures, None

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
    completed = run_process(command)
    summary_path = out_dir / "module.c_api_summary.json"
    manifest_path = out_dir / "module.manifest.json"
    checks_total += require(completed.returncode == 0, display_path(out_dir), "M255-A002-RUNNER-EXIT", "runner probe must succeed", failures)
    checks_total += require(summary_path.exists(), display_path(summary_path), "M255-A002-RUNNER-SUMMARY", "runner summary is missing", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M255-A002-RUNNER-MANIFEST", "runner manifest is missing", failures)
    if failures:
        return checks_total, failures, None

    summary = load_json(summary_path)
    manifest = load_json(manifest_path)
    checks_total += require(summary.get("status") == 0 and summary.get("success") is True, display_path(summary_path), "M255-A002-RUNNER-STATUS", "runner summary must report success", failures)
    emit_stage = summary.get("stages", {}).get("emit")
    checks_total += require(isinstance(emit_stage, dict) and emit_stage.get("attempted") is False and emit_stage.get("skipped") is True, display_path(summary_path), "M255-A002-RUNNER-EMIT", "manifest-only runner probe must skip emit stage", failures)
    checks_total += require(str(summary.get("paths", {}).get("manifest", "")).endswith("module.manifest.json"), display_path(summary_path), "M255-A002-RUNNER-MANIFEST-PATH", "runner summary must publish manifest path", failures)

    semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    surface_payload = semantic_surface.get("objc_dispatch_surface_classification_surface")
    checks_total += require(isinstance(surface_payload, dict), display_path(manifest_path), "M255-A002-SURFACE-EXISTS", f"missing {SURFACE_PATH}", failures)
    if isinstance(surface_payload, dict):
        checks_total += check_dispatch_surface_payload(surface_payload, display_path(manifest_path), "M255-A002-SURFACE", failures)

    lowering_payload = manifest.get("lowering_dispatch_surface_classification")
    checks_total += require(isinstance(lowering_payload, dict), display_path(manifest_path), "M255-A002-LOWERING-EXISTS", f"missing {LOWERING_PATH}", failures)
    if isinstance(lowering_payload, dict):
        checks_total += require(lowering_payload.get("lane_contract") == LOWERING_CONTRACT_ID, display_path(manifest_path), "M255-A002-LOWERING-CONTRACT", "lowering handoff contract mismatch", failures)
        checks_total += require(lowering_payload.get("deterministic_handoff") is True, display_path(manifest_path), "M255-A002-LOWERING-DETERMINISTIC", "lowering handoff must be deterministic", failures)
        checks_total += require(lowering_payload.get("replay_key") == surface_payload.get("replay_key"), display_path(manifest_path), "M255-A002-LOWERING-REPLAY", "lowering replay key must match semantic surface replay key", failures)

    case_payload = {
        "mode": "runner-manifest-only",
        "fixture": display_path(fixture_path),
        "summary_path": display_path(summary_path),
        "manifest_path": display_path(manifest_path),
        "surface": surface_payload,
        "lowering": lowering_payload,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, failures, case_payload


def run_native_probe(*, native_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    failures: list[Finding] = []
    checks_total = 0
    checks_total += require(native_exe.exists(), display_path(native_exe), "M255-A002-NATIVE-EXISTS", "native binary is missing", failures)
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M255-A002-FIXTURE-EXISTS-2", "fixture is missing", failures)
    if failures:
        return checks_total, failures, None

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(native_exe),
        str(fixture_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    completed = run_process(command)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    backend_path = out_dir / "module.object-backend.txt"
    checks_total += require(completed.returncode == 0, display_path(out_dir), "M255-A002-NATIVE-EXIT", "native compile must succeed", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M255-A002-NATIVE-MANIFEST", "native manifest is missing", failures)
    checks_total += require(ir_path.exists(), display_path(ir_path), "M255-A002-NATIVE-IR", "native IR is missing", failures)
    checks_total += require(backend_path.exists(), display_path(backend_path), "M255-A002-NATIVE-BACKEND", "native backend marker is missing", failures)
    if failures:
        return checks_total, failures, None

    manifest = load_json(manifest_path)
    ir_text = ir_path.read_text(encoding="utf-8")
    backend_text = backend_path.read_text(encoding="utf-8").strip()
    semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    surface_payload = semantic_surface.get("objc_dispatch_surface_classification_surface")
    checks_total += require(isinstance(surface_payload, dict), display_path(manifest_path), "M255-A002-NATIVE-SURFACE-EXISTS", f"missing {SURFACE_PATH}", failures)
    if isinstance(surface_payload, dict):
        checks_total += check_dispatch_surface_payload(surface_payload, display_path(manifest_path), "M255-A002-NATIVE-SURFACE", failures)
    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M255-A002-NATIVE-BACKEND-TEXT", "native backend must remain llvm-direct", failures)
    checks_total += require("frontend_objc_dispatch_surface_classification_profile" in ir_text, display_path(ir_path), "M255-A002-NATIVE-IR-COMMENT", "IR must contain dispatch-surface profile comment", failures)
    checks_total += require("!objc3.objc_dispatch_surface_classification = !{!66}" in ir_text, display_path(ir_path), "M255-A002-NATIVE-IR-METADATA", "IR must contain dispatch-surface named metadata", failures)

    case_payload = {
        "mode": "native-llvm-direct",
        "fixture": display_path(fixture_path),
        "manifest_path": display_path(manifest_path),
        "ir_path": display_path(ir_path),
        "backend": backend_text,
        "surface": surface_payload,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, failures, case_payload


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(list(argv) if argv is not None else sys.argv[1:])
    checks_total = 0
    failures: list[Finding] = []
    static_checks = (
        (args.expectations_doc, "M255-A002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M255-A002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.native_doc, "M255-A002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M255-A002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M255-A002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.ast_header, "M255-A002-AST-EXISTS", AST_SNIPPETS),
        (args.parser, "M255-A002-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.frontend_pipeline, "M255-A002-PIPE-EXISTS", FRONTEND_PIPELINE_SNIPPETS),
        (args.sema, "M255-A002-SEMA-EXISTS", SEMA_SNIPPETS),
        (args.lowering_header, "M255-A002-HDR-EXISTS", LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, "M255-A002-LCPP-EXISTS", LOWERING_CPP_SNIPPETS),
        (args.frontend_artifacts, "M255-A002-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_header, "M255-A002-IRH-EXISTS", IR_HEADER_SNIPPETS),
        (args.ir_cpp, "M255-A002-IRC-EXISTS", IR_CPP_SNIPPETS),
        (args.runtime_shim, "M255-A002-SHIM-EXISTS", RUNTIME_SHIM_SNIPPETS),
        (args.fixture, "M255-A002-FIX-EXISTS", FIXTURE_SNIPPETS),
        (args.readiness_runner, "M255-A002-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.package_json, "M255-A002-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in static_checks:
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    dynamic_cases: list[dict[str, object]] = []
    if not args.skip_dynamic_probes:
        count, findings, case_payload = run_runner_probe(
            runner_exe=args.runner_exe.resolve(),
            fixture_path=args.fixture.resolve(),
            out_dir=args.probe_root.resolve() / "runner_manifest_only",
        )
        checks_total += count
        failures.extend(findings)
        if case_payload is not None:
            dynamic_cases.append(case_payload)

        count, findings, case_payload = run_native_probe(
            native_exe=args.native_exe.resolve(),
            fixture_path=args.fixture.resolve(),
            out_dir=args.probe_root.resolve() / "native_llvm_direct",
        )
        checks_total += count
        failures.extend(findings)
        if case_payload is not None:
            dynamic_cases.append(case_payload)

    checks_passed = checks_total - len(failures)
    payload = {
        "mode": MODE,
        "implementation_contract_id": IMPLEMENTATION_CONTRACT_ID,
        "lowering_contract_id": LOWERING_CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_cases": dynamic_cases,
        "next_implementation_issue": "M255-B001",
        "failures": [failure.__dict__ for failure in failures],
    }

    summary_out = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        print(f"[FAIL] M255-A002 dispatch-site modeling drift; summary: {display_path(summary_out)}", file=sys.stderr)
        return 1
    print(f"[PASS] M255-A002 dispatch-site modeling preserved; summary: {display_path(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
