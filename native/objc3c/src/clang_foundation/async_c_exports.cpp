#include "clang_foundation/async_c_exports.h"

#include <iterator>
#include <optional>

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/ObjC3Borrowed.h"
#include "clang/AST/ObjC3Isolation.h"
#include "clang/AST/ObjC3ResourceTypes.h"
#include "clang/AST/RecordLayout.h"
#include "clang/Basic/TargetInfo.h"
#include "llvm/ADT/SmallPtrSet.h"
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

struct CExportType {
  std::string spelling;
  const char *kind = "VALUE";
};

// Serialize a C spelling of each reachable storage type and verify the
// provider's size, alignment and field offsets when the client compiles it.
class CExportRecords {
public:
  CExportRecords(clang::ASTContext &context, std::vector<std::string> &exports)
      : context_(context), exports_(exports), policy_(context.getLangOpts()) {
    policy_.Bool = true;
    policy_.SuppressTagKeyword = false;
  }

  std::optional<std::string> Render(clang::QualType type) {
    if (!Admit(type)) return std::nullopt;
    return type.getCanonicalType().getAsString(policy_);
  }

  llvm::StringRef declarations() const { return declarations_; }

private:
  bool Admit(clang::QualType type, unsigned depth = 0) {
    if (depth > 32 || clang::containsObjC3BorrowedType(context_, type) ||
        clang::getObjC3ResourceTypeContract(context_, type).Cleanup)
      return false;
    if (PortableValueType(context_, type)) return true;
    type = type.getCanonicalType();
    if (type.hasAddressSpace()) return false;
    if (const auto *array = context_.getAsConstantArrayType(type))
      return Admit(array->getElementType(), depth + 1);

    bool by_value = true;
    while (const auto *pointer = type->getAs<clang::PointerType>()) {
      by_value = false;
      type = pointer->getPointeeType();
      if (type.hasAddressSpace()) return false;
    }
    const auto *record_type = type->getAs<clang::RecordType>();
    const auto *record = record_type
        ? llvm::cast<clang::RecordDecl>(record_type->getDecl()->getCanonicalDecl()) : nullptr;
    if (!record || !(record->isStruct() || record->isUnion()) ||
        !record->getIdentifier() || !IsSourceIdentifier(record->getName()) ||
        !record->getDeclContext()->getRedeclContext()->isTranslationUnit())
      return false;

    const std::string tag = (record->isUnion() ? "union " : "struct ") + record->getName().str();
    if (forwarded_.insert(record).second) {
      declarations_ += tag + ";\n";
      exports_.push_back(record->getName().str());
    }
    if (!by_value || complete_.contains(record)) return true;
    const auto *definition = record->getDefinition();
    if (!definition || definition->field_empty() || definition->hasAttrs() ||
        definition->hasFlexibleArrayMember() || !active_.insert(record).second)
      return false;
    if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(definition);
        cxx && (!cxx->isTrivial() || !cxx->isStandardLayout() ||
                !cxx->isAggregate() || cxx->getNumBases()))
      return false;

    std::string fields;
    llvm::raw_string_ostream out(fields);
    for (const auto *field : definition->fields()) {
      if (!field->getIdentifier() || field->isBitField() || field->hasAttrs() ||
          !IsSourceIdentifier(field->getName()) || !Admit(field->getType(), depth + 1))
        return false;
      out << "  ";
      field->getType().getCanonicalType().print(out, policy_, field->getName());
      out << ";\n";
    }

    const auto &layout = context_.getASTRecordLayout(definition);
    const auto size = std::to_string(layout.getSize().getQuantity());
    const auto alignment = std::to_string(layout.getAlignment().getQuantity());
    declarations_ += tag + " {\n" + fields + "};\n";
    declarations_ += "static_assert(sizeof(" + tag + ") == " + size +
        " && __alignof__(" + tag + ") == " + alignment + ", \"C async aggregate layout\");\n";
    unsigned index = 0;
    for (const auto *field : definition->fields()) {
      const auto offset = layout.getFieldOffset(index++) / context_.getCharWidth();
      declarations_ += "static_assert(offsetof(" + tag + ", " + field->getName().str() +
          ") == " + std::to_string(offset) + ", \"C async aggregate field offset\");\n";
    }
    complete_.insert(record);
    active_.erase(record);
    return true;
  }

  clang::ASTContext &context_;
  std::vector<std::string> &exports_;
  clang::PrintingPolicy policy_;
  std::string declarations_;
  llvm::SmallPtrSet<const clang::RecordDecl *, 16> forwarded_, complete_, active_;
};

std::optional<CExportType> ExportType(clang::ASTContext &context,
                                     clang::QualType type, llvm::StringRef alias,
                                     llvm::raw_ostream &declarations,
                                     CExportRecords &records) {
  if (auto value = PortableValueType(context, type))
    return CExportType{std::move(*value)};
  if (clang::containsObjC3BorrowedType(context, type) ||
      clang::getObjC3ResourceTypeContract(context, type).Cleanup ||
      type.hasAddressSpace())
    return std::nullopt;

  type = type.getCanonicalType();
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.Bool = true;
  policy.SuppressLifetimeQualifiers = true;
  const char *kind;
  if (const auto *object = type->getAs<clang::ObjCObjectPointerType>()) {
    if (object->isSpecialized()) return std::nullopt;
    kind = "OBJECT";
    declarations << "#ifdef __OBJC__\n";
    if (const auto *interface = object->getInterfaceDecl())
      declarations << "@class " << interface->getName() << ";\n";
    for (const auto *protocol : object->quals())
      declarations << "@protocol " << protocol->getName() << ";\n";
  } else if (const auto *block = type->getAs<clang::BlockPointerType>()) {
    const auto *signature = block->getPointeeType()->getAs<clang::FunctionProtoType>();
    if (!signature || signature->isVariadic() || signature->getCallConv() != clang::CC_C ||
        signature->getObjC3FunctionEffects().Async ||
        signature->getObjC3FunctionEffects().Throws != clang::ObjC3ThrowsKind::None ||
        !signature->getFunctionEffects().empty() ||
        !PortableValueType(context, signature->getReturnType()))
      return std::nullopt;
    for (unsigned index = 0; index < signature->getNumParams(); ++index)
      if (!PortableValueType(context, signature->getParamType(index)) ||
          signature->getExtParameterInfo(index).getOpaqueValue())
        return std::nullopt;
    kind = "BLOCK";
    declarations << "#ifdef __BLOCKS__\n";
  } else {
    if (auto value = records.Render(type))
      return CExportType{std::move(*value)};
    return std::nullopt;
  }

  declarations << "typedef ";
  type.print(declarations, policy, alias);
  declarations << ";\n#else\ntypedef void *" << alias << ";\n#endif\n";
  return CExportType{alias.str(), kind};
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

  std::string entries;
  llvm::raw_string_ostream header(entries), definitions(output.definitions);
  definitions << "\n\n#include <stdbool.h>\n";
  llvm::StringSet<> selected;
  auto *tu = context.getTranslationUnitDecl();
  CExportRecords records(context, output.interface_exports);
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

    std::string types;
    llvm::raw_string_ostream declarations_text(types);
    const auto type_prefix = name + "_async_c_";
    auto export_type = [&](clang::QualType type, const std::string &alias) {
      auto emitted = ExportType(context, type, alias, declarations_text, records);
      if (emitted && emitted->spelling == alias) {
        if (!tu->lookup(&context.Idents.get(alias)).empty())
          return std::optional<CExportType>{};
        output.interface_exports.push_back(alias);
      }
      return emitted;
    };
    auto result = export_type(prototype->getReturnType(), type_prefix + "result_t");
    if (!result)
      return reject("the result has no self-contained C value declaration");
    std::string parameters, inputs;
    for (unsigned index = 0; index < prototype->getNumParams(); ++index) {
      auto type = export_type(prototype->getParamType(index),
                              type_prefix + "arg" + std::to_string(index) + "_t");
      if (!type || prototype->getExtParameterInfo(index).getOpaqueValue())
        return reject("parameter " + std::to_string(index + 1) +
                      " has no self-contained C value declaration");
      const auto argument = "__objc3_c_argument_" + std::to_string(index);
      parameters += ", " + type->spelling + " " + argument;
      if (!inputs.empty()) inputs += ", ";
      inputs += argument;
    }

    const bool returns_void = prototype->getReturnType()->isVoidType();
    const auto declaration = (returns_void
        ? "OBJC3_ASYNC_C_DECLARE_VOID(" + name
        : "OBJC3_ASYNC_C_DECLARE(" + name + ", " + result->spelling) + parameters + ");\n";
    header << types << declaration;
    output.interface_exports.push_back(thunk);
    output.interface_exports.push_back(callback);
    // The unqualified C preprocessing spelling must select the original
    // declaration even when the provider reuses a name as a macro later.
    definitions << "#undef " << name << "\n#undef " << thunk << "\n#undef "
                << callback << "\n" << types << declaration
                << "OBJC3_ASYNC_C_DEFINE_" << (returns_void ? "VOID" : result->kind)
                << '(' << name;
    if (!returns_void) definitions << ", " << result->spelling;
    definitions << ", " << (language.CPlusPlus ? "::" : "") << name << '(' << inputs << "), "
                << (effects.Throws == clang::ObjC3ThrowsKind::None ? 0 : 1)
                << ", 0, 0" << parameters << ")\n";
  }
  output.header = "#pragma once\n#include <stdbool.h>\n"
                  "#include <objc3c/runtime/objc3_async_c.h>\n\n";
  if (!records.declarations().empty()) {
    output.header += "#include <assert.h>\n#include <stddef.h>\n\n";
    output.header += records.declarations();
  }
  output.header += entries;
  return output;
}

}  // namespace objc3::clang_foundation
