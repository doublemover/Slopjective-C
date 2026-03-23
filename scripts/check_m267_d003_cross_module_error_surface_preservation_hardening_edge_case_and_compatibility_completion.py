from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_cross_module_error_surface_preservation_hardening_edge_case_and_compatibility_completion_d003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_d003_cross_module_error_surface_preservation_hardening_edge_case_and_compatibility_completion_packet.md"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
PACKAGE_JSON = ROOT / "package.json"
PROCESS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
RUNTIME_INTERNAL_H = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_d003_cross_module_error_surface_preservation_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m267_d003_cross_module_error_surface_preservation_consumer.objc3"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m267" / "d003"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m267" / "M267-D003" / "cross_module_error_surface_preservation_summary.json"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"
SIDECAR_ARTIFACT = "module.part6-error-replay.json"
LINK_PLAN_ARTIFACT = "module.cross-module-runtime-link-plan.json"
CONTRACT_ID = "objc3c-cross-module-error-surface-preservation-hardening/m267-d003-v1"
PART6_CONTRACT_ID = "objc3c-part6-result-and-bridging-artifact-replay/m267-c003-v1"
PART6_SOURCE_CONTRACT_ID = "objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1"
LINK_PLAN_CONTRACT_ID = "objc3c-cross-module-runtime-packaging-link-plan/m258-d002-v1"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    check_id: str
    path: str
    detail: str


def display_path(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def require(condition: bool, path: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if not condition:
        failures.append(Finding(check_id, path, detail))
    return 1


def run(cmd: list[str], *, cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=cwd or ROOT, text=True, capture_output=True)


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def semantic_surface(manifest: dict[str, Any]) -> dict[str, Any]:
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    if not isinstance(surface, dict):
        raise TypeError("manifest missing frontend.pipeline.semantic_surface")
    return surface


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M267-D003-DOC-01", "# M267 Cross-Module Error-Surface Preservation Hardening Edge-Case And Compatibility Completion Expectations (D003)"),
        SnippetCheck("M267-D003-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M267-D003-DOC-03", "`module.cross-module-runtime-link-plan.json`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M267-D003-PKT-01", "# M267-D003 Cross-Module Error-Surface Preservation Hardening Edge-Case And Compatibility Completion Packet"),
        SnippetCheck("M267-D003-PKT-02", "`M267-D002`"),
        SnippetCheck("M267-D003-PKT-03", "`M267-C003`"),
        SnippetCheck("M267-D003-PKT-04", "`M267-E001`"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M267-D003-NDOCSRC-01", "## M267 Part 6 cross-module error-surface preservation hardening (M267-D003)"),
        SnippetCheck("M267-D003-NDOCSRC-02", "part6_cross_module_preservation_ready"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M267-D003-NDOC-01", "## M267 Part 6 cross-module error-surface preservation hardening (M267-D003)"),
        SnippetCheck("M267-D003-NDOC-02", "part6_imported_module_names_lexicographic"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M267-D003-ABS-01", "M267-D003 cross-module preservation note:"),
        SnippetCheck("M267-D003-ABS-02", "cross-module link-plan construction fail-closes"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M267-D003-ATTR-01", "Current implementation status (`M267-D003`):"),
        SnippetCheck("M267-D003-ATTR-02", "expected_part6_contract_id"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M267-D003-ARCH-01", "## M267 Part 6 Cross-Module Error-Surface Preservation Hardening (D003)"),
        SnippetCheck("M267-D003-ARCH-02", "TryBuildObjc3CrossModuleRuntimeLinkPlanArtifact(...)"),
    ),
    RUNTIME_README: (
        SnippetCheck("M267-D003-RUN-01", "## M267 cross-module error-surface preservation probe"),
        SnippetCheck("M267-D003-RUN-02", "tampered imported runtime surface must fail closed"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M267-D003-PKG-01", '"check:objc3c:m267-d003-cross-module-error-surface-preservation-hardening-edge-case-and-compatibility-completion"'),
        SnippetCheck("M267-D003-PKG-02", '"check:objc3c:m267-d003-lane-d-readiness"'),
    ),
    PROCESS_H: (
        SnippetCheck("M267-D003-PH-01", "expected_part6_contract_id"),
        SnippetCheck("M267-D003-PH-02", "part6_result_and_bridging_artifact_replay_present"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M267-D003-PC-01", "part6_cross_module_preservation_ready"),
        SnippetCheck("M267-D003-PC-02", '"cross-module runtime link-plan Part 6 replay surface incomplete for "'),
        SnippetCheck("M267-D003-PC-03", '"cross-module runtime link-plan duplicate imported Part 6 replay key: "'),
    ),
    IMPORT_SURFACE_H: (
        SnippetCheck("M267-D003-IMH-01", "std::string part6_contract_id;"),
        SnippetCheck("M267-D003-IMH-02", "std::string part6_source_contract_id;"),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M267-D003-IMC-01", "surface.part6_contract_id = std::move(contract_id);"),
        SnippetCheck("M267-D003-IMC-02", "surface.part6_source_contract_id = std::move(source_contract_id);"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M267-D003-DRV-01", "expected_part6_contract_id"),
        SnippetCheck("M267-D003-DRV-02", "imported_input.part6_result_and_bridging_artifact_replay_present ="),
    ),
    RUNTIME_INTERNAL_H: (
        SnippetCheck("M267-D003-RIH-01", "M267-D003 cross-module preservation anchor"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M267-D003-RC-01", "M267-D003 cross-module preservation anchor"),
    ),
}


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    checks = 0
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks += require(snippet.snippet in text, display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}", failures)
    return checks, failures


def ensure_binaries(failures: list[Finding]) -> int:
    completed = run([sys.executable, str(ROOT / "scripts" / "ensure_objc3c_native_build.py"), "--mode", "fast"])
    return require(completed.returncode == 0, display_path(ROOT / "scripts" / "ensure_objc3c_native_build.py"), "M267-D003-BUILD", completed.stderr or completed.stdout or "fast native build failed", failures)


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
    return run(args)


def validate_happy_path(*, failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    provider_out = PROBE_ROOT / "provider"
    consumer_out = PROBE_ROOT / "consumer"

    provider_completed = compile_fixture(fixture=PROVIDER_FIXTURE, out_dir=provider_out, registration_order_ordinal=1)
    checks_total += require(provider_completed.returncode == 0, display_path(PROVIDER_FIXTURE), "M267-D003-PROV-COMPILE", provider_completed.stderr or provider_completed.stdout or "provider compile failed", failures)
    if failures:
        return checks_total, {}

    provider_manifest_path = provider_out / "module.manifest.json"
    provider_import_path = provider_out / IMPORT_ARTIFACT
    provider_sidecar_path = provider_out / SIDECAR_ARTIFACT
    for check_id, path in (
        ("M267-D003-PROV-MANIFEST", provider_manifest_path),
        ("M267-D003-PROV-IMPORT", provider_import_path),
        ("M267-D003-PROV-SIDECAR", provider_sidecar_path),
    ):
        checks_total += require(path.exists(), display_path(path), check_id, f"missing artifact: {path.name}", failures)
    if failures:
        return checks_total, {}

    provider_manifest = load_json(provider_manifest_path)
    provider_import = load_json(provider_import_path)
    provider_surface = semantic_surface(provider_manifest).get("objc_part6_result_and_bridging_artifact_replay")
    provider_import_replay = provider_import.get("objc_part6_result_and_bridging_artifact_replay")
    checks_total += require(isinstance(provider_surface, dict), display_path(provider_manifest_path), "M267-D003-PROV-SURFACE", "provider semantic Part 6 replay surface missing", failures)
    checks_total += require(isinstance(provider_import_replay, dict), display_path(provider_import_path), "M267-D003-PROV-IMPORT-REPLAY", "provider import replay packet missing", failures)
    if isinstance(provider_import_replay, dict):
        checks_total += require(provider_import_replay.get("contract_id") == PART6_CONTRACT_ID, display_path(provider_import_path), "M267-D003-PROV-IMPORT-CONTRACT", "provider import replay contract mismatch", failures)
        checks_total += require(provider_import_replay.get("source_contract_id") == PART6_SOURCE_CONTRACT_ID, display_path(provider_import_path), "M267-D003-PROV-IMPORT-SOURCE", "provider import replay source contract mismatch", failures)
        checks_total += require(provider_import_replay.get("binary_artifact_replay_ready") is True, display_path(provider_import_path), "M267-D003-PROV-IMPORT-BINARY", "provider import replay binary readiness missing", failures)
        checks_total += require(provider_import_replay.get("runtime_import_artifact_ready") is True, display_path(provider_import_path), "M267-D003-PROV-IMPORT-RUNTIME", "provider import replay runtime-import readiness missing", failures)
        checks_total += require(provider_import_replay.get("separate_compilation_replay_ready") is True, display_path(provider_import_path), "M267-D003-PROV-IMPORT-SEPARATE", "provider import replay separate-compilation readiness missing", failures)
    if failures:
        return checks_total, {}

    consumer_completed = compile_fixture(
        fixture=CONSUMER_FIXTURE,
        out_dir=consumer_out,
        registration_order_ordinal=2,
        import_surface=provider_import_path,
    )
    checks_total += require(consumer_completed.returncode == 0, display_path(CONSUMER_FIXTURE), "M267-D003-CONS-COMPILE", consumer_completed.stderr or consumer_completed.stdout or "consumer compile failed", failures)
    if failures:
        return checks_total, {}

    consumer_plan_path = consumer_out / LINK_PLAN_ARTIFACT
    consumer_import_path = consumer_out / IMPORT_ARTIFACT
    checks_total += require(consumer_plan_path.exists(), display_path(consumer_plan_path), "M267-D003-CONS-PLAN", "consumer cross-module link plan missing", failures)
    checks_total += require(consumer_import_path.exists(), display_path(consumer_import_path), "M267-D003-CONS-IMPORT", "consumer import artifact missing", failures)
    if failures:
        return checks_total, {}

    consumer_plan = load_json(consumer_plan_path)
    imported_modules = consumer_plan.get("imported_modules")
    checks_total += require(consumer_plan.get("contract_id") == LINK_PLAN_CONTRACT_ID, display_path(consumer_plan_path), "M267-D003-CONS-PLAN-CONTRACT", "cross-module link plan contract mismatch", failures)
    checks_total += require(consumer_plan.get("expected_part6_contract_id") == PART6_CONTRACT_ID, display_path(consumer_plan_path), "M267-D003-CONS-EXPECTED-CONTRACT", "expected Part 6 contract id mismatch", failures)
    checks_total += require(consumer_plan.get("expected_part6_source_contract_id") == PART6_SOURCE_CONTRACT_ID, display_path(consumer_plan_path), "M267-D003-CONS-EXPECTED-SOURCE", "expected Part 6 source contract id mismatch", failures)
    checks_total += require(consumer_plan.get("part6_imported_module_count") == 1, display_path(consumer_plan_path), "M267-D003-CONS-PART6-COUNT", "expected exactly one imported Part 6 module", failures)
    checks_total += require(consumer_plan.get("part6_cross_module_preservation_ready") is True, display_path(consumer_plan_path), "M267-D003-CONS-PRESERVATION", "Part 6 cross-module preservation should be ready", failures)
    checks_total += require(isinstance(imported_modules, list) and len(imported_modules) == 1, display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-MODULES", "unexpected imported module payload", failures)
    provider_module = provider_manifest.get("module")
    checks_total += require(provider_module in consumer_plan.get("part6_imported_module_names_lexicographic", []), display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-NAMES", "provider module missing from imported Part 6 module list", failures)

    if isinstance(imported_modules, list) and imported_modules:
        imported = imported_modules[0]
        checks_total += require(imported.get("module_name") == provider_module, display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-MODULE", "imported module name mismatch", failures)
        checks_total += require(imported.get("part6_result_and_bridging_artifact_replay_present") is True, display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-PRESENT", "imported Part 6 replay presence missing", failures)
        checks_total += require(imported.get("part6_contract_id") == PART6_CONTRACT_ID, display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-CONTRACT", "imported Part 6 contract mismatch", failures)
        checks_total += require(imported.get("part6_source_contract_id") == PART6_SOURCE_CONTRACT_ID, display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-SOURCE", "imported Part 6 source contract mismatch", failures)
        checks_total += require(imported.get("part6_binary_artifact_replay_ready") is True, display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-BINARY", "imported Part 6 binary readiness missing", failures)
        checks_total += require(imported.get("part6_runtime_import_artifact_ready") is True, display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-RUNTIME", "imported Part 6 runtime-import readiness missing", failures)
        checks_total += require(imported.get("part6_separate_compilation_replay_ready") is True, display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-READY", "imported Part 6 separate-compilation readiness missing", failures)
        if isinstance(provider_import_replay, dict):
            checks_total += require(imported.get("part6_replay_key") == provider_import_replay.get("part6_replay_key"), display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-PART6-KEY", "imported Part 6 replay key mismatch", failures)
            checks_total += require(imported.get("throws_replay_key") == provider_import_replay.get("throws_replay_key"), display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-THROWS-KEY", "imported throws replay key mismatch", failures)
            checks_total += require(imported.get("result_like_replay_key") == provider_import_replay.get("result_like_replay_key"), display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-RESULT-KEY", "imported result-like replay key mismatch", failures)
            checks_total += require(imported.get("ns_error_replay_key") == provider_import_replay.get("ns_error_replay_key"), display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-NSERROR-KEY", "imported NSError replay key mismatch", failures)
            checks_total += require(imported.get("unwind_replay_key") == provider_import_replay.get("unwind_replay_key"), display_path(consumer_plan_path), "M267-D003-CONS-IMPORTED-UNWIND-KEY", "imported unwind replay key mismatch", failures)

    return checks_total, {"provider_module": provider_module, "provider_import_path": str(provider_import_path), "consumer_plan_path": str(consumer_plan_path)}


def validate_tampered_path(*, provider_import_path: Path, provider_module: str, failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    tampered_dir = provider_import_path.parent / "tampered_import_surface"
    tampered_dir.mkdir(parents=True, exist_ok=True)
    for sibling in provider_import_path.parent.glob("module.*"):
      if sibling.is_file():
        shutil.copy2(sibling, tampered_dir / sibling.name)
    tampered_import_path = tampered_dir / IMPORT_ARTIFACT
    tampered_out = PROBE_ROOT / "consumer_tampered"

    payload = load_json(provider_import_path)
    replay = payload.get("objc_part6_result_and_bridging_artifact_replay")
    if not isinstance(replay, dict):
        checks_total += require(False, display_path(provider_import_path), "M267-D003-TAMPER-SOURCE", "provider import replay packet missing before tamper", failures)
        return checks_total, {}
    replay["separate_compilation_replay_ready"] = False
    replay["throws_replay_key"] = ""
    tampered_import_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    completed = compile_fixture(
        fixture=CONSUMER_FIXTURE,
        out_dir=tampered_out,
        registration_order_ordinal=2,
        import_surface=tampered_import_path,
    )
    checks_total += require(completed.returncode != 0, display_path(CONSUMER_FIXTURE), "M267-D003-TAMPER-COMPILE", "tampered import surface should fail closed", failures)
    combined = (completed.stderr or "") + "\n" + (completed.stdout or "")
    checks_total += require(
        f"cross-module runtime link-plan Part 6 replay surface incomplete for {provider_module}" in combined,
        display_path(tampered_import_path),
        "M267-D003-TAMPER-ERROR",
        "tampered import surface must fail with deterministic Part 6 replay-surface error",
        failures,
    )
    return checks_total, {
        "tampered_import_path": display_path(tampered_import_path),
        "returncode": completed.returncode,
        "combined_output": combined.strip(),
    }


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
            happy_checks, happy_evidence = validate_happy_path(failures=failures)
            checks_total += happy_checks
            if not failures:
                tamper_checks, tamper_evidence = validate_tampered_path(
                    provider_import_path=Path(happy_evidence["provider_import_path"]),
                    provider_module=str(happy_evidence["provider_module"]),
                    failures=failures,
                )
                checks_total += tamper_checks
                dynamic_summary.update({"happy_path": happy_evidence, "tampered_path": tamper_evidence})

    passed = checks_total - len(failures)
    summary = {
        "issue": "M267-D003",
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": passed,
        "failures": [finding.__dict__ for finding in failures],
        "static": static_summary,
        "dynamic": dynamic_summary,
    }
    return summary, failures, 0 if not failures else 1


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    summary, failures, code = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.path}: {finding.detail}", file=sys.stderr)
    else:
        print(f"[ok] wrote summary to {display_path(args.summary_out)}")
    return code


if __name__ == "__main__":
    raise SystemExit(main())
