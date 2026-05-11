#!/usr/bin/env python3
"""Audit Objective-C 3 planning publication drift and refresh durable references."""

from __future__ import annotations

import sys

from scripts.objc3c_planning_publication_audit.cli import parse_args
from scripts.objc3c_planning_publication_audit.errors import DriftAuditError
from scripts.objc3c_planning_publication_audit.live_github import compare_live_github
from scripts.objc3c_planning_publication_audit.paths import (
    DEFAULT_DRIFT_REPORT,
    DEFAULT_MARKDOWN_REPORT,
    DEFAULT_PAYLOAD,
    DEFAULT_PUBLICATION_REPORT,
    ROOT,
    repo_path,
)
from scripts.objc3c_planning_publication_audit.runner import main
from scripts.objc3c_planning_publication_audit.shared import publisher, require_dict

publication_snapshot = publisher.publication_snapshot
render_markdown_report = publisher.render_markdown_report


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
