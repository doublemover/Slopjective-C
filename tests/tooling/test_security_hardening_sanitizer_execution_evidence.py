from __future__ import annotations

import hashlib
import importlib.util
import sys
from copy import deepcopy
from pathlib import Path
from typing import Any

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object
from objc3c_shared.json_io import write_json_file


def _load_script(relative_path: str, module_name: str):
    path = ROOT / relative_path
    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Unable to load {relative_path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def _load_checker():
    return _load_script(
        "scripts/check_security_sanitizer_execution_evidence.py",
        "check_security_sanitizer_execution_evidence",
    )


def _repo_tmp_path(tmp_path: Path) -> Path:
    return ROOT / "tmp" / "pytest" / tmp_path.parent.name / tmp_path.name


def _sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def test_sanitizer_execution_evidence_contract_keeps_native_runs_non_promoting(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = _repo_tmp_path(tmp_path) / "summary.json"

    assert checker.main() == 0

    summary = load_json_object(checker.SUMMARY_PATH)
    assert summary["contract_id"] == "objc3c.security.hardening.sanitizer.execution-evidence.summary.v1"
    assert summary["status"] == "PASS"
    assert summary["source_contract"] == (
        "tests/tooling/fixtures/security_hardening/sanitizer_execution_evidence_contract.json"
    )
    assert summary["public_action"] == "check-security-sanitizer-execution-evidence"
    assert summary["public_action_surface"] == {
        "public_action": "check-security-sanitizer-execution-evidence",
        "source_surface": "tests/tooling/fixtures/security_hardening/source_surface.json",
        "workflow_surface": "tests/tooling/fixtures/security_hardening/workflow_surface.json",
        "source_contract": "tests/tooling/fixtures/security_hardening/sanitizer_execution_evidence_contract.json",
        "schema": "schemas/objc3c-sanitizer-execution-evidence-v1.schema.json",
        "checker": "scripts/check_security_sanitizer_execution_evidence.py",
        "source_surface_metadata": {
            "public_action": "check-security-sanitizer-execution-evidence",
            "source_contract": "tests/tooling/fixtures/security_hardening/sanitizer_execution_evidence_contract.json",
            "schema": "schemas/objc3c-sanitizer-execution-evidence-v1.schema.json",
            "checker": "scripts/check_security_sanitizer_execution_evidence.py",
            "support_truth": False,
            "native_execution_claimed": False,
            "support_promotion_allowed": False,
        },
        "workflow_surface_metadata": {
            "public_action": "check-security-sanitizer-execution-evidence",
            "source_contract": "tests/tooling/fixtures/security_hardening/sanitizer_execution_evidence_contract.json",
            "schema": "schemas/objc3c-sanitizer-execution-evidence-v1.schema.json",
            "checker": "scripts/check_security_sanitizer_execution_evidence.py",
            "support_truth": False,
            "native_execution_claimed": False,
            "support_promotion_allowed": False,
        },
    }
    assert summary["support_truth"] is False
    assert summary["native_execution_claimed"] is False
    assert summary["support_promotion_allowed"] is False
    assert {
        case["variant_id"]
        for case in summary["positive_contract_fixtures"]  # type: ignore[index]
    } == {
        "objc3c.toolchain.sanitizer.address",
        "objc3c.toolchain.sanitizer.undefined",
    }
    assert summary["negative_contract_fixtures"]["failure_kinds"] == [  # type: ignore[index]
        "generated-only-evidence",
        "missing-runtime-manifest",
        "mixed-release-sanitizer-runtime",
        "stale-runtime-manifest-digest",
        "unsupported-host",
    ]


def _write_contract_variant(tmp_path: Path, mutator: Any) -> Any:
    checker = _load_checker()
    contract = load_json_object(checker.CONTRACT_PATH)
    mutated = deepcopy(contract)
    mutator(mutated)
    checker.CONTRACT_PATH = tmp_path / "contract.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.CONTRACT_PATH, mutated, sort_keys=True)
    return checker


def _write_surface_variant(tmp_path: Path, mutator: Any) -> Any:
    checker = _load_checker()
    source_surface = load_json_object(checker.SOURCE_SURFACE)
    workflow_surface = load_json_object(checker.WORKFLOW_SURFACE)
    mutator(source_surface, workflow_surface)
    checker.SOURCE_SURFACE = tmp_path / "source_surface.json"
    checker.WORKFLOW_SURFACE = tmp_path / "workflow_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SOURCE_SURFACE, source_surface, sort_keys=True)
    write_json_file(checker.WORKFLOW_SURFACE, workflow_surface, sort_keys=True)
    return checker


def test_sanitizer_execution_evidence_rejects_public_action_drift(
    tmp_path: Path,
) -> None:
    def drift_public_action(contract: dict[str, Any]) -> None:
        contract["public_action"] = "collect-sanitizer-runtime-evidence-asan"

    checker = _write_contract_variant(tmp_path, drift_public_action)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_sanitizer_execution_evidence_rejects_missing_workflow_action_surface(
    tmp_path: Path,
) -> None:
    def remove_workflow_action(
        source_surface: dict[str, Any],
        workflow_surface: dict[str, Any],
    ) -> None:
        action = "check-security-sanitizer-execution-evidence"
        workflow_surface["required_actions"].remove(action)

    checker = _write_surface_variant(tmp_path, remove_workflow_action)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_sanitizer_execution_evidence_rejects_support_truth_promotion(
    tmp_path: Path,
) -> None:
    def promote_support(contract: dict[str, Any]) -> None:
        contract["positive_contract_fixtures"][0]["support_truth"] = True

    checker = _write_contract_variant(tmp_path, promote_support)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_sanitizer_execution_evidence_rejects_generated_only_evidence(
    tmp_path: Path,
) -> None:
    def allow_generated_only(contract: dict[str, Any]) -> None:
        contract["positive_contract_fixtures"][0]["native_execution_command"][
            "generated_only_evidence_allowed"
        ] = True

    checker = _write_contract_variant(tmp_path, allow_generated_only)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_sanitizer_execution_evidence_rejects_missing_ubsan_trap_mode(
    tmp_path: Path,
) -> None:
    def remove_ubsan_mode(contract: dict[str, Any]) -> None:
        del contract["positive_contract_fixtures"][1]["trap_or_recover_mode"]

    checker = _write_contract_variant(tmp_path, remove_ubsan_mode)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_sanitizer_execution_evidence_rejects_missing_negative_case(
    tmp_path: Path,
) -> None:
    def remove_negative_case(contract: dict[str, Any]) -> None:
        contract["negative_contract_fixtures"] = [
            case
            for case in contract["negative_contract_fixtures"]
            if not (
                case["variant_id"] == "objc3c.toolchain.sanitizer.address"
                and case["failure_kind"] == "stale-runtime-manifest-digest"
            )
        ]

    checker = _write_contract_variant(tmp_path, remove_negative_case)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_sanitizer_runtime_evidence_action_pins_all_contract_rerouting() -> None:
    from scripts.objc3c_workflow.actions import sanitizer_runtime_evidence

    expected_pinned_options = {
        "--sanitizer-variant",
        "--target-platform",
        "--report",
        "--probe-script",
        "--package-root",
        "--manifest-relative-path",
        "--fixture-glob",
        "--parallelism",
        "--run-id",
        "--summary-out",
        "--work-dir",
    }
    assert expected_pinned_options <= set(
        sanitizer_runtime_evidence.PINNED_SANITIZER_RUNTIME_EVIDENCE_ARGS
    )
    for option in expected_pinned_options:
        assert sanitizer_runtime_evidence._has_pinned_option_override([option, "value"]) == option
        assert sanitizer_runtime_evidence._has_pinned_option_override([f"{option}=value"]) == option

    for contract in sanitizer_runtime_evidence.SANITIZER_RUNTIME_EVIDENCE_ACTION_CONTRACTS:
        spec = contract.action_spec()
        payload = contract.public_contract_payload()
        assert spec.pass_through_args is False
        assert spec.backend == contract.checker_backend()
        assert f"--package-root {contract.pinned_package_root}" in spec.backend
        assert f"--run-id {contract.pinned_run_id}" in spec.backend
        assert payload["public_pass_through_args_allowed"] is False
        assert payload["report_or_probe_rerouting_allowed"] is False
        assert payload["package_root_rerouting_allowed"] is False
        assert payload["fixture_rerouting_allowed"] is False
        assert payload["pinned_package_root"] == contract.pinned_package_root
        assert payload["pinned_run_id"] == contract.pinned_run_id
        assert payload["pinned_parallelism"] == 2
        assert payload["support_claim_authority"] is False
        assert payload["support_promotion_allowed"] is False
        assert payload["native_execution_claim_promotion_allowed"] is False


def test_sanitizer_runtime_evidence_public_actions_reject_any_passthrough(
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    from scripts.objc3c_workflow.actions import sanitizer_runtime_evidence

    def forbidden_run(_: list[str]) -> int:
        raise AssertionError("runtime evidence action must reject passthrough before running")

    monkeypatch.setattr(sanitizer_runtime_evidence, "run", forbidden_run)

    assert (
        sanitizer_runtime_evidence.action_check_sanitizer_runtime_evidence_asan(
            ["--parallelism", "99"]
        )
        == 1
    )
    assert (
        sanitizer_runtime_evidence.action_check_sanitizer_runtime_evidence_ubsan(
            ["--unexpected"]
        )
        == 1
    )


def test_sanitizer_runtime_checker_rejects_path_component_run_ids_and_external_roots(
    tmp_path: Path,
) -> None:
    checker = _load_script(
        "scripts/check_objc3c_sanitizer_runtime_evidence.py",
        "check_objc3c_sanitizer_runtime_evidence_test",
    )
    repo_tmp = _repo_tmp_path(tmp_path)
    checker.PACKAGE_ROOT_BASE = repo_tmp / "owned"

    assert checker.validate_run_id("run_20260524.address-1") == "run_20260524.address-1"
    for bad_run_id in ("../escape", "nested/run", ".hidden", "has space"):
        with pytest.raises(RuntimeError):
            checker.validate_run_id(bad_run_id)

    owned_package = checker.PACKAGE_ROOT_BASE / "run" / "address" / "package"
    owned_package.mkdir(parents=True, exist_ok=True)
    (owned_package / "stale.txt").write_text("stale", encoding="utf-8")
    checker.prepare_package_root(owned_package)
    assert owned_package.is_dir()
    assert list(owned_package.iterdir()) == []

    outside_package = repo_tmp / "outside" / "address" / "package"
    outside_package.mkdir(parents=True, exist_ok=True)
    with pytest.raises(RuntimeError):
        checker.prepare_package_root(outside_package)


def test_sanitizer_runtime_probe_requires_contract_runtime_artifacts_in_packaged_link_dir(
    tmp_path: Path,
) -> None:
    probe = _load_script(
        "scripts/probe_objc3c_sanitizer_runtime_evidence.py",
        "probe_objc3c_sanitizer_runtime_evidence_test",
    )
    probe.PACKAGE_ROOT_BASE = tmp_path / "packages"
    package_root = probe.PACKAGE_ROOT_BASE / "run" / "address" / "package"
    contract = probe.runtime_package_variant_contract("address")
    runtime_dir = package_root / "artifacts" / "runtime" / "sanitizer" / "address"
    runtime_dir.mkdir(parents=True)
    artifacts: list[dict[str, str]] = []
    for artifact_rel in probe.expected_runtime_library_artifacts("address"):
        artifact_path = package_root / artifact_rel.replace("/", "/")
        artifact_path.parent.mkdir(parents=True, exist_ok=True)
        artifact_path.write_text(artifact_rel, encoding="utf-8")
        artifacts.append(
            {
                "runtime_library_id": "clang_rt.asan",
                "artifact": artifact_rel,
                "source_file_name": Path(artifact_rel).name,
                "sha256": _sha256(artifact_path),
            }
        )
    manifest_path = package_root / str(contract.runtime_library_manifest_path).replace("/", "/")
    write_json_file(
        manifest_path,
        {
            "contract_id": "objc3c.sanitizer.runtime-library-manifest.v1",
            "sanitizer": "address",
            "target_platform_id": "windows-x64",
            "runtime_library_ids": list(contract.runtime_library_ids),
            "runtime_library_artifacts": artifacts,
            "runtime_library_root_kind": "llvm-clang-runtime-windows-x64",
            "missing_runtime_behavior": "fail-closed-before-package-install",
            "support_truth": False,
            "native_execution_claimed": False,
        },
    )

    loaded_manifest_path, payload = probe.load_runtime_manifest(package_root, "address")
    assert loaded_manifest_path == manifest_path
    assert payload["runtime_library_artifacts"] == artifacts

    escaped_artifacts = deepcopy(artifacts)
    escaped_artifacts[0]["artifact"] = (
        "artifacts/runtime/sanitizer/undefined/clang_rt.asan_dynamic-x86_64.lib"
    )
    write_json_file(
        manifest_path,
        {
            "contract_id": "objc3c.sanitizer.runtime-library-manifest.v1",
            "sanitizer": "address",
            "target_platform_id": "windows-x64",
            "runtime_library_ids": list(contract.runtime_library_ids),
            "runtime_library_artifacts": escaped_artifacts,
            "runtime_library_root_kind": "llvm-clang-runtime-windows-x64",
            "missing_runtime_behavior": "fail-closed-before-package-install",
            "support_truth": False,
            "native_execution_claimed": False,
        },
    )
    with pytest.raises(RuntimeError):
        probe.load_runtime_manifest(package_root, "address")


def test_sanitizer_execution_fixture_matches_runtime_probe_environment_defaults() -> None:
    contract = load_json_object(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "security_hardening"
        / "sanitizer_execution_evidence_contract.json"
    )
    cases = {
        case["sanitizer"]: case
        for case in contract["positive_contract_fixtures"]
    }

    assert cases["address"]["native_execution_command"]["environment"] == {
        "ASAN_OPTIONS": "detect_leaks=0:halt_on_error=1:symbolize=1"
    }
    assert cases["undefined"]["native_execution_command"]["environment"] == {
        "UBSAN_OPTIONS": "halt_on_error=1:print_stacktrace=1"
    }
