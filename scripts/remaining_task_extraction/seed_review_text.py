from __future__ import annotations

import hashlib
import re

from remaining_task_extraction.seed_review_models import RawTask, ReviewedTask

ISSUE_REF_PATTERN = re.compile(r"Issue #(\d+)")
TASK_REF_PATTERN = re.compile(r"\b([ABCD]-\d{2})\b")
LINK_PATTERN = re.compile(r"\[([^\]]+)\]\([^\)]+\)")
CODE_PATTERN = re.compile(r"`([^`]+)`")
SPACES_PATTERN = re.compile(r"\s+")
PLANNING_ISSUE_PATTERN = re.compile(r"issue_(\d+)_")


def clean_markdown_text(text: str) -> str:
    without_links = LINK_PATTERN.sub(r"\1", text)
    without_code = CODE_PATTERN.sub(r"\1", without_links)
    without_emphasis = without_code.replace("**", "")
    without_issue_url = re.sub(r"\(\[Issue #\d+\]\([^\)]+\)\)", "", without_emphasis)
    compact = SPACES_PATTERN.sub(" ", without_issue_url)
    return compact.strip(" .:")


def normalize_title(cleaned: str) -> str:
    text = cleaned
    text = re.sub(r"\s*\([^)]+\)$", "", text)
    text = text.replace('"', "'")
    text = text.strip(" .:")
    if len(text) > 150:
        text = text[:147].rstrip() + "..."
    return text


def build_title(task_id: str, lane: str, cleaned: str) -> str:
    base = normalize_title(cleaned)
    title = f"[{task_id}][Lane {lane}] {base}"
    if len(title) > 240:
        title = title[:237].rstrip() + "..."
    return title


def build_task_key(task: RawTask) -> str:
    return f"{task.path}:{task.line}"


def build_source_line_hash(task: RawTask) -> str:
    payload = f"{task.origin_path}:{task.line}:{task.text}".encode("utf-8")
    return hashlib.sha256(payload).hexdigest()


def format_source_reference(task: ReviewedTask) -> str:
    return f"{task.path}:{task.line}"
