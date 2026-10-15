from __future__ import annotations

from scripts.objc3c_workflow.actions import validation_timing_changed_paths as changed


def test_changed_path_inventory_includes_branch_staged_worktree_untracked_and_deletes(monkeypatch):
    base = "known-merge-base"

    def fake_git(*args: str):
        class Result:
            returncode = 0
            stdout = ""

        result = Result()
        if args[:2] == ("diff", "--no-renames") and any(
            f"{base}...HEAD" == item for item in args
        ):
            result.stdout = "committed/parser-change.cpp\0deleted/old-module.objc3i\0"
        elif args[:2] == ("diff", "--cached"):
            result.stdout = "staged/interface.json\0"
        elif args[:2] == ("diff", "HEAD"):
            result.stdout = "worktree/runtime.cpp\0"
        elif args[:2] == ("ls-files", "--others"):
            result.stdout = "untracked/new-fixture.mm\0"
        else:
            raise AssertionError(f"unexpected git query: {args!r}")
        return result

    monkeypatch.setattr(changed, "_integration_base", lambda: base)
    monkeypatch.setattr(changed, "_git", fake_git)

    assert changed.git_changed_paths() == [
        "committed/parser-change.cpp",
        "deleted/old-module.objc3i",
        "staged/interface.json",
        "untracked/new-fixture.mm",
        "worktree/runtime.cpp",
    ]


def test_git_inventory_failure_widens_to_unknown_repo_profile(monkeypatch):
    class Failed:
        returncode = 128
        stdout = ""

    monkeypatch.setattr(changed, "_integration_base", lambda: "base")
    monkeypatch.setattr(changed, "_git", lambda *_: Failed())

    paths = changed.git_changed_paths()
    assert paths == ["*"]
    selected = changed.select_validation_profiles(paths)
    assert "repo" in {profile["profile"] for profile in selected["profiles"]}
