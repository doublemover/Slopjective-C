#!/usr/bin/env python3
"""Fail-closed checker for M267-E001 error-model conformance gate."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m267-e001-error-model-conformance-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-error-model-conformance-gate/m267-e001-v1"
EVIDENCE_MODEL = "a002-b003-c003-d003-summary-chain-plus-provider-consumer-proof"
FAILURE_MODEL = "fail-closed-on-error-model-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M267-E002"

A002_CONTRACT_ID = "objc3c-part6-error-bridge-markers/m267-a002-v1"
B003_CONTRACT_ID = "objc3c-part6-error-bridge-legality/m267-b003-v1"
C003_CONTRACT_ID = "objc3c-part6-result-and-bridging-artifact-replay/m267-c003-v1"
D003_CONTRACT_ID = "objc3c-cross-module-error-surface-preservation-hardening/m267-d003-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_error_model_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_e001_error_model_conformance_gate_contract_and_architecture_freeze_packet.md"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m267_e001_lane_e_readiness.py"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_d003_cross_module_error_surface_preservation_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_d003_cross_module_error_surface_preservation_consumer.objc3"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m267" / "M267-E001" / "error_model_conformance_gate_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "e001"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"
REPLAY_ARTIFACT = "module.part6-error-replay.json"
LINK_PLAN_ARTIFACT = "module.cross-module-runtime-link-plan.json"

UPSTREAM_SUMMARIES: tuple[tuple[str, Path, str], ...] = (
    ("M267-A002", ROOT / "tmp" / "reports" / "m267" / "M267-A002" / "error_bridge_marker_surface_summary.json", A002_CONTRACT_ID),
    ("M267-B003", ROOT / "tmp" / "reports" / "m267" / "M267-B003" / "bridging_legality_summary.json", B003_CONTRACT_ID),
    ("M267-C003", ROOT / "tmp" / "reports" / "m267" / "M267-C003" / "result_and_bridging_artifact_replay_completion_summary.json", C003_CONTRACT_ID),
    ("M267-D003", ROOT / "tmp" / "reports" / "m267" / "M267-D003" / "cross_module_error_surface_preservation_summary.json", D003_CONTRACT_ID),
)


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
        SnippetCheck("M267-E001-EXP-01", "# M267 Error-Model Conformance Gate Contract And Architecture Freeze Expectations (E001)"),
        SnippetCheck("M267-E001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M267-E001-EXP-03", "M267-A002"),
        SnippetCheck("M267-E001-EXP-04", "The gate must explicitly hand off to `M267-E002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M267-E001-PKT-01", "# M267-E001 Error-Model Conformance Gate Contract And Architecture Freeze Packet"),
        SnippetCheck("M267-E001-PKT-02", "Issue: `#7280`"),
        SnippetCheck("M267-E001-PKT-03", "- `M267-A002`"),
        SnippetCheck("M267-E001-PKT-04", "- `M267-D003`"),
        SnippetCheck("M267-E001-PKT-05", "Next issue: `M267-E002`"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M267-E001-SRC-01", "## M267 error-model conformance gate (M267-E001)"),
        SnippetCheck("M267-E001-SRC-02", "M267-A002"),
        SnippetCheck("M267-E001-SRC-03", "M267-E002"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M267-E001-NDOC-01", "## M267 error-model conformance gate (M267-E001)"),
        SnippetCheck("M267-E001-NDOC-02", "M267-A002"),
        SnippetCheck("M267-E001-NDOC-03", "M267-E002"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M267-E001-ABS-01", "M267-E001 error-model conformance gate note:"),
        SnippetCheck("M267-E001-ABS-02", "M267-A002"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M267-E001-ATTR-01", "Current implementation status (`M267-E001`):"),
        SnippetCheck("M267-E001-ATTR-02", "M267-B003"),
        SnippetCheck("M267-E001-ATTR-03", "M267-E002"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M267-E001-DRV-01", "M267-E001 error-model conformance gate anchor"),
        SnippetCheck("M267-E001-DRV-02", "canonical integrated proof"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M267-E001-MAN-01", "M267-E001 error-model conformance gate anchor"),
        SnippetCheck("M267-E001-MAN-02", "canonical cross-module link-plan artifact"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M267-E001-FAPI-01", "M267-E001 error-model conformance gate anchor"),
        SnippetCheck("M267-E001-FAPI-02", "same runtime-import surface"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M267-E001-RUN-01", "run_m267_a002_lane_a_readiness.py"),
        SnippetCheck("M267-E001-RUN-02", "check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py"),
        SnippetCheck("M267-E001-RUN-03", "run_m267_c003_lane_c_readiness.py"),
        SnippetCheck("M267-E001-RUN-04", "run_m267_d003_lane_d_readiness.py"),
        SnippetCheck("M267-E001-RUN-05", "test_check_m267_e001_error_model_conformance_gate_contract_and_architecture_freeze.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M267-E001-PKG-01", '"check:objc3c:m267-e001-error-model-conformance-gate"'),
        SnippetCheck("M267-E001-PKG-02", '"test:tooling:m267-e001-error-model-conformance-gate"'),
        SnippetCheck("M267-E001-PKG-03", '"check:objc3c:m267-e001-lane-e-readiness"'),
    ),
}


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_process(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT if cwd is None else cwd, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M267-E001-MISSING", f"required artifact missing: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def semantic_surface_from_manifest(path: Path) -> dict[str, Any]:
    manifest = read_json(path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    surface = pipeline.get("semantic_surface")
    if not isinstance(surface, dict):
        raise TypeError(f"missing semantic surface in {display_path(path)}")
    return surface


def compile_fixture(*, fixture: Path, out_dir: Path, registration_order_ordinal: int, import_surface: Path | None = None) -> subprocess.CompletedProcess[str]:
    args = [
        str(NATIVE_EXE),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--objc3-bootstrap-registration-order-ordinal",
        str(registration_order_ordinal),
    ]
    if import_surface is not None:
        args.extend(["--objc3-import-runtime-surface", str(import_surface)])
    return run_process(args)


def validate_upstream_summaries(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    upstream: dict[str, Any] = {}
    for issue, path, contract_id in UPSTREAM_SUMMARIES:
        artifact = display_path(path)
        checks_total += 1
        checks_passed += require(path.exists(), artifact, f"{issue}-SUM-01", "missing upstream summary", failures)
        if not path.exists():
            upstream[issue] = {"missing": True}
            continue
        payload = read_json(path)
        checks_total += 1
        checks_passed += require(payload.get("contract_id") == contract_id, artifact, f"{issue}-SUM-02", "upstream contract drifted", failures)
        checks_total += 1
        checks_passed += require(payload.get("checks_total", 0) == payload.get("checks_passed", -1), artifact, f"{issue}-SUM-03", "upstream summary lost full coverage", failures)
        checks_total += 1
        checks_passed += require(payload.get("checks_total", 0) > 0, artifact, f"{issue}-SUM-04", "upstream summary reports zero checks", failures)
        upstream[issue] = {
            "contract_id": payload.get("contract_id"),
            "checks_total": payload.get("checks_total"),
            "checks_passed": payload.get("checks_passed"),
        }
    return checks_total, checks_passed, upstream


def validate_happy_path(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    provider_out = PROBE_ROOT / "provider"
    consumer_out = PROBE_ROOT / "consumer"
    provider_out.mkdir(parents=True, exist_ok=True)
    consumer_out.mkdir(parents=True, exist_ok=True)

    provider = compile_fixture(fixture=PROVIDER_FIXTURE, out_dir=provider_out, registration_order_ordinal=1)
    checks_total += 1
    checks_passed += require(provider.returncode == 0, display_path(PROVIDER_FIXTURE), "M267-E001-DYN-01", provider.stderr or provider.stdout or "provider compile failed", failures)

    provider_import = provider_out / IMPORT_ARTIFACT
    provider_replay = provider_out / REPLAY_ARTIFACT
    checks_total += 1
    checks_passed += require(provider_import.exists(), display_path(provider_import), "M267-E001-DYN-02", "provider runtime-import artifact missing", failures)
    checks_total += 1
    checks_passed += require(provider_replay.exists(), display_path(provider_replay), "M267-E001-DYN-03", "provider replay artifact missing", failures)

    replay_payload: dict[str, Any] = {}
    if provider_replay.exists():
        replay_payload = read_json(provider_replay)
        checks_total += 1
        checks_passed += require(replay_payload.get("contract_id") == C003_CONTRACT_ID, display_path(provider_replay), "M267-E001-DYN-04", "provider replay contract mismatch", failures)
        checks_total += 1
        checks_passed += require(replay_payload.get("binary_artifact_replay_ready") is True, display_path(provider_replay), "M267-E001-DYN-05", "binary replay readiness missing", failures)
        checks_total += 1
        checks_passed += require(replay_payload.get("separate_compilation_replay_ready") is True, display_path(provider_replay), "M267-E001-DYN-06", "separate-compilation replay readiness missing", failures)
        checks_total += 1
        checks_passed += require("part6_bridge_legality_contract=objc3c-part6-error-bridge-legality/m267-b003-v1" in str(replay_payload.get("part6_replay_key", "")), display_path(provider_replay), "M267-E001-DYN-07", "bridge-legality replay key missing", failures)

    consumer = compile_fixture(fixture=CONSUMER_FIXTURE, out_dir=consumer_out, registration_order_ordinal=2, import_surface=provider_import)
    checks_total += 1
    checks_passed += require(consumer.returncode == 0, display_path(CONSUMER_FIXTURE), "M267-E001-DYN-08", consumer.stderr or consumer.stdout or "consumer compile failed", failures)

    consumer_plan = consumer_out / LINK_PLAN_ARTIFACT
    checks_total += 1
    checks_passed += require(consumer_plan.exists(), display_path(consumer_plan), "M267-E001-DYN-09", "consumer cross-module link plan missing", failures)

    consumer_plan_payload: dict[str, Any] = {}
    if consumer_plan.exists():
        consumer_plan_payload = read_json(consumer_plan)
        checks_total += 1
        checks_passed += require(consumer_plan_payload.get("part6_cross_module_preservation_ready") is True, display_path(consumer_plan), "M267-E001-DYN-10", "cross-module preservation bit missing", failures)
        checks_total += 1
        checks_passed += require(consumer_plan_payload.get("part6_imported_module_count") == 1, display_path(consumer_plan), "M267-E001-DYN-11", "unexpected imported module count", failures)

    return checks_total, checks_passed, {
        "provider_import_path": display_path(provider_import),
        "provider_replay_path": display_path(provider_replay),
        "consumer_plan_path": display_path(consumer_plan),
        "provider_replay": {
            "contract_id": replay_payload.get("contract_id"),
            "binary_artifact_replay_ready": replay_payload.get("binary_artifact_replay_ready"),
            "separate_compilation_replay_ready": replay_payload.get("separate_compilation_replay_ready"),
            "part6_replay_key": replay_payload.get("part6_replay_key"),
        },
        "consumer_plan": {
            "part6_cross_module_preservation_ready": consumer_plan_payload.get("part6_cross_module_preservation_ready"),
            "part6_imported_module_count": consumer_plan_payload.get("part6_imported_module_count"),
        },
    }


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_summary: dict[str, Any] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        total = len(snippets)
        passed = ensure_snippets(path, snippets, failures)
        checks_total += total
        checks_passed += passed
        static_summary[display_path(path)] = {"checks": total, "ok": passed == total}

    upstream_total, upstream_passed, upstream_summary = validate_upstream_summaries(failures)
    checks_total += upstream_total
    checks_passed += upstream_passed

    dynamic: dict[str, Any] = {"skipped": skip_dynamic_probes}
    if not skip_dynamic_probes:
        build = run_process([sys.executable, str(BUILD_HELPER), "--mode", "fast", "--reason", "m267-e001-error-model-conformance-gate", "--summary-out", str(ROOT / "tmp" / "reports" / "m267" / "M267-E001" / "ensure_objc3c_native_build_summary.json")])
        checks_total += 1
        checks_passed += require(build.returncode == 0, display_path(BUILD_HELPER), "M267-E001-BUILD-01", build.stderr or build.stdout or "fast native build failed", failures)
        checks_total += 1
        checks_passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M267-E001-BUILD-02", "native executable missing after build", failures)
        if not failures:
            dyn_total, dyn_passed, dyn_payload = validate_happy_path(failures)
            checks_total += dyn_total
            checks_passed += dyn_passed
            dynamic = {"skipped": False, "happy_path": dyn_payload}

    summary = {
        "issue": "M267-E001",
        "contract_id": CONTRACT_ID,
        "mode": MODE,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "ok": not failures,
        "dynamic_probes_executed": not skip_dynamic_probes,
        "static": static_summary,
        "upstream": upstream_summary,
        "dynamic": dynamic,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }
    return summary, failures


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    summary, failures = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
