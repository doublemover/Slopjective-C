#!/usr/bin/env python3
"""Deterministically probe LLVM tool availability and llvm-direct capability surface."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "objc3c-llvm-capabilities-v1"


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--clang", type=Path, default=Path("clang"))
    parser.add_argument("--llc", type=Path, default=Path("llc"))
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/objc3c_llvm_capabilities_summary.json"),
    )
    return parser.parse_args(argv)


def run_command(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        capture_output=True,
        text=True,
        check=False,
    )


def first_non_empty_line(text: str) -> str:
    for line in text.splitlines():
        stripped = line.strip()
        if stripped:
            return stripped
    return ""


def probe_executable(path: Path, *, role: str) -> dict[str, object]:
    version_cmd = [str(path), "--version"]
    version_result = run_command(version_cmd)
    version_text = (version_result.stdout or "") + (version_result.stderr or "")

    found = version_result.returncode != 127
    head = first_non_empty_line(version_text)
    payload: dict[str, object] = {
        "role": role,
        "path": str(path),
        "found": found,
        "version_exit_code": version_result.returncode,
        "version_headline": head,
    }
    if not found:
        payload["diagnostic"] = f"{role} executable not found: {path}"
    return payload


def probe_llc_filetype_obj(path: Path) -> dict[str, object]:
    help_cmd = [str(path), "--help"]
    help_result = run_command(help_cmd)
    help_text = (help_result.stdout or "") + (help_result.stderr or "")

    mentions_filetype = "--filetype" in help_text or "-filetype" in help_text
    mentions_obj = re.search(r"\bobj\b", help_text) is not None
    supports_from_help = mentions_filetype and mentions_obj

    version_with_filetype_cmd = [str(path), "--filetype=obj", "--version"]
    filetype_version_result = run_command(version_with_filetype_cmd)
    supports_from_command = filetype_version_result.returncode != 127

    supports_filetype_obj = supports_from_help or supports_from_command

    return {
        "help_exit_code": help_result.returncode,
        "help_mentions_filetype": mentions_filetype,
        "help_mentions_obj": mentions_obj,
        "version_with_filetype_exit_code": filetype_version_result.returncode,
        "supports_filetype_obj": supports_filetype_obj,
    }


def write_json(path: Path, payload: object) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(canonical_json(payload), encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    clang_probe = probe_executable(args.clang, role="clang")
    llc_probe = probe_executable(args.llc, role="llc")

    llc_features: dict[str, object] = {
        "supports_filetype_obj": False,
    }
    if bool(llc_probe["found"]):
        llc_features = probe_llc_filetype_obj(args.llc)

    failures: list[str] = []
    if not bool(clang_probe["found"]):
        failures.append(str(clang_probe.get("diagnostic", "clang executable missing")))
    if not bool(llc_probe["found"]):
        failures.append(str(llc_probe.get("diagnostic", "llc executable missing")))
    if bool(llc_probe["found"]) and not bool(llc_features["supports_filetype_obj"]):
        failures.append("llc capability probe failed: --filetype=obj support not detected")

    summary = {
        "mode": MODE,
        "clang": clang_probe,
        "llc": llc_probe,
        "llc_features": llc_features,
        "failures": failures,
        "ok": not failures,
    }
    write_json(args.summary_out, summary)

    if failures:
        for failure in failures:
            print(f"LLVM-CAP-FAIL: {failure}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("LLVM-CAP-PASS: clang+llc capabilities discovered")
    print(f"wrote summary: {display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


if __name__ == "__main__":
    main()
