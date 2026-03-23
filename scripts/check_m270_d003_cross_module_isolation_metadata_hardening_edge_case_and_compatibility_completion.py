#!/usr/bin/env python3
"""Checker for M270-D003 cross-module isolation metadata hardening."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_ID = "objc3c-cross-module-actor-isolation-metadata-hardening/m270-d003-v1"
ACTOR_IMPORT_CONTRACT_ID = "objc3c-part7-actor-mailbox-isolation-import-surface/m270-d003-v1"
ACTOR_IMPORT_SOURCE_CONTRACT_ID = "objc3c-part7-actor-lowering-and-metadata-contract/m270-c001-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m270" / "M270-D003" / "cross_module_actor_isolation_metadata_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m270_cross_module_isolation_metadata_hardening_edge_case_and_compatibility_completion_d003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m270" / "m270_d003_cross_module_isolation_metadata_hardening_edge_case_and_compatibility_completion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
CONFORMANCE_SPEC = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
PROCESS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
RUNTIME_INTERNAL_H = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PACKAGE_JSON = ROOT / "package.json"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_d003_cross_module_actor_isolation_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m270_d003_cross_module_actor_isolation_consumer.objc3"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m270" / "d003"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"
LINK_PLAN_ARTIFACT = "module.cross-module-runtime-link-plan.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    check_id: str
    path: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M270-D003-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M270-D003-EXP-02", "module.cross-module-runtime-link-plan.json"),
        SnippetCheck("M270-D003-EXP-03", "M270-E001"),
    ),
    PACKET_DOC: (
        SnippetCheck("M270-D003-PKT-01", "Packet: `M270-D003`"),
        SnippetCheck("M270-D003-PKT-02", "Issue: `#7317`"),
        SnippetCheck("M270-D003-PKT-03", "`M270-D002`"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M270-D003-DOCSRC-01", "M270 live actor mailbox and isolation runtime"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M270-D003-DOC-01", "M270 live actor mailbox and isolation runtime"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M270-D003-ABS-01", "M270-D002 live-runtime note:"),
    ),
    CONFORMANCE_SPEC: (
        SnippetCheck("M270-D003-CONF-01", "M270-D002 live-runtime note:"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M270-D003-ARCH-01", "M270 Part 7 Live Actor Mailbox And Isolation Runtime (D002)"),
    ),
    RUNTIME_README: (
        SnippetCheck("M270-D003-RTR-01", "M270 live actor mailbox runtime probe"),
    ),
    PROCESS_H: (
        SnippetCheck("M270-D003-PH-01", "expected_part7_actor_contract_id"),
        SnippetCheck("M270-D003-PH-02", "part7_actor_mailbox_runtime_import_present"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M270-D003-PC-01", "M270-D003 actor cross-module isolation-metadata hardening anchor:"),
        SnippetCheck("M270-D003-PC-02", "cross-module runtime link-plan Part 7 actor replay surface incomplete for"),
    ),
    IMPORT_SURFACE_H: (
        SnippetCheck("M270-D003-IMH-01", "part7_actor_mailbox_runtime_import_present"),
        SnippetCheck("M270-D003-IMH-02", "part7_actor_isolation_lowering_replay_key"),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M270-D003-IMC-01", "PopulateImportedPart7ActorMailboxRuntimeImport"),
        SnippetCheck("M270-D003-IMC-02", "unexpected Part 7 actor mailbox runtime import contract id in import surface"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M270-D003-DRV-01", "expected_part7_actor_contract_id"),
        SnippetCheck("M270-D003-DRV-02", "part7_actor_mailbox_runtime_import_present"),
    ),
    RUNTIME_INTERNAL_H: (
        SnippetCheck("M270-D003-RIH-01", "M270-D003 cross-module isolation-metadata hardening anchor:"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M270-D003-RC-01", "M270-D003 cross-module isolation-metadata hardening anchor:"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M270-D003-PKG-01", '"check:objc3c:m270-d003-cross-module-isolation-metadata-hardening-edge-case-and-compatibility-completion"'),
        SnippetCheck("M270-D003-PKG-02", '"check:objc3c:m270-d003-lane-d-readiness"'),
    ),
    PROVIDER_FIXTURE: (
        SnippetCheck("M270-D003-PROV-01", "actor_mailbox_enqueue"),
        SnippetCheck("M270-D003-PROV-02", "actor_mailbox_drain_next"),
    ),
    CONSUMER_FIXTURE: (
        SnippetCheck("M270-D003-CONS-01", "localConsumer"),
    ),
}


def display_path(path: Path) -> str:
    return path.resolve().relative_to(ROOT).as_posix()


def require(condition: bool, path: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if not condition:
        failures.append(Finding(check_id, path, detail))
    return 1


def run(cmd: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=ROOT, text=True, capture_output=True, check=False)


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def check_static(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    text = path.read_text(encoding="utf-8")
    checks = 0
    for snippet in snippets:
        checks += require(snippet.snippet in text, display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}", failures)
    return checks, failures


def ensure_build(failures: list[Finding]) -> int:
    completed = run([sys.executable, str(ROOT / "scripts" / "ensure_objc3c_native_build.py"), "--mode", "fast"])
    return require(completed.returncode == 0, display_path(ROOT / "scripts" / "ensure_objc3c_native_build.py"), "M270-D003-BUILD", completed.stderr or completed.stdout or "fast native build failed", failures)


def compile_fixture(fixture: Path, out_dir: Path, ordinal: int, import_surface: Path | None = None) -> subprocess.CompletedProcess[str]:
    cmd = [
        str(NATIVE_EXE),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--objc3-bootstrap-registration-order-ordinal",
        str(ordinal),
    ]
    if import_surface is not None:
        cmd.extend(["--objc3-import-runtime-surface", str(import_surface)])
    return run(cmd)


def validate_dynamic(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks = 0
    provider_out = PROBE_ROOT / "provider"
    consumer_out = PROBE_ROOT / "consumer"
    tampered_out = PROBE_ROOT / "tampered"
    provider = compile_fixture(PROVIDER_FIXTURE, provider_out, 1)
    checks += require(provider.returncode == 0, display_path(PROVIDER_FIXTURE), "M270-D003-PROV-COMPILE", provider.stderr or provider.stdout or "provider compile failed", failures)
    provider_import = provider_out / IMPORT_ARTIFACT
    checks += require(provider_import.exists(), display_path(provider_import), "M270-D003-PROV-IMPORT", "provider import artifact missing", failures)
    if failures:
        return checks, {}

    provider_import_json = load_json(provider_import)
    actor_packet = provider_import_json.get("objc_part7_actor_mailbox_and_isolation_runtime_import_surface")
    checks += require(isinstance(actor_packet, dict), display_path(provider_import), "M270-D003-PROV-ACTOR-PACKET", "provider actor import packet missing", failures)
    if isinstance(actor_packet, dict):
        checks += require(actor_packet.get("contract_id") == ACTOR_IMPORT_CONTRACT_ID, display_path(provider_import), "M270-D003-PROV-CONTRACT", "provider actor import contract mismatch", failures)
        checks += require(actor_packet.get("source_contract_id") == ACTOR_IMPORT_SOURCE_CONTRACT_ID, display_path(provider_import), "M270-D003-PROV-SOURCE", "provider actor import source contract mismatch", failures)
        checks += require(actor_packet.get("actor_mailbox_runtime_ready") is True, display_path(provider_import), "M270-D003-PROV-READY", "provider actor import readiness missing", failures)
        checks += require(bool(actor_packet.get("actor_lowering_replay_key")), display_path(provider_import), "M270-D003-PROV-LOWERING-KEY", "provider actor lowering replay key missing", failures)
        checks += require(bool(actor_packet.get("actor_isolation_lowering_replay_key")), display_path(provider_import), "M270-D003-PROV-ISOLATION-KEY", "provider actor isolation replay key missing", failures)
    if failures:
        return checks, {}

    consumer = compile_fixture(CONSUMER_FIXTURE, consumer_out, 2, provider_import)
    checks += require(consumer.returncode == 0, display_path(CONSUMER_FIXTURE), "M270-D003-CONS-COMPILE", consumer.stderr or consumer.stdout or "consumer compile failed", failures)
    plan_path = consumer_out / LINK_PLAN_ARTIFACT
    checks += require(plan_path.exists(), display_path(plan_path), "M270-D003-CONS-PLAN", "consumer link plan missing", failures)
    if failures:
        return checks, {}

    plan = load_json(plan_path)
    checks += require(plan.get("expected_part7_actor_contract_id") == ACTOR_IMPORT_CONTRACT_ID, display_path(plan_path), "M270-D003-PLAN-CONTRACT", "expected Part 7 actor contract mismatch", failures)
    checks += require(plan.get("expected_part7_actor_source_contract_id") == ACTOR_IMPORT_SOURCE_CONTRACT_ID, display_path(plan_path), "M270-D003-PLAN-SOURCE", "expected Part 7 actor source contract mismatch", failures)
    checks += require(plan.get("part7_actor_imported_module_count") == 1, display_path(plan_path), "M270-D003-PLAN-COUNT", "expected one imported Part 7 actor module", failures)
    checks += require(plan.get("part7_actor_cross_module_isolation_ready") is True, display_path(plan_path), "M270-D003-PLAN-READY", "actor cross-module isolation should be ready", failures)
    imported_modules = plan.get("imported_modules")
    checks += require(isinstance(imported_modules, list) and len(imported_modules) == 1, display_path(plan_path), "M270-D003-PLAN-IMPORTED", "unexpected imported module payload", failures)
    if isinstance(imported_modules, list) and imported_modules:
        imported = imported_modules[0]
        checks += require(imported.get("part7_actor_mailbox_runtime_import_present") is True, display_path(plan_path), "M270-D003-PLAN-PRESENT", "imported actor packet missing from link plan", failures)
        checks += require(imported.get("part7_actor_mailbox_runtime_ready") is True, display_path(plan_path), "M270-D003-PLAN-IMPORTED-READY", "imported actor packet readiness missing", failures)
        checks += require(imported.get("part7_actor_contract_id") == ACTOR_IMPORT_CONTRACT_ID, display_path(plan_path), "M270-D003-PLAN-IMPORTED-CONTRACT", "imported actor contract mismatch", failures)
        checks += require(imported.get("part7_actor_source_contract_id") == ACTOR_IMPORT_SOURCE_CONTRACT_ID, display_path(plan_path), "M270-D003-PLAN-IMPORTED-SOURCE", "imported actor source contract mismatch", failures)
        checks += require(imported.get("part7_actor_mailbox_runtime_replay_key") == actor_packet.get("replay_key"), display_path(plan_path), "M270-D003-PLAN-IMPORTED-REPLAY", "imported actor replay key mismatch", failures)
        checks += require(imported.get("part7_actor_lowering_replay_key") == actor_packet.get("actor_lowering_replay_key"), display_path(plan_path), "M270-D003-PLAN-IMPORTED-LOWERING", "imported actor lowering replay key mismatch", failures)
        checks += require(imported.get("part7_actor_isolation_lowering_replay_key") == actor_packet.get("actor_isolation_lowering_replay_key"), display_path(plan_path), "M270-D003-PLAN-IMPORTED-ISOLATION", "imported actor isolation replay key mismatch", failures)

    tampered_import = tampered_out / IMPORT_ARTIFACT
    tampered_out.mkdir(parents=True, exist_ok=True)
    tampered_payload = dict(provider_import_json)
    tampered_actor_packet = dict(actor_packet)
    tampered_actor_packet["contract_id"] = "objc3c-part7-actor-mailbox-isolation-import-surface/m270-d003-bad"
    tampered_payload["objc_part7_actor_mailbox_and_isolation_runtime_import_surface"] = tampered_actor_packet
    tampered_import.write_text(json.dumps(tampered_payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    tampered = compile_fixture(CONSUMER_FIXTURE, tampered_out, 2, tampered_import)
    tampered_text = (tampered.stdout or "") + (tampered.stderr or "")
    checks += require(tampered.returncode != 0, display_path(tampered_import), "M270-D003-TAMPERED-EXIT", "tampered import surface unexpectedly compiled", failures)
    checks += require("actor mailbox runtime import contract id" in tampered_text or (tampered_out / "module.diagnostics.txt").exists(), display_path(tampered_import), "M270-D003-TAMPERED-DIAG", "tampered import surface did not fail closed with actor import diagnostics", failures)

    return checks, {
        "provider_import_artifact": display_path(provider_import),
        "consumer_link_plan_artifact": display_path(plan_path),
        "tampered_import_artifact": display_path(tampered_import),
    }


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    args = parser.parse_args(argv)

    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    for path, snippets in STATIC_SNIPPETS.items():
        count, static_failures = check_static(path, snippets)
        checks_total += count
        checks_passed += count - len(static_failures)
        failures.extend(static_failures)

    dynamic: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        build_checks = ensure_build(failures)
        checks_total += build_checks
        checks_passed += build_checks - len([f for f in failures if f.check_id == "M270-D003-BUILD"])
        if not failures:
            dynamic_checks, dynamic = validate_dynamic(failures)
            checks_total += dynamic_checks
            checks_passed += dynamic_checks - len([f for f in failures if f.check_id.startswith("M270-D003-") and f.check_id not in {s.check_id for snippets in STATIC_SNIPPETS.values() for s in snippets} and f.check_id != "M270-D003-BUILD"])

    ok = not failures
    summary = {
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [f.__dict__ for f in failures],
        "dynamic": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2, sort_keys=True))
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
