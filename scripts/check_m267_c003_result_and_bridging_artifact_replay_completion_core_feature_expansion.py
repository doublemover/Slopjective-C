#!/usr/bin/env python3
"""Checker for M267-C003 result and bridging artifact replay completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m267-c003-result-and-bridging-artifact-replay-completion-core-feature-expansion-v1"
CONTRACT_ID = "objc3c-part6-result-and-bridging-artifact-replay/m267-c003-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part6_result_and_bridging_artifact_replay"
SIDECAR_NAME = "module.part6-error-replay.json"
IMPORT_ARTIFACT_NAME = "module.runtime-import-surface.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m267" / "M267-C003" / "result_and_bridging_artifact_replay_completion_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "c003-result-and-bridging-artifact-replay"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_result_and_bridging_artifact_replay_completion_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_c003_result_and_bridging_artifact_replay_completion_core_feature_expansion_packet.md"
DOC_NATIVE_SRC = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_LOWER = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
MANIFEST_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m267_c003_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m267_c003_result_and_bridging_artifact_replay_completion_core_feature_expansion.py"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_c003_part6_artifact_replay_producer.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_c003_result_bridge_consumer.objc3"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M267-C003-EXP-01", "# M267 Result And Bridging Artifact Replay Completion Core Feature Expansion Expectations (C003)"),
        SnippetCheck("M267-C003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M267-C003-EXP-03", f"`{SIDECAR_NAME}`"),
        SnippetCheck("M267-C003-EXP-04", "Issue: `#7276`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M267-C003-PKT-01", "# M267-C003 Result And Bridging Artifact Replay Completion Core Feature Expansion Packet"),
        SnippetCheck("M267-C003-PKT-02", "Issue: `#7276`"),
        SnippetCheck("M267-C003-PKT-03", "Dependencies: `M267-C002`, `M258-A002`"),
        SnippetCheck("M267-C003-PKT-04", "Next issue: `M267-D001`"),
    ),
    DOC_NATIVE_SRC: (
        SnippetCheck("M267-C003-DSRC-01", "## M267 Part 6 result and bridging artifact replay completion (M267-C003)"),
        SnippetCheck("M267-C003-DSRC-02", f"`{CONTRACT_ID}`"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M267-C003-DNAT-01", "## M267 Part 6 result and bridging artifact replay completion (M267-C003)"),
        SnippetCheck("M267-C003-DNAT-02", f"`{SIDECAR_NAME}`"),
    ),
    SPEC_AM: (
        SnippetCheck("M267-C003-SAM-01", "M267-C003 replay completion note:"),
        SnippetCheck("M267-C003-SAM-02", "`!objc3.objc_part6_result_and_bridging_artifact_replay = !{!88}`"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M267-C003-SAT-01", "Current implementation status (`M267-C003`):"),
        SnippetCheck("M267-C003-SAT-02", "`module.part6-error-replay.json`"),
    ),
    SPEC_LOWER: (
        SnippetCheck("M267-C003-SLOW-01", "## M267 Part 6 result and bridging artifact replay completion (C003)"),
        SnippetCheck("M267-C003-SLOW-02", f"`{SURFACE_PATH}`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M267-C003-ARCH-01", "## M267 Part 6 replay artifact chain"),
        SnippetCheck("M267-C003-ARCH-02", "`module.part6-error-replay.json`"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M267-C003-LH-01", CONTRACT_ID),
        SnippetCheck("M267-C003-LH-02", ".part6-error-replay.json"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M267-C003-LCPP-01", "// M267-C003 Part 6 replay-completion anchor"),
        SnippetCheck("M267-C003-LCPP-02", ";next_issue=M267-D001"),
    ),
    FRONTEND_ARTIFACTS_H: (
        SnippetCheck("M267-C003-FAH-01", "part6_result_bridge_artifact_replay_json"),
    ),
    FRONTEND_ARTIFACTS_CPP: (
        SnippetCheck("M267-C003-FAC-01", "BuildPart6ResultAndBridgingArtifactReplaySummary("),
        SnippetCheck("M267-C003-FAC-02", "objc_part6_result_and_bridging_artifact_replay"),
        SnippetCheck("M267-C003-FAC-03", "lowering_part6_result_and_bridging_artifact_replay"),
    ),
    IMPORT_SURFACE_H: (
        SnippetCheck("M267-C003-IMPH-01", "part6_result_and_bridging_artifact_replay_present"),
        SnippetCheck("M267-C003-IMPH-02", "part6_part6_replay_key"),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M267-C003-IMPC-01", "PopulateImportedPart6ResultAndBridgingArtifactReplay("),
        SnippetCheck("M267-C003-IMPC-02", "unexpected Part 6 result/bridging artifact replay contract id in import surface"),
    ),
    MANIFEST_ARTIFACTS_H: (
        SnippetCheck("M267-C003-MAH-01", "BuildPart6ResultBridgeArtifactReplayPath("),
        SnippetCheck("M267-C003-MAH-02", "WritePart6ResultBridgeArtifactReplay("),
    ),
    MANIFEST_ARTIFACTS_CPP: (
        SnippetCheck("M267-C003-MAC-01", "BuildPart6ResultBridgeArtifactReplayPath("),
        SnippetCheck("M267-C003-MAC-02", "WritePart6ResultBridgeArtifactReplay("),
    ),
    DRIVER_CPP: (
        SnippetCheck("M267-C003-DRV-01", "WritePart6ResultBridgeArtifactReplay("),
        SnippetCheck("M267-C003-DRV-02", "has_runtime_import_artifact"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M267-C003-ANCHOR-01", "BuildPart6ResultBridgeArtifactReplayPath("),
        SnippetCheck("M267-C003-ANCHOR-02", "has_runtime_import_artifact"),
    ),
    IR_EMITTER_H: (
        SnippetCheck("M267-C003-IRH-01", "lowering_part6_result_and_bridging_artifact_replay_key"),
        SnippetCheck("M267-C003-IRH-02", "imported_part6_result_and_bridging_artifact_modules"),
    ),
    IR_EMITTER_CPP: (
        SnippetCheck("M267-C003-IRC-01", "; part6_result_and_bridging_artifact_replay = "),
        SnippetCheck("M267-C003-IRC-02", "!objc3.objc_part6_result_and_bridging_artifact_replay = !{!88}"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M267-C003-PKG-01", '"check:objc3c:m267-c003-result-and-bridging-artifact-replay-completion-core-feature-expansion"'),
        SnippetCheck("M267-C003-PKG-02", '"test:tooling:m267-c003-result-and-bridging-artifact-replay-completion-core-feature-expansion"'),
        SnippetCheck("M267-C003-PKG-03", '"check:objc3c:m267-c003-lane-c-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M267-C003-RUN-01", "M267-B002 + M267-B003 + M267-C002 + M267-C003"),
        SnippetCheck("M267-C003-RUN-02", "check_m267_c003_result_and_bridging_artifact_replay_completion_core_feature_expansion.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M267-C003-TEST-01", "def test_checker_passes_static(tmp_path: Path) -> None:"),
        SnippetCheck("M267-C003-TEST-02", "def test_checker_passes_dynamic(tmp_path: Path) -> None:"),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, failures
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, failures


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def ensure_binaries(failures: list[Finding]) -> int:
    build = run_process(
        [
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m267-c003-check",
            "--summary-out",
            "tmp/reports/m267/M267-C003/ensure_objc3c_native_build_summary.json",
        ]
    )
    return require(build.returncode == 0, display_path(BUILD_HELPER), "M267-C003-BUILD", build.stderr or build.stdout or "native build failed", failures)


def compile_fixture(
    *,
    fixture: Path,
    out_dir: Path,
    registration_order_ordinal: int,
    extra_args: Sequence[str] = (),
) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
            "--objc3-bootstrap-registration-order-ordinal",
            str(registration_order_ordinal),
            *extra_args,
        ]
    )


def semantic_surface(manifest: dict[str, Any]) -> dict[str, Any]:
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    if not isinstance(surface, dict):
        raise TypeError("manifest missing frontend.pipeline.semantic_surface")
    return surface


def validate_provider(
    *,
    out_dir: Path,
    failures: list[Finding],
) -> tuple[int, dict[str, Any], dict[str, Any], dict[str, Any], dict[str, Any]]:
    checks_total = 0
    manifest_path = out_dir / "module.manifest.json"
    import_path = out_dir / IMPORT_ARTIFACT_NAME
    sidecar_path = out_dir / SIDECAR_NAME
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"

    for check_id, path, detail in (
        ("M267-C003-PROV-MANIFEST", manifest_path, "provider manifest missing"),
        ("M267-C003-PROV-IMPORT", import_path, "provider import artifact missing"),
        ("M267-C003-PROV-SIDECAR", sidecar_path, "provider sidecar missing"),
        ("M267-C003-PROV-IR", ir_path, "provider IR missing"),
        ("M267-C003-PROV-OBJ", obj_path, "provider object missing"),
        ("M267-C003-PROV-BACKEND", backend_path, "provider backend marker missing"),
    ):
        checks_total += require(path.exists(), display_path(path), check_id, detail, failures)

    if failures:
        return checks_total, {}, {}, {}, {}

    manifest = load_json(manifest_path)
    import_payload = load_json(import_path)
    sidecar_payload = load_json(sidecar_path)
    surface = semantic_surface(manifest)
    replay_surface = surface.get("objc_part6_result_and_bridging_artifact_replay")
    closure_surface = surface.get("objc_runtime_aware_import_module_frontend_closure")
    lowering = manifest.get("lowering_part6_result_and_bridging_artifact_replay")

    checks_total += require(isinstance(replay_surface, dict), display_path(manifest_path), "M267-C003-PROV-SURFACE", "provider semantic replay surface missing", failures)
    checks_total += require(isinstance(closure_surface, dict), display_path(manifest_path), "M267-C003-PROV-CLOSURE", "provider runtime import closure missing", failures)
    checks_total += require(isinstance(lowering, dict), display_path(manifest_path), "M267-C003-PROV-LOWER", "provider lowering packet missing", failures)

    if isinstance(replay_surface, dict):
        checks_total += require(replay_surface.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M267-C003-PROV-CONTRACT", "provider replay contract id mismatch", failures)
        checks_total += require(replay_surface.get("runtime_import_artifact_ready") is True, display_path(manifest_path), "M267-C003-PROV-IMPORT-READY", "provider must publish runtime import artifact readiness", failures)
        checks_total += require(replay_surface.get("separate_compilation_replay_ready") is True, display_path(manifest_path), "M267-C003-PROV-SEPARATE-READY", "provider should truthfully publish replay readiness once import artifact is emitted", failures)
    if isinstance(closure_surface, dict):
        checks_total += require(closure_surface.get("ready") is True, display_path(manifest_path), "M267-C003-PROV-CLOSURE-READY", "provider import closure must be ready", failures)
    if isinstance(lowering, dict) and isinstance(replay_surface, dict):
        checks_total += require(lowering.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M267-C003-PROV-LOWER-CONTRACT", "provider lowering contract mismatch", failures)
        checks_total += require(lowering.get("replay_key") == replay_surface.get("replay_key"), display_path(manifest_path), "M267-C003-PROV-LOWER-REPLAY", "provider lowering replay key must match semantic replay key", failures)

    checks_total += require(import_payload.get("module_name") == manifest.get("module"), display_path(import_path), "M267-C003-PROV-IMPORT-MODULE", "provider import payload module mismatch", failures)
    provider_import_replay = import_payload.get("objc_part6_result_and_bridging_artifact_replay")
    checks_total += require(isinstance(provider_import_replay, dict), display_path(import_path), "M267-C003-PROV-IMPORT-REPLAY", "provider import payload missing nested replay packet", failures)
    if isinstance(provider_import_replay, dict) and isinstance(replay_surface, dict):
        checks_total += require(provider_import_replay.get("contract_id") == CONTRACT_ID, display_path(import_path), "M267-C003-PROV-IMPORT-CONTRACT", "provider import replay contract mismatch", failures)
        checks_total += require(provider_import_replay.get("replay_key") == replay_surface.get("replay_key"), display_path(import_path), "M267-C003-PROV-IMPORT-REPLAYKEY", "provider import replay key mismatch", failures)

    checks_total += require(sidecar_payload.get("contract_id") == CONTRACT_ID, display_path(sidecar_path), "M267-C003-PROV-SIDECAR-CONTRACT", "provider sidecar contract mismatch", failures)
    if isinstance(replay_surface, dict):
        checks_total += require(sidecar_payload.get("replay_key") == replay_surface.get("replay_key"), display_path(sidecar_path), "M267-C003-PROV-SIDECAR-REPLAY", "provider sidecar replay key mismatch", failures)

    backend = backend_path.read_text(encoding="utf-8").strip()
    checks_total += require(backend == "llvm-direct", display_path(backend_path), "M267-C003-PROV-BACKEND-TEXT", f"unexpected provider backend: {backend}", failures)

    ir_text = ir_path.read_text(encoding="utf-8")
    checks_total += require("; part6_result_and_bridging_artifact_replay = " in ir_text, display_path(ir_path), "M267-C003-PROV-IR-COMMENT", "provider IR comment anchor missing", failures)
    checks_total += require("!objc3.objc_part6_result_and_bridging_artifact_replay = !{!88}" in ir_text, display_path(ir_path), "M267-C003-PROV-IR-META", "provider IR metadata anchor missing", failures)

    return checks_total, manifest, import_payload, sidecar_payload, replay_surface if isinstance(replay_surface, dict) else {}


def validate_consumer(
    *,
    out_dir: Path,
    provider_manifest: dict[str, Any],
    failures: list[Finding],
) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    manifest_path = out_dir / "module.manifest.json"
    import_path = out_dir / IMPORT_ARTIFACT_NAME
    sidecar_path = out_dir / SIDECAR_NAME
    ir_path = out_dir / "module.ll"
    backend_path = out_dir / "module.object-backend.txt"

    for check_id, path, detail in (
        ("M267-C003-CONS-MANIFEST", manifest_path, "consumer manifest missing"),
        ("M267-C003-CONS-IMPORT", import_path, "consumer import artifact missing"),
        ("M267-C003-CONS-SIDECAR", sidecar_path, "consumer sidecar missing"),
        ("M267-C003-CONS-IR", ir_path, "consumer IR missing"),
        ("M267-C003-CONS-BACKEND", backend_path, "consumer backend marker missing"),
    ):
        checks_total += require(path.exists(), display_path(path), check_id, detail, failures)

    if failures:
        return checks_total, {}

    manifest = load_json(manifest_path)
    import_payload = load_json(import_path)
    sidecar_payload = load_json(sidecar_path)
    surface = semantic_surface(manifest)
    replay_surface = surface.get("objc_part6_result_and_bridging_artifact_replay")
    checks_total += require(isinstance(replay_surface, dict), display_path(manifest_path), "M267-C003-CONS-SURFACE", "consumer semantic replay surface missing", failures)
    if not isinstance(replay_surface, dict):
        return checks_total, {}

    provider_module = provider_manifest.get("module")
    provider_surface = semantic_surface(provider_manifest).get("objc_part6_result_and_bridging_artifact_replay")
    provider_part6_replay_key = provider_surface.get("part6_replay_key") if isinstance(provider_surface, dict) else None
    provider_result_like_key = provider_surface.get("result_like_replay_key") if isinstance(provider_surface, dict) else None
    provider_ns_error_key = provider_surface.get("ns_error_replay_key") if isinstance(provider_surface, dict) else None

    checks_total += require(replay_surface.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M267-C003-CONS-CONTRACT", "consumer replay contract mismatch", failures)
    checks_total += require(replay_surface.get("runtime_import_artifact_ready") is True, display_path(manifest_path), "M267-C003-CONS-IMPORT-READY", "consumer runtime import readiness must be true", failures)
    checks_total += require(replay_surface.get("separate_compilation_replay_ready") is True, display_path(manifest_path), "M267-C003-CONS-SEPARATE-READY", "consumer separate compilation readiness must be true", failures)
    checks_total += require(provider_module in replay_surface.get("imported_module_names_lexicographic", []), display_path(manifest_path), "M267-C003-CONS-IMPORTED-MODULE", "consumer must preserve imported provider module name", failures)
    checks_total += require(provider_part6_replay_key in replay_surface.get("imported_part6_replay_keys_lexicographic", []), display_path(manifest_path), "M267-C003-CONS-IMPORTED-PART6", "consumer must preserve imported provider Part 6 replay key", failures)
    checks_total += require(provider_result_like_key in replay_surface.get("imported_result_like_replay_keys_lexicographic", []), display_path(manifest_path), "M267-C003-CONS-IMPORTED-RESULT", "consumer must preserve imported provider result-like replay key", failures)
    checks_total += require(provider_ns_error_key in replay_surface.get("imported_ns_error_replay_keys_lexicographic", []), display_path(manifest_path), "M267-C003-CONS-IMPORTED-BRIDGE", "consumer must preserve imported provider NSError replay key", failures)

    consumer_import_replay = import_payload.get("objc_part6_result_and_bridging_artifact_replay")
    checks_total += require(isinstance(consumer_import_replay, dict), display_path(import_path), "M267-C003-CONS-IMPORT-REPLAY", "consumer import payload missing nested replay packet", failures)
    if isinstance(consumer_import_replay, dict):
        checks_total += require(consumer_import_replay.get("replay_key") == replay_surface.get("replay_key"), display_path(import_path), "M267-C003-CONS-IMPORT-REPLAYKEY", "consumer import replay key mismatch", failures)
        checks_total += require(provider_module in consumer_import_replay.get("imported_module_names_lexicographic", []), display_path(import_path), "M267-C003-CONS-IMPORT-IMPORTED-MODULE", "consumer import payload must preserve imported provider module name", failures)

    checks_total += require(sidecar_payload.get("replay_key") == replay_surface.get("replay_key"), display_path(sidecar_path), "M267-C003-CONS-SIDECAR-REPLAY", "consumer sidecar replay key mismatch", failures)

    backend = backend_path.read_text(encoding="utf-8").strip()
    checks_total += require(backend == "llvm-direct", display_path(backend_path), "M267-C003-CONS-BACKEND-TEXT", f"unexpected consumer backend: {backend}", failures)

    ir_text = ir_path.read_text(encoding="utf-8")
    checks_total += require("; part6_result_and_bridging_artifact_replay = " in ir_text, display_path(ir_path), "M267-C003-CONS-IR-COMMENT", "consumer IR comment anchor missing", failures)
    checks_total += require("!objc3.objc_part6_result_and_bridging_artifact_replay = !{!88}" in ir_text, display_path(ir_path), "M267-C003-CONS-IR-META", "consumer IR metadata anchor missing", failures)

    return checks_total, replay_surface


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding], int]:
    failures: list[Finding] = []
    checks_total = 0
    static_summary: dict[str, object] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        path_checks, path_failures = check_static_contract(path, snippets)
        checks_total += path_checks
        failures.extend(path_failures)
        static_summary[display_path(path)] = {"checks": path_checks, "ok": not path_failures}

    dynamic_summary: dict[str, object] = {"skipped": skip_dynamic_probes}
    if not skip_dynamic_probes:
        checks_total += ensure_binaries(failures)
        if not failures:
            provider_out = PROBE_ROOT / "provider"
            consumer_out = PROBE_ROOT / "consumer"
            provider_completed = compile_fixture(
                fixture=PROVIDER_FIXTURE,
                out_dir=provider_out,
                registration_order_ordinal=1,
            )
            checks_total += require(provider_completed.returncode == 0, display_path(PROVIDER_FIXTURE), "M267-C003-PROV-COMPILE", provider_completed.stderr or provider_completed.stdout or "provider compile failed", failures)
            if not failures:
                provider_checks, provider_manifest, provider_import, provider_sidecar, provider_replay = validate_provider(
                    out_dir=provider_out,
                    failures=failures,
                )
                checks_total += provider_checks
                if not failures:
                    provider_import_path = provider_out / IMPORT_ARTIFACT_NAME
                    consumer_completed = compile_fixture(
                        fixture=CONSUMER_FIXTURE,
                        out_dir=consumer_out,
                        registration_order_ordinal=2,
                        extra_args=("--objc3-import-runtime-surface", str(provider_import_path)),
                    )
                    checks_total += require(consumer_completed.returncode == 0, display_path(CONSUMER_FIXTURE), "M267-C003-CONS-COMPILE", consumer_completed.stderr or consumer_completed.stdout or "consumer compile failed", failures)
                    if not failures:
                        consumer_checks, consumer_replay = validate_consumer(
                            out_dir=consumer_out,
                            provider_manifest=provider_manifest,
                            failures=failures,
                        )
                        checks_total += consumer_checks
                        dynamic_summary.update(
                            {
                                "provider": {
                                    "module_name": provider_manifest.get("module"),
                                    "replay_surface": provider_replay,
                                    "import_payload_module": provider_import.get("module_name"),
                                    "sidecar_contract_id": provider_sidecar.get("contract_id"),
                                },
                                "consumer": {
                                    "replay_surface": consumer_replay,
                                },
                            }
                        )

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total if not failures else checks_total - len(failures),
        "failures": [failure.__dict__ for failure in failures],
        "static_contract": static_summary,
        "dynamic_probes": dynamic_summary,
    }
    return payload, failures, checks_total


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    payload, failures, _ = build_summary(skip_dynamic_probes=args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        for failure in failures:
            print(f"[fail] {failure.artifact} :: {failure.check_id} :: {failure.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {CONTRACT_ID} validated")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
