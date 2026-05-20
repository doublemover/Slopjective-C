from __future__ import annotations

import copy
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_release_manifest import abi_api_drift
from objc3c_release_manifest.model import PackageAssembly, PayloadEntry
from objc3c_release_manifest.validate import validate_release_inputs
from objc3c_shared.json_io import load_json_object, write_json_file


def _governance() -> dict[str, object]:
    return load_json_object(abi_api_drift.GOVERNANCE_MANIFEST)


def _run_with_governance(
    tmp_path: Path,
    governance: dict[str, object],
) -> tuple[int, dict[str, object]]:
    governance_path = tmp_path / "abi_api_governance.json"
    summary_path = tmp_path / "abi-api-drift-summary.json"
    write_json_file(governance_path, governance, sort_keys=True)
    return abi_api_drift.run_check(
        governance_path=governance_path,
        summary_path=summary_path,
    )


def test_release_abi_api_drift_gate_accepts_checked_in_governance(tmp_path: Path) -> None:
    rc, summary = _run_with_governance(tmp_path, _governance())

    assert rc == 0
    assert summary["status"] == "PASS"
    assert summary["failure_count"] == 0
    assert summary["source_manifest_count"] == 5
    assert summary["public_api_symbol_count"] == 49
    assert summary["runtime_abi_symbol_count"] == 9
    assert summary["frontend_c_api_symbol_count"] == 33


def test_release_abi_api_drift_gate_rejects_unreviewed_public_symbol_addition(
    tmp_path: Path,
) -> None:
    governance = _governance()
    core_surface = governance["stdlib_public_api"][0]  # type: ignore[index]
    del core_surface["symbols"]["objc3_core_array_count"]  # type: ignore[index]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert summary["status"] == "FAIL"
    assert any(
        "unreviewed public-api symbol addition in objc3.core" in failure
        and "objc3_core_array_count" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_missing_inventory_module_governance(
    tmp_path: Path,
) -> None:
    governance = _governance()
    governance["stdlib_public_api"] = [  # type: ignore[index]
        entry
        for entry in governance["stdlib_public_api"]  # type: ignore[index]
        if entry["module"] != "objc3.system"
    ]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "ABI/API governance missing stdlib_public_api modules: objc3.system"
        in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_live_frontend_header_drift(
    tmp_path: Path,
) -> None:
    governance = _governance()
    frontend_c_api = governance["frontend_c_api"]  # type: ignore[index]
    frontend_c_api["required_helpers"] = [  # type: ignore[index]
        *frontend_c_api["required_helpers"],  # type: ignore[index]
        "objc3c_frontend_c_missing_live_helper",
    ]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "frontend C API live headers missing required helpers" in failure
        and "objc3c_frontend_c_missing_live_helper" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_unreviewed_public_symbol_removal(
    tmp_path: Path,
) -> None:
    governance = _governance()
    core_surface = governance["stdlib_public_api"][0]  # type: ignore[index]
    core_surface["symbols"]["objc3_core_not_real"] = "fn objc3_core_not_real()"  # type: ignore[index]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "unreviewed public-api symbol removal in objc3.core" in failure
        and "objc3_core_not_real" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_signature_drift(tmp_path: Path) -> None:
    governance = _governance()
    core_surface = governance["stdlib_public_api"][0]  # type: ignore[index]
    core_surface["symbols"]["objc3_core_array_count"] = (  # type: ignore[index]
        "fn objc3_core_array_count(count: i32, extra: i32)"
    )

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "public-api signature drifted for objc3.core.objc3_core_array_count"
        in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_private_helper_graduation_without_policy(
    tmp_path: Path,
) -> None:
    governance = _governance()
    module_policies = governance["module_policies"]  # type: ignore[index]
    core_policy = copy.deepcopy(module_policies["objc3.core"])  # type: ignore[index]
    core_policy["allowed_public_prefixes"] = ["objc3_promoted_"]
    core_policy["allowed_public_symbols"] = []
    module_policies["objc3.core"] = core_policy  # type: ignore[index]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "private helper graduation lacks policy for objc3.core.objc3_core_array_count"
        in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_compatibility_window_violation(
    tmp_path: Path,
) -> None:
    governance = _governance()
    compatibility_policy = governance["compatibility_window_policy"]  # type: ignore[index]
    compatibility_policy["expected_supported_major_line"] = 2  # type: ignore[index]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "supported major line drifted" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_manifest_input_validation_rejects_failed_abi_api_gate(
    tmp_path: Path,
) -> None:
    package_root = tmp_path / "package"
    package_root.mkdir()
    (package_root / "repo-superclean.json").write_text("{}", encoding="utf-8")
    manifest_path = package_root / "manifest.json"
    manifest_path.write_text("{}", encoding="utf-8")
    entry = PayloadEntry(
        path="artifacts/bin/objc3c.exe",
        sha256="0" * 64,
        byte_count=1,
        component_group="native-binaries",
    )
    assembly = PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest={
            "repo_superclean_surface": "repo-superclean.json",
            "copied_file_count": 1,
        },
        entries=(entry,),
        payload_digest="a" * 64,
    )
    failed_summary = tmp_path / "abi-api-drift-summary.json"
    write_json_file(
        failed_summary,
        {
            "contract_id": abi_api_drift.SUMMARY_CONTRACT_ID,
            "status": "FAIL",
            "failure_count": 1,
        },
    )

    with pytest.raises(RuntimeError, match="ABI/API drift gate did not pass"):
        validate_release_inputs(
            first=assembly,
            second=assembly,
            payload_policy={"required_payload_prefixes": ["artifacts/bin/"]},
            abi_api_drift_summary_path=failed_summary,
        )


def test_release_manifest_input_validation_rejects_wrong_abi_api_summary_contract(
    tmp_path: Path,
) -> None:
    package_root = tmp_path / "package"
    package_root.mkdir()
    (package_root / "repo-superclean.json").write_text("{}", encoding="utf-8")
    manifest_path = package_root / "manifest.json"
    manifest_path.write_text("{}", encoding="utf-8")
    entry = PayloadEntry(
        path="artifacts/bin/objc3c.exe",
        sha256="0" * 64,
        byte_count=1,
        component_group="native-binaries",
    )
    assembly = PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest={
            "repo_superclean_surface": "repo-superclean.json",
            "copied_file_count": 1,
        },
        entries=(entry,),
        payload_digest="a" * 64,
    )
    wrong_contract_summary = tmp_path / "abi-api-drift-summary.json"
    write_json_file(
        wrong_contract_summary,
        {
            "contract_id": "objc3c.release.foundation.other.v1",
            "status": "PASS",
            "failure_count": 0,
        },
    )

    with pytest.raises(RuntimeError, match="ABI/API drift gate summary contract drifted"):
        validate_release_inputs(
            first=assembly,
            second=assembly,
            payload_policy={"required_payload_prefixes": ["artifacts/bin/"]},
            abi_api_drift_summary_path=wrong_contract_summary,
        )
