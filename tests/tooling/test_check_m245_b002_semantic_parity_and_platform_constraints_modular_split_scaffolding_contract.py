from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _replace_all_or_fail(*, text: str, old: str, new: str) -> str:
    if old not in text:
        raise AssertionError(f"expected to replace missing token: {old}")
    mutated = text.replace(old, new)
    if old in mutated:
        raise AssertionError(f"replacement did not remove all occurrences of: {old}")
    return mutated


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 60
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m245_b002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m245/M245-B002/")


def test_contract_fails_closed_when_expectations_dependency_drifts(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m245_semantic_parity_and_platform_constraints_modular_split_scaffolding_b002_expectations.md"
    )
    original = contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8")
    drift_doc.write_text(
        _replace_all_or_fail(
            text=original,
            old="Dependencies: `M245-B001`",
            new="Dependencies: `M245-B001-DRIFT`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B002-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_typed_surface_runtime_dispatch_upper_bound_drifts(tmp_path: Path) -> None:
    drift_surface = tmp_path / "objc3_typed_sema_to_lowering_contract_surface.h"
    original = contract.DEFAULT_TYPED_SURFACE.read_text(encoding="utf-8")
    drift_surface.write_text(
        _replace_all_or_fail(
            text=original,
            old="lowering_boundary.runtime_dispatch_arg_slots <= kObjc3RuntimeDispatchMaxArgs &&",
            new="lowering_boundary.runtime_dispatch_arg_slots < kObjc3RuntimeDispatchMaxArgs &&",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--typed-surface", str(drift_surface), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B002-TYP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drops_b001(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    original = contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8")
    drift_package.write_text(
        _replace_all_or_fail(
            text=original,
            old='"check:objc3c:m245-b002-lane-b-readiness": '
            '"npm run check:objc3c:m245-b001-lane-b-readiness '
            '&& npm run check:objc3c:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract '
            '&& npm run test:tooling:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract"',
            new='"check:objc3c:m245-b002-lane-b-readiness": '
            '"npm run check:objc3c:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B002-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_lowering_regression_input(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    original = contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8")
    drift_package.write_text(
        _replace_all_or_fail(
            text=original,
            old='"test:objc3c:lowering-regression": ',
            new='"test:objc3c:lowering-regression-disabled": ',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B002-PKG-05" for failure in payload["failures"])


def test_contract_fails_closed_when_b001_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_m245_b001_checker.py"
    exit_code = contract.run(
        [
            "--b001-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B002-DEP-B001-ARG-01" for failure in payload["failures"])
