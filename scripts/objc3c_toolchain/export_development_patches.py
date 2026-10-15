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
        "clang/lib/AST/Stmt.cpp",
        "clang/lib/AST/StmtPrinter.cpp",
        "clang/lib/Parse/ParseObjC3.cpp",
        "clang/lib/Parse/ParseStmt.cpp",
        "clang/lib/Serialization/ASTReaderStmt.cpp",
        "clang/lib/Serialization/ASTWriterStmt.cpp",
        "clang/test/ObjC3/Parser/contextual-identifiers.m",
        "clang/test/ObjC3/Parser/contextual-identifiers.mm",
        "clang/test/ObjC3/Parser/contextual-macros.m",
        "clang/test/ObjC3/Parser/defer-macro-alias.mm",
        "clang/test/ObjC3/Parser/defer-pch.mm",
        "clang/test/ObjC3/Parser/defer-recovery.mm",
        "clang/test/ObjC3/Parser/defer-template.mm",
        "clang/test/ObjC3/Parser/defer-type-temporary.mm",
        "clang/test/ObjC3/Parser/defer.mm",
        "clang/test/ObjC3/Parser/Inputs/defer-pch.h",
        "clang/test/ObjC3/Parser/Inputs/defer-pch-consumer.mm",
    ],
    "0003-objc3-native-payload-type-facts.patch": [
        "clang/include/clang/AST/ObjC3TypeContracts.h",
        "clang/lib/AST/ObjC3TypeContracts.cpp",
        "clang/lib/AST/CMakeLists.txt",
        "clang/unittests/AST/ObjC3TypeContractsTest.cpp",
    ],
    "0004-objc3-canonical-carriers-and-function-effects.patch": [
        "clang/include/clang/AST/ASTContext.h",
        "clang/include/clang/AST/ASTNodeTraverser.h",
        "clang/include/clang/AST/AbstractBasicReader.h",
        "clang/include/clang/AST/AbstractBasicWriter.h",
        "clang/include/clang/AST/CanonicalType.h",
        "clang/include/clang/AST/TextNodeDumper.h",
        "clang/include/clang/AST/PropertiesBase.td",
        "clang/include/clang/AST/RecursiveASTVisitor.h",
        "clang/include/clang/AST/ObjC3MatchCoverage.h",
        "clang/include/clang/AST/ObjC3MatchNodes.inc",
        "clang/include/clang/AST/ObjC3MatchSemantic.h",
        "clang/include/clang/AST/TypeBase.h",
        "clang/include/clang/AST/TypeLoc.h",
        "clang/include/clang/AST/TypeLocNodes.def",
        "clang/include/clang/AST/TypeProperties.td",
        "clang/include/clang/Basic/DiagnosticParseKinds.td",
        "clang/include/clang/Basic/DiagnosticGroups.td",
        "clang/include/clang/Basic/DiagnosticSemaKinds.td",
        "clang/include/clang/Basic/TypeNodes.td",
        "clang/include/clang/Parse/Parser.h",
        "clang/include/clang/Sema/DeclSpec.h",
        "clang/include/clang/Sema/ExternalSemaSource.h",
        "clang/include/clang/Sema/Sema.h",
        "clang/include/clang/Serialization/ASTBitCodes.h",
        "clang/include/clang/Serialization/TypeBitCodes.def",
        "clang/lib/AST/ASTContext.cpp",
        "clang/lib/AST/ASTImporter.cpp",
        "clang/lib/AST/ASTStructuralEquivalence.cpp",
        "clang/lib/AST/ODRHash.cpp",
        "clang/lib/AST/ObjC3MatchNodes.cpp",
        "clang/lib/AST/DeclPrinter.cpp",
        "clang/lib/AST/TextNodeDumper.cpp",
        "clang/lib/AST/Type.cpp",
        "clang/lib/AST/TypeLoc.cpp",
        "clang/lib/AST/TypePrinter.cpp",
        "clang/lib/AST/ItaniumMangle.cpp",
        "clang/lib/AST/MicrosoftMangle.cpp",
        "clang/lib/Parse/CMakeLists.txt",
        "clang/lib/Parse/ParseDecl.cpp",
        "clang/lib/Parse/ParseTentative.cpp",
        "clang/lib/Parse/Parser.cpp",
        "clang/lib/Parse/ParseObjc.cpp",
        "clang/lib/Parse/ParseObjC3Match.cpp",
        "clang/lib/Parse/ParseObjC3QualifiedName.cpp",
        "clang/lib/Parse/ParseObjC3Type.cpp",
        "clang/lib/Index/USRGeneration.cpp",
        "clang/lib/Sema/CMakeLists.txt",
        "clang/lib/Sema/DeclSpec.cpp",
        "clang/lib/Sema/SemaObjC3Carriers.cpp",
        "clang/lib/Sema/SemaObjC3CarrierTransfer.cpp",
        "clang/lib/Sema/SemaObjC3ModuleLookup.cpp",
        "clang/lib/Sema/ObjC3MatchCoverage.cpp",
        "clang/lib/Sema/ObjC3MatchSemantic.cpp",
        "clang/lib/Sema/SemaObjC3Match.cpp",
        "clang/lib/Sema/TreeTransformObjC3Match.inc",
        "clang/lib/Sema/SemaObjC3CarrierARC.h",
        "clang/lib/Sema/SemaObjC3CarrierARC.cpp",
        "clang/lib/Sema/SemaObjC3Effects.cpp",
        "clang/lib/Sema/SemaObjC3Propagation.cpp",
        "clang/lib/Sema/SemaObjC3ErrorContext.h",
        "clang/lib/Sema/SemaObjC3DoCatch.cpp",
        "clang/lib/Sema/JumpDiagnostics.cpp",
        "clang/lib/Sema/SemaStmt.cpp",
        "clang/lib/Sema/SemaType.cpp",
        "clang/lib/Sema/SemaDecl.cpp",
        "clang/lib/Sema/SemaDeclCXX.cpp",
        "clang/lib/Sema/SemaExpr.cpp",
        "clang/lib/Sema/SemaOverload.cpp",
        "clang/lib/Sema/SemaTemplate.cpp",
        "clang/lib/Sema/SemaTemplateDeduction.cpp",
        "clang/lib/Sema/TreeTransform.h",
        "clang/lib/Serialization/ASTReader.cpp",
        "clang/lib/Serialization/ASTWriter.cpp",
        "clang/lib/Serialization/TemplateArgumentHasher.cpp",
        "clang/unittests/AST/CMakeLists.txt",
        "clang/unittests/AST/ObjC3CarrierTypeTest.cpp",
        "clang/unittests/AST/ObjC3FunctionEffectTest.cpp",
        "clang/unittests/AST/ObjC3IndexIdentityTest.cpp",
        "clang/unittests/AST/ObjC3CarrierMangleTest.cpp",
        "clang/unittests/AST/ObjC3CarrierExprMangleTest.cpp",
        "clang/unittests/AST/ObjC3FunctionEffectImportTest.cpp",
        "clang/unittests/AST/ObjC3DoCatchPersistenceTest.cpp",
        "clang/unittests/AST/ObjC3TryThrowConsumerTest.cpp",
        "clang/unittests/AST/ObjC3CarrierCommonSugarTest.cpp",
        "clang/unittests/AST/ObjC3CarrierAdversarialTest.cpp",
        "clang/unittests/AST/ObjC3CarrierIdentityTest.cpp",
        "clang/unittests/AST/ObjC3CarrierExprIdentityTest.cpp",
        "clang/unittests/AST/ObjC3MatchImporterTest.cpp",
        "clang/test/ObjC3/AST",
        "clang/test/ObjC3/Sema",
        "clang/test/ObjC3/Parser/type-constructors.m",
        "clang/test/ObjC3/Parser/type-constructors.mm",
        "clang/test/ObjC3/Parser/type-contexts.mm",
        "clang/test/ObjC3/Parser/type-declarator-grammar.mm",
        "clang/test/ObjC3/Parser/type-pch.mm",
        "clang/test/ObjC3/Parser/type-recovery.mm",
        "clang/test/ObjC3/Parser/Inputs/objc3-type-consumer.mm",
        "clang/test/ObjC3/Parser/Inputs/objc3-type-constructors.h",
    ],
    "0005-objc3-codegen-qualification-boundaries.patch": [
        "clang/lib/Analysis/CFG.cpp",
        "clang/lib/StaticAnalyzer/Frontend/AnalysisConsumer.cpp",
        "clang/lib/CodeGen/CGObjC3Carriers.h",
        "clang/lib/CodeGen/CGObjC3Carriers.cpp",
        "clang/lib/CodeGen/CGObjC3Match.h",
        "clang/lib/CodeGen/CGObjC3Match.cpp",
        "clang/lib/CodeGen/CGObjC3ArcCarriers.h",
        "clang/lib/CodeGen/CGObjC3ArcCarriers.cpp",
        "clang/lib/CodeGen/CGObjC3ResultExpr.h",
        "clang/lib/CodeGen/CGObjC3ResultExpr.cpp",
        "clang/lib/CodeGen/CGObjC3ABIBoundaries.h",
        "clang/lib/CodeGen/CGObjC3ABIBoundaries.cpp",
        "clang/lib/CodeGen/CGObjC3TypedErrors.h",
        "clang/lib/CodeGen/CGObjC3TypedErrors.cpp",
        "clang/lib/CodeGen/CGObjC3ErrorHandlers.h",
        "clang/lib/CodeGen/CGObjC3ErrorHandlers.cpp",
        "clang/lib/CodeGen/CMakeLists.txt",
        "clang/lib/CodeGen/CGCall.cpp",
        "clang/lib/CodeGen/CGBlocks.cpp",
        "clang/lib/CodeGen/CGAtomic.cpp",
        "clang/lib/CodeGen/CGDecl.cpp",
        "clang/lib/CodeGen/CGDebugInfo.cpp",
        "clang/lib/CodeGen/CGExpr.cpp",
        "clang/lib/CodeGen/CGExprAgg.cpp",
        "clang/lib/CodeGen/CGExprComplex.cpp",
        "clang/lib/CodeGen/CGExprScalar.cpp",
        "clang/lib/CodeGen/CGObjC.cpp",
        "clang/lib/CodeGen/CGObjCGNU.cpp",
        "clang/lib/CodeGen/CGStmt.cpp",
        "clang/lib/CodeGen/CGVTables.cpp",
        "clang/lib/CodeGen/CodeGenFunction.cpp",
        "clang/lib/CodeGen/CodeGenFunction.h",
        "clang/lib/CodeGen/CodeGenModule.cpp",
        "clang/lib/CodeGen/CodeGenTypes.cpp",
        "clang/lib/CodeGen/CodeGenTypes.h",
        "clang/test/ObjC3/CodeGen",
    ],
    "0006-objc3-native-carrier-expressions.patch": [
        "clang/include/clang/AST/ComputeDependence.h",
        "clang/lib/AST/ComputeDependence.cpp",
        "clang/include/clang/Basic/StmtNodes.td",
        "clang/include/clang/AST/Expr.h",
        "clang/lib/AST/Expr.cpp",
        "clang/lib/AST/StmtProfile.cpp",
        "clang/lib/AST/ExprClassification.cpp",
        "clang/lib/AST/ExprConstant.cpp",
        "clang/lib/Parse/ParseExpr.cpp",
        "clang/lib/Sema/SemaExceptionSpec.cpp",
        "clang/lib/Sema/SemaObjC3OptionalExpr.cpp",
        "clang/lib/Sema/SemaObjC3ResultExpr.cpp",
        "clang/unittests/AST/ObjC3OptionalExprTest.cpp",
        "clang/unittests/AST/ObjC3ResultExprTest.cpp",
        "clang/unittests/AST/ObjC3CarrierExprSemanticsTest.cpp",
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
    changed = git("diff", "--name-only", "-z", "HEAD").split(b"\0")
    changed_paths = [p.decode() for p in changed if p] + untracked_paths
    for relative in changed_paths:
        if relative == ".objc3c-prepared-source.json":
            continue
        owners = [name for name, paths in PACKETS.items()
                  if any(relative == p or relative.startswith(p + "/") for p in paths)]
        if len(owners) != 1:
            raise RuntimeError(f"Source path requires exactly one patch owner: {relative}: {owners}")
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
