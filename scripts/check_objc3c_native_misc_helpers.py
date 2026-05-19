#!/usr/bin/env python3
"""Fast native smoke test for shared objc3 misc support helpers."""

from __future__ import annotations

import subprocess
import shutil
import uuid
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import ROOT
from objc3c_tooling.probe_compile import find_clangxx
from objc3c_tooling.subprocesses import failure_snippet, run_capture


def main(argv: Sequence[str] | None = None) -> int:
    _ = argv
    clangxx = find_clangxx()
    temp_root = ROOT / "tmp" / "probe-work"
    temp_root.mkdir(parents=True, exist_ok=True)
    tmp = temp_root / f"objc3c-misc-helpers-{uuid.uuid4().hex}"
    tmp.mkdir(parents=True)
    try:
        probe = tmp / "objc3_misc_helpers_probe.cpp"
        exe = tmp / "objc3_misc_helpers_probe.exe"
        probe.write_text(
            r'''
#include "support/objc3_ascii_predicates.h"
#include "support/objc3_file_reading.h"
#include "support/objc3_identifier_safe_suffix.h"
#include "support/objc3_ir_object_backend_token.h"
#include "support/objc3_method_family.h"
#include "support/objc3_runtime_dispatch_symbol.h"
#include "support/objc3_runtime_metadata_record_set.h"
#include "support/objc3_value_type_names.h"

#include <filesystem>
#include <fstream>
#include <string>

namespace {

bool Expect(bool condition) {
  return condition;
}

}  // namespace

int main(int argc, char **argv) {
  if (argc != 2) {
    return 1;
  }
  const std::filesystem::path scratch = argv[1];

  if (!Expect(objc3c::support::IsHexDigit('f') &&
              objc3c::support::IsHexDigit('F') &&
              objc3c::support::IsHexDigit('9') &&
              !objc3c::support::IsHexDigit('g') &&
              !objc3c::support::IsHexDigit(static_cast<char>(0xe9)))) {
    return 2;
  }
  if (!Expect(objc3c::support::IsBinaryDigit('0') &&
              objc3c::support::IsBinaryDigit('1') &&
              !objc3c::support::IsBinaryDigit('2') &&
              objc3c::support::IsOctalDigit('7') &&
              !objc3c::support::IsOctalDigit('8') &&
              objc3c::support::IsDigitSeparator('_'))) {
    return 3;
  }

  if (!Expect(objc3c::support::IsValidRuntimeDispatchSymbol("objc3_runtime.dispatch$i32") &&
              objc3c::support::IsValidRuntimeDispatchSymbol("._$") &&
              !objc3c::support::IsValidRuntimeDispatchSymbol("") &&
              !objc3c::support::IsValidRuntimeDispatchSymbol("3bad") &&
              !objc3c::support::IsValidRuntimeDispatchSymbol("bad-name"))) {
    return 4;
  }

  objc3c::support::IrObjectBackendToken backend = objc3c::support::IrObjectBackendToken::Clang;
  if (!Expect(objc3c::support::ParseIrObjectBackendToken("llvm-direct", backend) &&
              backend == objc3c::support::IrObjectBackendToken::LLVMDirect &&
              objc3c::support::ParseIrObjectBackendToken("clang", backend) &&
              backend == objc3c::support::IrObjectBackendToken::Clang &&
              !objc3c::support::ParseIrObjectBackendToken("llc", backend))) {
    return 5;
  }

  if (!Expect(objc3c::support::MakeIdentifierSafeSuffix("mod-name.1", "retired-route") == "mod_name_1" &&
              objc3c::support::MakeIdentifierSafeSuffix("", "retired-route") == "retired-route" &&
              objc3c::support::MakeIdentifierSafeSuffix(std::string("x") + static_cast<char>(0xe9),
                                                        "retired-route") == "x_")) {
    return 6;
  }

  if (!Expect(std::string(objc3c::support::ValueTypeName(ValueType::I32)) == "i32" &&
              std::string(objc3c::support::ValueTypeName(ValueType::ObjCObjectPtr)) == "object-pointer" &&
              std::string(objc3c::support::ValueTypeName(ValueType::Unknown)) == "unknown")) {
    return 7;
  }

  if (!Expect(objc3c::support::ClassifyMethodFamilyFromSelector("mutableCopyWithZone:") == "mutableCopy" &&
              objc3c::support::ClassifyMethodFamilyFromSelector("copy") == "copy" &&
              objc3c::support::ClassifyMethodFamilyFromSelector("initWithValue:") == "init" &&
              objc3c::support::ClassifyMethodFamilyFromSelector("newWidget") == "new" &&
              objc3c::support::ClassifyMethodFamilyFromSelector("alloc") == "none")) {
    return 8;
  }

  const std::filesystem::path text_path = scratch / "text.txt";
  {
    std::ofstream out(text_path, std::ios::binary);
    out << "hello";
  }
  std::string contents;
  std::string error;
  if (!Expect(objc3c::support::TryReadTextFile(text_path, contents, error, "open failed", "read failed") &&
              contents == "hello" &&
              error.empty())) {
    return 9;
  }
  if (!Expect(!objc3c::support::TryReadTextFile(scratch / "missing.txt",
                                                contents,
                                                error,
                                                "open failed",
                                                "read failed") &&
              error == "open failed")) {
    return 10;
  }

  Objc3RuntimeMetadataSourceRecordSet records;
  records.classes_lexicographic.resize(1u);
  records.classes_lexicographic[0].has_super = true;
  records.classes_lexicographic[0].super_name = "Base";
  records.classes_lexicographic[0].adopted_protocols_lexicographic = {"P"};
  records.protocols_lexicographic.resize(1u);
  records.protocols_lexicographic[0].inherited_protocols_lexicographic = {"Q"};
  records.properties_lexicographic.resize(1u);
  records.properties_lexicographic[0].effective_getter_selector = "value";
  records.properties_lexicographic[0].effective_setter_available = true;
  records.properties_lexicographic[0].effective_setter_selector = "setValue:";
  records.properties_lexicographic[0].ivar_binding_symbol = "_value";
  records.methods_lexicographic.resize(1u);
  records.methods_lexicographic[0].selector = "run";
  if (!Expect(objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations(records) == 4u &&
              objc3c::support::CountRuntimeMetadataSourceRecordSetReferences(records) == 7u)) {
    return 11;
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
            raise RuntimeError(f"native misc helper probe compile failed\n{failure_snippet(compile_result)}")
        run_result = subprocess.run([str(exe), str(tmp)], cwd=ROOT, text=True, capture_output=True)
        if run_result.returncode != 0:
            raise RuntimeError(
                f"native misc helper probe failed with exit {run_result.returncode}\n"
                f"{failure_snippet(run_result)}"
            )

        print("objc3c-native-misc-helpers: PASS")
        return 0
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


if __name__ == "__main__":
    raise SystemExit(main())
