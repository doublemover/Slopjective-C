#!/usr/bin/env python3
"""Fail-closed contract checker for M252-B003 category ambiguity diagnostics."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-b003-category-attachment-duplication-ambiguity-diagnostics-v1"
CONTRACT_ID = "objc3c-runtime-export-diagnostics/m252-b003-v1"
SOURCE_GRAPH_CONTRACT_ID = "objc3c-executable-metadata-source-graph-completeness/m252-a002-v1"
SEMANTIC_CONSISTENCY_CONTRACT_ID = "objc3c-executable-metadata-semantic-consistency-freeze/m252-b001-v1"
SEMANTIC_VALIDATION_CONTRACT_ID = "objc3c-executable-metadata-semantic-validation/m252-b002-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m252_category_attachment_duplication_ambiguity_diagnostics_b003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m252" / "m252_b003_category_attachment_duplication_ambiguity_diagnostics_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
DEFAULT_SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_VALID_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b003_valid_category_attachment.objc3"
DEFAULT_COLLISION_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b003_category_attachment_collision.objc3"
DEFAULT_AMBIGUITY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b003_duplicate_runtime_member_ambiguity.objc3"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "b003-category-diagnostics"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-B003/category_attachment_duplication_ambiguity_diagnostics_summary.json")


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
    SnippetCheck("M252-B003-DOC-EXP-01", "# M252 Category Attachment Duplication Ambiguity Diagnostics Expectations (B003)"),
    SnippetCheck("M252-B003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M252-B003-DOC-EXP-03", "`O3S261`"),
    SnippetCheck("M252-B003-DOC-EXP-04", "`O3S262`"),
    SnippetCheck("M252-B003-DOC-EXP-05", "`O3S263`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M252-B003-DOC-PKT-01", "# M252-B003 Category Attachment Duplication Ambiguity Diagnostics Packet"),
    SnippetCheck("M252-B003-DOC-PKT-02", "Packet: `M252-B003`"),
    SnippetCheck("M252-B003-DOC-PKT-03", "- `M252-B002`"),
    SnippetCheck("M252-B003-DOC-PKT-04", "Duplicate runtime members emit `O3S262`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M252-B003-ARCH-01", "M252 lane-B B003 category attachment, duplication, and ambiguity diagnostics"),
    SnippetCheck("M252-B003-ARCH-02", "class-plus-category input, category attachment collisions, duplicate runtime"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M252-B003-NDOC-01", "## Category attachment duplication ambiguity diagnostics (M252-B003)"),
    SnippetCheck("M252-B003-NDOC-02", "`O3S261` for category attachment collisions"),
    SnippetCheck("M252-B003-NDOC-03", "`tmp/reports/m252/M252-B003/category_attachment_duplication_ambiguity_diagnostics_summary.json`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M252-B003-SPC-01", "## M252 category attachment duplication ambiguity diagnostics (B003)"),
    SnippetCheck("M252-B003-SPC-02", "`O3S261` for category attachment collisions"),
    SnippetCheck("M252-B003-SPC-03", "`O3S263` for ambiguous runtime metadata graph resolution"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M252-B003-META-01", "## M252 category attachment duplication ambiguity diagnostics metadata anchors (B003)"),
    SnippetCheck("M252-B003-META-02", "the runtime export blocker diagnostics `O3S261`, `O3S262`, and `O3S263`"),
    SnippetCheck("M252-B003-META-03", "`tmp/reports/m252/M252-B003/category_attachment_duplication_ambiguity_diagnostics_summary.json`"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M252-B003-PARSE-01", "M252-B003 diagnostic precision anchor: category owners keep a canonical"),
    SnippetCheck("M252-B003-PARSE-02", "M252-B003 diagnostic precision anchor: category implementation owners"),
)
SEMA_CONTRACT_SNIPPETS = (
    SnippetCheck("M252-B003-SEMA-01", "M252-B003 diagnostic precision anchor: these maps model class containers"),
)
SEMA_PASSES_SNIPPETS = (
    SnippetCheck("M252-B003-PASS-01", "M252-B003 diagnostic precision anchor: class interface/implementation"),
)
FRONTEND_PIPELINE_SNIPPETS = (
    SnippetCheck("M252-B003-PIPE-01", "M252-B003 diagnostic precision anchor: the semantic type-metadata handoff"),
    SnippetCheck("M252-B003-PIPE-02", '"O3S261"'),
    SnippetCheck("M252-B003-PIPE-03", '"O3S262"'),
    SnippetCheck("M252-B003-PIPE-04", '"O3S263"'),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M252-B003-ART-01", "M252-B003 diagnostic precision anchor: manifest evidence must"),
)
VALID_FIXTURE_SNIPPETS = (
    SnippetCheck("M252-B003-FIX-VALID-01", "module m252ValidCategoryAttachment;"),
    SnippetCheck("M252-B003-FIX-VALID-02", "@interface Root (Extras)"),
    SnippetCheck("M252-B003-FIX-VALID-03", "- (i32) extra {"),
)
COLLISION_FIXTURE_SNIPPETS = (
    SnippetCheck("M252-B003-FIX-COLLISION-01", "module m252CategoryAttachmentCollision;"),
    SnippetCheck("M252-B003-FIX-COLLISION-02", "@interface Root (Extras)"),
    SnippetCheck("M252-B003-FIX-COLLISION-03", "- (i32) extraAgain;"),
)
AMBIGUITY_FIXTURE_SNIPPETS = (
    SnippetCheck("M252-B003-FIX-AMBIGUITY-01", "module m252DuplicateRuntimeMemberAmbiguity;"),
    SnippetCheck("M252-B003-FIX-AMBIGUITY-02", "@implementation Root (Extras)"),
    SnippetCheck("M252-B003-FIX-AMBIGUITY-03", "return 3;"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M252-B003-PKG-01", '"check:objc3c:m252-b003-category-attachment-duplication-ambiguity-diagnostics": "python scripts/check_m252_b003_category_attachment_duplication_ambiguity_diagnostics.py"'),
    SnippetCheck("M252-B003-PKG-02", '"test:tooling:m252-b003-category-attachment-duplication-ambiguity-diagnostics": "python -m pytest tests/tooling/test_check_m252_b003_category_attachment_duplication_ambiguity_diagnostics.py -q"'),
    SnippetCheck("M252-B003-PKG-03", '"check:objc3c:m252-b003-lane-b-readiness": "npm run check:objc3c:m252-b002-lane-b-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-b003-category-attachment-duplication-ambiguity-diagnostics && npm run test:tooling:m252-b003-category-attachment-duplication-ambiguity-diagnostics"'),
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-contract", dest="sema_contract", type=Path, default=DEFAULT_SEMA_CONTRACT)
    parser.add_argument("--sema-passes", type=Path, default=DEFAULT_SEMA_PASSES)
    parser.add_argument("--frontend-pipeline", type=Path, default=DEFAULT_FRONTEND_PIPELINE)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--valid-fixture", type=Path, default=DEFAULT_VALID_FIXTURE)
    parser.add_argument("--collision-fixture", type=Path, default=DEFAULT_COLLISION_FIXTURE)
    parser.add_argument("--ambiguity-fixture", type=Path, default=DEFAULT_AMBIGUITY_FIXTURE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-runner-probes", action="store_true")
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required artifact is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
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


def run_runner_case(
    *,
    runner_exe: Path,
    fixture_path: Path,
    out_dir: Path,
    expected_success: bool,
    expected_status: int,
    expected_codes: list[str],
) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0

    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-B003-FIXTURE-EXISTS", "fixture is missing", findings)
    if findings:
        return checks_total, findings, None

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
    completed = subprocess.run(command, capture_output=True, text=True, check=False)
    summary_path = out_dir / "module.c_api_summary.json"
    diagnostics_path = out_dir / "module.diagnostics.json"

    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-B003-RUNNER-SUMMARY", "frontend C API runner did not write module.c_api_summary.json", findings)
    checks_total += require(diagnostics_path.exists(), display_path(diagnostics_path), "M252-B003-RUNNER-DIAGNOSTICS", "frontend C API runner did not write module.diagnostics.json", findings)
    if findings:
        return checks_total, findings, None

    summary_payload = load_json(summary_path)
    diagnostics_payload = load_json(diagnostics_path)
    diagnostics = diagnostics_payload.get("diagnostics")
    checks_total += require(isinstance(diagnostics, list), display_path(diagnostics_path), "M252-B003-DIAGNOSTICS-LIST", "diagnostics payload must contain a diagnostics list", findings)
    if not isinstance(diagnostics, list):
        return checks_total, findings, None

    observed_codes = [entry.get("code") for entry in diagnostics if isinstance(entry, dict)]
    checks_total += require(summary_payload.get("success") is expected_success, display_path(summary_path), "M252-B003-RUNNER-SUCCESS", f"runner success mismatch for {display_path(fixture_path)}", findings)
    checks_total += require(summary_payload.get("status") == expected_status, display_path(summary_path), "M252-B003-RUNNER-STATUS", f"runner status mismatch for {display_path(fixture_path)}", findings)
    checks_total += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M252-B003-SEMANTIC-SKIPPED", "semantic stage unexpectedly skipped", findings)
    checks_total += require(completed.returncode == expected_status, display_path(summary_path), "M252-B003-PROCESS-EXIT", f"runner process exit mismatch for {display_path(fixture_path)}", findings)
    checks_total += require(observed_codes == expected_codes, display_path(diagnostics_path), "M252-B003-DIAGNOSTIC-CODES", f"diagnostic code sequence mismatch: expected {expected_codes}, observed {observed_codes}", findings)

    case_payload: dict[str, object] = {
        "fixture": display_path(fixture_path),
        "summary_path": display_path(summary_path),
        "diagnostics_path": display_path(diagnostics_path),
        "observed_codes": observed_codes,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }

    if expected_success:
        manifest_rel = summary_payload.get("paths", {}).get("manifest")
        manifest_path: Path | None = None
        if isinstance(manifest_rel, str) and manifest_rel:
            manifest_candidate = Path(manifest_rel)
            manifest_path = manifest_candidate if manifest_candidate.is_absolute() else ROOT / manifest_candidate
        checks_total += require(manifest_path is not None, display_path(summary_path), "M252-B003-MANIFEST-PATH", "successful runner summary must report a manifest path", findings)
        if manifest_path is None:
            return checks_total, findings, case_payload
        checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-B003-MANIFEST-EXISTS", "successful runner manifest path is missing", findings)
        checks_total += require(observed_codes == [], display_path(diagnostics_path), "M252-B003-VALID-DIAGNOSTICS", "happy-path diagnostics must be empty", findings)
        if findings and any(f.check_id == "M252-B003-MANIFEST-EXISTS" for f in findings):
            return checks_total, findings, case_payload
        manifest_payload = load_json(manifest_path)
        semantic_surface = manifest_payload.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
        source_graph = semantic_surface.get("objc_executable_metadata_source_graph", {})
        consistency_boundary = semantic_surface.get("objc_executable_metadata_semantic_consistency_boundary", {})
        validation_surface = semantic_surface.get("objc_executable_metadata_semantic_validation_surface", {})
        checks_total += require(isinstance(source_graph, dict), display_path(manifest_path), "M252-B003-SOURCE-GRAPH", "manifest is missing the executable metadata source graph", findings)
        checks_total += require(isinstance(consistency_boundary, dict), display_path(manifest_path), "M252-B003-CONSISTENCY-BOUNDARY", "manifest is missing the semantic consistency boundary", findings)
        checks_total += require(isinstance(validation_surface, dict), display_path(manifest_path), "M252-B003-VALIDATION-SURFACE", "manifest is missing the semantic validation surface", findings)
        if not all(isinstance(node, dict) for node in (source_graph, consistency_boundary, validation_surface)):
            return checks_total, findings, case_payload
        checks_total += require(source_graph.get("contract_id") == SOURCE_GRAPH_CONTRACT_ID, display_path(manifest_path), "M252-B003-SOURCE-GRAPH-CONTRACT", "source graph contract id mismatch", findings)
        checks_total += require(consistency_boundary.get("contract_id") == SEMANTIC_CONSISTENCY_CONTRACT_ID, display_path(manifest_path), "M252-B003-CONSISTENCY-CONTRACT", "semantic consistency contract id mismatch", findings)
        checks_total += require(validation_surface.get("contract_id") == SEMANTIC_VALIDATION_CONTRACT_ID, display_path(manifest_path), "M252-B003-VALIDATION-CONTRACT", "semantic validation contract id mismatch", findings)
        checks_total += require(source_graph.get("class_nodes") == 1, display_path(manifest_path), "M252-B003-CLASS-NODES", "happy path must preserve one class node", findings)
        checks_total += require(source_graph.get("category_nodes") == 1, display_path(manifest_path), "M252-B003-CATEGORY-NODES", "happy path must preserve one category node", findings)
        checks_total += require(consistency_boundary.get("ready") is True, display_path(manifest_path), "M252-B003-CONSISTENCY-READY", "semantic consistency boundary must remain ready", findings)
        checks_total += require(consistency_boundary.get("category_attachment_edges_complete") is True, display_path(manifest_path), "M252-B003-CATEGORY-EDGES", "category attachment edges must remain complete", findings)
        checks_total += require(validation_surface.get("ready") is True, display_path(manifest_path), "M252-B003-VALIDATION-READY", "semantic validation surface must remain ready", findings)
        case_payload["manifest_path"] = display_path(manifest_path)
    else:
        expected_phrases = {
            "O3S261": "category attachment collision",
            "O3S262": "duplicate runtime member",
            "O3S263": "ambiguous runtime metadata graph resolution",
        }
        for index, code in enumerate(expected_codes):
            checks_total += require(index < len(diagnostics), display_path(diagnostics_path), f"M252-B003-{code}-INDEX", f"diagnostic list missing index {index} for {code}", findings)
            if index >= len(diagnostics):
                continue
            entry = diagnostics[index]
            message = entry.get("message") if isinstance(entry, dict) else None
            raw = entry.get("raw") if isinstance(entry, dict) else None
            checks_total += require(isinstance(message, str) and expected_phrases[code] in message, display_path(diagnostics_path), f"M252-B003-{code}-MESSAGE", f"{code} diagnostic message is missing the expected phrase", findings)
            checks_total += require(isinstance(raw, str) and code in raw, display_path(diagnostics_path), f"M252-B003-{code}-RAW", f"{code} diagnostic raw text is missing its code", findings)
        if expected_codes == ["O3S261", "O3S263"]:
            checks_total += require(any("multiple @interface declarations" in (entry.get("message") or "") for entry in diagnostics if isinstance(entry, dict)), display_path(diagnostics_path), "M252-B003-COLLISION-INTERFACE", "collision fixture must report duplicate category interface declarations", findings)
        if expected_codes == ["O3S261", "O3S263", "O3S262"]:
            checks_total += require(any("multiple @implementation declarations" in (entry.get("message") or "") for entry in diagnostics if isinstance(entry, dict)), display_path(diagnostics_path), "M252-B003-AMBIGUITY-IMPLEMENTATION", "ambiguity fixture must report duplicate category implementations", findings)
            checks_total += require(any("instance selector 'extra'" in (entry.get("message") or "") for entry in diagnostics if isinstance(entry, dict)), display_path(diagnostics_path), "M252-B003-AMBIGUITY-SELECTOR", "ambiguity fixture must report the duplicate runtime selector", findings)

    return checks_total, findings, case_payload


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    static_checks = (
        (args.expectations_doc, "M252-B003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-B003-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-B003-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-B003-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-B003-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-B003-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-B003-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-B003-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-B003-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_pipeline, "M252-B003-PIPE-EXISTS", FRONTEND_PIPELINE_SNIPPETS),
        (args.frontend_artifacts, "M252-B003-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.valid_fixture, "M252-B003-FIX-VALID-EXISTS", VALID_FIXTURE_SNIPPETS),
        (args.collision_fixture, "M252-B003-FIX-COLLISION-EXISTS", COLLISION_FIXTURE_SNIPPETS),
        (args.ambiguity_fixture, "M252-B003-FIX-AMBIGUITY-EXISTS", AMBIGUITY_FIXTURE_SNIPPETS),
        (args.package_json, "M252-B003-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in static_checks:
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    runner_cases: dict[str, object] = {}
    if not args.skip_runner_probes:
        checks_total += 1
        if not args.runner_exe.exists():
            failures.append(Finding(display_path(args.runner_exe), "M252-B003-RUNNER-EXISTS", "frontend C API runner binary is missing; run npm run build:objc3c-native"))
        else:
            case_specs = (
                ("valid_category_attachment", args.valid_fixture, True, 0, []),
                ("category_attachment_collision", args.collision_fixture, False, 1, ["O3S261", "O3S263"]),
                ("duplicate_runtime_member_ambiguity", args.ambiguity_fixture, False, 1, ["O3S261", "O3S263", "O3S262"]),
            )
            for case_name, fixture_path, expected_success, expected_status, expected_codes in case_specs:
                count, findings, case_payload = run_runner_case(
                    runner_exe=args.runner_exe,
                    fixture_path=fixture_path,
                    out_dir=args.probe_root / case_name,
                    expected_success=expected_success,
                    expected_status=expected_status,
                    expected_codes=expected_codes,
                )
                checks_total += count
                failures.extend(findings)
                if case_payload is not None:
                    runner_cases[case_name] = case_payload

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "runner_probes_executed": not args.skip_runner_probes,
        "runner_cases": runner_cases,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }
    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
