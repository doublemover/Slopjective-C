from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m247_d004_runtime_link_build_throughput_optimization_core_feature_expansion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_d004_runtime_link_build_throughput_optimization_core_feature_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_d004_runtime_link_build_throughput_optimization_core_feature_expansion_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_once(text: str, old: str, new: str) -> str:
    assert old in text
    return text.replace(old, new, 1)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m247-d004-runtime-link-build-throughput-optimization-core-feature-expansion-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_d004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-D004/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_d004_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-D003`",
            "Dependencies: `M247-D099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D004-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_d004_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6762`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D004-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_drops_pending_d003_chain(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_once(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m247-d004-lane-d-readiness": '
            '"npm run --if-present check:objc3c:m247-d003-lane-d-readiness '
            "&& npm run check:objc3c:m247-d004-runtime-link-build-throughput-optimization-core-feature-expansion-contract "
            '&& npm run test:tooling:m247-d004-runtime-link-build-throughput-optimization-core-feature-expansion-contract"',
            '"check:objc3c:m247-d004-lane-d-readiness": '
            '"npm run check:objc3c:m247-d004-runtime-link-build-throughput-optimization-core-feature-expansion-contract '
            '&& npm run test:tooling:m247-d004-runtime-link-build-throughput-optimization-core-feature-expansion-contract"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D004-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_architecture_anchor_drifts(tmp_path: Path) -> None:
    drift_architecture = tmp_path / "ARCHITECTURE.md"
    drift_architecture.write_text(
        replace_once(
            contract.DEFAULT_ARCHITECTURE_DOC.read_text(encoding="utf-8"),
            "M247 lane-D D004 runtime/link/build throughput optimization core feature",
            "M247 lane-D D004 runtime/link throughput optimization core feature",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--architecture-doc", str(drift_architecture), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D004-ARCH-01" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_perf_budget_input(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_once(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"test:objc3c:perf-budget": ',
            '"test:objc3c:perf-budget-disabled": ',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D004-PKG-07" for failure in payload["failures"])
