#!/usr/bin/env python3
"""Fail-closed M12 fixture/manifest parity checker for conformance lanes A-D."""

from __future__ import annotations

import argparse
import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CONFORMANCE_ROOT = ROOT / "tests" / "conformance"


@dataclass(frozen=True)
class LaneSpec:
    lane: str
    bucket: str
    first_issue: int
    last_issue: int
    required_group_name: str | None = None

    @property
    def expected_issues(self) -> tuple[int, ...]:
        return tuple(range(self.first_issue, self.last_issue + 1))

    @property
    def expected_files(self) -> tuple[str, ...]:
        count = self.last_issue - self.first_issue + 1
        return tuple(f"M12-{self.lane}{index:03d}.json" for index in range(1, count + 1))

    @property
    def lane_pattern(self) -> re.Pattern[str]:
        return re.compile(rf"^M12-{self.lane}\d{{3}}\.json$")


LANE_SPECS: tuple[LaneSpec, ...] = (
    LaneSpec(
        lane="A",
        bucket="parser",
        first_issue=1724,
        last_issue=1735,
    ),
    LaneSpec(lane="B", bucket="semantic", first_issue=1736, last_issue=1747),
    LaneSpec(lane="C", bucket="lowering_abi", first_issue=1748, last_issue=1759),
    LaneSpec(lane="D", bucket="diagnostics", first_issue=1760, last_issue=1771),
)


def resolve_path(path: Path) -> Path:
    if path.is_absolute():
        return path
    return ROOT / path


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def load_manifest_groups(path: Path, problems: list[str], lane_tag: str) -> list[dict[str, Any]] | None:
    if not path.exists():
        problems.append(f"{lane_tag}: missing manifest file {display_path(path)}")
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        problems.append(f"{lane_tag}: failed to read/parse manifest {display_path(path)} ({exc})")
        return None

    if not isinstance(payload, dict):
        problems.append(f"{lane_tag}: manifest root must be an object ({display_path(path)})")
        return None

    groups = payload.get("groups")
    if not isinstance(groups, list):
        problems.append(f"{lane_tag}: manifest missing list field 'groups' ({display_path(path)})")
        return None

    normalized_groups: list[dict[str, Any]] = []
    for index, entry in enumerate(groups):
        if isinstance(entry, dict):
            normalized_groups.append(entry)
        else:
            problems.append(
                f"{lane_tag}: manifest group[{index}] is not an object ({display_path(path)})"
            )
    return normalized_groups


def extract_issue_numbers(group: dict[str, Any]) -> set[int]:
    values: set[int] = set()
    issue = group.get("issue")
    if isinstance(issue, int):
        values.add(issue)
    issues = group.get("issues")
    if isinstance(issues, list):
        for value in issues:
            if isinstance(value, int):
                values.add(value)
    return values


def group_name(group: dict[str, Any], index: int) -> str:
    value = group.get("name")
    if isinstance(value, str) and value:
        return value
    return f"<group[{index}]>"


def validate_lane_fixture_files(spec: LaneSpec, bucket_dir: Path, problems: list[str]) -> None:
    lane_tag = spec.bucket
    expected = set(spec.expected_files)

    actual: set[str] = set()
    if bucket_dir.exists():
        for path in bucket_dir.iterdir():
            if path.is_file() and spec.lane_pattern.fullmatch(path.name):
                actual.add(path.name)

    missing = sorted(expected - actual)
    unexpected = sorted(actual - expected)

    for name in missing:
        problems.append(f"{lane_tag}: missing fixture file {display_path(bucket_dir / name)}")
    for name in unexpected:
        problems.append(f"{lane_tag}: unexpected fixture file {display_path(bucket_dir / name)}")


def validate_lane_manifest(spec: LaneSpec, bucket_dir: Path, problems: list[str]) -> None:
    lane_tag = spec.bucket
    expected_files = set(spec.expected_files)
    expected_issues = set(spec.expected_issues)
    manifest_path = bucket_dir / "manifest.json"

    groups = load_manifest_groups(manifest_path, problems, lane_tag)
    if groups is None:
        return

    manifest_lane_files: set[str] = set()
    manifest_lane_issues: set[int] = set()

    for index, group in enumerate(groups):
        files = group.get("files")
        if not isinstance(files, list):
            continue
        lane_files: list[str] = []
        for entry in files:
            if isinstance(entry, str) and spec.lane_pattern.fullmatch(entry):
                lane_files.append(entry)
            elif not isinstance(entry, str):
                problems.append(
                    f"{lane_tag}: manifest group '{group_name(group, index)}' has non-string file entry"
                )
        if not lane_files:
            continue
        manifest_lane_files.update(lane_files)
        issue_numbers = extract_issue_numbers(group)
        if not issue_numbers:
            problems.append(
                f"{lane_tag}: manifest group '{group_name(group, index)}' has lane files but no issue anchors"
            )
        manifest_lane_issues.update(issue_numbers)

    if spec.required_group_name is not None:
        named_groups = [group for group in groups if group.get("name") == spec.required_group_name]
        if not named_groups:
            problems.append(f"{lane_tag}: missing required manifest group '{spec.required_group_name}'")
        elif len(named_groups) > 1:
            problems.append(f"{lane_tag}: duplicate manifest group '{spec.required_group_name}'")
        else:
            group = named_groups[0]
            issue_value = group.get("issue")
            if issue_value != spec.first_issue:
                problems.append(
                    f"{lane_tag}: group '{spec.required_group_name}' issue drift "
                    f"(expected {spec.first_issue}, found {issue_value!r})"
                )

            issues_value = group.get("issues")
            expected_issue_list = list(spec.expected_issues)
            if issues_value != expected_issue_list:
                problems.append(
                    f"{lane_tag}: group '{spec.required_group_name}' issues drift "
                    f"(expected {expected_issue_list}, found {issues_value!r})"
                )

            files_value = group.get("files")
            expected_file_list = list(spec.expected_files)
            if files_value != expected_file_list:
                problems.append(
                    f"{lane_tag}: group '{spec.required_group_name}' files drift "
                    f"(expected {expected_file_list}, found {files_value!r})"
                )

    missing_manifest_files = sorted(expected_files - manifest_lane_files)
    unexpected_manifest_files = sorted(manifest_lane_files - expected_files)
    missing_manifest_issues = sorted(expected_issues - manifest_lane_issues)
    unexpected_manifest_issues = sorted(manifest_lane_issues - expected_issues)

    for name in missing_manifest_files:
        problems.append(f"{lane_tag}: manifest missing file entry {name}")
    for name in unexpected_manifest_files:
        problems.append(f"{lane_tag}: manifest has unexpected lane file entry {name}")
    for issue in missing_manifest_issues:
        problems.append(f"{lane_tag}: manifest missing issue {issue}")
    for issue in unexpected_manifest_issues:
        problems.append(f"{lane_tag}: manifest has unexpected issue {issue}")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Check M12 lane A-D fixture/manifest parity fail-closed."
    )
    parser.add_argument(
        "--conformance-root",
        type=Path,
        default=DEFAULT_CONFORMANCE_ROOT,
        help=(
            "Conformance root directory containing parser/semantic/lowering_abi/diagnostics "
            f"(default: {display_path(DEFAULT_CONFORMANCE_ROOT)})."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    conformance_root = resolve_path(args.conformance_root)

    problems: list[str] = []
    for spec in LANE_SPECS:
        bucket_dir = conformance_root / spec.bucket
        validate_lane_fixture_files(spec=spec, bucket_dir=bucket_dir, problems=problems)
        validate_lane_manifest(spec=spec, bucket_dir=bucket_dir, problems=problems)

    print(f"conformance_root: {display_path(conformance_root)}")
    if problems:
        print(f"status: FAIL (drift={len(problems)})")
        for problem in problems:
            print(f"drift: {problem}")
        return 1

    print("status: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())



