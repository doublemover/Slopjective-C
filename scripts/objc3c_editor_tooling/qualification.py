#!/usr/bin/env python3
"""Qualify the sealed ObjC3 Clang, clang-format, and clangd development tools."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from scripts.objc3c_editor_tooling.lsp_session import qualify_lsp

REPORT_PATH: Path | None = None
REPORT: dict[str, Any] = {}

def save_report() -> None:
    if REPORT_PATH is not None:
        REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
        temporary = REPORT_PATH.with_suffix(REPORT_PATH.suffix + ".tmp")
        temporary.write_text(json.dumps(REPORT, indent=2) + "\n", encoding="utf-8")
        temporary.replace(REPORT_PATH)

def directory_digest(root: Path) -> str:
    digest = hashlib.sha256()
    for path in sorted(root.rglob("*")):
        if path.is_file():
            digest.update(path.relative_to(root).as_posix().encode("utf-8"))
            digest.update(b"\0")
            digest.update(bytes.fromhex(sha256(path)))
    return digest.hexdigest()

def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()

def run(argv: list[str], *, cwd: Path, env: dict[str, str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(argv, cwd=cwd, env=env, text=True, capture_output=True, check=False)
    if result.returncode:
        detail = (result.stdout + result.stderr).strip()
        raise RuntimeError(f"command failed ({result.returncode}): {argv!r}\n{detail}")
    return result

def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)

def main() -> int:
    global REPORT_PATH
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", type=Path, default=Path.cwd())
    parser.add_argument("--work-dir", type=Path)
    parser.add_argument("--sdk-root", type=Path, required=True)
    parser.add_argument("--target", required=True)
    parser.add_argument("--report", type=Path)
    args = parser.parse_args()
    repo = args.repo.resolve()
    build = args.sdk_root.resolve(strict=True)
    source = repo / "out" / "toolchains" / "objc3-development" / "src"
    fixtures = repo / "tests" / "tooling" / "fixtures" / "objc3_editor"
    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    verify = run([sys.executable, "-B", "-m", "scripts.objc3c_toolchain", "verify-development",
                  "--repo", str(repo), "--source-dir", str(source)], cwd=repo, env=env)
    sealed: dict[str, Any] = json.loads(verify.stdout)
    require(sealed.get("source_kind") == "development", "source is not sealed development source")
    patch_digest = sealed.get("patch_digest")
    require(isinstance(patch_digest, str) and re.fullmatch(r"[0-9a-f]{64}", patch_digest),
            "sealed source has no SHA256 patch digest")
    require(isinstance(sealed.get("build_id"), str) and sealed["build_id"], "sealed source has no build id")
    run_identity = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S.%fZ") + f"-pid{os.getpid()}"
    work = (args.work_dir or (repo / "temp" / "full-compatibility" /
                              "editor-qualification" / sealed["build_id"] / run_identity)).resolve()
    work.mkdir(parents=True, exist_ok=True)
    REPORT_PATH = (args.report or work / "results.json").resolve()
    REPORT.update(status="running", run_identity=run_identity, started_utc=datetime.now(timezone.utc).isoformat(),
                  build_identity=sealed["build_id"], patch_digest=patch_digest)
    save_report()
    tools = {name: build / "bin" / name for name in ("clang", "clang-format", "clangd")}
    for name, path in tools.items():
        require(path.is_file(), f"required tool is missing: {name}: {path}")
    tool_identity: dict[str, Any] = {}
    for name, path in tools.items():
        version = run([str(path), "--version"], cwd=repo, env=env).stdout.strip()
        require(f"ObjC3-patches={patch_digest}" in version,
                f"{name} version does not identify sealed patch digest {patch_digest}: {version}")
        tool_identity[name] = {"path": str(path.resolve()), "sha256": sha256(path), "version": version}
    clang = tools["clang"]
    resource_dir = Path(run([str(clang), "-print-resource-dir"], cwd=repo, env=env).stdout.strip()).resolve()
    version_match = re.search(r"clang version (\d+)", tool_identity["clang"]["version"])
    require(version_match is not None, "cannot extract Clang major version")
    expected_resource = (build / "lib" / "clang" / version_match.group(1)).resolve()
    require(resource_dir == expected_resource, f"resource directory mismatch: expected {expected_resource}, got {resource_dir}")
    require((resource_dir / "include").is_dir(), f"resource headers missing under {resource_dir}")
    cases = json.loads((fixtures / "qualification-cases.json").read_text(encoding="utf-8"))
    normalized_dir = work / "normalized"
    normalized_dir.mkdir(parents=True, exist_ok=True)
    expected_formatted = (fixtures / "public_core.formatted").read_text(encoding="utf-8").replace("\r\n", "\n")
    expected_source_hash: str | None = None
    normalized_sources: list[tuple[Path, dict[str, Any]]] = []
    format_evidence: dict[str, Any] = {}
    for case in cases["cases"]:
        source_fixture = fixtures / case["source"]
        source_hash = sha256(source_fixture)
        if expected_source_hash is None:
            expected_source_hash = source_hash
        require(source_hash == expected_source_hash, "C17 .m and C++20 .mm fixture bytes must match")
        formatted = run([str(tools["clang-format"]), f"--style={cases['format_style']}", str(source_fixture)],
                        cwd=repo, env=env).stdout.replace("\r\n", "\n")
        (work / f"{source_fixture.stem}.formatted{source_fixture.suffix}").write_text(formatted, encoding="utf-8", newline="\n")
        for spelling in cases.get("required_ordinary_spellings", []):
            require(spelling in formatted, f"clang-format changed or removed ordinary spelling {spelling!r}")
        for spelling in cases.get("required_private_spellings", []):
            require(spelling in formatted, f"clang-format changed or removed private spelling {spelling!r}")
        for spelling in cases.get("required_public_spellings", []):
            require(spelling in formatted, f"clang-format changed or removed public spelling {spelling!r}")
        require(formatted == expected_formatted,
                f"clang-format output for {source_fixture.name} differs from public_core.formatted; actual saved in {work}")
        normalized = (normalized_dir / source_fixture.name).resolve()
        normalized.write_text(formatted, encoding="utf-8", newline="\n")
        second_format = run([str(tools["clang-format"]), f"--style={cases['format_style']}",
                             str(normalized)], cwd=repo, env=env).stdout.replace("\r\n", "\n")
        require(second_format == formatted, f"formatting is not idempotent for {source_fixture.name}")
        format_evidence[case["source"]] = {"input_sha256": source_hash,
            "formatted_sha256": hashlib.sha256(formatted.encode("utf-8")).hexdigest()}
        normalized_sources.append((normalized, case))
    require(len(normalized_sources) == 2, "both Objective-C and Objective-C++ cases are required")
    compile_commands: list[dict[str, Any]] = []
    normalized_argv: dict[str, list[str]] = {}
    for path, case in normalized_sources:
        argv = [str(clang), "-x", case["language"], f"-std={case['standard']}", "-fobjc-version=3",
                "--target=" + args.target, "-fobjc-runtime=gnustep-2.2",
                "-fobjc-arc", "-fblocks",
                f"-resource-dir={resource_dir}", "-fsyntax-only", str(path)]
        normalized_argv[path.suffix] = argv
        compile_commands.append({"directory": str(normalized_dir), "file": str(path), "arguments": argv})
    database = normalized_dir / "compile_commands.json"
    database.write_text(json.dumps(compile_commands, indent=2) + "\n", encoding="utf-8")
    command_results: dict[str, Any] = {}
    for path, case in normalized_sources:
        compile_result = run(normalized_argv[path.suffix], cwd=normalized_dir, env=env)
        check = run([str(tools["clangd"]), f"--check={path}",
                     f"--compile-commands-dir={normalized_dir}", f"--resource-dir={resource_dir}",
                     "--log=verbose", "--enable-config=0"], cwd=repo, env=env)
        transcript = check.stdout + check.stderr
        (work / ("clangd" + path.suffix + ".log")).write_text(transcript, encoding="utf-8")
        require("All checks completed, 0 errors" in transcript, f"clangd reported errors for {path.name}:\n{transcript}")
        require("Compile command from CDB is:" in transcript, f"clangd ignored normalized compilation database for {path.name}")
        require(f"-std={case['standard']}" in transcript and case["language"] in transcript and
                "-fobjc-version=3" in transcript,
                f"clangd did not use expected language mode/opt-in for {path.name}:\n{transcript}")
        resource = str(resource_dir).replace("/", "\\").lower()
        require(resource in transcript.replace("/", "\\").lower(),
                f"clangd did not use matching sealed resource dir for {path.name}")
        command_results[path.suffix] = {"source": str(path), "compile_exit_code": compile_result.returncode,
            "clangd_exit_code": check.returncode,
            "clangd_transcript_sha256": hashlib.sha256(transcript.encode("utf-8")).hexdigest(),
            "language": case["language"], "standard": case["standard"]}
        live = qualify_lsp(tools["clangd"], path, normalized_dir, resource_dir, work, env,
                           probes=cases.get("live_probes", []) + case.get("live_probes", []))
        live["transcript_sha256"] = sha256(Path(live["transcript"]))
        live["stderr_sha256"] = sha256(Path(live["stderr"]))
        command_results[path.suffix]["live_lsp"] = live
    report = {"schema_version": 1,
        "source": {"build_id": sealed["build_id"], "tree_sha256": sealed["tree_sha256"], "patch_digest": patch_digest},
        "tools": tool_identity, "target_triple": args.target, "resource_dir": str(resource_dir),
        "resource_headers_sha256": directory_digest(resource_dir / "include"), "fixtures": format_evidence,
        "compile_commands": {"path": str(database), "sha256": sha256(database), "normalized_arguments": normalized_argv},
        "results": command_results}
    REPORT.update(report, status="passed", finished_utc=datetime.now(timezone.utc).isoformat())
    save_report()
    rendered = json.dumps(REPORT, indent=2) + "\n"
    print(rendered, end="")
    return 0

if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        REPORT.update(status="failed", error=str(exc), finished_utc=datetime.now(timezone.utc).isoformat())
        save_report()
        print(f"editor qualification failed: {exc}", file=sys.stderr)
        raise SystemExit(1)


