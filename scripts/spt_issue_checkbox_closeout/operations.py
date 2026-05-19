from __future__ import annotations

import re
from collections.abc import Callable
from dataclasses import dataclass

from .github import close_issue, fetch_open_spt_issues
from .models import IssueRef


@dataclass(frozen=True)
class IssueOperations:
    fetch_open_spt_issues: Callable[[re.Pattern[str]], dict[str, IssueRef]]
    close_issue: Callable[[int, str], None]


def default_operations() -> IssueOperations:
    return IssueOperations(
        fetch_open_spt_issues=fetch_open_spt_issues,
        close_issue=close_issue,
    )
