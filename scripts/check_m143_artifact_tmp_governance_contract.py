#!/usr/bin/env python3
"""Fail-closed validator for M143 deterministic artifact/tmp-path governance."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m143-artifact-tmp-governance-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "driver_header": ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h",
    "compile_wrapper": ROOT / "scripts" / "objc3c_native_compile.ps1",
    "c_api_runner": ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp",
    "parity_script": ROOT / "scripts" / "check_objc3c_library_cli_parity.py",
    "sema_tmp_governance_script": ROOT / "scripts" / "check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1",
    "lowering_tmp_governance_script": ROOT / "scripts" / "run_objc3c_lowering_regression_suite.ps1",
    "typed_abi_tmp_governance_script": ROOT / "scripts" / "check_objc3c_typed_abi_replay_proof.ps1",
    "lowering_replay_tmp_governance_script": ROOT / "scripts" / "check_objc3c_lowering_replay_proof.ps1",
    "parity_test": ROOT / "tests" / "tooling" / "test_objc3c_library_cli_parity.py",
    "c_api_runner_test": ROOT / "tests" / "tooling" / "test_objc3c_c_api_runner_extraction.py",
    "driver_cli_test": ROOT / "tests" / "tooling" / "test_objc3c_driver_cli_extraction.py",
    "lane_c_tmp_governance_test": ROOT
    / "tests"
    / "tooling"
    / "test_objc3c_m143_lowering_runtime_abi_tmp_governance_contract.py",
    "cli_fragment": ROOT / "docs" / "objc3c-native" / "src" / "10-cli.md",
    "semantics_fragment": ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md",
    "artifacts_fragment": ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md",
    "tests_fragment": ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md",
    "contract_doc": ROOT / "docs" / "contracts" / "artifact_tmp_governance_expectations.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "driver_header": (
        ("M143-TMP-01", 'std::filesystem::path("tmp") / "artifacts" / "compilation" / "objc3c-native"'),
    ),
    "compile_wrapper": (
        ("M143-TMP-01B", '$defaultOutDir = "tmp/artifacts/compilation/objc3c-native"'),
    ),
    "c_api_runner": (
        ("M143-TMP-02", 'fs::path("tmp") / "artifacts" / "compilation" / "objc3c-native"'),
    ),
    "parity_script": (
        ("M143-TMP-03A", 'default=Path("tmp/artifacts/compilation/objc3c-native/library-cli-parity/work")'),
        ("M143-TMP-03B", 'WORK_KEY_PATTERN = re.compile(r"^[A-Za-z0-9][A-Za-z0-9._-]{0,63}$")'),
        ("M143-TMP-03C", "ensure_under_tmp(work_dir, label=\"work-dir\")"),
        ("M143-TMP-03D", "assert_no_stale_source_mode_outputs("),
        ("M143-TMP-03E", "contains stale generated artifacts; choose a unique --work-key"),
        ("M143-TMP-03F", "args.emit_prefix = normalize_artifact_name("),
    ),
    "sema_tmp_governance_script": (
        ("M143-TMP-06A", '$suiteRoot = Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract"'),
        ("M143-TMP-06B", '$defaultRunId = "m143-sema-type-system-default"'),
        ("M143-TMP-06C", "return $DefaultRunId"),
        ("M143-TMP-06D", '$runDirRel = "tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/$runId"'),
    ),
    "lowering_tmp_governance_script": (
        ("M143-TMP-07A", '$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/lowering-regression"'),
        ("M143-TMP-07B", '$defaultRunId = "m143-lane-c-lowering-regression-default"'),
        ("M143-TMP-07C", "$configuredRunId = $env:OBJC3C_NATIVE_LOWERING_RUN_ID"),
        ("M143-TMP-07D", "$runId = Resolve-ValidatedRunId -ConfiguredRunId $configuredRunId -DefaultRunId $defaultRunId"),
    ),
    "typed_abi_tmp_governance_script": (
        ("M143-TMP-08A", '$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/typed-abi-replay-proof"'),
        ("M143-TMP-08B", '$defaultRunId = "m143-lane-c-typed-abi-default"'),
        ("M143-TMP-08C", "$configuredRunId = $env:OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID"),
        ("M143-TMP-08D", "$runId = Resolve-ValidatedRunId -ConfiguredRunId $configuredRunId -DefaultRunId $defaultRunId"),
    ),
    "lowering_replay_tmp_governance_script": (
        ("M143-TMP-09A", '$proofRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/lowering-replay-proof"'),
        ("M143-TMP-09B", '$defaultProofRunId = "m143-lane-c-lowering-replay-proof-default"'),
        ("M143-TMP-09C", "$configuredProofRunId = $env:OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID"),
        ("M143-TMP-09D", "-FailurePrefix \"lowering replay proof\""),
    ),
    "parity_test": (
        ("M143-TST-01", "test_parity_source_mode_default_work_key_is_deterministic_for_same_inputs"),
        ("M143-TST-02", "test_parity_source_mode_rejects_invalid_emit_prefix"),
        ("M143-TST-03", "test_parity_source_mode_fails_when_stale_proxy_outputs_exist"),
    ),
    "c_api_runner_test": (
        ("M143-TST-04", 'fs::path("tmp") / "artifacts" / "compilation" / "objc3c-native"'),
    ),
    "driver_cli_test": (
        ("M143-TST-05", "test_cli_default_out_dir_is_tmp_governed"),
    ),
    "lane_c_tmp_governance_test": (
        ("M143-TST-06", "test_lane_c_scripts_default_run_ids_are_deterministic_and_tmp_governed"),
        ("M143-TST-07", "test_lane_c_docs_and_contract_publish_deterministic_tmp_path_rules"),
        ("M143-TST-08", "test_m143_governance_test_script_includes_lane_c_contract_tests"),
    ),
    "cli_fragment": (
        ("M143-DOC-CLI-01", "Default `--out-dir`: `tmp/artifacts/compilation/objc3c-native`"),
        ("M143-DOC-CLI-02", "## Artifact tmp-path governance (M143-D001)"),
    ),
    "semantics_fragment": (
        ("M143-DOC-SEM-01", "## Artifact tmp-path governance contract (M143-D001)"),
        ("M143-DOC-SEM-02", "deterministic `--work-key`"),
        ("M143-DOC-SEM-03", "Parser/AST-facing lane-A closeout coverage"),
        ("M143-DOC-SEM-04", "m143-sema-type-system-default"),
        ("M143-DOC-SEM-05", "Lowering/LLVM IR/runtime-ABI lane-C closeout coverage"),
        ("M143-DOC-SEM-06", "m143-lane-c-lowering-regression-default"),
        ("M143-DOC-SEM-07", "m143-lane-c-typed-abi-default"),
        ("M143-DOC-SEM-08", "m143-lane-c-lowering-replay-proof-default"),
    ),
    "artifacts_fragment": (
        ("M143-DOC-ART-01", "## Artifact tmp-path governance artifacts (M143-D001)"),
        ("M143-DOC-ART-02", "npm run check:objc3c:library-cli-parity:source:m143"),
        ("M143-DOC-ART-03", "tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json"),
        ("M143-DOC-ART-04", "tmp/artifacts/objc3c-native/lowering-regression/<run_id>/summary.json"),
        ("M143-DOC-ART-05", "tmp/artifacts/objc3c-native/typed-abi-replay-proof/<run_id>/summary.json"),
        ("M143-DOC-ART-06", "tmp/artifacts/objc3c-native/lowering-replay-proof/<proof_run_id>/summary.json"),
    ),
    "tests_fragment": (
        ("M143-DOC-TST-01", "npm run test:objc3c:m143-artifact-governance"),
        ("M143-DOC-TST-02", "npm run check:compiler-closeout:m143"),
        ("M143-DOC-TST-03", "tests/tooling/test_objc3c_parser_extraction.py"),
        ("M143-DOC-TST-04", "tests/tooling/test_objc3c_parser_ast_builder_extraction.py"),
        ("M143-DOC-TST-05", "m143-sema-type-system-default"),
        ("M143-DOC-TST-06", "tests/tooling/test_objc3c_m143_lowering_runtime_abi_tmp_governance_contract.py"),
        ("M143-DOC-TST-07", "OBJC3C_NATIVE_LOWERING_RUN_ID"),
        ("M143-DOC-TST-08", "OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID"),
        ("M143-DOC-TST-09", "OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID"),
    ),
    "contract_doc": (
        ("M143-DOC-CON-01", "Contract ID: `objc3c-artifact-tmp-governance-contract/m143-v1`"),
        ("M143-DOC-CON-02", "| `M143-A001` |"),
        ("M143-DOC-CON-03", "test_objc3c_parser_extraction.py"),
        ("M143-DOC-CON-04", "test_objc3c_parser_ast_builder_extraction.py"),
        ("M143-DOC-CON-05", "| `M143-B001` |"),
        ("M143-DOC-CON-06", "m143-sema-type-system-default"),
        ("M143-DOC-CON-07", "| `M143-C001` |"),
        ("M143-DOC-CON-08", "m143-lane-c-lowering-regression-default"),
        ("M143-DOC-CON-09", "m143-lane-c-typed-abi-default"),
        ("M143-DOC-CON-10", "m143-lane-c-lowering-replay-proof-default"),
    ),
}

REQUIRED_PACKAGE_SCRIPTS: dict[str, tuple[str, ...]] = {
    "check:objc3c:library-cli-parity:source:m143": (
        "python scripts/check_objc3c_library_cli_parity.py",
        "--source tests/tooling/fixtures/native/hello.objc3",
        "--cli-bin artifacts/bin/objc3c-native.exe",
        "--c-api-bin artifacts/bin/objc3c-frontend-c-api-runner.exe",
        "--summary-out tmp/artifacts/compilation/objc3c-native/m143/library-cli-parity/summary.json",
        "--cli-ir-object-backend clang",
    ),
    "test:objc3c:m143-artifact-governance": (
        "tests/tooling/test_objc3c_library_cli_parity.py",
        "tests/tooling/test_objc3c_c_api_runner_extraction.py",
        "tests/tooling/test_objc3c_driver_cli_extraction.py",
        "tests/tooling/test_objc3c_parser_extraction.py",
        "tests/tooling/test_objc3c_parser_ast_builder_extraction.py",
        "tests/tooling/test_objc3c_sema_extraction.py",
        "tests/tooling/test_objc3c_sema_pass_manager_extraction.py",
        "tests/tooling/test_objc3c_frontend_types_extraction.py",
        "tests/tooling/test_objc3c_lowering_contract.py",
        "tests/tooling/test_objc3c_ir_emitter_extraction.py",
        "tests/tooling/test_objc3c_m143_artifact_tmp_governance_contract.py",
        "tests/tooling/test_objc3c_m143_sema_type_system_tmp_governance_contract.py",
        "tests/tooling/test_objc3c_m143_lowering_runtime_abi_tmp_governance_contract.py",
        "tests/tooling/test_check_m143_artifact_tmp_governance_contract.py",
    ),
    "check:compiler-closeout:m143": (
        "python scripts/check_m143_artifact_tmp_governance_contract.py",
        "npm run test:objc3c:m143-artifact-governance",
    ),
    "check:task-hygiene": (
        "check:compiler-closeout:m143",
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/m143_artifact_tmp_governance_contract_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {display_path(path)}")
    return path.read_text(encoding="utf-8")


def collect_text_findings(*, artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
        if snippet not in text:
            findings.append(
                Finding(artifact, check_id, f"expected snippet missing: {snippet}")
            )
    return findings


def collect_package_findings(package_json: Path) -> list[Finding]:
    payload = json.loads(load_text(package_json, artifact="package_json"))
    scripts = payload.get("scripts")
    if not isinstance(scripts, dict):
        return [
            Finding(
                "package_json",
                "M143-PKG-00",
                "package.json scripts field must be an object",
            )
        ]

    findings: list[Finding] = []
    for script_name, required_tokens in REQUIRED_PACKAGE_SCRIPTS.items():
        script_value = scripts.get(script_name)
        if not isinstance(script_value, str):
            findings.append(
                Finding(
                    "package_json",
                    "M143-PKG-01",
                    f"missing scripts['{script_name}']",
                )
            )
            continue
        for index, token in enumerate(required_tokens, start=1):
            if token not in script_value:
                findings.append(
                    Finding(
                        "package_json",
                        f"M143-PKG-{script_name}-{index}",
                        f"scripts['{script_name}'] missing token: {token}",
                    )
                )
    return findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []

    for artifact, path in ARTIFACTS.items():
        if artifact == "package_json":
            continue
        findings.extend(
            collect_text_findings(
                artifact=artifact,
                text=load_text(path, artifact=artifact),
            )
        )
    findings.extend(collect_package_findings(ARTIFACTS["package_json"]))
    findings.sort(key=lambda item: (item.artifact, item.check_id, item.detail))

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": sum(len(items) for items in REQUIRED_SNIPPETS.values())
        + sum(len(items) for items in REQUIRED_PACKAGE_SCRIPTS.values()),
        "checks_passed": (
            sum(len(items) for items in REQUIRED_SNIPPETS.values())
            + sum(len(items) for items in REQUIRED_PACKAGE_SCRIPTS.values())
            - len(findings)
        ),
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        print(
            "m143-artifact-tmp-governance-contract: contract drift detected "
            f"({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(
                f"- {finding.artifact}:{finding.check_id} {finding.detail}",
                file=sys.stderr,
            )
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("m143-artifact-tmp-governance-contract: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={summary['checks_passed']}")
    print("- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


if __name__ == "__main__":
    main()
