#include "clang_foundation/async_c_exports.h"

#include <algorithm>
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
#include "llvm/ADT/SmallVector.h"
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

struct CExportActorMethod {
  const clang::ObjCInterfaceDecl *actor;
  const clang::ObjCMethodDecl *method;
  std::string symbol;
};

std::optional<CExportActorMethod> SelectActorMethod(clang::ASTContext &context,
                                                    llvm::StringRef selection) {
  const auto [owner, spelling] = selection.split("::");
  if (!IsSourceIdentifier(owner) || spelling.empty()) return std::nullopt;
  const auto matches = context.getTranslationUnitDecl()->lookup(&context.Idents.get(owner));
  const auto *decl = matches.empty() || std::next(matches.begin()) != matches.end()
      ? nullptr : llvm::dyn_cast<clang::ObjCInterfaceDecl>(matches.front());
  const auto *actor = decl ? decl->getDefinition() : nullptr;
  if (!actor || !clang::isObjC3Actor(actor)) return std::nullopt;

  const bool has_parameters = spelling.ends_with(":");
  llvm::SmallVector<llvm::StringRef, 4> pieces;
  (has_parameters ? spelling.drop_back() : spelling).split(pieces, ':');
  if ((!has_parameters && pieces.size() != 1) ||
      !llvm::all_of(pieces, IsSourceIdentifier))
    return std::nullopt;
  llvm::SmallVector<const clang::IdentifierInfo *, 4> identifiers;
  std::string symbol = owner.str();
  for (auto piece : pieces) {
    identifiers.push_back(&context.Idents.get(piece));
    symbol += '_' + piece.str();
  }
  const auto selector = context.Selectors.getSelector(
      has_parameters ? identifiers.size() : 0, identifiers.data());
  const auto *method = actor->lookupInstanceMethod(selector);
  if (!method || !llvm::isa<clang::ObjCInterfaceDecl>(method->getDeclContext()) ||
      method->isVariadic() || !clang::isObjC3ActorIsolated(method) ||
      method->hasAttr<clang::NSConsumesSelfAttr>() ||
      method->hasAttr<clang::ObjCReturnsInnerPointerAttr>())
    return std::nullopt;
  return CExportActorMethod{actor, method, std::move(symbol)};
}

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

  // Ownership belongs to the bridge. Its aliases must also parse in C
  // consumers using Blocks, where ARC qualifiers are unavailable.
  type = context.getQualifiedType(type.getLocalUnqualifiedType(),
                                  type.getLocalQualifiers().withoutObjCLifetime());
  declarations << "typedef ";
  type.print(declarations, policy, alias);
  declarations << ";\n#else\ntypedef void *" << alias << ";\n#endif\n";
  return CExportType{alias.str(), kind};
}

}  // namespace

AsyncCExportSource GenerateAsyncCExports(clang::ASTContext &context,
                                        std::span<const std::string> symbols,
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
  for (const auto &selection : symbols) {
    auto reject = [&](llvm::StringRef reason) {
      failure = "Cannot export C async entry " + selection + ": " + reason.str();
      return AsyncCExportSource{};
    };
    std::optional<CExportActorMethod> actor_method;
    const clang::Decl *source = nullptr;
    const clang::FunctionProtoType *prototype = nullptr;
    clang::QualType result_type;
    llvm::ArrayRef<clang::ParmVarDecl *> source_parameters;
    clang::ObjC3FunctionEffectInfo effects;
    std::string name = selection;
    if (llvm::StringRef(selection).contains("::")) {
      actor_method = SelectActorMethod(context, selection);
      if (!actor_method)
        return reject("expected an isolated instance selector in the named actor class hierarchy");
      const auto *method = actor_method->method;
      name = actor_method->symbol;
      source = method;
      if (!method->getObjC3FunctionType().isNull())
        prototype = method->getObjC3FunctionType()->getAs<clang::FunctionProtoType>();
      effects = method->getObjC3FunctionEffects();
      if (effects.Async && !effects.AsyncFrame)
        return reject("the actor method requires a native async frame");
      result_type = method->getReturnType();
      source_parameters = method->parameters();
    } else {
      if (!IsSourceIdentifier(name)) return reject("expected a source identifier");
      const auto declarations = tu->lookup(&context.Idents.get(name));
      const auto *function = !declarations.empty() &&
          std::next(declarations.begin()) == declarations.end()
          ? llvm::dyn_cast<clang::FunctionDecl>(declarations.front()) : nullptr;
      prototype = function ? function->getType()->getAs<clang::FunctionProtoType>() : nullptr;
      if (!prototype || prototype->isVariadic() ||
          function->getTemplatedKind() != clang::FunctionDecl::TK_NonTemplate)
        return reject("expected one nonvariadic free function with a native prototype");
      effects = prototype->getObjC3FunctionEffects();
      if (!effects.AsyncFrame || clang::isObjC3ActorIsolated(function))
        return reject("expected a native async frame entry with independent result lifetime");
      if (!function->isExternallyVisible() && !function->getDefinition())
        return reject("the internal async declaration needs a definition in this translation unit");
      source = function;
      result_type = prototype->getReturnType();
      source_parameters = function->parameters();
    }
    if (prototype && (prototype->hasObjC3BorrowedResult() ||
                      prototype->getCallConv() != clang::CC_C))
      return reject("the source result lifetime or calling convention cannot cross the C ABI");
    if (!selected.insert(name).second) return reject("the generated C symbol is already selected");

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
    auto result = export_type(result_type, type_prefix + "result_t");
    if (!result)
      return reject("the result has no self-contained C value declaration");
    std::string parameters, inputs;
    bool owning_bridge = actor_method.has_value();
    constexpr llvm::StringLiteral receiver = "__objc3_c_receiver";
    constexpr llvm::StringLiteral opaque_receiver_type = "void *";
    if (actor_method) {
      // The C signature can be imported without exporting the actor's class
      // API. The private frame alone needs its typed receiver.
      parameters = ", " + opaque_receiver_type.str() + receiver.str();
      inputs = receiver.str();
    }
    std::string message = "[" + receiver.str();
    for (unsigned index = 0; index < source_parameters.size(); ++index) {
      const auto *parameter = source_parameters[index];
      const auto parameter_type =
          prototype ? prototype->getParamType(index) : parameter->getType();
      auto type = export_type(parameter_type,
                              type_prefix + "arg" + std::to_string(index) + "_t");
      if (!type || (prototype && prototype->getExtParameterInfo(index).getOpaqueValue()) ||
          parameter->hasAttr<clang::NSConsumedAttr>() || parameter->hasAttr<clang::CFConsumedAttr>())
        return reject("parameter " + std::to_string(index + 1) +
                      " has no self-contained C value declaration");
      const auto argument = "__objc3_c_argument_" + std::to_string(index);
      owning_bridge |= parameter_type->isObjCRetainableType();
      parameters += ", " + type->spelling + " " + argument;
      if (!inputs.empty()) inputs += ", ";
      inputs += argument;
      if (actor_method)
        message += " " + actor_method->method->getSelector().getNameForSlot(index).str() + ':' + argument;
    }

    const bool returns_void = result_type->isVoidType();
    const auto declaration = (returns_void
        ? "OBJC3_ASYNC_C_DECLARE_VOID(" + name
        : "OBJC3_ASYNC_C_DECLARE(" + name + ", " + result->spelling) + parameters + ");\n";
    header << types << declaration;
    output.interface_exports.push_back(thunk);
    output.interface_exports.push_back(callback);
    if (actor_method && source_parameters.empty())
      message += " " + actor_method->method->getSelector().getAsString();
    const auto call = actor_method ? message + ']' :
        (language.CPlusPlus ? "::" : "") + name + '(' + inputs + ')';
    auto frame = call;
    // The unqualified C preprocessing spelling must select the original
    // declaration even when the provider reuses a name as a macro later.
    if (actor_method) {
      definitions << "#undef " << actor_method->actor->getName() << '\n';
      const auto selector = actor_method->method->getSelector();
      for (unsigned slot = 0; slot < std::max(1u, selector.getNumArgs()); ++slot)
        definitions << "#undef " << selector.getNameForSlot(slot) << '\n';
    }
    definitions << "#undef " << name << "\n#undef " << thunk << "\n#undef "
                << callback << "\n" << types << declaration;
    if (owning_bridge || clang::getObjC3Executor(source)) {
      const auto bridge = "__objc3_c_executor_bridge_" + name;
      if (!tu->lookup(&context.Idents.get(bridge)).empty())
        return reject("the generated executor bridge name is already declared");
      const bool checked = effects.Throws != clang::ObjC3ThrowsKind::None;
      auto bridge_parameters = parameters.empty() ? "void" : parameters.substr(2);
      auto bridge_inputs = inputs;
      if (actor_method) {
        const auto actor_pointer = actor_method->actor->getName().str() + " *";
        bridge_parameters.replace(0, opaque_receiver_type.size(), actor_pointer);
        bridge_inputs = (language.ObjCAutoRefCount ? "(__bridge " : "(") +
            actor_pointer + ")" + bridge_inputs;
      }
      // A native await supplies the same executor admission, startup errors,
      // and cancellation propagation as an ordinary Objective-C caller.
      if (owning_bridge) {
        // The native async Block ramp copies its captures before suspending,
        // retaining objects and copying Blocks in both ARC and MRC.
        definitions << "static objc3_async_frame " << bridge << '('
                    << bridge_parameters << ") { return (^" << result->spelling
                    << "(void) __objc3_async_frame";
      } else {
        definitions << "static " << result->spelling << ' ' << bridge << '('
                    << bridge_parameters << ") __objc3_async_frame";
      }
      if (checked) {
        definitions << " __objc3_throws";
        if (effects.Throws == clang::ObjC3ThrowsKind::Typed)
          definitions << '(' << effects.ErrorType.getAsString(context.getPrintingPolicy()) << ')';
      }
      definitions << " { ";
      if (!returns_void) definitions << "return ";
      if (checked) definitions << "__objc3_try(";
      definitions << "__objc3_await " << call;
      if (checked) definitions << ')';
      definitions << (owning_bridge ? "; })(); }\n" : "; }\n");
      frame = bridge + '(' + bridge_inputs + ')';
    }
    definitions << "OBJC3_ASYNC_C_DEFINE_" << (returns_void ? "VOID" : result->kind)
                << '(' << name;
    if (!returns_void) definitions << ", " << result->spelling;
    definitions << ", " << frame << ", "
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
