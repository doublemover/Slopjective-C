#!/usr/bin/env python3
"""Validate M259-C002 object/IR replay-proof and metadata inspection evidence."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-c002-object-ir-replay-proof-and-metadata-inspection-v1"
CONTRACT_ID = "objc3c-runnable-object-ir-replay-and-metadata-inspection/m259-c002-v1"
REPLAY_MODEL = "a002-canonical-runnable-sample-ir-object-and-readobj-section-replay"
EVIDENCE_MODEL = "execution-replay-proof-script-emits-live-ir-object-and-section-inspection-hashes-for-a002"
FAILURE_MODEL = "fail-closed-on-ir-object-or-metadata-inspection-replay-drift"
NEXT_ISSUE = "M259-D001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-C002" / "object_and_ir_replay_proof_plus_metadata_inspection_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_c002_object_and_ir_replay_proof_plus_metadata_inspection_evidence_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_c002_object_and_ir_replay_proof_plus_metadata_inspection_evidence_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_SCRIPT = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
C001_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-C001" / "end_to_end_replay_and_inspection_summary.json"
M258_E002_SUMMARY = ROOT / "tmp" / "reports" / "m258" / "M258-E002" / "runnable_import_module_execution_matrix_summary.json"
REPLAY_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-replay-proof"
A002_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m259_a002_canonical_runnable_sample_set.objc3"
REQUIRED_SECTIONS = {
    "objc3.runtime.class_descriptors",
    "objc3.runtime.protocol_descriptors",
    "objc3.runtime.category_descriptors",
    "objc3.runtime.property_descriptors",
    "objc3.runtime.ivar_descriptors",
    "objc3.runtime.selector_pool",
    "objc3.runtime.string_pool",
    "objc3.runtime.discovery_root",
    "objc3.runtime.linker_anchor",
    "objc3.runtime.image_root",
    "objc3.runtime.registration_descriptor",
}


def host_path_str(path: Path) -> str:
    text = str(path)
    if sys.platform.startswith("win") and text.startswith("\\\\?\\"):
        return text[4:]
    return text


def host_arg(arg: str) -> str:
    if sys.platform.startswith("win") and arg.startswith("\\\\?\\"):
        return arg[4:]
    return arg


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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
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


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    normalized = [host_arg(part) for part in command]
    return subprocess.run(
        normalized,
        cwd=host_path_str(ROOT),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def latest_summary(root: Path) -> Path | None:
    if not root.exists():
        return None
    summaries = list(root.glob("*/summary.json"))
    if not summaries:
        return None
    return max(summaries, key=lambda path: path.stat().st_mtime)


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    previous_summary = latest_summary(REPLAY_ROOT)
    previous_summary_key: tuple[str, int] | None = None
    if previous_summary is not None and previous_summary.exists():
        previous_summary_key = (str(previous_summary.resolve()), previous_summary.stat().st_mtime_ns)

    replay = run_command(["pwsh", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", host_path_str(REPLAY_SCRIPT)])
    checks_total += 1
    checks_passed += require(
        replay.returncode == 0,
        display_path(REPLAY_SCRIPT),
        "M259-C002-DYN-replay",
        f"execution replay proof failed: {replay.stderr or replay.stdout}",
        failures,
    )

    replay_summary_path = latest_summary(REPLAY_ROOT)
    if replay_summary_path is not None and replay_summary_path.exists() and previous_summary_key is not None:
        replay_summary_key = (str(replay_summary_path.resolve()), replay_summary_path.stat().st_mtime_ns)
        if replay_summary_key == previous_summary_key:
            replay_summary_path = None

    checks_total += 1
    checks_passed += require(
        replay_summary_path is not None and replay_summary_path.exists(),
        display_path(REPLAY_ROOT),
        "M259-C002-DYN-summary",
        "replay summary missing or stale",
        failures,
    )
    if replay_summary_path is None or not replay_summary_path.exists():
        return checks_total, checks_passed, {}

    payload = load_json(replay_summary_path)
    checks_total += 9
    checks_passed += require(payload.get("status") == "PASS", display_path(replay_summary_path), "M259-C002-DYN-status", "replay summary status must be PASS", failures)
    checks_passed += require(payload.get("run1_sha256") == payload.get("run2_sha256"), display_path(replay_summary_path), "M259-C002-DYN-smoke-hash", "smoke replay hashes must match", failures)

    canonical = payload.get("canonical_runnable_replay")
    checks_passed += require(isinstance(canonical, dict), display_path(replay_summary_path), "M259-C002-DYN-canonical", "canonical_runnable_replay payload missing", failures)
    if not isinstance(canonical, dict):
        return checks_total, checks_passed, {"replay_summary": display_path(replay_summary_path)}

    checks_passed += require(canonical.get("status") == "PASS", display_path(replay_summary_path), "M259-C002-DYN-canonical-status", "canonical runnable replay status must be PASS", failures)
    checks_passed += require(canonical.get("fixture") == display_path(A002_FIXTURE), display_path(replay_summary_path), "M259-C002-DYN-fixture", "canonical runnable replay fixture drifted", failures)
    checks_passed += require(canonical.get("ir_sha256") == canonical.get("run1", {}).get("ir_sha256") == canonical.get("run2", {}).get("ir_sha256"), display_path(replay_summary_path), "M259-C002-DYN-ir-hash", "IR replay hashes must match", failures)
    checks_passed += require(canonical.get("object_sha256") == canonical.get("run1", {}).get("object_sha256") == canonical.get("run2", {}).get("object_sha256"), display_path(replay_summary_path), "M259-C002-DYN-object-hash", "object replay hashes must match", failures)
    checks_passed += require(canonical.get("section_inspection_sha256") == canonical.get("run1", {}).get("section_inspection_sha256") == canonical.get("run2", {}).get("section_inspection_sha256"), display_path(replay_summary_path), "M259-C002-DYN-inspection-hash", "section inspection hashes must match", failures)
    run1_sections = set(canonical.get("run1", {}).get("section_names", []))
    run2_sections = set(canonical.get("run2", {}).get("section_names", []))
    checks_passed += require(REQUIRED_SECTIONS.issubset(run1_sections) and REQUIRED_SECTIONS.issubset(run2_sections), display_path(replay_summary_path), "M259-C002-DYN-sections", "required runtime sections missing from canonical replay payload", failures)

    artifact_paths = [
        ROOT / canonical.get("run1", {}).get("ir", ""),
        ROOT / canonical.get("run1", {}).get("object", ""),
        ROOT / canonical.get("run1", {}).get("readobj_sections_log", ""),
        ROOT / canonical.get("run2", {}).get("ir", ""),
        ROOT / canonical.get("run2", {}).get("object", ""),
        ROOT / canonical.get("run2", {}).get("readobj_sections_log", ""),
    ]
    for index, artifact_path in enumerate(artifact_paths, start=1):
        checks_total += 1
        checks_passed += require(artifact_path.exists(), display_path(artifact_path), f"M259-C002-DYN-artifact-{index}", f"missing replay artifact {display_path(artifact_path)}", failures)

    return checks_total, checks_passed, {
        "replay_summary": display_path(replay_summary_path),
        "canonical_runnable_replay": canonical,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    checks_total += 4
    checks_passed += ensure_snippets(
        EXPECTATIONS_DOC,
        (
            SnippetCheck("M259-C002-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M259-C002-DOC-02", "Add deterministic checker coverage over the live happy path."),
            SnippetCheck("M259-C002-DOC-03", "The contract must explicitly hand off to `M259-D001`."),
            SnippetCheck("M259-C002-DOC-04", "tmp/reports/m259/M259-C002/object_and_ir_replay_proof_plus_metadata_inspection_summary.json"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        PACKET_DOC,
        (
            SnippetCheck("M259-C002-PKT-01", "Packet: `M259-C002`"),
            SnippetCheck("M259-C002-PKT-02", "Issue: `#7213`"),
            SnippetCheck("M259-C002-PKT-03", "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3"),
            SnippetCheck("M259-C002-PKT-04", "Next issue: `M259-D001`."),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        DOC_SOURCE,
        (
            SnippetCheck("M259-C002-SRC-01", "## M259 object and IR replay-proof plus metadata inspection evidence (C002)"),
            SnippetCheck("M259-C002-SRC-02", REPLAY_MODEL),
            SnippetCheck("M259-C002-SRC-03", "`M259-D001`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        DOC_NATIVE,
        (
            SnippetCheck("M259-C002-NDOC-01", "## M259 object and IR replay-proof plus metadata inspection evidence (C002)"),
            SnippetCheck("M259-C002-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-C002-NDOC-03", "`tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        LOWERING_SPEC,
        (
            SnippetCheck("M259-C002-SPC-01", "## M259 object and IR replay-proof plus metadata inspection evidence (C002)"),
            SnippetCheck("M259-C002-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-C002-SPC-03", f"`{FAILURE_MODEL}`"),
            SnippetCheck("M259-C002-SPC-04", "`M259-D001`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        METADATA_SPEC,
        (
            SnippetCheck("M259-C002-META-01", "## M259 replay-proof metadata inspection evidence anchors (C002)"),
            SnippetCheck("M259-C002-META-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-C002-META-03", "`canonical_runnable_replay.section_inspection_sha256`"),
        ),
        failures,
    )
    checks_total += 2
    checks_passed += ensure_snippets(
        SMOKE_SCRIPT,
        (
            SnippetCheck("M259-C002-SMOKE-01", "M259-C002 object-ir-replay-proof anchor:"),
            SnippetCheck("M259-C002-SMOKE-02", "canonical object/IR replay plus metadata section"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        REPLAY_SCRIPT,
        (
            SnippetCheck("M259-C002-REPLAY-01", "M259-C002 object-ir-replay-proof anchor:"),
            SnippetCheck("M259-C002-REPLAY-02", "canonical_runnable_replay"),
            SnippetCheck("M259-C002-REPLAY-03", "section_inspection_sha256"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        PACKAGE_JSON,
        (
            SnippetCheck("M259-C002-PKG-01", '"check:objc3c:m259-c002-object-ir-replay-proof-and-metadata-inspection"'),
            SnippetCheck("M259-C002-PKG-02", '"test:tooling:m259-c002-object-ir-replay-proof-and-metadata-inspection"'),
            SnippetCheck("M259-C002-PKG-03", '"check:objc3c:m259-c002-lane-c-readiness"'),
        ),
        failures,
    )

    c001_summary = load_json(C001_SUMMARY)
    m258_summary = load_json(M258_E002_SUMMARY)
    checks_total += 4
    checks_passed += require(c001_summary.get("contract_id") == "objc3c-runnable-replay-and-inspection-evidence-freeze/m259-c001-v1", display_path(C001_SUMMARY), "M259-C002-C001-contract", "M259-C001 contract drift", failures)
    checks_passed += require(c001_summary.get("ok") is True, display_path(C001_SUMMARY), "M259-C002-C001-ok", "M259-C001 summary must remain green", failures)
    checks_passed += require(m258_summary.get("contract_id") == "objc3c-runnable-import-module-execution-matrix/m258-e002-v1", display_path(M258_E002_SUMMARY), "M259-C002-M258-contract", "M258-E002 contract drift", failures)
    checks_passed += require(m258_summary.get("ok") is True, display_path(M258_E002_SUMMARY), "M259-C002-M258-ok", "M258-E002 summary must remain green", failures)

    probe_details: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, probe_details = run_dynamic_probes(failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "replay_model": REPLAY_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "failures": [finding.__dict__ for finding in failures],
        "dependency": {
            "M259-C001": {
                "summary": display_path(C001_SUMMARY),
                "contract_id": c001_summary.get("contract_id"),
                "ok": c001_summary.get("ok"),
            },
            "M258-E002": {
                "summary": display_path(M258_E002_SUMMARY),
                "contract_id": m258_summary.get("contract_id"),
                "ok": m258_summary.get("ok"),
            },
        },
        "probe_details": probe_details,
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
