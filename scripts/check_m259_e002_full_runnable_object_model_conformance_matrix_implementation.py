#!/usr/bin/env python3
"""Validate M259-E002 full runnable object-model conformance matrix."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-e002-full-runnable-object-model-conformance-matrix-v1"
CONTRACT_ID = "objc3c-runnable-object-model-conformance-matrix/m259-e002-v1"
MATRIX_MODEL = "deterministic-row-per-runnable-claim-with-fixture-or-command-proof"
EVIDENCE_MODEL = "tracked-json-matrix-over-a002-b002-c002-d003-and-live-script-anchors"
FAILURE_MODEL = "fail-closed-on-matrix-row-drift-or-unbacked-runnable-claim"
NEXT_ISSUE = "M259-E003"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-E002" / "full_runnable_object_model_conformance_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_full_runnable_object_model_conformance_matrix_implementation_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_e002_full_runnable_object_model_conformance_matrix_implementation_packet.md"
MATRIX_JSON = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_e002_full_runnable_object_model_conformance_matrix.json"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
SMOKE_SCRIPT = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_SCRIPT = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"


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
    parser.add_argument("--expectations-doc", type=Path, default=EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=PACKET_DOC)
    parser.add_argument("--matrix-json", type=Path, default=MATRIX_JSON)
    parser.add_argument("--doc-source", type=Path, default=DOC_SOURCE)
    parser.add_argument("--native-doc", type=Path, default=DOC_NATIVE)
    parser.add_argument("--lowering-spec", type=Path, default=LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=METADATA_SPEC)
    parser.add_argument("--smoke-script", type=Path, default=SMOKE_SCRIPT)
    parser.add_argument("--replay-script", type=Path, default=REPLAY_SCRIPT)
    parser.add_argument("--package-json", type=Path, default=PACKAGE_JSON)
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


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    checks_total += 5
    checks_passed += ensure_snippets(
        args.expectations_doc,
        (
            SnippetCheck("M259-E002-DOC-01", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M259-E002-DOC-02", "Issue: `#7218`"),
            SnippetCheck("M259-E002-DOC-03", "`spec/planning/compiler/m259/m259_e002_full_runnable_object_model_conformance_matrix.json`"),
            SnippetCheck("M259-E002-DOC-04", "Every matrix row must have:"),
            SnippetCheck("M259-E002-DOC-05", "`M259-E003`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.packet_doc,
        (
            SnippetCheck("M259-E002-PKT-01", "Packet: `M259-E002`"),
            SnippetCheck("M259-E002-PKT-02", "Issue: `#7218`"),
            SnippetCheck("M259-E002-PKT-03", "Dependencies: `M259-E001`, `M259-A002`, `M259-B002`, `M259-C002`, `M259-D003`"),
            SnippetCheck("M259-E002-PKT-04", "The tracked matrix JSON exists and is deterministic."),
            SnippetCheck("M259-E002-PKT-05", "`M259-E003`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.doc_source,
        (
            SnippetCheck("M259-E002-SRC-01", "## M259 full runnable object-model conformance matrix (E002)"),
            SnippetCheck("M259-E002-SRC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-E002-SRC-03", "`spec/planning/compiler/m259/m259_e002_full_runnable_object_model_conformance_matrix.json`"),
            SnippetCheck("M259-E002-SRC-04", "every row has a stable `row_id`"),
            SnippetCheck("M259-E002-SRC-05", "`M259-E003`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.native_doc,
        (
            SnippetCheck("M259-E002-NDOC-01", "## M259 full runnable object-model conformance matrix (E002)"),
            SnippetCheck("M259-E002-NDOC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-E002-NDOC-03", "`spec/planning/compiler/m259/m259_e002_full_runnable_object_model_conformance_matrix.json`"),
            SnippetCheck("M259-E002-NDOC-04", "no block/ARC conformance claim lands here"),
            SnippetCheck("M259-E002-NDOC-05", "`M259-E003`"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.lowering_spec,
        (
            SnippetCheck("M259-E002-SPC-01", "## M259 full runnable object-model conformance matrix (E002)"),
            SnippetCheck("M259-E002-SPC-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-E002-SPC-03", f"`{MATRIX_MODEL}`"),
            SnippetCheck("M259-E002-SPC-04", f"`{FAILURE_MODEL}`"),
            SnippetCheck("M259-E002-SPC-05", "`M259-E003`"),
        ),
        failures,
    )
    checks_total += 4
    checks_passed += ensure_snippets(
        args.metadata_spec,
        (
            SnippetCheck("M259-E002-META-01", "## M259 runnable object-model conformance-matrix metadata anchors (E002)"),
            SnippetCheck("M259-E002-META-02", f"`{CONTRACT_ID}`"),
            SnippetCheck("M259-E002-META-03", "`tmp/reports/m259/M259-D003/platform_prerequisites_and_runtime_bring_up_documentation_summary.json`"),
            SnippetCheck("M259-E002-META-04", "`M259-E003`"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        args.smoke_script,
        (
            SnippetCheck("M259-E002-SMOKE-01", "M259-E002 conformance-matrix anchor:"),
            SnippetCheck("M259-E002-SMOKE-02", "command-backed row"),
            SnippetCheck("M259-E002-SMOKE-03", "unsupported runtime surfaces"),
        ),
        failures,
    )
    checks_total += 3
    checks_passed += ensure_snippets(
        args.replay_script,
        (
            SnippetCheck("M259-E002-REPLAY-01", "M259-E002 conformance-matrix anchor:"),
            SnippetCheck("M259-E002-REPLAY-02", "command-backed row"),
            SnippetCheck("M259-E002-REPLAY-03", "canonical fixture and section-inspection path"),
        ),
        failures,
    )
    checks_total += 5
    checks_passed += ensure_snippets(
        args.package_json,
        (
            SnippetCheck("M259-E002-PKG-01", '"check:objc3c:m259-e002-full-runnable-object-model-conformance-matrix":'),
            SnippetCheck("M259-E002-PKG-02", '"test:tooling:m259-e002-full-runnable-object-model-conformance-matrix":'),
            SnippetCheck("M259-E002-PKG-03", '"check:objc3c:m259-e002-lane-e-readiness":'),
            SnippetCheck("M259-E002-PKG-04", '"check:objc3c:m259-e002-lane-e-readiness": "python scripts/run_m259_e002_lane_e_readiness.py"'),
            SnippetCheck("M259-E002-PKG-05", '"test:objc3c:execution-smoke":'),
        ),
        failures,
    )

    matrix_payload = load_json(args.matrix_json)
    checks_total += 3
    checks_passed += require(matrix_payload.get("contract_id") == CONTRACT_ID, display_path(args.matrix_json), "M259-E002-MATRIX-01", "matrix contract drift", failures)
    checks_passed += require(matrix_payload.get("matrix_model") == MATRIX_MODEL, display_path(args.matrix_json), "M259-E002-MATRIX-02", "matrix model drift", failures)
    checks_passed += require(matrix_payload.get("next_issue") == NEXT_ISSUE, display_path(args.matrix_json), "M259-E002-MATRIX-03", "matrix next-issue drift", failures)

    rows = matrix_payload.get("rows")
    checks_total += 1
    checks_passed += require(isinstance(rows, list) and len(rows) >= 8, display_path(args.matrix_json), "M259-E002-MATRIX-04", "matrix must contain at least eight rows", failures)
    seen: set[str] = set()
    dependency_summaries: dict[str, dict[str, Any]] = {}
    if isinstance(rows, list):
        for index, row in enumerate(rows):
            artifact = f"{display_path(args.matrix_json)}#row[{index}]"
            checks_total += 6
            checks_passed += require(isinstance(row, dict), artifact, "M259-E002-ROW-01", "matrix row must be an object", failures)
            if not isinstance(row, dict):
                continue
            row_id = row.get("row_id")
            checks_passed += require(isinstance(row_id, str) and bool(row_id.strip()), artifact, "M259-E002-ROW-02", "row_id must be a non-empty string", failures)
            if isinstance(row_id, str) and row_id in seen:
                failures.append(Finding(artifact, "M259-E002-ROW-03", f"duplicate row_id: {row_id}"))
            elif isinstance(row_id, str):
                seen.add(row_id)
                checks_passed += 1
            has_fixture = isinstance(row.get("fixture"), str) and bool(str(row.get("fixture")).strip())
            has_probe = isinstance(row.get("probe"), str) and bool(str(row.get("probe")).strip())
            has_command = isinstance(row.get("command"), str) and bool(str(row.get("command")).strip())
            checks_passed += require(has_fixture or has_probe or has_command, artifact, "M259-E002-ROW-04", "row must name a fixture, probe, or command", failures)
            evidence = row.get("evidence_summary")
            checks_passed += require(isinstance(evidence, str) and bool(evidence.strip()), artifact, "M259-E002-ROW-05", "row must name an evidence summary path", failures)
            if has_fixture:
                checks_passed += require((ROOT / str(row["fixture"])).exists(), artifact, "M259-E002-ROW-06", f"fixture missing: {row['fixture']}", failures)
            elif has_probe:
                checks_passed += require((ROOT / str(row["probe"])).exists(), artifact, "M259-E002-ROW-06", f"probe missing: {row['probe']}", failures)
            else:
                checks_passed += 1
            if isinstance(evidence, str) and evidence.strip():
                evidence_path = ROOT / evidence
                checks_total += 2
                checks_passed += require(evidence_path.exists(), artifact, "M259-E002-ROW-07", f"evidence summary missing: {evidence}", failures)
                if evidence_path.exists():
                    evidence_payload = load_json(evidence_path)
                    dependency_summaries[evidence] = evidence_payload
                    checks_passed += require(evidence_payload.get("ok") is True, artifact, "M259-E002-ROW-08", f"evidence summary not green: {evidence}", failures)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "matrix_model": MATRIX_MODEL,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "row_count": len(rows) if isinstance(rows, list) else 0,
        "failures": [finding.__dict__ for finding in failures],
        "dependency_summaries": {path: {"contract_id": payload.get("contract_id"), "ok": payload.get("ok")} for path, payload in dependency_summaries.items()},
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
