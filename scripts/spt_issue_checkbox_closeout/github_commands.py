from __future__ import annotations


def list_open_issue_json_args() -> list[str]:
    return ["issue", "list", "--state", "open", "--limit", "2000", "--json", "number,title,url"]


def close_issue_args(number: int, comment: str) -> list[str]:
    return ["gh", "issue", "close", str(number), "--comment", comment]
