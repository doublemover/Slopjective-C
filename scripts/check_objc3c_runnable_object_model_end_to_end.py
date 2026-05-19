#!/usr/bin/env python3
"""CLI facade for the runnable object-model E2E checker."""

from __future__ import annotations

from check_objc3c_runnable_object_model_end_to_end import (
    Any,
    CompileArtifacts,
    E2ERunPaths,
    MANIFEST_KEYS,
    PACKAGE_CONTRACT_ID,
    PACKAGE_PS1,
    PWSH,
    PackagedToolchain,
    Path,
    REPORT_PATH,
    ROOT,
    RUNNER_PATH,
    SUMMARY_CONTRACT_ID,
    Sequence,
    build_payload,
    build_run_paths,
    compile_artifacts_for,
    compile_canonical_fixture,
    compile_packaged_probe,
    datetime,
    expect,
    expect_compile_artifacts,
    expect_manifest_contract,
    expect_manifest_files,
    expect_probe_payload,
    extract_output_value,
    extract_report_paths,
    find_clangxx,
    load_json,
    load_package_manifest,
    main,
    normalize_rel_path,
    package_toolchain,
    parse_json_output,
    re,
    repo_rel,
    resolve_packaged_toolchain,
    run_capture,
    run_object_model_end_to_end,
    run_packaged_probe,
    run_packaged_replay,
    run_packaged_smoke,
    shutil,
    sys,
    timezone,
    write_json_file,
    write_summary,
)


if __name__ == "__main__":
    raise SystemExit(main())
