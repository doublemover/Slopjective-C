from __future__ import annotations

import importlib.util
import sys
from copy import deepcopy
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object, write_json_file


SCRIPT_PATH = ROOT / "scripts" / "check_objc3c_metaprogramming_public_surface.py"
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "metaprogramming_public_surface"
    / "macro_metaprogramming_public_surface_contract.json"
)


def _load_checker() -> Any:
    spec = importlib.util.spec_from_file_location(
        "check_objc3c_metaprogramming_public_surface", SCRIPT_PATH
    )
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load metaprogramming public surface checker")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def _redirect_outputs(checker: Any, tmp_path: Path) -> None:
    checker.REPORT_PATH = (
        ROOT
        / "tmp"
        / "tests"
        / "metaprogramming-public-surface"
        / tmp_path.name
        / "summary.json"
    )


def test_metaprogramming_public_surface_contract_passes(tmp_path: Path) -> None:
    checker = _load_checker()
    _redirect_outputs(checker, tmp_path)

    assert checker.main() == 0

    summary = load_json_object(checker.REPORT_PATH)
    assert summary["status"] == "PASS"
    assert summary["issue_ref"] == 8168
    assert summary["schema_path"] == (
        "schemas/objc3c-macro-metaprogramming-public-surface-v1.schema.json"
    )
    assert summary["artifact_ownership_contract_id"] == (
        "objc3c.metaprogramming.macro.expansion.artifact.ownership.v1"
    )
    assert summary["public_command"] == "npm run objc3c -- validate-metaprogramming-conformance"
    assert "objc3c.behavior.language.metaprogramming.derive-expansion-inventory" in summary[
        "support_claim_ids"
    ]
    assert "objc3c.behavior.language.metaprogramming.macro-safety-sandbox-determinism" in summary[
        "support_claim_ids"
    ]
    assert summary["lead_owned_support_rows"] == [
        "language.metaprogramming.derive-expansion-inventory",
        "language.metaprogramming.macro-safety-sandbox-determinism",
    ]
    assert summary["surface_counts"]["artifact_ownership_required_fields"] >= 40
    assert summary["surface_counts"]["artifact_ownership_fail_closed_cases"] == 4
    assert summary["surface_counts"]["reserved_surface_entries"] == 6
    assert summary["surface_counts"]["fail_closed_validation_cases"] == 8
    assert summary["supported_surface"]["macro_declaration_model"][
        "unsafe_host_execution_allowed"
    ] is False
    assert summary["supported_surface"]["expansion_metadata_model"][
        "generated_artifact_authority"
    ] is False
    assert summary["supported_surface"]["expansion_metadata_model"][
        "deterministic_replay_required"
    ] is True
    assert summary["supported_surface"]["rejection_behavior_model"][
        "fail_closed_before_expansion"
    ] is True
    assert summary["expansion_security_policy"][
        "arbitrary_host_process_execution_allowed"
    ] is False
    assert summary["reserved_surface"]["arbitrary_compile_time_execution"][
        "status"
    ] == "rejected"
    assert summary["fail_closed_validation"]["tmp_report_committable"] is False


def test_metaprogramming_public_surface_rejects_derive_selector_drift(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    _redirect_outputs(checker, tmp_path)
    contract = deepcopy(load_json_object(CONTRACT_PATH))
    contract["derive_surface"]["supported_forms"][0]["selector"] = "isEqualToPublicSurface:"
    tmp_contract = tmp_path / "contract.json"
    write_json_file(tmp_contract, contract, sort_keys=True)
    checker.CONTRACT_PATH = tmp_contract

    assert checker.main() == 1

    summary = load_json_object(checker.REPORT_PATH)
    assert summary["status"] == "FAIL"
    assert any("isEqualToPublicSurface:" in failure for failure in summary["failures"])


def test_metaprogramming_public_surface_rejects_arbitrary_execution_claim(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    _redirect_outputs(checker, tmp_path)
    contract = deepcopy(load_json_object(CONTRACT_PATH))
    contract["reserved_surface"]["arbitrary_compile_time_execution"][
        "status"
    ] = "supported"
    contract["supported_surface"]["macro_declaration_model"][
        "unsafe_host_execution_allowed"
    ] = True
    tmp_contract = tmp_path / "contract.json"
    write_json_file(tmp_contract, contract, sort_keys=True)
    checker.CONTRACT_PATH = tmp_contract

    assert checker.main() == 1

    summary = load_json_object(checker.REPORT_PATH)
    assert summary["status"] == "FAIL"
    assert any(
        "arbitrary_compile_time_execution" in failure
        or "unsafe host execution" in failure
        for failure in summary["failures"]
    )


def test_metaprogramming_public_surface_rejects_generated_metadata_authority(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    _redirect_outputs(checker, tmp_path)
    contract = deepcopy(load_json_object(CONTRACT_PATH))
    contract["supported_surface"]["expansion_metadata_model"][
        "generated_artifact_authority"
    ] = True
    tmp_contract = tmp_path / "contract.json"
    write_json_file(tmp_contract, contract, sort_keys=True)
    checker.CONTRACT_PATH = tmp_contract

    assert checker.main() == 1

    summary = load_json_object(checker.REPORT_PATH)
    assert summary["status"] == "FAIL"
    assert any("expansion metadata" in failure for failure in summary["failures"])


def test_metaprogramming_public_surface_rejects_rejection_diagnostic_drift(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    _redirect_outputs(checker, tmp_path)
    contract = deepcopy(load_json_object(CONTRACT_PATH))
    contract["supported_surface"]["rejection_behavior_model"][
        "required_diagnostic_codes"
    ].remove("O3S332")
    tmp_contract = tmp_path / "contract.json"
    write_json_file(tmp_contract, contract, sort_keys=True)
    checker.CONTRACT_PATH = tmp_contract

    assert checker.main() == 1

    summary = load_json_object(checker.REPORT_PATH)
    assert summary["status"] == "FAIL"
    assert any(
        "rejection behavior drifted" in failure for failure in summary["failures"]
    )
