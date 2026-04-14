#!/usr/bin/env python3
"""Fast native smoke test for shared objc3 diagnostic helpers."""

from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import ROOT, repo_rel
from objc3c_tooling.probe_compile import find_clangxx
from objc3c_tooling.subprocesses import failure_snippet, run_capture


def main(argv: Sequence[str] | None = None) -> int:
    _ = argv
    clangxx = find_clangxx()
    helper_source = ROOT / "native" / "objc3c" / "src" / "diag" / "objc3_diag_utils.cpp"
    with tempfile.TemporaryDirectory(prefix="objc3c-diag-utils-") as raw_tmp:
        tmp = Path(raw_tmp)
        probe = tmp / "objc3_diag_utils_probe.cpp"
        exe = tmp / "objc3_diag_utils_probe.exe"
        probe.write_text(
            r'''
#include "contracts/objc3_frontend_diagnostics_bus_contract.h"
#include "diag/objc3_diag_utils.h"

#include <limits>
#include <string>
#include <vector>

namespace {

bool Expect(bool condition) {
  return condition;
}

}  // namespace

int main() {
  if (!Expect(MakeDiag(7u, 11u, "O3P123", "parser message") ==
              "error:7:11: parser message [O3P123]")) {
    return 1;
  }
  if (!Expect(StartsWith("error:7", "error:") && !StartsWith("note:7", "error:"))) {
    return 2;
  }

  unsigned value = 0u;
  if (!Expect(TryParseUnsignedSegment("abc123:def", 3u, 6u, value) && value == 123u)) {
    return 3;
  }
  if (!Expect(!TryParseUnsignedSegment("abc:def", 3u, 3u, value) &&
              !TryParseUnsignedSegment("abc12x:def", 3u, 6u, value) &&
              !TryParseUnsignedSegment("42949672960:", 0u, 11u, value))) {
    return 4;
  }

  unsigned line = 0u;
  unsigned column = 0u;
  std::string code;
  if (!Expect(TryParseDiagnosticCoordinateAndCode(
                  "error:7:11: parser message [O3P123]", line, column, code) &&
              line == 7u && column == 11u && code == "O3P123")) {
    return 5;
  }
  if (!Expect(!TryParseDiagnosticCoordinateAndCode("error:7:11:message [O3P123]", line, column, code) &&
              !TryParseDiagnosticCoordinateAndCode("error:7:11: message []", line, column, code) &&
              !TryParseDiagnosticCoordinateAndCode("warning:7:11: message [O3P123]", line, column, code))) {
    return 6;
  }

  const DiagSortKey key = ParseDiagSortKey("error:7:11: parser message [O3P123]");
  if (!Expect(key.severity == "error" &&
              key.severity_rank == 1u &&
              key.line == 7u &&
              key.column == 11u &&
              key.code == "O3P123" &&
              key.message == "parser message")) {
    return 7;
  }

  const DiagSortKey malformed = ParseDiagSortKey("error:not-a-line:11: parser message [O3P123]");
  if (!Expect(malformed.line == std::numeric_limits<unsigned>::max() &&
              malformed.column == std::numeric_limits<unsigned>::max())) {
    return 8;
  }

  Objc3FrontendDiagnosticsBus bus;
  bus.lexer = {"lexer"};
  bus.parser = {"parser"};
  bus.semantic = {"semantic"};
  const std::vector<std::string> flattened = FlattenStageDiagnostics(bus);
  if (!Expect(flattened == std::vector<std::string>{"lexer", "parser", "semantic"})) {
    return 9;
  }
  const std::vector<std::string> flattened_with_post = FlattenStageDiagnostics(bus, {"post"});
  if (!Expect(flattened_with_post == std::vector<std::string>{"lexer", "parser", "semantic", "post"})) {
    return 10;
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
            str(helper_source),
            "-o",
            str(exe),
        ]
        compile_result = run_capture(command, cwd=ROOT)
        if compile_result.returncode != 0:
            raise RuntimeError(
                f"native diagnostic helper probe compile failed for {repo_rel(helper_source)}\n"
                f"{failure_snippet(compile_result)}"
            )
        run_result = subprocess.run([str(exe)], cwd=ROOT, text=True, capture_output=True)
        if run_result.returncode != 0:
            raise RuntimeError(
                f"native diagnostic helper probe failed with exit {run_result.returncode}\n"
                f"{failure_snippet(run_result)}"
            )

    print("objc3c-diag-utils: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
