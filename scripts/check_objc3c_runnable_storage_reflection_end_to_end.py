#!/usr/bin/env python3
"""Validate runnable storage/reflection execution end to end from the staged package root."""

from __future__ import annotations

from check_objc3c_runnable_storage_reflection_end_to_end import (
    Any,
    PACKAGE_CONTRACT_ID,
    PACKAGE_PS1,
    PWSH,
    REPORT_PATH,
    ROOT,
    SUMMARY_CONTRACT_ID,
    Path,
    Sequence,
    datetime,
    expect,
    extract_output_value,
    extract_report_paths,
    find_clangxx,
    load_json,
    main,
    normalize_rel_path,
    parse_json_output,
    re,
    repo_rel,
    run_capture,
    shutil,
    sys,
    timezone,
    write_json_file,
)


if __name__ == "__main__":
    raise SystemExit(main())
