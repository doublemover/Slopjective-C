from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_objc3c_public_claim_drift.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_objc3c_public_claim_drift", SCRIPT_PATH
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_objc3c_public_claim_drift.py")
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "public_claim_drift"


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def test_public_claim_drift_maps_claims_and_allows_guarded_unsupported_text(
    tmp_path: Path,
) -> None:
    json_out = tmp_path / "public_claim_drift_summary.json"
    md_out = tmp_path / "public_claim_drift_summary.md"

    code = checker.main(
        [
            "--root",
            str(FIXTURE_ROOT),
            "--support-summary",
            "positive_support_summary.json",
            "--summary-json",
            str(json_out),
            "--summary-md",
            str(md_out),
        ]
    )

    assert code == 0
    payload = read_json(json_out)
    assert payload["contract_id"] == checker.SUMMARY_CONTRACT_ID
    assert payload["status"] == "PASS"
    assert payload["public_claim_surface_count"] == 2
    assert payload["finding_count"] == 0
    assert payload["checks"]["every_claim_has_mapping"] is True
    assert payload["checks"]["unsupported_surfaces_fail_closed"] is True

    mapped_claims = payload["claim_mappings"]
    assert isinstance(mapped_claims, list)
    assert any(
        "fixture-supported-runtime" in claim["mapped_surfaces"]
        for claim in mapped_claims
        if isinstance(claim, dict)
    )
    assert "# Objective-C 3.0 Public Claim Drift Summary" in md_out.read_text(
        encoding="utf-8"
    )


def test_public_claim_drift_rejects_unguarded_unsupported_overclaim(
    tmp_path: Path,
) -> None:
    json_out = tmp_path / "bad_summary.json"
    md_out = tmp_path / "bad_summary.md"

    code = checker.main(
        [
            "--root",
            str(FIXTURE_ROOT),
            "--support-summary",
            "negative_support_summary.json",
            "--summary-json",
            str(json_out),
            "--summary-md",
            str(md_out),
        ]
    )

    assert code == 1
    payload = read_json(json_out)
    assert payload["status"] == "FAIL"
    assert payload["finding_count"] == 1
    assert payload["findings"][0]["kind"] == "unguarded-forbidden-public-claim"
    assert payload["findings"][0]["pattern_id"] == "universal-foreign-topology-support"


def test_default_public_claim_drift_report_is_checkable(tmp_path: Path) -> None:
    json_out = tmp_path / "default_summary.json"
    md_out = tmp_path / "default_summary.md"

    code = checker.main(["--summary-json", str(json_out), "--summary-md", str(md_out)])

    assert code == 0
    payload = read_json(json_out)
    assert payload["support_summary_path"] == (
        "reports/claimability/support-classification/support_classification_summary.json"
    )
    assert payload["checks"]["source_truth_excludes_tmp"] is True
    assert payload["public_claim_surface_count"] >= 1
    assert payload["claim_mapping_count"] >= 1


def test_check_mode_fails_on_public_claim_drift(tmp_path: Path, capsys: object) -> None:
    json_out = tmp_path / "summary.json"
    md_out = tmp_path / "summary.md"
    json_out.write_text("{}\n", encoding="utf-8")
    md_out.write_text("# stale\n", encoding="utf-8")

    code = checker.main(
        [
            "--root",
            str(FIXTURE_ROOT),
            "--support-summary",
            "positive_support_summary.json",
            "--summary-json",
            str(json_out),
            "--summary-md",
            str(md_out),
            "--check",
        ]
    )

    assert code == 1
    captured = capsys.readouterr()
    assert "public claim drift output drift" in captured.err
