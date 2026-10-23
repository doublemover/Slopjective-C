"""Run authored native feature witnesses against an already-built Linux compiler.

This driver never builds/seals an SDK or qualifies an archive. Source and PCH
cases compile into private temporary directories; only command logs and the
batch report survive. All execution is explicit through this entry point.
"""
from __future__ import annotations

import argparse
from dataclasses import dataclass
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

if __package__:
    from .sdk_example import environment, write_json
else:
    from sdk_example import environment, write_json

ROOT = Path(__file__).resolve().parents[2]
FEATURES = ("captures", "keypaths", "async")
MODES = (("objective-c", "c17"), ("objective-c++", "c++20"),
         ("objective-c++", "c++23"))


@dataclass(frozen=True)
class Configuration:
    feature: str
    language: str
    standard: str
    ownership: str
    optimization: str
    transport: str
    provider_ownership: str = ""

    @property
    def name(self) -> str:
        return "-".join(filter(None, (self.feature, self.standard, self.ownership,
            self.optimization, self.transport, self.provider_ownership)))


def configurations(features=FEATURES, transports=("source", "pch")):
    for feature in features:
        for language, standard in MODES:
            for ownership in ("arc", "mrc"):
                for optimization in ("O0", "O2"):
                    for transport in transports:
                        providers = ("arc", "mrc") if feature == "async" else ("",)
                        for provider in providers:
                            yield Configuration(feature, language, standard, ownership,
                                                optimization, transport, provider)


def run_batch(sdk: Path, output: Path, *, compiler: Path | None = None,
              headers: Path | None = None, include_root: Path | None = None,
              runtime_library: Path | None = None, objc_runtime: Path | None = None,
              fixtures: Path | None = None, features=FEATURES,
              transports=("source", "pch")) -> dict:
    sdk = sdk.resolve(strict=True)
    output = output.resolve()
    compiler = (compiler or sdk / "bin/clang").resolve(strict=True)
    headers = (headers or sdk / "include/objc3c/runtime").resolve(strict=True)
    include_root = (include_root or sdk / "include").resolve(strict=True)
    runtime_library = (runtime_library or sdk / "lib/libobjc3_runtime_services.a").resolve(strict=True)
    objc_runtime = (objc_runtime or sdk / "lib/libobjc.so").resolve(strict=True)
    source_fixtures = ROOT / "tests/clang_foundation"
    fixtures = (fixtures or (source_fixtures if source_fixtures.is_dir() else
        sdk / "share/objc3c/examples/linux-features")).resolve(strict=True)
    # Outputs must not replace source, runtime inputs or the active installation.
    for protected in (sdk, headers, include_root, fixtures):
        if output == protected or protected in output.parents or output in protected.parents:
            raise ValueError("batch output overlaps an input tree: " + str(protected))
    for protected in (compiler, runtime_library, objc_runtime):
        if output == protected or output in protected.parents:
            raise ValueError("batch output contains an input file: " + str(protected))
    output.mkdir(parents=True, exist_ok=True)
    report_path = output / "batch.json"
    report_path.unlink(missing_ok=True)
    env = environment(sdk)
    env["LD_LIBRARY_PATH"] = os.pathsep.join((str(objc_runtime.parent), str(sdk / "lib")))
    report = {"schema": "objc3c.linux-development-features.v1", "status": "running",
              "compiler": str(compiler), "runtime_library": str(runtime_library),
              "headers": str(headers), "include_root": str(include_root),
              "objc_runtime": str(objc_runtime), "fixtures": str(fixtures),
              "commands": [], "cases": []}

    def invoke(label, argv, cwd):
        log = output / (label + ".log")
        command = list(map(str, argv))
        with log.open("w", encoding="utf-8") as stream:
            try:
                completed = subprocess.run(command, cwd=cwd, env=env, stdout=stream,
                    stderr=subprocess.STDOUT, timeout=180, check=False)
                code = completed.returncode
            except subprocess.TimeoutExpired:
                stream.write("\nCommand timed out after 180 seconds.\n")
                code = 124
        report["commands"].append({"name": label, "argv": command,
                                   "exit_code": code, "log": log.name})
        if code:
            raise RuntimeError("native feature command failed: " + label)

    def flags(language, standard, ownership, optimization, work):
        result = ["--no-default-config", "-x", language, "-std=" + standard,
            "-" + optimization, "-g", "-UNDEBUG", "-fobjc-version=3",
            "-fobjc-runtime=gnustep-2.2", "-fblocks", "-fexceptions",
            "-fobjc-exceptions", "-fobjc-arc-exceptions",
            "-fmodules", "-fmodules-cache-path=" + str(work / "modules"),
            "-I" + str(work / "include"),
            "-I" + str(headers), "-I" + str(include_root), "-I" + str(sdk / "include")]
        if language != "c++":
            result.append("-fobjc-arc" if ownership == "arc" else "-fno-objc-arc")
        if "++" in language:
            result.append("-fcoro-aligned-allocation")
        return result

    def compile_source(case, source, work, *, language=None, standard=None,
                       ownership=None, extra=(), output_name=None):
        target = work / (output_name or source.stem + ".o")
        opts = flags(language or case.language, standard or case.standard,
                     ownership or case.ownership, case.optimization, work)
        invoke(case.name + "-" + target.stem, [compiler, *opts, *extra,
            "-c", source, "-o", target], work)
        return target

    def pch(case, header, work):
        target = work / "fixture.pch"
        opts = flags(case.language + "-header", case.standard, case.ownership,
                     case.optimization, work)
        # Header language spellings preserve Objective-C/Objective-C++ ownership.
        invoke(case.name + "-pch", [compiler, *opts, header, "-o", target], work)
        return ["-include-pch", str(target)]

    def execute(case, objects, work, suffix=""):
        executable = work / ("witness" + suffix)
        invoke(case.name + suffix + "-link", [sdk / "bin/clang++", "--no-default-config",
            "-fuse-ld=lld", *objects, runtime_library, objc_runtime, "-pthread",
            "-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff",
            "-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff",
            "-Wl,-rpath," + str(objc_runtime.parent), "-o", executable], work)
        invoke(case.name + suffix + "-execute", [executable], work)

    try:
        for case in configurations(features, transports):
            with tempfile.TemporaryDirectory(prefix="objc3-feature-") as temporary:
                work = Path(temporary)
                (work / "include/objc3c").mkdir(parents=True)
                (work / "include/objc3c/runtime").symlink_to(headers, target_is_directory=True)
                family = {"captures": "explicit_capture_lists", "keypaths": "typed_keypaths",
                          "async": "native_c_async"}[case.feature]
                source = fixtures / family
                shutil.copytree(source, work, dirs_exist_ok=True)
                if case.feature == "async":
                    provider = compile_source(case, work / "provider.m", work,
                        language="objective-c", standard="c17", ownership=case.provider_ownership,
                        output_name="provider.o")
                    foreign = compile_source(case, work / "foreign.cpp", work,
                        language="c++", standard="c++20", output_name="foreign.o")
                    # The implementation source is absent during consumer/PCH compilation.
                    (work / "provider.m").unlink()
                    extra = pch(case, work / "provider.h", work) if case.transport == "pch" else []
                    client = work / ("client.m" if case.language == "objective-c" else "client.mm")
                    consumer = compile_source(case, client, work, extra=extra,
                                              output_name="consumer.o")
                    execute(case, [provider, foreign, consumer], work)
                else:
                    extra = []
                    if case.transport == "pch":
                        extra = pch(case, work / "runtime.h", work)
                        extra.append("-D" + ("CAPTURE_RUNTIME_PCH" if case.feature == "captures"
                                             else "TYPED_KEYPATH_RUNTIME_PCH"))
                    objects = []
                    if case.feature == "keypaths":
                        objects.append(compile_source(case, work / "provider.m", work, extra=extra))
                        (work / "provider.m").unlink()
                    objects.append(compile_source(case, work / "main.m", work, extra=extra))
                    execute(case, objects, work)
                    if case.feature == "captures" and "++" in case.language and case.transport == "source":
                        exception = compile_source(case, work / "exceptions.mm", work)
                        execute(case, [exception], work, "-exceptions")
                report["cases"].append({"name": case.name, "status": "pass"})
            write_json(report_path, report)
        report["status"] = "pass"
    except (OSError, RuntimeError, ValueError) as error:
        report["status"] = "failed"
        report["error"] = str(error)
        write_json(report_path, report)
        raise
    write_json(report_path, report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sdk", type=Path)
    parser.add_argument("output", type=Path)
    for name in ("compiler", "headers", "include-root", "runtime-library", "objc-runtime", "fixtures"):
        parser.add_argument("--" + name, type=Path)
    parser.add_argument("--feature", action="append", choices=FEATURES)
    parser.add_argument("--transport", action="append", choices=("source", "pch"))
    args = parser.parse_args()
    options = {name: getattr(args, name) for name in
        ("compiler", "headers", "include_root", "runtime_library", "objc_runtime", "fixtures")}
    report = run_batch(args.sdk, args.output, **options,
                       features=tuple(dict.fromkeys(args.feature or FEATURES)),
                       transports=tuple(dict.fromkeys(args.transport or ("source", "pch"))))
    print(json.dumps({"status": report["status"], "cases": len(report["cases"]),
                      "commands": len(report["commands"])}, sort_keys=True))


if __name__ == "__main__":
    main()
