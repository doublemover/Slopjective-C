#!/usr/bin/env python3
"""Fast native smoke test for the production objc3 JSON helper."""

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
    helper_source = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_json.cpp"
    with tempfile.TemporaryDirectory(prefix="objc3c-json-helper-") as raw_tmp:
        tmp = Path(raw_tmp)
        probe = tmp / "objc3_json_probe.cpp"
        exe = tmp / "objc3_json_probe.exe"
        probe.write_text(
            r'''
#include "io/objc3_json.h"

#include <sstream>
#include <string>

namespace {

bool ExpectEqual(const std::string &actual, const std::string &expected) {
  return actual == expected;
}

}  // namespace

int main() {
  using objc3::io::EscapeJsonString;
  using objc3::io::WriteJsonString;

  if (!ExpectEqual(EscapeJsonString("plain"), "plain")) {
    return 1;
  }
  if (!ExpectEqual(EscapeJsonString("quote: \" slash: \\"), "quote: \\\" slash: \\\\")) {
    return 2;
  }
  if (!ExpectEqual(EscapeJsonString(std::string("line\ncarriage\rtab\tback\bform\f")),
                   "line\\ncarriage\\rtab\\tback\\bform\\f")) {
    return 3;
  }
  std::string controls;
  controls.push_back(static_cast<char>(0x01));
  controls.push_back(static_cast<char>(0x1f));
  if (!ExpectEqual(EscapeJsonString(controls), "\\u0001\\u001f")) {
    return 4;
  }
  std::ostringstream out;
  WriteJsonString(out, "x\ny");
  if (!ExpectEqual(out.str(), "\"x\\ny\"")) {
    return 5;
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
                f"native JSON helper probe compile failed for {repo_rel(helper_source)}\n"
                f"{failure_snippet(compile_result)}"
            )
        run_result = subprocess.run([str(exe)], cwd=ROOT, text=True, capture_output=True)
        if run_result.returncode != 0:
            raise RuntimeError(f"native JSON helper probe failed with exit {run_result.returncode}\n{failure_snippet(run_result)}")

    print("objc3c-native-json-helper: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
