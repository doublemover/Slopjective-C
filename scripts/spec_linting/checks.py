from __future__ import annotations

from pathlib import Path
from urllib.parse import urlparse

from .constants import (
    BULLET_LINE_RE,
    CONJUNCTION_TAIL_RE,
    HEADING_ANCHOR_RE,
    HEADING_LINE_RE,
    HEADING_WITH_NUMBER_RE,
    HORIZONTAL_RULE_RE,
    MARKDOWN_LINK_RE,
    TOP_LEVEL_SECTION_RE,
)
from .models import LintError


def next_nonblank_index(lines: list[str], start: int) -> int | None:
    idx = start
    while idx < len(lines):
        if lines[idx].strip():
            return idx
        idx += 1
    return None


def collect_anchor_index(paths: list[Path]) -> tuple[set[str], dict[Path, set[str]]]:
    global_anchors: set[str] = set()
    file_anchors: dict[Path, set[str]] = {}

    for path in paths:
        lines = path.read_text(encoding="utf-8").splitlines()
        anchors: set[str] = set()
        in_fence = False

        for line in lines:
            stripped = line.strip()
            if stripped.startswith("```") or stripped.startswith("~~~"):
                in_fence = not in_fence
                continue
            if in_fence:
                continue

            heading_anchor_match = HEADING_ANCHOR_RE.match(line)
            if heading_anchor_match:
                anchors.add(heading_anchor_match.group(2))

        resolved = path.resolve()
        file_anchors[resolved] = anchors
        global_anchors.update(anchors)

    return global_anchors, file_anchors


def is_external_link(target: str) -> bool:
    if target.startswith("//"):
        return True
    parsed = urlparse(target)
    return bool(parsed.scheme)


def normalize_link_target(target: str) -> str:
    target = target.strip()
    if target.startswith("<") and target.endswith(">"):
        target = target[1:-1].strip()
    return target


def lint_file(
    path: Path,
    *,
    global_anchors: set[str],
    file_anchors: dict[Path, set[str]],
) -> list[LintError]:
    errors: list[LintError] = []
    lines = path.read_text(encoding="utf-8").splitlines()

    in_fence = False
    fence_start_line: int | None = None
    section_numbers: dict[str, int] = {}
    anchors: dict[str, int] = {}
    last_major: int | None = None
    last_minor: int | None = None

    for idx, line in enumerate(lines):
        line_no = idx + 1
        stripped = line.strip()

        if stripped.startswith("```") or stripped.startswith("~~~"):
            if not in_fence:
                fence_start_line = line_no
            in_fence = not in_fence
            if not in_fence:
                fence_start_line = None
            continue
        if in_fence:
            continue

        heading_number_match = HEADING_WITH_NUMBER_RE.match(line)
        if heading_number_match:
            number = heading_number_match.group(2)
            if number in section_numbers:
                errors.append(
                    LintError(
                        path=path,
                        line=line_no,
                        message=(
                            f"duplicate section number '{number}' "
                            f"(first seen on line {section_numbers[number]})"
                        ),
                    )
                )
            else:
                section_numbers[number] = line_no

        heading_anchor_match = HEADING_ANCHOR_RE.match(line)
        if heading_anchor_match:
            anchor = heading_anchor_match.group(2)
            if anchor in anchors:
                errors.append(
                    LintError(
                        path=path,
                        line=line_no,
                        message=(
                            f"duplicate heading anchor '{anchor}' "
                            f"(first seen on line {anchors[anchor]})"
                        ),
                    )
                )
            else:
                anchors[anchor] = line_no

        top_level_match = TOP_LEVEL_SECTION_RE.match(line)
        if top_level_match:
            major = int(top_level_match.group(1))
            minor = int(top_level_match.group(2))
            if last_major == major and last_minor is not None and minor <= last_minor:
                errors.append(
                    LintError(
                        path=path,
                        line=line_no,
                        message=(
                            f"non-monotonic top-level section number '{major}.{minor}' "
                            f"(previous was '{major}.{last_minor}')"
                        ),
                    )
                )
            last_major = major
            last_minor = minor

        bullet_match = BULLET_LINE_RE.match(line)
        if bullet_match and CONJUNCTION_TAIL_RE.search(bullet_match.group(1)):
            next_idx = next_nonblank_index(lines, idx + 1)
            if next_idx is None:
                errors.append(
                    LintError(
                        path=path,
                        line=line_no,
                        message="bullet item ends with dangling conjunction at EOF",
                    )
                )
                continue

            next_line = lines[next_idx]
            if HEADING_LINE_RE.match(next_line) or HORIZONTAL_RULE_RE.match(next_line):
                errors.append(
                    LintError(
                        path=path,
                        line=line_no,
                        message=(
                            "bullet item ends with dangling conjunction before section boundary"
                        ),
                    )
                )

        for raw_target in MARKDOWN_LINK_RE.findall(line):
            target = normalize_link_target(raw_target)
            if not target or is_external_link(target):
                continue

            if target.startswith("#"):
                anchor = target[1:]
                if anchor and anchor not in global_anchors:
                    errors.append(
                        LintError(
                            path=path,
                            line=line_no,
                            message=f"invalid cross reference '#{anchor}'",
                        )
                    )
                continue

            if ".md" not in target:
                continue

            file_part, _, frag = target.partition("#")
            file_part = file_part.split("?", 1)[0]
            ref_path = (path.parent / file_part).resolve()
            if not ref_path.exists():
                errors.append(
                    LintError(
                        path=path,
                        line=line_no,
                        message=f"broken markdown link target '{file_part}'",
                    )
                )
                continue

            if frag and ref_path in file_anchors and frag not in file_anchors[ref_path]:
                errors.append(
                    LintError(
                        path=path,
                        line=line_no,
                        message=f"invalid anchor reference '{file_part}#{frag}'",
                    )
                )

    if in_fence:
        errors.append(
            LintError(
                path=path,
                line=fence_start_line or len(lines),
                message="unclosed code fence",
            )
        )

    return errors
