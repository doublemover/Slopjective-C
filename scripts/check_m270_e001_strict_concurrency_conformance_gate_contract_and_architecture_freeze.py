#!/usr/bin/env python3
"""Fail-closed checker for M270-E001 strict concurrency conformance gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m270-e001-strict-concurrency-conformance-gate-v1"
CONTRACT_ID = "objc3c-strict-concurrency-conformance-gate/m270-e001-v1"
EVIDENCE_MODEL = "a002-b003-c003-summary-chain-plus-d002-runtime-proof-plus-d003-cross-module-preservation"
FAILURE_MODEL = "fail-closed-on-strict-concurrency-gate-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M270-E002"

A002_CONTRACT_ID = "objc3c-part7-actor-member-isolation-source-closure/m270-a002-v1"
B003_CONTRACT_ID = "objc3c-part7-actor-race-hazard-escape-diagnostics/m270-b003-v1"
C003_CONTRACT_ID = "objc3c-part7-actor-replay-proof-and-race-guard-integration/m270-c003-v1"
D002_CONTRACT_ID = "objc3c-part7-live-actor-mailbox-and-isolation-runtime/m270-d002-v1"
D003_CONTRACT_ID = "objc3c-cross-module-actor-isolation-metadata-hardening/m270-d003-v1"
D003_IMPORT_CONTRACT_ID = "objc3c-part7-actor-mailbox-isolation-import-surface/m270-d003-v1"
D003_IMPORT_SOURCE_CONTRACT_ID = "objc3c-part7-actor-lowering-and-metadata-contract/m270-c001-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m270_strict_concurrency_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m270" / "m270_e001_strict_concurrency_conformance_gate_contract_and_architecture_freeze_packet.md"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
CONFORMANCE_SPEC = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m270_e001_lane_e_readiness.py"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m270" / "M270-E001" / "strict_concurrency_conformance_gate_summary.json"
ENSURE_BUILD_SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-E001" / "ensure_objc3c_native_build_summary.json"

A002_SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-A002" / "actor_member_isolation_source_closure_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-B003" / "actor_race_hazard_escape_diagnostics_summary.json"
C003_SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-C003" / "actor_replay_race_guard_summary.json"
D002_SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-D002" / "live_actor_mailbox_runtime_summary.json"
D003_SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-D003" / "cross_module_actor_isolation_metadata_summary.json"

UPSTREAM_SUMMARIES: tuple[tuple[str, Path, str], ...] = (
    ("M270-A002", A002_SUMMARY, A002_CONTRACT_ID),
    ("M270-B003", B003_SUMMARY, B003_CONTRACT_ID),
    ("M270-C003", C003_SUMMARY, C003_CONTRACT_ID),
    ("M270-D003", D003_SUMMARY, D003_CONTRACT_ID),
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
        SnippetCheck("M270-E001-EXP-01", "# M270 Strict Concurrency Conformance Gate Contract And Architecture Freeze Expectations (E001)"),
        SnippetCheck("M270-E001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M270-E001-EXP-03", "M270-D003"),
        SnippetCheck("M270-E001-EXP-04", "The next issue is `M270-E002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M270-E001-PKT-01", "# M270-E001 Strict Concurrency Conformance Gate Contract And Architecture Freeze Packet"),
        SnippetCheck("M270-E001-PKT-02", "Issue: `#7318`"),
        SnippetCheck("M270-E001-PKT-03", "- `M270-A002`"),
        SnippetCheck("M270-E001-PKT-04", "- `M270-D003`"),
        SnippetCheck("M270-E001-PKT-05", "- `M270-E002` is the next issue"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M270-E001-SRC-01", "## M270 strict concurrency conformance gate"),
        SnippetCheck("M270-E001-SRC-02", "`M270-D002` live mailbox runtime probe"),
        SnippetCheck("M270-E001-SRC-03", "`M270-E002`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M270-E001-NDOC-01", "## M270 strict concurrency conformance gate"),
        SnippetCheck("M270-E001-NDOC-02", "`M270-D002` live mailbox runtime probe"),
        SnippetCheck("M270-E001-NDOC-03", "`M270-E002`"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M270-E001-ABS-01", "M270-E001 strict concurrency conformance gate note:"),
        SnippetCheck("M270-E001-ABS-02", "`M270-A002`, `M270-B003`, `M270-C003`, and `M270-D003`"),
    ),
    CONFORMANCE_SPEC: (
        SnippetCheck("M270-E001-CONF-01", "M270-E001 strict concurrency conformance gate note:"),
        SnippetCheck("M270-E001-CONF-02", "`M270-A002`, `M270-B003`, `M270-C003`, and `M270-D003`"),
        SnippetCheck("M270-E001-CONF-03", "`M270-E002`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M270-E001-ARCH-01", "## M270 Part 7 Strict Concurrency Conformance Gate (E001)"),
        SnippetCheck("M270-E001-ARCH-02", "`M270-D003` cross-module preservation"),
        SnippetCheck("M270-E001-ARCH-03", "`M270-E002`"),
    ),
    RUNTIME_README: (
        SnippetCheck("M270-E001-RTR-01", "## M270 strict concurrency conformance gate"),
        SnippetCheck("M270-E001-RTR-02", "does not add a new runtime probe"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M270-E001-DRV-01", "M270-E001 strict concurrency conformance gate anchor"),
        SnippetCheck("M270-E001-DRV-02", "front-door actor publication path remains fail-closed"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M270-E001-MAN-01", "M270-E001 strict concurrency conformance gate anchor"),
        SnippetCheck("M270-E001-MAN-02", "front-door actor publication path remains intentionally fail-closed"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M270-E001-FAPI-01", "M270-E001 strict concurrency conformance gate anchor"),
        SnippetCheck("M270-E001-FAPI-02", "D003 cross-module preservation artifacts"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M270-E001-RUN-01", "run_m270_a002_lane_a_readiness.py"),
        SnippetCheck("M270-E001-RUN-02", "run_m270_b003_lane_b_readiness.py"),
        SnippetCheck("M270-E001-RUN-03", "run_m270_c003_lane_c_readiness.py"),
        SnippetCheck("M270-E001-RUN-04", "run_m270_d003_lane_d_readiness.py"),
        SnippetCheck("M270-E001-RUN-05", "test_check_m270_e001_strict_concurrency_conformance_gate_contract_and_architecture_freeze.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M270-E001-PKG-01", '"check:objc3c:m270-e001-strict-concurrency-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M270-E001-PKG-02", '"test:tooling:m270-e001-strict-concurrency-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M270-E001-PKG-03", '"check:objc3c:m270-e001-lane-e-readiness"'),
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


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M270-E001-MISSING", f"required artifact missing: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


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
        total = payload.get("checks_total", payload.get("total_checks", 0))
        passed = payload.get("checks_passed", payload.get("passed_checks", -1))
        checks_passed += require(total == passed, artifact, f"{issue}-SUM-03", "upstream summary lost full coverage", failures)
        checks_total += 1
        checks_passed += require(total > 0, artifact, f"{issue}-SUM-04", "upstream summary reports zero checks", failures)
        upstream[issue] = {
            "contract_id": payload.get("contract_id"),
            "checks_total": total,
            "checks_passed": passed,
        }
    return checks_total, checks_passed, upstream


def validate_actor_runtime_proof(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    payload = read_json(D002_SUMMARY)
    artifact = display_path(D002_SUMMARY)

    checks_total += 1
    checks_passed += require(payload.get("contract_id") == D002_CONTRACT_ID, artifact, "M270-E001-D002-01", "D002 contract drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_total", 0) == payload.get("checks_passed", -1), artifact, "M270-E001-D002-02", "D002 summary lost full coverage", failures)

    probe = payload.get("dynamic", {}).get("probe_output", {})
    required_pairs = {
        "bind_executor_call_count": "1",
        "mailbox_enqueue_call_count": "1",
        "mailbox_drain_call_count": "1",
        "last_bound_actor_handle": "41",
        "last_bound_executor_tag": "1",
        "last_mailbox_enqueued_value": "23",
        "last_mailbox_drained_value": "23",
        "replay_proof_call_count": "1",
        "race_guard_call_count": "1",
    }
    for key, expected in required_pairs.items():
        checks_total += 1
        checks_passed += require(
            probe.get(key) == expected,
            artifact,
            f"M270-E001-D002-{key}",
            f"expected {key}={expected}, got {probe.get(key)!r}",
            failures,
        )

    return checks_passed, checks_total, {"probe_output": probe}


def validate_cross_module_actor_artifacts(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    d003_payload = read_json(D003_SUMMARY)
    artifact = display_path(D003_SUMMARY)

    checks_total += 1
    checks_passed += require(d003_payload.get("contract_id") == D003_CONTRACT_ID, artifact, "M270-E001-D003-01", "D003 contract drifted", failures)
    checks_total += 1
    checks_passed += require(d003_payload.get("checks_total", 0) == d003_payload.get("checks_passed", -1), artifact, "M270-E001-D003-02", "D003 summary lost full coverage", failures)

    dynamic = d003_payload.get("dynamic", {})
    provider_path = ROOT / str(dynamic.get("provider_import_artifact", ""))
    consumer_path = ROOT / str(dynamic.get("consumer_link_plan_artifact", ""))

    checks_total += 1
    checks_passed += require(provider_path.exists(), artifact, "M270-E001-D003-03", "provider import artifact missing", failures)
    checks_total += 1
    checks_passed += require(consumer_path.exists(), artifact, "M270-E001-D003-04", "consumer link-plan artifact missing", failures)

    provider = read_json(provider_path)
    consumer = read_json(consumer_path)
    provider_actor = provider.get("objc_part7_actor_mailbox_and_isolation_runtime_import_surface", {})

    checks_total += 1
    checks_passed += require(provider_actor.get("contract_id") == D003_IMPORT_CONTRACT_ID, display_path(provider_path), "M270-E001-D003-05", "provider actor import contract drifted", failures)
    checks_total += 1
    checks_passed += require(provider_actor.get("source_contract_id") == D003_IMPORT_SOURCE_CONTRACT_ID, display_path(provider_path), "M270-E001-D003-06", "provider actor import source contract drifted", failures)
    checks_total += 1
    checks_passed += require(provider_actor.get("actor_mailbox_runtime_ready") is True, display_path(provider_path), "M270-E001-D003-07", "provider actor import surface lost readiness", failures)
    checks_total += 1
    checks_passed += require(provider_actor.get("deterministic") is True, display_path(provider_path), "M270-E001-D003-08", "provider actor import surface lost determinism", failures)

    checks_total += 1
    checks_passed += require(consumer.get("expected_part7_actor_contract_id") == D003_IMPORT_CONTRACT_ID, display_path(consumer_path), "M270-E001-D003-09", "consumer expected actor contract drifted", failures)
    checks_total += 1
    checks_passed += require(consumer.get("expected_part7_actor_source_contract_id") == D003_IMPORT_SOURCE_CONTRACT_ID, display_path(consumer_path), "M270-E001-D003-10", "consumer expected actor source contract drifted", failures)
    checks_total += 1
    checks_passed += require(consumer.get("part7_actor_cross_module_isolation_ready") is True, display_path(consumer_path), "M270-E001-D003-11", "consumer lost actor cross-module readiness", failures)
    checks_total += 1
    checks_passed += require(consumer.get("part7_actor_imported_module_count") == 1, display_path(consumer_path), "M270-E001-D003-12", "consumer actor import count drifted", failures)

    imported_modules = consumer.get("imported_modules", [])
    imported = imported_modules[0] if imported_modules else {}
    checks_total += 1
    checks_passed += require(imported.get("part7_actor_mailbox_runtime_import_present") is True, display_path(consumer_path), "M270-E001-D003-13", "consumer imported module lost actor import presence", failures)
    checks_total += 1
    checks_passed += require(imported.get("part7_actor_mailbox_runtime_ready") is True, display_path(consumer_path), "M270-E001-D003-14", "consumer imported module lost actor import readiness", failures)
    checks_total += 1
    checks_passed += require(bool(imported.get("part7_actor_mailbox_runtime_replay_key")), display_path(consumer_path), "M270-E001-D003-15", "consumer imported module lost actor replay key", failures)
    checks_total += 1
    checks_passed += require(bool(imported.get("part7_actor_lowering_replay_key")), display_path(consumer_path), "M270-E001-D003-16", "consumer imported module lost actor lowering replay key", failures)
    checks_total += 1
    checks_passed += require(bool(imported.get("part7_actor_isolation_lowering_replay_key")), display_path(consumer_path), "M270-E001-D003-17", "consumer imported module lost actor isolation replay key", failures)

    return checks_passed, checks_total, {
        "provider_import_artifact": display_path(provider_path),
        "consumer_link_plan_artifact": display_path(consumer_path),
        "provider_actor_packet": {
            "contract_id": provider_actor.get("contract_id"),
            "source_contract_id": provider_actor.get("source_contract_id"),
            "actor_mailbox_runtime_ready": provider_actor.get("actor_mailbox_runtime_ready"),
            "deterministic": provider_actor.get("deterministic"),
        },
        "consumer_actor_link_plan": {
            "expected_part7_actor_contract_id": consumer.get("expected_part7_actor_contract_id"),
            "expected_part7_actor_source_contract_id": consumer.get("expected_part7_actor_source_contract_id"),
            "part7_actor_imported_module_count": consumer.get("part7_actor_imported_module_count"),
            "part7_actor_cross_module_isolation_ready": consumer.get("part7_actor_cross_module_isolation_ready"),
        },
    }


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream_total, upstream_passed, upstream = validate_upstream_summaries(failures)
    checks_total += upstream_total
    checks_passed += upstream_passed

    dynamic: dict[str, Any] = {"skipped": skip_dynamic_probes}
    if not skip_dynamic_probes:
        actor_total, actor_passed, actor_runtime = validate_actor_runtime_proof(failures)
        checks_total += actor_total
        checks_passed += actor_passed
        cross_total, cross_passed, cross_module = validate_cross_module_actor_artifacts(failures)
        checks_total += cross_total
        checks_passed += cross_passed
        dynamic.update(
            {
                "skipped": False,
                "actor_runtime_proof": actor_runtime,
                "cross_module_actor_artifacts": cross_module,
            }
        )

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "upstream": upstream,
        "dynamic": dynamic,
        "ensure_build_summary": display_path(ENSURE_BUILD_SUMMARY),
        "failures": [finding.__dict__ for finding in failures],
    }
    return summary, failures


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    args = parser.parse_args(argv)

    summary, failures = build_summary(skip_dynamic_probes=args.skip_dynamic_probes)
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        print(f"[fail] M270-E001 strict concurrency conformance gate check failed ({len(failures)} findings)", file=sys.stderr)
        return 1

    print(f"[ok] M270-E001 strict concurrency conformance gate check passed ({summary['checks_passed']}/{summary['checks_total']} checks)")
    print(f"[ok] wrote summary: {display_path(SUMMARY_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
