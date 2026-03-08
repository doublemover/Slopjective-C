from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _native_is_stale() -> bool:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        return True
    native_mtime = contract.DEFAULT_NATIVE_EXE.stat().st_mtime
    freshness_inputs = (
        contract.DEFAULT_AST_HEADER,
        contract.DEFAULT_FRONTEND_TYPES,
        contract.DEFAULT_FRONTEND_ARTIFACTS_HEADER,
        contract.DEFAULT_FRONTEND_ARTIFACTS,
        contract.DEFAULT_MANIFEST_ARTIFACTS_HEADER,
        contract.DEFAULT_MANIFEST_ARTIFACTS_CPP,
        contract.DEFAULT_DRIVER_CPP,
        contract.DEFAULT_PROCESS_HEADER,
        contract.DEFAULT_PROCESS_CPP,
        contract.DEFAULT_FRONTEND_ANCHOR_CPP,
    )
    return any(path.stat().st_mtime > native_mtime for path in freshness_inputs)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == (
        "m254-a002-registration-manifests-and-constructor-root-ownership-"
        "core-feature-implementation-v1"
    )
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 55
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["next_implementation_issue"] == "M254-B001"
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m254_a002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m254/M254-A002/")


def test_contract_fails_closed_when_expectations_drop_manifest_authority_model(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m254_registration_manifests_and_constructor_root_ownership_core_feature_implementation_a002_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`registration-manifest-authoritative-for-constructor-root-shape`",
            "`registration-manifest-authoritative-for-root-shape-v2`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M254-A002-DOC-EXP-05"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_frontend_artifacts_drop_manifest_surface_name(tmp_path: Path) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            "objc_runtime_translation_unit_registration_manifest",
            "objc_runtime_translation_unit_registration_template",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-artifacts",
            str(drift_artifacts),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M254-A002-ART-03"
        for failure in payload["failures"]
    )


def test_dynamic_probe_covers_registration_manifest_artifact(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")
    if _native_is_stale():
        pytest.skip("native frontend is older than the A002 source inputs")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 1

    case = payload["dynamic_cases"][0]
    assert case["fixture"] == "tests/tooling/fixtures/native/hello.objc3"
    assert case["manifest_path"].endswith("module.manifest.json")
    assert case["registration_manifest_path"].endswith(
        "module.runtime-registration-manifest.json"
    )
    assert case["payload_artifact"].endswith("module.runtime-metadata.bin")
    assert case["linker_response_artifact"].endswith(
        "module.runtime-metadata-linker-options.rsp"
    )
    assert case["discovery_artifact"].endswith(
        "module.runtime-metadata-discovery.json"
    )
    assert case["backend"] == "llvm-direct"
    assert case["translation_unit_identity_key"]
    assert case["constructor_init_stub_symbol"].startswith(
        "__objc3_runtime_register_image_init_stub_"
    )
    assert (
        case["runtime_support_library_archive_relative_path"]
        == "artifacts/lib/objc3_runtime.lib"
    )
