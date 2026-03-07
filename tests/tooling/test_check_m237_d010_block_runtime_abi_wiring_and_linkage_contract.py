from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m237_d010_block_runtime_abi_wiring_and_linkage_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m237_d010_block_runtime_abi_wiring_and_linkage_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m237_d010_block_runtime_abi_wiring_and_linkage_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m237-d010-block-runtime-abi-wiring-and-linkage-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m237_d010() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m237/M237-D010/")


def test_contract_fails_closed_when_expectations_drop_issue_anchor(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m237_block_runtime_abi_wiring_and_linkage_conformance_corpus_expansion_d010_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Issue `#5927` defines canonical lane-D conformance corpus expansion scope.",
            "Issue `#9999` defines canonical lane-D conformance corpus expansion scope.",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M237-D010-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_token_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m237_d010_packet.md"
    drift_packet.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M237-C001`",
            "Dependencies: `M237-C999`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M237-D010-DOC-PKT-04" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m237-c001-lane-c-readiness": '
            '"npm run check:objc3c:m237-c001-block-lowering-and-invoke-emission-contract '
            '&& npm run test:tooling:m237-c001-block-lowering-and-invoke-emission-contract"',
            '"check:objc3c:m237-c001-lane-c-readiness": '
            '"npm run check:objc3c:m237-c001-block-lowering-and-invoke-emission-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M237-D010-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_metadata_anchor_drifts(tmp_path: Path) -> None:
    drift_metadata = tmp_path / "MODULE_METADATA_AND_ABI_TABLES.md"
    drift_metadata.write_text(
        contract.DEFAULT_METADATA_SPEC.read_text(encoding="utf-8").replace(
            "deterministic lane-C qualified type lowering and ABI representation metadata anchors for `M237-C001`",
            "deterministic lane-C qualified type lowering and ABI representation metadata anchors for `M237-C999`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--metadata-spec", str(drift_metadata), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M237-D010-META-01" for failure in payload["failures"])










