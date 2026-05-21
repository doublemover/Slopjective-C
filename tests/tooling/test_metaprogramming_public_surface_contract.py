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
