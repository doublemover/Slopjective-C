from __future__ import annotations

from scripts.objc3c_workflow.report_files import write_json_report
from scripts.objc3c_workflow.report_output import emit_json
from scripts.objc3c_workflow.report_policy import (
    REPORT_FILE_OWNER,
    REPORT_RENDERING_OWNER,
    REPORT_STDOUT_OWNER,
    REPORT_SUCCESS_EXIT_CODE,
    report_file_path,
    report_success_exit_code,
)
from scripts.objc3c_workflow.report_rendering import render_report_json


def test_workflow_report_policy_owns_stdout_file_and_success_exit(tmp_path, capsys) -> None:
    assert REPORT_RENDERING_OWNER == "objc3c-workflow-report-rendering"
    assert REPORT_STDOUT_OWNER == "objc3c-workflow-report-stdout"
    assert REPORT_FILE_OWNER == "objc3c-workflow-report-file"
    assert REPORT_SUCCESS_EXIT_CODE == 0
    assert report_success_exit_code() == REPORT_SUCCESS_EXIT_CODE

    path = tmp_path / "report.json"
    assert report_file_path(path) == path
    assert render_report_json({"status": "ok"}) == '{\n  "status": "ok"\n}\n'

    assert emit_json({"status": "ok"}) == REPORT_SUCCESS_EXIT_CODE
    assert capsys.readouterr().out == '{\n  "status": "ok"\n}\n'
    assert write_json_report(path, {"status": "ok"}) == path
    assert path.read_text(encoding="utf-8") == '{\n  "status": "ok"\n}\n'
