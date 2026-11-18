#include "clang_foundation/async_c_exports.h"

#include <iterator>
#include <optional>

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/ObjC3Borrowed.h"
#include "clang/AST/ObjC3Isolation.h"
#include "clang/AST/ObjC3ResourceTypes.h"
#include "clang/Basic/TargetInfo.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

namespace objc3::clang_foundation {
namespace {

bool IsSourceIdentifier(llvm::StringRef name) {
  return !name.empty() && !name.starts_with("__") &&
      (llvm::isAlpha(name.front()) || name.front() == '_') &&
      llvm::all_of(name.drop_front(), [](char part) {
        return llvm::isAlnum(part) || part == '_';
      });
}

std::optional<std::string> PortableValueType(clang::ASTContext &context,
                                             clang::QualType type) {
  if (clang::containsObjC3BorrowedType(context, type) ||
      clang::getObjC3ResourceTypeContract(context, type).Cleanup)
    return std::nullopt;
  type = type.getCanonicalType();
  auto element = type;
  while (const auto *pointer = element->getAs<clang::PointerType>()) {
    if (element.hasAddressSpace()) return std::nullopt;
    element = pointer->getPointeeType();
  }
  // Builtin C integers, real numbers and pointers to them have declarations
  // independent of provider typedefs, C++ records and ObjC class definitions.
  if (element.hasAddressSpace() || !element->isBuiltinType() ||
      !(element->isIntegerType() || element->isRealFloatingType() ||
        element->isVoidType()))
    return std::nullopt;
  if (element->isChar8Type() || element->isChar16Type() ||
      element->isChar32Type() || element->isWideCharType() ||
      element->isHalfType() || element->isBFloat16Type())
    return std::nullopt;
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.Bool = true;
  return type.getAsString(policy);
}

}  // namespace

AsyncCExportSource GenerateAsyncCExports(clang::ASTContext &context,
                                        llvm::ArrayRef<std::string> symbols,
                                        std::string &failure) {
  failure.clear();
  AsyncCExportSource output;
  auto &language = context.getLangOpts();
  if (!language.ObjC3 || !language.Blocks || !context.getTargetInfo().getTriple().isOSLinux()) {
    failure = "C completion exports require Linux Objective-C 3.0 with Blocks enabled";
    return {};
  }

  llvm::raw_string_ostream header(output.header), definitions(output.definitions);
  header << "#pragma once\n#include <stdbool.h>\n"
            "#include <objc3c/runtime/objc3_async_c.h>\n\n";
  definitions << "\n\n#include <stdbool.h>\n";
  llvm::StringSet<> selected;
  auto *tu = context.getTranslationUnitDecl();
  for (const auto &name : symbols) {
    auto reject = [&](llvm::StringRef reason) {
      failure = "Cannot export C async entry " + name + ": " + reason.str();
      return AsyncCExportSource{};
    };
    if (!IsSourceIdentifier(name) || !selected.insert(name).second)
      return reject("expected a distinct source identifier");

    const auto declarations = tu->lookup(&context.Idents.get(name));
    const auto *source = !declarations.empty() &&
        std::next(declarations.begin()) == declarations.end()
        ? llvm::dyn_cast<clang::FunctionDecl>(declarations.front()) : nullptr;
    const auto *prototype = source ? source->getType()->getAs<clang::FunctionProtoType>() : nullptr;
    if (!prototype || prototype->isVariadic() ||
        source->getTemplatedKind() != clang::FunctionDecl::TK_NonTemplate ||
        prototype->getCallConv() != clang::CC_C)
      return reject("expected one nonvariadic free function with a native prototype");
    const auto effects = prototype->getObjC3FunctionEffects();
    if (!effects.AsyncFrame || clang::getObjC3Executor(source) ||
        clang::isObjC3ActorIsolated(source) || prototype->hasObjC3BorrowedResult())
      return reject("expected a global native async frame entry with independent result lifetime");
    if (!source->isExternallyVisible() && !source->getDefinition())
      return reject("the internal async declaration needs a definition in this translation unit");

    const auto thunk = name + "_async_c";
    const auto callback = name + "_completion_t";
    if (!tu->lookup(&context.Idents.get(thunk)).empty() ||
        !tu->lookup(&context.Idents.get(callback)).empty())
      return reject("the generated C thunk or callback name is already declared");

    auto result = PortableValueType(context, prototype->getReturnType());
    if (!result)
      return reject("the result has no self-contained C value declaration");
    std::string parameters, inputs;
    for (unsigned index = 0; index < prototype->getNumParams(); ++index) {
      auto type = PortableValueType(context, prototype->getParamType(index));
      if (!type || prototype->getExtParameterInfo(index).getOpaqueValue())
        return reject("parameter " + std::to_string(index + 1) +
                      " has no self-contained C value declaration");
      const auto argument = "__objc3_c_argument_" + std::to_string(index);
      parameters += ", " + *type + " " + argument;
      if (!inputs.empty()) inputs += ", ";
      inputs += argument;
    }

    const bool returns_void = prototype->getReturnType()->isVoidType();
    const auto declaration = (returns_void
        ? "OBJC3_ASYNC_C_DECLARE_VOID(" + name
        : "OBJC3_ASYNC_C_DECLARE(" + name + ", " + *result) + parameters + ");\n";
    header << declaration;
    // The unqualified C preprocessing spelling must select the original
    // declaration even when the provider reuses a name as a macro later.
    definitions << "#undef " << name << "\n#undef " << thunk << "\n#undef "
                << callback << "\n" << declaration
                << (returns_void ? "OBJC3_ASYNC_C_DEFINE_VOID(" : "OBJC3_ASYNC_C_DEFINE_VALUE(")
                << name;
    if (!returns_void) definitions << ", " << *result;
    definitions << ", " << (language.CPlusPlus ? "::" : "") << name << '(' << inputs << "), "
                << (effects.Throws == clang::ObjC3ThrowsKind::None ? 0 : 1)
                << ", 0, 0" << parameters << ")\n";
  }
  return output;
}

}  // namespace objc3::clang_foundation
