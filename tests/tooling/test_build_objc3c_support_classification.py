from __future__ import annotations

from pathlib import Path

from build_objc3c_support_classification_artifacts import (
    assert_support_classification_artifacts_absent,
    assert_support_classification_artifacts_exist,
)
from build_objc3c_support_classification_behavior import (
    assert_check_mode_drift_rejection,
    assert_default_contract_summary_payload,
    assert_positive_fixture_report_text,
    assert_positive_fixture_summary_payload,
    assert_unknown_surface_class_rejection,
)
from build_objc3c_support_classification_json import read_support_classification_json
from build_objc3c_support_classification_support import FIXTURE_ROOT, builder


def generator_emits_durable_summary_from_positive_fixture(tmp_path: Path) -> None:
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
    assert_support_classification_artifacts_exist(json_out, md_out)

    payload = read_support_classification_json(json_out)
    assert_positive_fixture_summary_payload(payload)

    report_text = md_out.read_text(encoding="utf-8")
    assert_positive_fixture_report_text(report_text)


def generator_rejects_unknown_surface_class(capsys: object, tmp_path: Path) -> None:
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
    assert_support_classification_artifacts_absent(json_out, md_out)
    captured = capsys.readouterr()
    assert_unknown_surface_class_rejection(captured.err)


def default_contract_generates_checked_in_support_classification(
    tmp_path: Path,
) -> None:
    json_out = tmp_path / "default_summary.json"
    md_out = tmp_path / "default_summary.md"

    code = builder.main(["--summary-json", str(json_out), "--summary-md", str(md_out)])

    assert code == 0
    payload = read_support_classification_json(json_out)
    assert_default_contract_summary_payload(payload)


def check_mode_fails_on_drift(tmp_path: Path, capsys: object) -> None:
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
    assert_check_mode_drift_rejection(captured.err)


test_generator_emits_durable_summary_from_positive_fixture = (
    generator_emits_durable_summary_from_positive_fixture
)
test_generator_rejects_unknown_surface_class = generator_rejects_unknown_surface_class
test_default_contract_generates_checked_in_support_classification = (
    default_contract_generates_checked_in_support_classification
)
test_check_mode_fails_on_drift = check_mode_fails_on_drift
