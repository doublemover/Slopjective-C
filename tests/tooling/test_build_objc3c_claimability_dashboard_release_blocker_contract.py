from __future__ import annotations

from pathlib import Path

from build_objc3c_claimability_dashboard_release_blocker_contract_behavior import (
    assert_dashboard_release_blocker_drift_rejection,
    assert_dashboard_release_blocker_projection_payload,
)
from build_objc3c_claimability_dashboard_release_blocker_contract_json import (
    read_dashboard_release_blocker_json,
)
from build_objc3c_claimability_dashboard_release_blocker_contract_support import (
    builder,
)


def dashboard_release_blocker_contract_summary(tmp_path: Path) -> None:
    json_out = tmp_path / "dashboard_release_blocker_contract_summary.json"
    md_out = tmp_path / "dashboard_release_blocker_contract_summary.md"

    code = builder.main(["--summary-json", str(json_out), "--summary-md", str(md_out)])

    assert code == 0
    payload = read_dashboard_release_blocker_json(json_out)
    assert_dashboard_release_blocker_projection_payload(payload)


def dashboard_release_blocker_contract_check_mode_fails_on_drift(
    tmp_path: Path,
    capsys: object,
) -> None:
    json_out = tmp_path / "summary.json"
    md_out = tmp_path / "summary.md"
    json_out.write_text("{}\n", encoding="utf-8")
    md_out.write_text("# stale\n", encoding="utf-8")

    code = builder.main(
        ["--summary-json", str(json_out), "--summary-md", str(md_out), "--check"]
    )

    assert code == 1
    captured = capsys.readouterr()
    assert_dashboard_release_blocker_drift_rejection(captured.err)


test_dashboard_release_blocker_contract_summary = (
    dashboard_release_blocker_contract_summary
)
test_dashboard_release_blocker_contract_check_mode_fails_on_drift = (
    dashboard_release_blocker_contract_check_mode_fails_on_drift
)
