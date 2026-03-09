#!/usr/bin/env python3
"""Fail-closed checker for M264-A001 runnable feature-claim inventory."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m264-a001-runnable-feature-claim-inventory-v1"
CONTRACT_ID = "objc3c-runnable-feature-claim-inventory/m264-a001-v1"
EXPECTED_LANGUAGE_MODE = "objc3-v1-native-subset"
EXPECTED_TRUTH_MODEL = "truthful-runnable-subset-plus-source-only-plus-fail-closed-unsupported"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-A001" / "runnable_feature_claim_inventory_and_mode_truth_surface_summary.json"
FIXTURE_HELLO = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
FIXTURE_METADATA = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"

EXPECTED_RUNNABLE = [
    "runnable:module-declaration",
    "runnable:global-let",
    "runnable:function-bodies",
    "runnable:extern-prototypes",
    "runnable:scalar-core",
    "runnable:control-flow",
    "runnable:message-send-basic",
]
EXPECTED_SOURCE_ONLY = [
    "source-only:protocol-declarations",
    "source-only:interface-declarations",
    "source-only:implementation-declarations",
    "source-only:category-declarations",
    "source-only:property-declarations",
    "source-only:object-pointer-nullability-generics",
]
EXPECTED_UNSUPPORTED = [
    "unsupported:strictness-selection",
    "unsupported:strict-concurrency-selection",
    "unsupported:throws",
    "unsupported:async-await",
    "unsupported:actors",
    "unsupported:blocks",
    "unsupported:arc",
]


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_runnable_feature_claim_inventory_and_mode_truth_surface_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_a001_runnable_feature_claim_inventory_and_mode_truth_surface_contract_and_architecture_freeze_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=EXPECTED_SUMMARY)
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=ROOT / "artifacts" / "bin" / "objc3c-native.exe")
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M264-A001-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(ROOT),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def inventory_from_manifest(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {})
    semantic_surface = pipeline.get("semantic_surface", {})
    inventory = semantic_surface.get("objc_runnable_feature_claim_inventory")
    if not isinstance(inventory, dict):
      raise TypeError(f"missing feature-claim inventory in {display_path(manifest_path)}")
    return inventory


def validate_inventory(
    inventory: dict[str, Any],
    artifact: str,
    expected_mode: str,
    expected_migration_assist: bool,
    failures: list[Finding],
) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    runnable = inventory.get("runnable_feature_claim_ids")
    source_only = inventory.get("source_only_feature_claim_ids")
    unsupported = inventory.get("unsupported_feature_claim_ids")

    checks_total += 1
    checks_passed += require(inventory.get("contract_id") == CONTRACT_ID, artifact, "M264-A001-INV-01", "contract id drifted", failures)
    checks_total += 1
    checks_passed += require(inventory.get("effective_language_mode") == EXPECTED_LANGUAGE_MODE, artifact, "M264-A001-INV-02", "effective language mode drifted", failures)
    checks_total += 1
    checks_passed += require(inventory.get("effective_language_version") == 3, artifact, "M264-A001-INV-03", "language version must stay 3", failures)
    checks_total += 1
    checks_passed += require(inventory.get("effective_compatibility_mode") == expected_mode, artifact, "M264-A001-INV-04", "compatibility mode mismatch", failures)
    checks_total += 1
    checks_passed += require(inventory.get("migration_assist_enabled") is expected_migration_assist, artifact, "M264-A001-INV-05", "migration assist truth mismatch", failures)
    checks_total += 1
    checks_passed += require(inventory.get("strictness_selection_supported") is False, artifact, "M264-A001-INV-06", "strictness must remain unsupported", failures)
    checks_total += 1
    checks_passed += require(inventory.get("strict_concurrency_mode_supported") is False, artifact, "M264-A001-INV-07", "strict concurrency must remain unsupported", failures)
    checks_total += 1
    checks_passed += require(inventory.get("mode_truth_fail_closed") is True, artifact, "M264-A001-INV-08", "mode truth must stay fail-closed", failures)
    checks_total += 1
    checks_passed += require(inventory.get("truth_model") == EXPECTED_TRUTH_MODEL, artifact, "M264-A001-INV-09", "truth model drifted", failures)
    checks_total += 1
    checks_passed += require(runnable == EXPECTED_RUNNABLE, artifact, "M264-A001-INV-10", "runnable claim ids drifted", failures)
    checks_total += 1
    checks_passed += require(source_only == EXPECTED_SOURCE_ONLY, artifact, "M264-A001-INV-11", "source-only claim ids drifted", failures)
    checks_total += 1
    checks_passed += require(unsupported == EXPECTED_UNSUPPORTED, artifact, "M264-A001-INV-12", "unsupported claim ids drifted", failures)
    checks_total += 1
    checks_passed += require(inventory.get("runnable_feature_claim_count") == len(EXPECTED_RUNNABLE), artifact, "M264-A001-INV-13", "runnable claim count mismatch", failures)
    checks_total += 1
    checks_passed += require(inventory.get("source_only_feature_claim_count") == len(EXPECTED_SOURCE_ONLY), artifact, "M264-A001-INV-14", "source-only claim count mismatch", failures)
    checks_total += 1
    checks_passed += require(inventory.get("unsupported_feature_claim_count") == len(EXPECTED_UNSUPPORTED), artifact, "M264-A001-INV-15", "unsupported claim count mismatch", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    build = run_command(["npm.cmd", "run", "build:objc3c-native"])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "npm run build:objc3c-native", "M264-A001-DYN-01", f"native build failed: {build.stderr or build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M264-A001-DYN-02", "frontend runner missing after build", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M264-A001-DYN-03", "native executable missing after build", failures)

    case_specs = [
        {
            "case_id": "hello-canonical-native",
            "fixture": FIXTURE_HELLO,
            "extra_args": [],
            "expected_mode": "canonical",
            "expected_migration_assist": False,
            "tool": "native",
        },
        {
            "case_id": "hello-legacy-migration-runner",
            "fixture": FIXTURE_HELLO,
            "extra_args": ["--objc3-compat-mode", "legacy", "--objc3-migration-assist"],
            "expected_mode": "legacy",
            "expected_migration_assist": True,
            "tool": "runner",
        },
        {
            "case_id": "metadata-canonical-runner",
            "fixture": FIXTURE_METADATA,
            "extra_args": [],
            "expected_mode": "canonical",
            "expected_migration_assist": False,
            "tool": "runner",
        },
    ]
    cases: list[dict[str, Any]] = []
    for spec in case_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "a001" / spec["case_id"]
        out_dir.mkdir(parents=True, exist_ok=True)
        if spec["tool"] == "runner":
            command = [
                str(args.runner_exe),
                str(spec["fixture"]),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                "--no-emit-ir",
                "--no-emit-object",
                *spec["extra_args"],
            ]
            manifest_path = out_dir / "module.manifest.json"
        else:
            command = [
                str(args.native_exe),
                str(spec["fixture"]),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                *spec["extra_args"],
            ]
            manifest_path = out_dir / "module.manifest.json"
        run = run_command(command)
        artifact = spec["case_id"]
        checks_total += 1
        checks_passed += require(run.returncode == 0, artifact, "M264-A001-DYN-04", f"compile failed: {run.stderr or run.stdout}", failures)
        checks_total += 1
        checks_passed += require(manifest_path.exists(), artifact, "M264-A001-DYN-05", "manifest missing", failures)
        if manifest_path.exists():
            inventory = inventory_from_manifest(manifest_path)
            sub_total, sub_passed = validate_inventory(
                inventory,
                artifact,
                spec["expected_mode"],
                spec["expected_migration_assist"],
                failures,
            )
            checks_total += sub_total
            checks_passed += sub_passed
            if spec["tool"] == "native":
                ir_path = out_dir / "module.ll"
                obj_path = out_dir / "module.obj"
                checks_total += 1
                checks_passed += require(ir_path.exists(), artifact, "M264-A001-DYN-06", "native runnable probe must emit IR", failures)
                checks_total += 1
                checks_passed += require(obj_path.exists(), artifact, "M264-A001-DYN-07", "native runnable probe must emit object output", failures)
            if spec["case_id"] == "metadata-canonical-runner":
                checks_total += 1
                checks_passed += require(inventory.get("declared_protocol_count", 0) > 0, artifact, "M264-A001-DYN-08", "metadata fixture must prove protocol source coverage", failures)
                checks_total += 1
                checks_passed += require(inventory.get("declared_interface_count", 0) > 0, artifact, "M264-A001-DYN-09", "metadata fixture must prove interface source coverage", failures)
                checks_total += 1
                checks_passed += require(inventory.get("declared_implementation_count", 0) > 0, artifact, "M264-A001-DYN-10", "metadata fixture must prove implementation source coverage", failures)
            cases.append(
                {
                    "case_id": spec["case_id"],
                    "tool": spec["tool"],
                    "fixture": display_path(spec["fixture"]),
                    "manifest_path": display_path(manifest_path),
                    "effective_compatibility_mode": inventory.get("effective_compatibility_mode"),
                    "migration_assist_enabled": inventory.get("migration_assist_enabled"),
                    "declared_protocol_count": inventory.get("declared_protocol_count"),
                    "declared_interface_count": inventory.get("declared_interface_count"),
                    "declared_implementation_count": inventory.get("declared_implementation_count"),
                }
            )
    return checks_total, checks_passed, {"cases": cases}


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M264-A001-DOC-01", "# M264 Runnable Feature-Claim Inventory And Mode Truth Surface Contract And Architecture Freeze Expectations (A001)"),
            SnippetCheck("M264-A001-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M264-A001-DOC-03", "frontend.pipeline.semantic_surface.objc_runnable_feature_claim_inventory"),
            SnippetCheck("M264-A001-DOC-04", "artifacts/bin/objc3c-native.exe"),
            SnippetCheck("M264-A001-DOC-05", "artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object"),
        ],
        PACKET_DOC: [
            SnippetCheck("M264-A001-PKT-01", "# M264-A001 Runnable Feature-Claim Inventory And Mode Truth Surface Contract And Architecture Freeze Packet"),
            SnippetCheck("M264-A001-PKT-02", "Dependencies:"),
            SnippetCheck("M264-A001-PKT-03", "- `M263-E002`"),
            SnippetCheck("M264-A001-PKT-04", "artifacts/bin/objc3c-native.exe"),
            SnippetCheck("M264-A001-PKT-05", "artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M264-A001-NDOC-01", "## Runnable feature-claim inventory (M264-A001)"),
            SnippetCheck("M264-A001-NDOC-02", "`frontend.pipeline.semantic_surface.objc_runnable_feature_claim_inventory`"),
            SnippetCheck("M264-A001-NDOC-03", "strictness selection: not yet supported"),
            SnippetCheck("M264-A001-NDOC-04", "artifacts/bin/objc3c-native.exe"),
            SnippetCheck("M264-A001-NDOC-05", "artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M264-A001-SPC-01", "## M264 frontend claim truth packet (implementation note)"),
            SnippetCheck("M264-A001-SPC-02", "objc_runnable_feature_claim_inventory"),
        ],
        SPEC_DECISIONS: [
            SnippetCheck("M264-A001-DEC-01", "## D-016: Conformance and version claims are bounded by one live frontend inventory"),
            SnippetCheck("M264-A001-DEC-02", "source-only recognized claims"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M264-A001-TOK-01", "kObjc3RunnableFeatureClaimInventoryContractId"),
            SnippetCheck("M264-A001-TOK-02", "kObjc3RunnableFeatureClaimTruthModel"),
        ],
        LEXER_CPP: [
            SnippetCheck("M264-A001-LEX-01", "M264-A001 mode-truth source anchor"),
            SnippetCheck("M264-A001-LEX-02", "runnable feature-claim inventory emitted by the frontend manifest"),
        ],
        PARSER_CPP: [
            SnippetCheck("M264-A001-PARSE-01", "M264-A001 source/frontend-truth anchor"),
            SnippetCheck("M264-A001-PARSE-02", "must not invent a second frontend source-of-truth surface"),
        ],
        ARTIFACTS_CPP: [
            SnippetCheck("M264-A001-ART-01", "objc_runnable_feature_claim_inventory"),
            SnippetCheck("M264-A001-ART-02", "BuildRunnableFeatureClaimInventoryJson"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M264-A001-PKG-01", "\"check:objc3c:m264-a001-runnable-feature-claim-inventory-and-mode-truth-surface\""),
            SnippetCheck("M264-A001-PKG-02", "\"check:objc3c:m264-a001-lane-a-readiness\""),
        ],
    }
    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    dynamic_summary: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if args.skip_dynamic_probes:
        checks_total += 1
        checks_passed += 1
    else:
        sub_total, sub_passed, dynamic_summary = run_dynamic_probes(args, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "truth_model": EXPECTED_TRUTH_MODEL,
        "dynamic_summary": dynamic_summary,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[FAIL] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
