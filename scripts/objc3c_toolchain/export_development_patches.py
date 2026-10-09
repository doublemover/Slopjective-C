"""Export owned development edits as the canonical, pinned Clang patch stack."""

from __future__ import annotations

import hashlib
import json
from pathlib import Path
import subprocess

from scripts.objc3c_toolchain.generate_features import AUTHORITY, generated


ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "out/toolchains/objc3-development/src"
STACK = ROOT / "toolchains/llvm/patches"
PACKETS = {
    "0001-objc3-language-mode-and-build-identity.patch": [
        "clang/include/clang/Basic/CMakeLists.txt",
        "clang/include/clang/Basic/LangOptions.def",
        "clang/include/clang/Basic/ObjC3Foundation.h.in",
        "clang/include/clang/Basic/ObjC3Features.h",
        "clang/include/clang/Basic/ObjC3Features.def",
        "clang/include/clang/Options/Options.td",
        "clang/lib/Basic/Version.cpp",
        "clang/lib/Basic/ObjC3Features.cpp",
        "clang/lib/Basic/CMakeLists.txt",
        "clang/lib/Driver/ToolChains/Clang.cpp",
        "clang/lib/Frontend/CompilerInvocation.cpp",
        "clang/lib/Frontend/InitPreprocessor.cpp",
        "clang/lib/Lex/PPMacroExpansion.cpp",
        "clang/test/ObjC3/Driver",
        "clang/test/ObjC3/LanguageMode",
    ],
    "0002-objc3-native-defer-statement.patch": [
        "clang/include/clang/AST/Stmt.h",
        "clang/include/clang/Parse/Parser.h",
        "clang/lib/AST/ASTImporter.cpp",
        "clang/lib/AST/StmtPrinter.cpp",
        "clang/lib/Parse/CMakeLists.txt",
        "clang/lib/Parse/ParseObjC3.cpp",
        "clang/lib/Parse/ParseStmt.cpp",
        "clang/lib/Sema/TreeTransform.h",
        "clang/lib/Serialization/ASTReaderStmt.cpp",
        "clang/lib/Serialization/ASTWriterStmt.cpp",
        "clang/test/ObjC3/Parser",
    ],
    "0003-objc3-native-payload-type-facts.patch": [
        "clang/include/clang/AST/ObjC3TypeContracts.h",
        "clang/lib/AST/ObjC3TypeContracts.cpp",
        "clang/lib/AST/CMakeLists.txt",
        "clang/unittests/AST/ObjC3TypeContractsTest.cpp",
        "clang/unittests/AST/CMakeLists.txt",
    ],
}


def git(*arguments: str) -> bytes:
    return subprocess.check_output(["git", "-C", str(SOURCE), *arguments])


def export() -> None:
    expected_table, _ = generated(json.loads(AUTHORITY.read_text(encoding="utf-8")))
    native_table = SOURCE / "clang/include/clang/Basic/ObjC3Features.def"
    if native_table.read_text(encoding="utf-8") != expected_table:
        raise RuntimeError("Native feature table differs from the capability authority")
    manifest_path = ROOT / "toolchains/llvm/manifest.json"
    manifest = json.loads(manifest_path.read_text(encoding="utf-8-sig"))
    head = git("rev-parse", "HEAD").decode().strip()
    if head != manifest["upstream"]["commit"]:
        raise RuntimeError(f"Development source has unexpected base {head}")
    untracked = git("ls-files", "--others", "--exclude-standard", "-z")
    untracked_paths = [p.decode() for p in untracked.split(b"\0") if p]
    records = []
    for name, paths in PACKETS.items():
        patch = git("diff", "--binary", "--no-ext-diff", "HEAD", "--", *paths)
        for relative in sorted(untracked_paths):
            if not any(relative == p or relative.startswith(p + "/") for p in paths):
                continue
            data = (SOURCE / relative).read_bytes().replace(b"\r\n", b"\n")
            if b"\0" in data:
                raise RuntimeError(f"Unexpected binary source: {relative}")
            lines = data.splitlines(keepends=True)
            header = (
                f"diff --git a/{relative} b/{relative}\n"
                "new file mode 100644\n"
                "--- /dev/null\n"
                f"+++ b/{relative}\n"
                f"@@ -0,0 +1,{len(lines)} @@\n"
            ).encode()
            patch += header
            for line in lines:
                patch += b"+" + line
                if not line.endswith(b"\n"):
                    patch += b"\n\\ No newline at end of file\n"
        if not patch:
            raise RuntimeError(f"Refusing empty patch packet: {name}")
        (STACK / name).write_bytes(patch)
        records.append({"file": name, "sha256": hashlib.sha256(patch).hexdigest()})
    (STACK / "series").write_text("\n".join(PACKETS) + "\n", encoding="utf-8")
    manifest["patches"] = records
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"base": head, "patches": records}, indent=2))


if __name__ == "__main__":
    export()
