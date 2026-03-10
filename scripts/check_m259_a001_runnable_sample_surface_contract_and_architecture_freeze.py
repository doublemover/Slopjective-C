#!/usr/bin/env python3
"""Validate M259-A001 runnable sample surface freeze."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-a001-runnable-sample-surface-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runnable-sample-surface/m259-a001-v1"
EVIDENCE_MODEL = "execution-smoke-replay-script-surface-plus-m256-d004-m257-e002-m258-e002-summary-chain"
SAMPLE_SURFACE_MODEL = "canonical-runnable-sample-surface-composes-scalar-smoke-object-property-and-import-module-proofs"
FAILURE_MODEL = "fail-closed-on-runnable-sample-surface-drift-or-untracked-sample-claims"
NEXT_ISSUE = "M259-A002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-A001" / "runnable_sample_surface_contract_summary.json"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_runnable_sample_surface_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_a001_runnable_sample_surface_contract_and_architecture_freeze_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
EXECUTION_SMOKE = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
EXECUTION_REPLAY = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m259_a001_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py"

EXECUTION_POSITIVE_DIR = ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "positive"
EXECUTION_NEGATIVE_DIR = ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "negative"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
OBJECT_SAMPLE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_d004_canonical_runnable_object_sample.objc3"
OBJECT_LIBRARY = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_d004_canonical_runnable_object_runtime_library.objc3"
OBJECT_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m256_d004_canonical_runnable_object_probe.cpp"
PROPERTY_SAMPLE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_ivar_execution_matrix_positive.objc3"
PROPERTY_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m257_e002_property_ivar_execution_matrix_probe.cpp"
IMPORT_PROVIDER = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m258_d002_runtime_packaging_provider.objc3"
IMPORT_CONSUMER = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m258_d002_runtime_packaging_consumer.objc3"
IMPORT_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m258_e002_import_module_execution_matrix_probe.cpp"

M256_SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-D004" / "canonical_runnable_object_sample_support_summary.json"
M257_SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-E002" / "runnable_property_ivar_execution_matrix_summary.json"
M258_SUMMARY = ROOT / "tmp" / "reports" / "m258" / "M258-E002" / "runnable_import_module_execution_matrix_summary.json"

M256_CONTRACT_ID = "objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1"
M257_CONTRACT_ID = "objc3c-runnable-property-ivar-accessor-execution-matrix/m257-e002-v1"
M258_CONTRACT_ID = "objc3c-runnable-import-module-execution-matrix/m258-e002-v1"


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
        SnippetCheck("M259-A001-DOC-EXP-01", "# M259 Runnable Sample Surface Contract And Architecture Freeze Expectations (A001)"),
        SnippetCheck("M259-A001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M259-A001-DOC-EXP-03", "No blocks, ARC, async, throws, or actor samples."),
        SnippetCheck("M259-A001-DOC-EXP-04", "The freeze must explicitly hand off to `M259-A002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M259-A001-DOC-PKT-01", "# M259-A001 Runnable Sample Surface Contract And Architecture Freeze Packet"),
        SnippetCheck("M259-A001-DOC-PKT-02", "Packet: `M259-A001`"),
        SnippetCheck("M259-A001-DOC-PKT-03", "Issue: `#7208`"),
        SnippetCheck("M259-A001-DOC-PKT-04", "Next issue: `M259-A002`."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M259-A001-NDOC-01", "## Runnable sample surface (M259-A001)"),
        SnippetCheck("M259-A001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M259-A001-NDOC-03", "`tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`"),
        SnippetCheck("M259-A001-NDOC-04", "`tests/tooling/fixtures/native/m258_d002_runtime_packaging_provider.objc3`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M259-A001-SPC-01", "## M259 runnable sample surface (A001)"),
        SnippetCheck("M259-A001-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M259-A001-SPC-03", f"`{SAMPLE_SURFACE_MODEL}`"),
        SnippetCheck("M259-A001-SPC-04", "`M259-A002`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M259-A001-META-01", "## M259 runnable sample surface metadata anchors (A001)"),
        SnippetCheck("M259-A001-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M259-A001-META-03", "`tests/tooling/runtime/m256_d004_canonical_runnable_object_probe.cpp`"),
        SnippetCheck("M259-A001-META-04", "`tmp/reports/m259/M259-A001/runnable_sample_surface_contract_summary.json`"),
    ),
    EXECUTION_SMOKE: (
        SnippetCheck("M259-A001-SMOKE-01", "M259-A001 runnable-sample-surface anchor:"),
        SnippetCheck("M259-A001-SMOKE-02", "scalar/core corpus boundary rooted at tests/tooling/fixtures/native/execution."),
    ),
    EXECUTION_REPLAY: (
        SnippetCheck("M259-A001-REPLAY-01", "M259-A001 runnable-sample-surface anchor:"),
        SnippetCheck("M259-A001-REPLAY-02", "scalar/core corpus carried by execution smoke."),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M259-A001-PKG-01", '"check:objc3c:m259-a001-runnable-sample-surface"'),
        SnippetCheck("M259-A001-PKG-02", '"test:tooling:m259-a001-runnable-sample-surface"'),
        SnippetCheck("M259-A001-PKG-03", '"check:objc3c:m259-a001-lane-a-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M259-A001-RUN-01", "check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py"),
        SnippetCheck("M259-A001-RUN-02", "test_check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M259-A001-TEST-01", "def test_checker_passes(tmp_path: Path) -> None:"),
        SnippetCheck("M259-A001-TEST-02", CONTRACT_ID),
    ),
}


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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = read_text(path)
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def count_fixtures(directory: Path) -> int:
    return sum(1 for _ in sorted(directory.glob("*.objc3")))


def main() -> int:
    failures: list[Finding] = []
    static_contracts: dict[str, dict[str, Any]] = {}
    checks_total = 0

    for path, snippets in STATIC_SNIPPETS.items():
        total, findings = check_static_contract(path, snippets)
        checks_total += total
        static_contracts[display_path(path)] = {"checks": total, "ok": len(findings) == 0}
        failures.extend(findings)

    positive_count = count_fixtures(EXECUTION_POSITIVE_DIR) if EXECUTION_POSITIVE_DIR.exists() else 0
    negative_count = count_fixtures(EXECUTION_NEGATIVE_DIR) if EXECUTION_NEGATIVE_DIR.exists() else 0

    sample_assets = (
        EXECUTION_POSITIVE_DIR,
        EXECUTION_NEGATIVE_DIR,
        HELLO_FIXTURE,
        OBJECT_SAMPLE,
        OBJECT_LIBRARY,
        OBJECT_PROBE,
        PROPERTY_SAMPLE,
        PROPERTY_PROBE,
        IMPORT_PROVIDER,
        IMPORT_CONSUMER,
        IMPORT_PROBE,
    )
    for asset in sample_assets:
        checks_total += require(asset.exists(), display_path(asset), "M259-A001-ASSET", f"missing canonical sample asset: {display_path(asset)}", failures)

    checks_total += require(positive_count > 0, display_path(EXECUTION_POSITIVE_DIR), "M259-A001-POSITIVE", "positive execution corpus must be non-empty", failures)
    checks_total += require(negative_count > 0, display_path(EXECUTION_NEGATIVE_DIR), "M259-A001-NEGATIVE", "negative execution corpus must be non-empty", failures)

    m256_summary = load_json(M256_SUMMARY)
    checks_total += require(m256_summary.get("contract_id") == M256_CONTRACT_ID, display_path(M256_SUMMARY), "M259-A001-M256-CONTRACT", "M256-D004 summary contract id drift", failures)
    checks_total += require(m256_summary.get("dynamic", {}).get("sample_exit_code") == 37, display_path(M256_SUMMARY), "M259-A001-M256-EXIT", "canonical object sample exit code must remain 37", failures)

    m257_summary = load_json(M257_SUMMARY)
    checks_total += require(m257_summary.get("contract_id") == M257_CONTRACT_ID, display_path(M257_SUMMARY), "M259-A001-M257-CONTRACT", "M257-E002 summary contract id drift", failures)
    checks_total += require(m257_summary.get("ok") is True, display_path(M257_SUMMARY), "M259-A001-M257-OK", "M257-E002 summary must remain green", failures)
    checks_total += require(m257_summary.get("upstream_summaries", {}).get("M257-A002", {}).get("contract_id") == "objc3c-executable-property-ivar-source-model-completion/m257-a002-v1", display_path(M257_SUMMARY), "M259-A001-M257-UPSTREAM-A002", "M257-E002 must continue to carry the M257-A002 upstream summary", failures)
    checks_total += require(m257_summary.get("upstream_summaries", {}).get("M257-D003", {}).get("contract_id") == "objc3c-runtime-property-metadata-reflection/m257-d003-v1", display_path(M257_SUMMARY), "M259-A001-M257-UPSTREAM-D003", "M257-E002 must continue to carry the M257-D003 upstream summary", failures)

    m258_summary = load_json(M258_SUMMARY)
    checks_total += require(m258_summary.get("contract_id") == M258_CONTRACT_ID, display_path(M258_SUMMARY), "M259-A001-M258-CONTRACT", "M258-E002 summary contract id drift", failures)
    checks_total += require(m258_summary.get("upstream_summaries", {}).get("M258-D002", {}).get("startup_registered_image_count") == 2, display_path(M258_SUMMARY), "M259-A001-M258-STARTUP", "import/module matrix must keep two-image startup registration", failures)
    checks_total += require(m258_summary.get("upstream_summaries", {}).get("M258-D002", {}).get("post_replay_registered_image_count") == 2, display_path(M258_SUMMARY), "M259-A001-M258-REPLAY", "import/module matrix must keep two-image replay registration", failures)

    checks_passed = checks_total - len(failures)
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "sample_surface_model": SAMPLE_SURFACE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "static_contracts": static_contracts,
        "sample_surface": {
            "execution_positive_dir": display_path(EXECUTION_POSITIVE_DIR),
            "execution_negative_dir": display_path(EXECUTION_NEGATIVE_DIR),
            "positive_fixture_count": positive_count,
            "negative_fixture_count": negative_count,
            "quickstart_fixture": display_path(HELLO_FIXTURE),
            "canonical_object_sample": display_path(OBJECT_SAMPLE),
            "canonical_object_library": display_path(OBJECT_LIBRARY),
            "canonical_object_probe": display_path(OBJECT_PROBE),
            "property_matrix_sample": display_path(PROPERTY_SAMPLE),
            "property_matrix_probe": display_path(PROPERTY_PROBE),
            "import_provider_fixture": display_path(IMPORT_PROVIDER),
            "import_consumer_fixture": display_path(IMPORT_CONSUMER),
            "import_matrix_probe": display_path(IMPORT_PROBE),
        },
        "upstream_summaries": {
            "M256-D004": {
                "summary": display_path(M256_SUMMARY),
                "contract_id": m256_summary.get("contract_id"),
                "sample_exit_code": m256_summary.get("dynamic", {}).get("sample_exit_code"),
            },
            "M257-E002": {
                "summary": display_path(M257_SUMMARY),
                "contract_id": m257_summary.get("contract_id"),
                "ok": m257_summary.get("ok"),
                "upstream_a002_contract_id": m257_summary.get("upstream_summaries", {}).get("M257-A002", {}).get("contract_id"),
                "upstream_d003_contract_id": m257_summary.get("upstream_summaries", {}).get("M257-D003", {}).get("contract_id"),
            },
            "M258-E002": {
                "summary": display_path(M258_SUMMARY),
                "contract_id": m258_summary.get("contract_id"),
                "startup_registered_image_count": m258_summary.get("upstream_summaries", {}).get("M258-D002", {}).get("startup_registered_image_count"),
                "post_replay_registered_image_count": m258_summary.get("upstream_summaries", {}).get("M258-D002", {}).get("post_replay_registered_image_count"),
            },
        },
    }

    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
