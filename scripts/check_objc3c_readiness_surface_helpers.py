#!/usr/bin/env python3
"""Fast native probe for shared readiness-surface string helpers."""

from __future__ import annotations

import shutil
import subprocess
import uuid
from typing import Sequence

from objc3c_tooling.paths import ROOT
from objc3c_tooling.probe_compile import find_clangxx
from objc3c_tooling.subprocesses import failure_snippet, run_capture


TARGET_HELPER_SURFACES = (
    ROOT / "native/objc3c/src/io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h",
    ROOT / "native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h",
    ROOT / "native/objc3c/src/io/objc3_cli_reporting_output_contract_core_feature_surface.h",
)

REMOVED_LOCAL_HELPERS = (
    "Objc3ToolchainRuntimeGaOperationsHasSuffix",
    "Objc3ParseLoweringReadinessKeyHasPrefix",
    "Objc3CliReportingOutputContractHasSuffix",
)


def check_source_ownership() -> None:
    for path in TARGET_HELPER_SURFACES:
        text = path.read_text(encoding="utf-8")
        if '#include "support/objc3_string_predicates.h"' not in text:
            raise RuntimeError(f"{path.relative_to(ROOT)} does not include shared string predicates")
        for helper_name in REMOVED_LOCAL_HELPERS:
            if helper_name in text:
                raise RuntimeError(
                    f"{path.relative_to(ROOT)} reintroduced local helper {helper_name}; "
                    "use support/objc3_string_predicates.h instead"
                )


def main(argv: Sequence[str] | None = None) -> int:
    _ = argv
    check_source_ownership()
    clangxx = find_clangxx()
    temp_root = ROOT / "tmp" / "probe-work"
    temp_root.mkdir(parents=True, exist_ok=True)
    tmp = temp_root / f"objc3c-readiness-surface-helpers-{uuid.uuid4().hex}"
    tmp.mkdir(parents=True)
    try:
        probe = tmp / "objc3_readiness_surface_helpers_probe.cpp"
        exe = tmp / "objc3_readiness_surface_helpers_probe.exe"
        probe.write_text(
            r'''
#include "io/objc3_cli_reporting_output_contract_core_feature_surface.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"
#include "support/objc3_string_predicates.h"

#include <filesystem>
#include <string>

namespace {

bool Expect(bool condition) {
  return condition;
}

}  // namespace

int main() {
  if (!Expect(objc3c::support::StartsWith("parser_snapshot=1", "parser_snapshot=") &&
              !objc3c::support::StartsWith("xparser_snapshot=1", "parser_snapshot=") &&
              objc3c::support::StartsWith("abc", "") &&
              objc3c::support::EndsWith("module.object-backend.txt", ".object-backend.txt") &&
              !objc3c::support::EndsWith("module.object-backend.txt.tmp", ".object-backend.txt") &&
              objc3c::support::EndsWith("abc", ""))) {
    return 2;
  }

  const Objc3CliReportingOutputContractScaffold cli_scaffold =
      BuildObjc3CliReportingOutputContractScaffold(
          std::filesystem::path("out"),
          "module",
          std::filesystem::path("out/module.json"),
          true);
  const Objc3CliReportingOutputContractCoreFeatureSurface cli_surface =
      BuildObjc3CliReportingOutputContractCoreFeatureSurface(
          cli_scaffold,
          std::filesystem::path("out/module.json"),
          std::filesystem::path("out/module.diagnostics.json"));
  if (!Expect(cli_surface.core_feature_impl_ready &&
              cli_surface.diagnostics_output_path_deterministic)) {
    return 3;
  }
  const Objc3CliReportingOutputContractCoreFeatureSurface cli_bad_suffix =
      BuildObjc3CliReportingOutputContractCoreFeatureSurface(
          cli_scaffold,
          std::filesystem::path("out/module.json"),
          std::filesystem::path("out/module.diagnostics.json.tmp"));
  if (!Expect(!cli_bad_suffix.core_feature_impl_ready &&
              !cli_bad_suffix.diagnostics_output_path_deterministic)) {
    return 4;
  }

  const Objc3ToolchainRuntimeGaOperationsScaffold toolchain_scaffold =
      BuildObjc3ToolchainRuntimeGaOperationsScaffold(
          true,
          false,
          std::filesystem::path("clang.exe"),
          std::filesystem::path("llc.exe"),
          false,
          std::filesystem::path("module.ll"),
          std::filesystem::path("module.obj"));
  const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface toolchain_surface =
      BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(
          toolchain_scaffold,
          0,
          true,
          std::filesystem::path("module.object-backend.txt"),
          "clang\n");
  if (!Expect(toolchain_surface.core_feature_impl_ready &&
              toolchain_surface.backend_output_path_deterministic &&
              toolchain_surface.performance_quality_guardrails_key_ready)) {
    return 5;
  }
  const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface toolchain_bad_suffix =
      BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(
          toolchain_scaffold,
          0,
          true,
          std::filesystem::path("module.object-backend.txt.tmp"),
          "clang\n");
  if (!Expect(!toolchain_bad_suffix.core_feature_impl_ready &&
              !toolchain_bad_suffix.backend_output_path_deterministic)) {
    return 6;
  }

  const bool recovery_consistent =
      IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
          true,
          true,
          true,
          true,
          true,
          "parser_snapshot=1",
          "parser_snapshot_fingerprint=1",
          "parser_diagnostics=0",
          "parser_tokens=1",
          "long-tail-grammar:v1:constructs=1",
          "parser_diagnostic_count=0",
          "snapshot_present=true");
  if (!Expect(recovery_consistent)) {
    return 7;
  }

  const bool recovery_bad_prefix =
      IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
          true,
          true,
          true,
          true,
          true,
          "xparser_snapshot=1",
          "parser_snapshot_fingerprint=1",
          "parser_diagnostics=0",
          "parser_tokens=1",
          "long-tail-grammar:v1:constructs=1",
          "parser_diagnostic_count=0",
          "snapshot_present=true");
  if (!Expect(!recovery_bad_prefix)) {
    return 8;
  }

  const std::string readiness_key =
      BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey(
          true,
          true,
          true,
          "case_count=1",
          "conformance_matrix_case_count=1",
          true,
          true);
  if (!Expect(readiness_key.find("parse_lowering_conformance_matrix_key_shape_deterministic=true") !=
                  std::string::npos &&
              readiness_key.find("long_tail_grammar_conformance_matrix_key_shape_deterministic=true") !=
                  std::string::npos)) {
    return 9;
  }

  return 0;
}
''',
            encoding="utf-8",
        )
        command = [
            clangxx,
            "-std=c++20",
            "-I",
            str((ROOT / "native" / "objc3c" / "src").resolve()),
            str(probe),
            "-o",
            str(exe),
        ]
        compile_result = run_capture(command, cwd=ROOT)
        if compile_result.returncode != 0:
            raise RuntimeError(
                f"readiness surface helper probe compile failed\n{failure_snippet(compile_result)}"
            )
        run_result = subprocess.run([str(exe)], cwd=ROOT, text=True, capture_output=True)
        if run_result.returncode != 0:
            raise RuntimeError(
                f"readiness surface helper probe failed with exit {run_result.returncode}\n"
                f"{failure_snippet(run_result)}"
            )

        print("objc3c-readiness-surface-helpers: PASS")
        return 0
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


if __name__ == "__main__":
    raise SystemExit(main())
