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

from objc3c_shared.json_io import load_json_object
from objc3c_shared.json_io import write_json_file

SCRIPT_PATH = ROOT / "scripts" / "build_security_hardening_macro_trust_policy_summary.py"


def _load_checker():
    spec = importlib.util.spec_from_file_location(
        "build_security_hardening_macro_trust_policy_summary", SCRIPT_PATH
    )
    if spec is None or spec.loader is None:
        raise RuntimeError(
            "Unable to load scripts/build_security_hardening_macro_trust_policy_summary.py"
        )
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def _redirect_outputs(checker: Any, tmp_path: Path) -> None:
    checker.OUT_DIR = tmp_path
    checker.JSON_OUT = tmp_path / "macro_trust_policy_summary.json"
    checker.MD_OUT = tmp_path / "macro_trust_policy_summary.md"


def _load_contract_and_registry() -> tuple[dict[str, Any], dict[str, Any]]:
    contract = load_json_object(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "security_hardening"
        / "macro_package_provenance_trust_policy.json"
    )
    registry = load_json_object(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "security_hardening"
        / "macro_supply_chain_trust_registry.json"
    )
    return contract, registry


def _write_tmp_contract(
    tmp_path: Path, contract: dict[str, Any], registry: dict[str, Any]
) -> Path:
    repo_tmp = ROOT / "tmp" / "tests" / "macro-trust-policy" / tmp_path.name
    registry_path = repo_tmp / "macro_supply_chain_trust_registry.json"
    contract_path = repo_tmp / "macro_package_provenance_trust_policy.json"
    write_json_file(registry_path, registry, sort_keys=True)
    tmp_contract = deepcopy(contract)
    tmp_contract["macro_supply_chain_trust_registry"] = registry_path.relative_to(
        ROOT
    ).as_posix()
    write_json_file(contract_path, tmp_contract, sort_keys=True)
    return contract_path


def test_macro_trust_policy_summary_enforces_supply_chain_registry(tmp_path: Path) -> None:
    checker = _load_checker()
    _redirect_outputs(checker, tmp_path)

    assert checker.main() == 0

    summary = load_json_object(checker.JSON_OUT)
    assert summary["status"] == "PASS"
    assert summary["trust_registry_path"] == (
        "tests/tooling/fixtures/security_hardening/macro_supply_chain_trust_registry.json"
    )
    assert summary["checks"]["deny_revoked_provenance"] is True
    assert summary["checks"]["require_replay_metadata"] is True
    assert summary["checks"]["positive_fixture_macros_signed"] is True
    assert summary["checks"]["negative_fixture_macros_denied"] is True
    assert summary["trust_policy"]["positive_macro_entry_count"] == 3
    assert summary["trust_policy"]["expected_denial_results"][
        "revoked-provenance-denied"
    ] == "revoked-provenance"
    assert summary["trust_policy"]["expected_denial_results"][
        "missing-replay-metadata-denied"
    ] == "missing-replay-metadata"


def test_macro_trust_policy_summary_rejects_removed_revocation(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    _redirect_outputs(checker, tmp_path)
    contract, registry = _load_contract_and_registry()
    registry["revocations"] = []
    checker.CONTRACT_PATH = _write_tmp_contract(tmp_path, contract, registry)

    assert checker.main() == 1

    summary = load_json_object(checker.JSON_OUT)
    assert summary["status"] == "FAIL"
    assert summary["trust_policy"]["expected_denial_results"][
        "revoked-provenance-denied"
    ] == "missing-signature"
    assert any("revoked-provenance-denied" in failure for failure in summary["failures"])


def test_macro_trust_policy_summary_rejects_missing_replay_metadata_on_positive(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    _redirect_outputs(checker, tmp_path)
    contract, registry = _load_contract_and_registry()
    stripped = deepcopy(registry)
    del stripped["signed_artifacts"][0]["replay_metadata"]
    checker.CONTRACT_PATH = _write_tmp_contract(tmp_path, contract, stripped)

    assert checker.main() == 1

    summary = load_json_object(checker.JSON_OUT)
    assert summary["status"] == "FAIL"
    assert summary["checks"]["positive_fixture_macros_signed"] is False
    assert any("missing-replay-metadata" in failure for failure in summary["failures"])
