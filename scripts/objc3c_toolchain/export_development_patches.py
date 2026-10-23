"""Export owned development edits as the canonical, pinned Clang patch stack."""

from __future__ import annotations

import argparse
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
        "clang/include/clang/Lex/Preprocessor.h",
        "clang/lib/Lex/PPDirectives.cpp",
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
        "clang/include/clang/AST/ObjC3Async.h",
        "clang/lib/AST/ObjC3TypeContracts.cpp",
        "clang/lib/AST/CMakeLists.txt",
        "clang/unittests/AST/ObjC3TypeContractsTest.cpp",
    ],
    "0004-objc3-canonical-carriers-and-function-effects.patch": [
        "clang/test/ObjC3/Parser/optional-access.mm",
        "clang/test/ObjC3/Parser/optional-access-pch.mm",
        "clang/test/ObjC3/Parser/Inputs/optional-access-pch.h",
        "clang/test/ObjC3/Parser/Inputs/optional-access-pch-consumer.mm",
        "clang/lib/Sema/SemaObjC3ResourceOperations.cpp",
        "clang/lib/Sema/SemaObjC3ResourceFlow.cpp",
        "clang/include/clang/AST/ObjC3ResourceTypes.h",
        "clang/test/ObjC3/Sema/resource-type-contracts.m",
        "clang/test/ObjC3/AST/Inputs/resource-type-contracts.h",
        "clang/test/ObjC3/AST/resource-type-contracts-pch.m",
        "clang/test/ObjC3/AST/resource-type-contracts-pch-errors.m",
        "clang/lib/Sema/SemaObjC3NonlocalJumps.cpp",
        "clang/test/ObjC3/Sema/cleanup-nonlocal-jumps.m",
        "clang/lib/Sema/SemaObjC3OptionalAccess.cpp",
        "clang/lib/Sema/SemaObjC3Effects.h",
        "clang/lib/Index/IndexBody.cpp",
        "clang/lib/AST/ObjC3Resources.cpp",
        "clang/lib/AST/ObjC3OptionalAccess.cpp",
        "clang/include/clang/Basic/DiagnosticSerializationKinds.td",
        "clang/include/clang/AST/ObjC3Resources.inc",
        "clang/include/clang/AST/ObjC3OptionalAccess.inc",
        "clang-tools-extra/clangd/unittests/ObjC3SourceToolsTests.cpp",
        "clang-tools-extra/clangd/unittests/CMakeLists.txt",
        "clang-tools-extra/clangd/SemanticHighlighting.cpp",
        "clang-tools-extra/clangd/Selection.cpp",
        "clang-tools-extra/clangd/ObjC3SourceInfo.h",
        "clang-tools-extra/clangd/Hover.cpp",
        "clang-tools-extra/clangd/FindTarget.cpp",
        "clang/utils/TableGen/ClangAttrEmitter.cpp",
        "clang/test/ObjC3/Parser/resource-attribute.m",
        "clang/test/ObjC3/Parser/resource-operations.m",
        "clang/test/ObjC3/Sema/cleanup-ownership.m",
        "clang/test/ObjC3/Sema/guard-unspecified-receivers.m",
        "clang/test/ObjC3/Parser/optional-coalescing.mm",
        "clang/test/ObjC3/Parser/optional-coalescing-pch.mm",
        "clang/test/ObjC3/Parser/optional-coalescing-lex.m",
        "clang/test/ObjC3/Parser/Inputs/optional-coalescing-pch.h",
        "clang/test/ObjC3/Parser/Inputs/optional-coalescing-pch-consumer.mm",
        "clang/lib/Sema/SemaObjC3Resources.h",
        "clang/lib/Sema/SemaObjC3Resources.cpp",
        "clang/lib/Sema/SemaObjC3OptionalOperators.cpp",
        "clang/lib/Sema/SemaDeclAttr.cpp",
        "clang/lib/Lex/Lexer.cpp",
        "clang/lib/Basic/OperatorPrecedence.cpp",
        "clang/lib/AST/ObjC3OptionalOperators.cpp",
        "clang/include/clang/Basic/OperatorPrecedence.h",
        "clang/include/clang/Basic/AttrDocs.td",
        "clang/include/clang/Basic/Attr.td",
        "clang/include/clang/AST/ObjC3OptionalOperators.inc",
        "clang-tools-extra/test/clang-query/objc3-conditions.mm",
        "clang/unittests/ASTMatchers/Dynamic/RegistryTest.cpp",
        "clang/unittests/ASTMatchers/ObjC3MatchersTest.cpp",
        "clang/unittests/ASTMatchers/CMakeLists.txt",
        "clang/unittests/ASTMatchers/Dynamic/CMakeLists.txt",
        "clang/lib/AST/ParentMapContext.cpp",
        "clang/lib/ASTMatchers/Dynamic/Registry.cpp",
        "clang/lib/ASTMatchers/ASTMatchFinder.cpp",
        "clang/lib/ASTMatchers/ASTMatchersInternal.cpp",
        "clang/include/clang/ASTMatchers/ASTMatchers.h",
        "clang/test/Index/objc3-native-expression-cursors.mm",
        "clang/test/Index/objc3-condition-cursors.mm",
        "clang/unittests/AST/ObjC3ConditionToolingTest.cpp",
        "clang/tools/libclang/CXCursor.cpp",
        "clang/tools/libclang/CIndex.cpp",
        "clang/include/clang/AST/JSONNodeDumper.h",
        "clang/CMakeLists.txt",
        "clang/include/clang/Basic/ObjC3Syntax.h",
        "clang/include/clang/Basic/TokenKinds.def",
        "clang/include/clang/AST/ASTContext.h",
        "clang/include/clang/AST/ASTNodeTraverser.h",
        "clang/include/clang/AST/AbstractBasicReader.h",
        "clang/include/clang/AST/AbstractBasicWriter.h",
        "clang/include/clang/AST/CanonicalType.h",
        "clang/include/clang/AST/DeclObjC.h",
        "clang/include/clang/AST/NonTrivialTypeVisitor.h",
        "clang/lib/AST/DeclCXX.cpp",
        "clang/lib/Sema/SemaChecking.cpp",
        "clang/include/clang/Basic/Builtins.td",
        "clang/lib/Sema/SemaLookup.cpp",
        "clang/include/clang/AST/TextNodeDumper.h",
        "clang/include/clang/AST/PropertiesBase.td",
        "clang/include/clang/AST/RecursiveASTVisitor.h",
        "clang/include/clang/AST/ObjC3MatchCoverage.h",
        "clang/include/clang/AST/ObjC3MatchNodes.inc",
        "clang/include/clang/AST/ObjC3ConditionNodes.inc",
        "clang/include/clang/AST/ObjC3SourceScopes.h",
        "clang/lib/AST/ObjC3ConditionNodes.cpp",
        "clang/include/clang/AST/ObjC3MatchSemantic.h",
        "clang/include/clang/AST/ObjC3ModuleNames.h",
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
        "clang/include/clang/Sema/MultiplexExternalSemaSource.h",
        "clang/lib/Sema/MultiplexExternalSemaSource.cpp",
        "clang/include/clang/Sema/Sema.h",
        "clang/include/clang/Sema/ObjC3GuardExit.h",
        "clang/include/clang/Sema/SemaObjC.h",
        "clang/include/clang/Serialization/ASTBitCodes.h",
        "clang/include/clang/Serialization/TypeBitCodes.def",
        "clang/lib/AST/ASTContext.cpp",
        "clang/lib/AST/ASTImporter.cpp",
        "clang/lib/AST/ASTStructuralEquivalence.cpp",
        "clang/lib/AST/ODRHash.cpp",
        "clang/lib/AST/ObjC3MatchNodes.cpp",
        "clang/lib/AST/DeclPrinter.cpp",
        "clang/lib/AST/Mangle.cpp",
        "clang/lib/AST/TextNodeDumper.cpp",
        "clang/lib/AST/JSONNodeDumper.cpp",
        "clang/lib/AST/Type.cpp",
        "clang/lib/AST/TypeLoc.cpp",
        "clang/lib/AST/TypePrinter.cpp",
        "clang/lib/AST/ItaniumMangle.cpp",
        "clang/lib/AST/MicrosoftMangle.cpp",
        "clang/lib/Parse/CMakeLists.txt",
        "clang/lib/Parse/ParseDecl.cpp",
        "clang/lib/Parse/Parser.cpp",
        "clang/lib/Sema/SemaModule.cpp",
        "clang/lib/Parse/ParseTentative.cpp",
        "clang/lib/Parse/Parser.cpp",
        "clang/lib/Parse/ParseObjc.cpp",
        "clang/lib/Parse/ParseObjC3Conditions.cpp",
        "clang/lib/Parse/ParseObjC3Match.cpp",
        "clang/lib/Parse/ParseObjC3QualifiedName.cpp",
        "clang/lib/Parse/ParseObjC3Type.cpp",
        "clang/lib/UnifiedSymbolResolution/USRGeneration.cpp",
        "clang/lib/Sema/CMakeLists.txt",
        "clang/lib/Sema/DeclSpec.cpp",
        "clang/lib/Sema/SemaObjC3Carriers.cpp",
        "clang/lib/Sema/SemaObjC3CarrierTransfer.cpp",
        "clang/lib/Sema/SemaObjC3ModuleLookup.cpp",
        "clang/lib/Sema/ObjC3MatchCoverage.cpp",
        "clang/lib/Sema/ObjC3MatchSemantic.cpp",
        "clang/lib/Sema/SemaObjC3Match.cpp",
        "clang/lib/Sema/SemaObjC3GuardExit.cpp",
        "clang/lib/Sema/SemaObjC3Conditions.cpp",
        "clang/lib/Sema/TreeTransformObjC3Match.inc",
        "clang/lib/Sema/TreeTransformObjC3Conditions.inc",
        "clang/lib/Sema/SemaObjC3CarrierARC.h",
        "clang/lib/Sema/SemaObjC3CarrierARC.cpp",
        "clang/lib/Sema/SemaObjC3Effects.cpp",
        "clang/lib/Sema/SemaObjC3Propagation.cpp",
        "clang/lib/Sema/SemaObjC3ErrorContext.h",
        "clang/lib/Sema/SemaObjC3DoCatch.cpp",
        "clang/lib/Sema/JumpDiagnostics.cpp",
        "clang/lib/Sema/SemaStmt.cpp",
        "clang/lib/Sema/SemaCoroutine.cpp",
        "clang/lib/Sema/CoroutineStmtBuilder.h",
        "clang/lib/Sema/TypeLocBuilder.h",
        "clang/lib/Sema/SemaType.cpp",
        "clang/lib/Sema/SemaDecl.cpp",
        "clang/lib/Sema/SemaInit.cpp",
        "clang/lib/Sema/SemaDeclCXX.cpp",
        "clang/lib/Sema/SemaDeclObjC.cpp",
        "clang/lib/Sema/SemaExprObjC.cpp",
        "clang/lib/Sema/SemaExpr.cpp",
        "clang/lib/Sema/SemaExprCXX.cpp",
        "clang/lib/Sema/SemaTypeTraits.cpp",
        "clang/lib/Sema/SemaOverload.cpp",
        "clang/lib/Sema/SemaTemplate.cpp",
        "clang/lib/Sema/SemaTemplateDeduction.cpp",
        "clang/lib/Sema/TreeTransform.h",
        "clang/lib/Serialization/ASTReader.cpp",
        "clang/lib/Serialization/ASTReaderDecl.cpp",
        "clang/lib/Serialization/ASTWriter.cpp",
        "clang/lib/Serialization/ASTWriterDecl.cpp",
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
        "clang/unittests/AST/ObjC3CheckedErrorCFGTest.cpp",
        "clang/unittests/AST/ObjC3GuardExitTest.cpp",
        "clang/unittests/AST/ObjC3ConditionNodesTest.cpp",
        "clang/unittests/AST/ObjC3ConditionScopeTest.cpp",
        "clang/unittests/AST/ObjC3ConditionCodeGenTest.cpp",
        "clang/unittests/AST/ObjC3CarrierCommonSugarTest.cpp",
        "clang/unittests/AST/ObjC3CarrierAdversarialTest.cpp",
        "clang/unittests/AST/ObjC3CarrierIdentityTest.cpp",
        "clang/unittests/AST/ObjC3CarrierExprIdentityTest.cpp",
        "clang/unittests/AST/ObjC3MatchImporterTest.cpp",
        "clang/test/ObjC3/AST",
        "clang/test/ObjC3/Sema",
        "clang/test/ObjC3/Parser/Inputs/guard-pch-consumer.mm",
        "clang/test/ObjC3/Parser/Inputs/guard-pch.h",
        "clang/test/ObjC3/Parser/guard-c-mode.m",
        "clang/test/ObjC3/Parser/guard-conditions.mm",
        "clang/test/ObjC3/Parser/guard-contextual.mm",
        "clang/test/ObjC3/Parser/guard-disabled.mm",
        "clang/test/ObjC3/Parser/guard-lookahead.mm",
        "clang/test/ObjC3/Parser/guard-pch.mm",
        "clang/test/ObjC3/Parser/guard-recovery.mm",
        "clang/test/ObjC3/Parser/guard-scopes.mm",
        "clang/test/ObjC3/Parser/guard-template.mm",
        "clang/test/ObjC3/Parser/public-carriers.mm",
        "clang/test/ObjC3/Parser/public-errors.mm",
        "clang/test/ObjC3/Parser/public-carriers-disabled.c",
        "clang/test/ObjC3/Parser/type-constructors.m",
        "clang/test/ObjC3/Parser/type-constructors.mm",
        "clang/test/ObjC3/Parser/type-contexts.mm",
        "clang/test/ObjC3/Parser/type-declarator-grammar.mm",
        "clang/test/ObjC3/Parser/type-pch.mm",
        "clang/test/ObjC3/Parser/type-recovery.mm",
        "clang/test/ObjC3/Parser/native-async-return.mm",
        "clang/test/ObjC3/Parser/native-await-context.mm",
        "clang/test/ObjC3/Parser/Inputs/objc3-type-consumer.mm",
        "clang/test/ObjC3/Parser/Inputs/objc3-type-constructors.h",
    ],
    "0005-objc3-codegen-qualification-boundaries.patch": [
        "clang/test/ObjC3/Analysis/optional-access-unsupported.m",
        "clang/test/ObjC3/Analysis/cleanup-unsupported.m",
        "clang/test/ObjC3/CodeGen/cleanup-ownership.m",
        "clang/lib/Analysis/CalledOnceCheck.cpp",
        "clang/lib/CodeGen/CGObjC3ResourceOperations.cpp",
        "clang/lib/CodeGen/CGObjC3OptionalAccess.h",
        "clang/lib/CodeGen/CGObjC3OptionalAccess.cpp",
        "clang/test/ObjC3/Analysis/defer-match-coalesce.mm",
        "clang/lib/CodeGen/CGObjC3Resources.h",
        "clang/lib/CodeGen/CGObjC3Resources.cpp",
        "clang/lib/CodeGen/CGObjC3OptionalOperators.h",
        "clang/lib/CodeGen/CGObjC3OptionalOperators.cpp",
        "clang/lib/CodeGen/CGObjC3MixedCarriers.h",
        "clang/lib/CodeGen/CGObjC3MixedCarriers.cpp",
        "clang/lib/CodeGen/CGExprCXX.cpp",
        "clang/test/ObjC3/Analysis/defer-native-cxx.mm",
        "clang/include/clang/Analysis/CFG.h",
        "clang/lib/Analysis/CFG.cpp",
        "clang/lib/Analysis/UninitializedValues.cpp",
        "clang/lib/Analysis/PathDiagnostic.cpp",
        "clang/lib/StaticAnalyzer/Core/SymbolManager.cpp",
        "clang/lib/StaticAnalyzer/Core/ExprEngine.cpp",
        "clang/lib/Sema/AnalysisBasedWarnings.cpp",
        "clang/lib/StaticAnalyzer/Frontend/AnalysisConsumer.cpp",
        "clang/lib/CodeGen/CGObjC3Carriers.h",
        "clang/lib/CodeGen/CGObjC3ManagedStorage.h",
        "clang/lib/CodeGen/CGObjC3ManagedStorage.cpp",
        "clang/lib/CodeGen/CGClass.cpp",
        "clang/lib/CodeGen/CGNonTrivialStruct.cpp",
        "clang/lib/CodeGen/CGDeclCXX.cpp",
        "clang/lib/CodeGen/CGObjCRuntime.cpp",
        "clang/lib/CodeGen/CGObjC3Carriers.cpp",
        "clang/lib/CodeGen/CGObjC3Match.h",
        "clang/lib/CodeGen/CGObjC3Match.cpp",
        "clang/lib/CodeGen/CGObjC3Conditions.cpp",
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
        "clang/lib/CodeGen/CGCoroutine.cpp",
        "clang/lib/CodeGen/CGBlocks.cpp",
        "clang/lib/CodeGen/CGBlocks.h",
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
        "clang/lib/CodeGen/CGCleanup.cpp",
        "clang/lib/CodeGen/CGObjC3Defer.h",
        "clang/lib/CodeGen/VarBypassDetector.cpp",
        "clang/lib/CodeGen/CGVTables.cpp",
        "clang/lib/CodeGen/CodeGenFunction.cpp",
        "clang/lib/CodeGen/CodeGenFunction.h",
        "clang/lib/CodeGen/CodeGenModule.cpp",
        "clang/lib/CodeGen/CodeGenTypes.cpp",
        "clang/lib/CodeGen/CodeGenTypes.h",
        "clang/lib/CodeGen/Targets/X86.cpp",
        "clang/test/ObjC3/CodeGen",
    ],
    "0006-objc3-native-carrier-expressions.patch": [
        "clang/include/clang/AST/ComputeDependence.h",
        "clang/lib/AST/ComputeDependence.cpp",
        "clang/include/clang/Basic/StmtNodes.td",
        "clang/include/clang/AST/Expr.h",
        "clang/lib/AST/Expr.cpp",
        "clang/lib/AST/ExprObjC.cpp",
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
    "0007-objc3-linux-debugger-runtime-contracts.patch": [
        "lldb/test/Shell/Expr/objc3-carrier-summaries.mm",
        "lldb/source/DataFormatters/FormatManager.cpp",
        "lldb/source/Plugins/Language/ObjC/CMakeLists.txt",
        "lldb/source/Plugins/Language/ObjC/ObjCLanguage.cpp",
        "lldb/source/Plugins/Language/ObjC/ObjC3Carriers.cpp",
        "lldb/source/Plugins/Language/ObjC/ObjC3Carriers.h",
        "lldb/test/Shell/Expr/objc3-condition-locals.mm",
        "lldb/source/Plugins/SymbolFile/DWARF/ObjCRuntimeName.h",
        "lldb/source/Plugins/SymbolFile/DWARF/DWARFASTParserClang.cpp",
        "lldb/source/Plugins/SymbolFile/DWARF/SymbolFileDWARF.cpp",
        "lldb/source/Plugins/ExpressionParser/Clang/ClangExpressionParser.cpp",
        "lldb/source/Plugins/ExpressionParser/Clang/ObjC3MethodNames.h",
        "lldb/source/Plugins/ExpressionParser/Clang/ClangExpressionDeclMap.cpp",
        "lldb/source/Plugins/ExpressionParser/Clang/IRForTarget.cpp",
        "lldb/source/Plugins/ExpressionParser/Clang/IRForTarget.h",
    ],
}


def git(*arguments: str) -> bytes:
    return subprocess.check_output(["git", "-C", str(SOURCE), *arguments])


def packet_sections(data: bytes) -> dict[bytes, bytes]:
    return {part.split(b"\n", 1)[0].split(b" b/", 1)[1]: b"diff --git " + part
            for part in data.split(b"diff --git ")[1:]}


def stable_packet(current: bytes, previous: bytes) -> bytes:
    """Keep packet identity independent of intent-to-add and existing ordering."""
    old, new = packet_sections(previous), packet_sections(current)
    for path, section in new.items():
        # Older packets contain both forms of Git's optional new-file index
        # metadata. Preserve that choice to avoid changing unrelated identities.
        if path in old and b"\nnew file mode " in section and b"\nindex " not in old[path]:
            new[path] = b"".join(line for line in section.splitlines(keepends=True)
                                  if not line.startswith(b"index "))
    order = [path for path in old if path in new] + sorted(new.keys() - old.keys())
    return b"".join(new[path] for path in order)


def export(selected_paths: list[str] | None = None) -> None:
    """Export all source edits, or replace only explicitly completed file sections.

    Selected files are exact repository-relative paths, never directory selectors.
    Other packet sections retain their previously committed contents, allowing
    independent development in one source tree without exporting unfinished work.
    """
    selected = set(selected_paths) if selected_paths is not None else None
    if selected is not None:
        if not selected:
            raise RuntimeError("Selective export requires at least one source file")
        for relative in selected:
            path = Path(relative)
            if (path.is_absolute() or ".." in path.parts or
                    path.as_posix() != relative or (SOURCE / path).is_dir()):
                raise RuntimeError(f"Selective export requires an exact relative file: {relative}")
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
    for relative in (changed_paths if selected is None else sorted(selected)):
        if relative == ".objc3c-prepared-source.json":
            continue
        owners = [name for name, paths in PACKETS.items()
                  if any(relative == p or relative.startswith(p + "/") for p in paths)]
        if len(owners) != 1:
            raise RuntimeError(f"Source path requires exactly one patch owner: {relative}: {owners}")
    records = []
    for name, paths in PACKETS.items():
        packet = STACK / name
        previous = packet.read_bytes() if packet.exists() else b""
        export_paths = paths if selected is None else sorted(
            relative for relative in selected
            if any(relative == p or relative.startswith(p + "/") for p in paths))
        if not export_paths:
            records.append({"file": name, "sha256": hashlib.sha256(previous).hexdigest()})
            continue
        patch = git("diff", "--binary", "--abbrev=9", "--no-ext-diff", "HEAD", "--", *export_paths)
        for relative in sorted(untracked_paths):
            if not any(relative == p or relative.startswith(p + "/") for p in export_paths):
                continue
            data = (SOURCE / relative).read_bytes().replace(b"\r\n", b"\n")
            if b"\0" in data:
                raise RuntimeError(f"Unexpected binary source: {relative}")
            lines = data.splitlines(keepends=True)
            header = (
                f"diff --git a/{relative} b/{relative}\n"
                f"new file mode {'100755' if (SOURCE / relative).stat().st_mode & 0o111 else '100644'}\n"
                f"index 000000000..{git('hash-object', '--', relative).decode().strip()[:9]}\n"
            ).encode()
            if lines:
                count = '' if len(lines) == 1 else f',{len(lines)}'
                header += (f"--- /dev/null\n+++ b/{relative}\n"
                           f"@@ -0,0 +1{count} @@\n").encode()
            patch += header
            for line in lines:
                patch += b"+" + line
                if not line.endswith(b"\n"):
                    patch += b"\n\\ No newline at end of file\n"
        if selected is not None:
            replacement = packet_sections(patch)
            retained = packet_sections(previous)
            for relative in export_paths:
                key = relative.encode()
                if key in replacement:
                    retained[key] = replacement[key]
                else:
                    retained.pop(key, None)
            patch = b"".join(retained.values())
        if not patch:
            raise RuntimeError(f"Refusing empty patch packet: {name}")
        patch = stable_packet(patch, previous)
        packet.write_bytes(patch)
        records.append({"file": name, "sha256": hashlib.sha256(patch).hexdigest()})
    (STACK / "series").write_text("\n".join(PACKETS) + "\n", encoding="utf-8")
    manifest["patches"] = records
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"base": head, "patches": records}, indent=2))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--paths", nargs="+", metavar="SOURCE_FILE",
                        help="export only completed files; preserve other packet sections")
    export(parser.parse_args().paths)
