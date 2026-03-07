from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m235_d006_interop_behavior_for_qualified_generic_apis_edge_case_expansion_and_robustness_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m235_d006_interop_behavior_for_qualified_generic_apis_edge_case_expansion_and_robustness_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m235_d006_interop_behavior_for_qualified_generic_apis_edge_case_expansion_and_robustness_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_all(text: str, old: str, new: str) -> str:
    assert old in text
    replaced = text.replace(old, new)
    assert old not in replaced
    return replaced


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m235-d006-interop-behavior-for-qualified-generic-apis-edge-case-expansion-and-robustness-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m235_d006() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m235/M235-D006/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m235_d006_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M235-D005`",
            "Dependencies: `M235-D099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-D006-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_anchor_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m235_d006_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#5836`",
            "Issue: `#5999`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-D006-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drops_d005(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m235-d005-lane-d-readiness": '
            '"npm run check:objc3c:m235-d004-lane-d-readiness '
            '&& npm run check:objc3c:m235-d005-interop-behavior-for-qualified-generic-apis-edge-case-and-compatibility-completion-contract '
            '&& npm run test:tooling:m235-d005-interop-behavior-for-qualified-generic-apis-edge-case-and-compatibility-completion-contract"',
            '"check:objc3c:m235-d005-lane-d-readiness": '
            '"npm run check:objc3c:m235-d005-interop-behavior-for-qualified-generic-apis-edge-case-and-compatibility-completion-contract '
            '&& npm run test:tooling:m235-d005-interop-behavior-for-qualified-generic-apis-edge-case-and-compatibility-completion-contract"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-D006-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_d006_lane_readiness_drops_pytest(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m235-d006-lane-d-readiness": '
            '"npm run check:objc3c:m235-d005-lane-d-readiness '
            '&& npm run check:objc3c:m235-d006-interop-behavior-for-qualified-generic-apis-edge-case-expansion-and-robustness-contract '
            '&& npm run test:tooling:m235-d006-interop-behavior-for-qualified-generic-apis-edge-case-expansion-and-robustness-contract"',
            '"check:objc3c:m235-d006-lane-d-readiness": '
            '"npm run check:objc3c:m235-d005-lane-d-readiness '
            '&& npm run check:objc3c:m235-d006-interop-behavior-for-qualified-generic-apis-edge-case-expansion-and-robustness-contract"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-D006-PKG-06" for failure in payload["failures"])


def test_contract_fails_closed_when_d005_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_d005_checker.py"
    exit_code = contract.run(
        [
            "--d005-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-D006-DEP-D005-ARG-01" for failure in payload["failures"])




