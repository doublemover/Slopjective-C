from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "build_objc3c_support_classification.py"
)
SPEC = importlib.util.spec_from_file_location(
    "build_objc3c_support_classification", SCRIPT_PATH
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/build_objc3c_support_classification.py")
builder = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = builder
SPEC.loader.exec_module(builder)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "support_classification"


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def test_generator_emits_durable_summary_from_positive_fixture(tmp_path: Path) -> None:
    json_out = tmp_path / "support_classification_summary.json"
    md_out = tmp_path / "support_classification_summary.md"

    code = builder.main(
        [
            "--contract",
            str(FIXTURE_ROOT / "positive_contract.json"),
            "--summary-json",
            str(json_out),
            "--summary-md",
            str(md_out),
        ]
    )

    assert code == 0
    assert json_out.is_file()
    assert md_out.is_file()

    payload = read_json(json_out)
    assert payload["contract_id"] == builder.SUMMARY_CONTRACT_ID
    assert payload["status"] == "PASS"
    assert payload["support_class_count"] == 4
    assert payload["evidence_family_count"] == 1
    assert payload["classification_count"] == 2
    assert payload["class_counts"] == {"supported": 1, "unsupported": 1}
    assert payload["tmp_source_truth_paths"] == []
    assert payload["checks"]["source_truth_excludes_tmp"] is True
    assert "fixture-unsupported-surface" in payload["fail_closed_surfaces"]

    report_text = md_out.read_text(encoding="utf-8")
    assert "# Objective-C 3.0 Support Classification Summary" in report_text
    assert "| fixture-supported-surface | supported | production-claimable | fixture-evidence |" in report_text


def test_generator_rejects_unknown_surface_class(capsys: object, tmp_path: Path) -> None:
    json_out = tmp_path / "invalid.json"
    md_out = tmp_path / "invalid.md"

    code = builder.main(
        [
            "--contract",
            str(FIXTURE_ROOT / "invalid_unknown_class.json"),
            "--summary-json",
            str(json_out),
            "--summary-md",
            str(md_out),
        ]
    )

    assert code == 1
    assert not json_out.exists()
    assert not md_out.exists()
    captured = capsys.readouterr()
    assert "claimed-without-evidence" in captured.err


def test_default_contract_generates_checked_in_support_classification(tmp_path: Path) -> None:
    json_out = tmp_path / "default_summary.json"
    md_out = tmp_path / "default_summary.md"

    code = builder.main(["--summary-json", str(json_out), "--summary-md", str(md_out)])

    assert code == 0
    payload = read_json(json_out)
    assert payload["source_contract_id"] == (
        "objc3c.full_envelope.claimability.support.matrix.claim.taxonomy.v1"
    )
    assert payload["summary_script"] == "scripts/build_objc3c_support_classification.py"
    assert payload["legacy_support_matrix_summary_script"] == (
        "scripts/build_full_envelope_claimability_support_matrix_summary.py"
    )
    assert payload["checks"]["canonical_support_classes_defined"] is True
    assert payload["checks"]["source_truth_excludes_tmp"] is True
    assert payload["tmp_source_truth_paths"] == []
    assert payload["class_counts"]["supported"] >= 1
    assert payload["class_counts"]["unsupported"] >= 1


def test_check_mode_fails_on_drift(tmp_path: Path, capsys: object) -> None:
    json_out = tmp_path / "summary.json"
    md_out = tmp_path / "summary.md"
    json_out.write_text("{}\n", encoding="utf-8")
    md_out.write_text("# stale\n", encoding="utf-8")

    code = builder.main(
        [
            "--contract",
            str(FIXTURE_ROOT / "positive_contract.json"),
            "--summary-json",
            str(json_out),
            "--summary-md",
            str(md_out),
            "--check",
        ]
    )

    assert code == 1
    captured = capsys.readouterr()
    assert "support classification output drift" in captured.err
