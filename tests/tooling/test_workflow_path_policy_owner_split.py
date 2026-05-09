from __future__ import annotations

from pathlib import Path

from scripts.objc3c_workflow.path_imports import install_import_roots
from scripts.objc3c_workflow.path_policy import (
    WORKFLOW_IMPORT_ROOT_POLICY_OWNER,
    WORKFLOW_PATH_POLICY_OWNER,
    import_root_text,
    missing_import_roots,
    ordered_import_roots,
)


def test_workflow_path_policy_owns_import_root_order_and_text() -> None:
    root = Path("repo")
    script_root = root / "scripts"

    assert WORKFLOW_PATH_POLICY_OWNER == "objc3c-workflow-path-policy"
    assert WORKFLOW_IMPORT_ROOT_POLICY_OWNER == "objc3c-workflow-import-root-policy"
    assert ordered_import_roots(root, script_root) == (root, script_root)
    assert import_root_text(script_root) == str(script_root)
    assert missing_import_roots([str(root)], [root, script_root]) == [str(script_root)]


def test_install_import_roots_preserves_policy_order(monkeypatch) -> None:
    path_entries = ["existing"]
    monkeypatch.setattr("scripts.objc3c_workflow.path_imports.sys.path", path_entries)

    install_import_roots([Path("repo"), Path("repo/scripts")])
    install_import_roots([Path("repo"), Path("repo/scripts")])

    assert path_entries[:2] == [str(Path("repo")), str(Path("repo/scripts"))]
    assert path_entries.count(str(Path("repo"))) == 1
    assert path_entries.count(str(Path("repo/scripts"))) == 1
