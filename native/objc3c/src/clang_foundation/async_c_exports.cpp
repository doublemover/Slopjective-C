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
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Twine.h"
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

struct CExportFreeFunction {
  const clang::FunctionDecl *function;
  std::string symbol;
  llvm::SmallVector<llvm::StringRef, 4> identifiers;
};

std::optional<CExportFreeFunction> SelectFreeFunction(clang::ASTContext &context,
                                                      llvm::StringRef selection) {
  CExportFreeFunction result{};
  selection.split(result.identifiers, "::");
  if (result.identifiers.empty() || !llvm::all_of(result.identifiers, IsSourceIdentifier) ||
      (result.identifiers.size() > 1 && !context.getLangOpts().CPlusPlus))
    return std::nullopt;
  const clang::DeclContext *scope = context.getTranslationUnitDecl();
  for (std::size_t index = 0; index < result.identifiers.size(); ++index) {
    const auto identifier = result.identifiers[index];
    const auto candidates = scope->lookup(&context.Idents.get(identifier));
    if (candidates.empty() || std::next(candidates.begin()) != candidates.end())
      return std::nullopt;
    if (!result.symbol.empty()) result.symbol += '_';
    result.symbol.append(identifier.data(), identifier.size());
    const auto *entry = candidates.front();
    if (index + 1 == result.identifiers.size()) {
      result.function = llvm::dyn_cast<clang::FunctionDecl>(entry);
      break;
    }
    if (const auto *alias = llvm::dyn_cast<clang::NamespaceAliasDecl>(entry))
      scope = alias->getNamespace();
    else if (const auto *name_space = llvm::dyn_cast<clang::NamespaceDecl>(entry))
      scope = name_space;
    else
      return std::nullopt;
  }
  if (!result.function || result.function->getTemplatedKind() != clang::FunctionDecl::TK_NonTemplate)
    return std::nullopt;
  return result;
}

void PrintThrows(llvm::raw_ostream &source, clang::ASTContext &context,
                 clang::ObjC3FunctionEffectInfo effects) {
  source << " __objc3_throws";
  if (effects.Throws == clang::ObjC3ThrowsKind::Typed)
    source << '(' << effects.ErrorType.getAsString(context.getPrintingPolicy()) << ')';
}

clang::QualType CExportSpellingType(clang::ASTContext &context, clang::QualType type,
                                   bool nullable_result = false) {
  const auto canonical = type.getCanonicalType();
  clang::QualType spelling = canonical.getLocalUnqualifiedType();
  if (const auto *pointer = type->getAs<clang::PointerType>()) {
    spelling = context.getPointerType(CExportSpellingType(context, pointer->getPointeeType()));
  } else if (const auto *array = context.getAsConstantArrayType(type)) {
    spelling = context.getConstantArrayType(CExportSpellingType(context, array->getElementType()),
        array->getSize(), nullptr, array->getSizeModifier(), array->getIndexTypeCVRQualifiers());
  } else if (const auto *block = type->getAs<clang::BlockPointerType>()) {
    spelling = context.getBlockPointerType(CExportSpellingType(context, block->getPointeeType()));
  } else if (const auto *function = type->getAs<clang::FunctionProtoType>()) {
    llvm::SmallVector<clang::QualType, 4> parameters;
    for (const auto parameter : function->param_types())
      parameters.push_back(CExportSpellingType(context, parameter));
    spelling = context.getFunctionType(CExportSpellingType(context, function->getReturnType()),
                                       parameters, function->getExtProtoInfo());
  }
  // Strip native ARC qualifiers before adding an attributed type wrapper,
  // so neither typed Blocks nor opaque aliases expose ownership to plain C.
  spelling = context.getQualifiedType(spelling, canonical.getLocalQualifiers().withoutObjCLifetime());
  auto nullability = type->getNullability();
  // The ordinary C envelope can transport null on failure or cancellation,
  // even if the logical source result is nonnull on success.
  if (type->isPointerType() || type->isObjCRetainableType()) {
    if (nullable_result) nullability = clang::NullabilityKind::Nullable;
    else if (!nullability) nullability = clang::NullabilityKind::Unspecified;
  }
  if (!nullability) return spelling;
  clang::attr::Kind attribute;
  switch (*nullability) {
  case clang::NullabilityKind::NonNull: attribute = clang::attr::TypeNonNull; break;
  case clang::NullabilityKind::Nullable: attribute = clang::attr::TypeNullable; break;
  case clang::NullabilityKind::Unspecified: attribute = clang::attr::TypeNullUnspecified; break;
  case clang::NullabilityKind::NullableResult: attribute = clang::attr::TypeNullableResult; break;
  }
  return context.getAttributedType(attribute, spelling, spelling);
}

std::optional<std::string> PortableValueType(clang::ASTContext &context,
                                             clang::QualType type,
                                             bool nullable_result = false) {
  if (clang::containsObjC3BorrowedType(context, type) ||
      clang::getObjC3ResourceTypeContract(context, type).Cleanup)
    return std::nullopt;
  auto element = type.getCanonicalType();
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
  return CExportSpellingType(context, type, nullable_result).getAsString(policy);
}

bool IsPortableVector(clang::ASTContext &context, clang::QualType type) {
  const auto *vector = type->getAs<clang::VectorType>();
  if (!vector || vector->getVectorKind() != clang::VectorKind::Generic || type.hasAddressSpace())
    return false;
  const auto &target = context.getTargetInfo().getTriple();
  if (!(target.getArch() == llvm::Triple::x86_64 || target.getArch() == llvm::Triple::aarch64))
    return false;
  const auto element = vector->getElementType();
  const auto width = context.getTypeSize(type);
  const auto lane = context.getTypeSize(element);
  // The common Linux SIMD calling conventions transport fully populated
  // 64/128-bit vectors without an optional wide-vector ISA contract.
  return (width == 64 || width == 128) && lane <= 64 &&
         width == lane * vector->getNumElements() && !element->isBooleanType() &&
         PortableValueType(context, element).has_value();
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
      method->hasAttr<clang::ObjCReturnsInnerPointerAttr>())
    return std::nullopt;
  return CExportActorMethod{actor, method, std::move(symbol)};
}

// Serialize a C spelling of each reachable storage type and verify the
// provider's size, alignment and field offsets when the client compiles it.
class CExportValues {
public:
  CExportValues(clang::ASTContext &context, std::vector<std::string> &exports)
      : context_(context), exports_(exports), policy_(context.getLangOpts()) {
    policy_.Bool = true;
    policy_.SuppressTagKeyword = false;
  }

  std::optional<std::string> Render(clang::QualType type, bool nullable_result) {
    if (!Admit(type)) return std::nullopt;
    return CExportSpellingType(context_, type, nullable_result).getAsString(policy_);
  }

  llvm::StringRef declarations() const { return declarations_; }

  bool SupportsCallback(const clang::FunctionProtoType *signature) {
    if (!signature || signature->isVariadic() || signature->getCallConv() != clang::CC_C ||
        signature->getObjC3FunctionEffects().Async ||
        signature->getObjC3FunctionEffects().Throws != clang::ObjC3ThrowsKind::None ||
        !signature->getFunctionEffects().empty() ||
        !Admit(signature->getReturnType()))
      return false;
    for (unsigned index = 0; index < signature->getNumParams(); ++index)
      if (signature->getExtParameterInfo(index).getOpaqueValue() ||
          !Admit(signature->getParamType(index)))
        return false;
    return true;
  }

private:
  bool AdmitEnum(const clang::EnumDecl *declaration) {
    const auto *definition = declaration->getDefinition();
    if (!definition || !definition->isFixed() || declaration->isScoped() ||
        definition->enumerator_begin() == definition->enumerator_end() || !declaration->getIdentifier() ||
        !IsSourceIdentifier(declaration->getName()) ||
        !declaration->getDeclContext()->getRedeclContext()->isTranslationUnit() ||
        definition->hasAttrs() || definition->getIntegerType().isNull())
      return false;
    if (enums_.contains(declaration)) return true;
    const auto integer = definition->getIntegerType();
    const auto spelling = PortableValueType(context_, integer);
    if (!spelling || context_.getTypeSize(integer) > 64) return false;

    const std::string tag = "enum " + declaration->getName().str();
    std::string body;
    llvm::raw_string_ostream out(body);
    // Fixed underlying types are standard in C23 and C++. Clang supports
    // their C17 spelling through __extension__ without dropping diagnostics
    // on the surrounding client's declarations.
    out << "__extension__ " << tag << " : " << *spelling << " {\n";
    for (const auto *constant : definition->enumerators()) {
      if (!IsSourceIdentifier(constant->getName()) || constant->hasAttrs()) return false;
      const auto &value = constant->getInitVal();
      out << "  " << constant->getName() << " = ";
      if (value.isSigned() && value.isNegative() && value.isMinSignedValue() &&
          value.getBitWidth() == 64) {
        out << "(-9223372036854775807LL - 1LL)";
      } else {
        llvm::SmallString<32> decimal;
        value.toString(decimal, 10);
        out << decimal << (value.isSigned() ? "LL" : "ULL");
      }
      out << ",\n";
    }
    out << "};\n";
    const auto type = context_.getCanonicalTagType(definition);
    out << "static_assert(sizeof(" << tag << ") == " << context_.getTypeSizeInChars(type).getQuantity()
        << " && __alignof__(" << tag << ") == " << context_.getTypeAlignInChars(type).getQuantity()
        << ", \"C export enum layout\");\n";
    declarations_ += body;
    enums_.insert(declaration);
    exports_.push_back(declaration->getName().str());
    return true;
  }

  bool Admit(clang::QualType type, unsigned depth = 0) {
    if (depth > 32 || clang::containsObjC3BorrowedType(context_, type) ||
        clang::getObjC3ResourceTypeContract(context_, type).Cleanup)
      return false;
    if (PortableValueType(context_, type) || IsPortableVector(context_, type)) return true;
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
    if (const auto *enumeration = type->getAs<clang::EnumType>())
      return AdmitEnum(enumeration->getDecl()->getCanonicalDecl());
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
      CExportSpellingType(context_, field->getType()).print(out, policy_, field->getName());
      out << ";\n";
    }

    const auto &layout = context_.getASTRecordLayout(definition);
    const auto size = std::to_string(layout.getSize().getQuantity());
    const auto alignment = std::to_string(layout.getAlignment().getQuantity());
    declarations_ += tag + " {\n" + fields + "};\n";
    declarations_ += "static_assert(sizeof(" + tag + ") == " + size +
        " && __alignof__(" + tag + ") == " + alignment + ", \"C export aggregate layout\");\n";
    unsigned index = 0;
    for (const auto *field : definition->fields()) {
      const auto offset = layout.getFieldOffset(index++) / context_.getCharWidth();
      declarations_ += "static_assert(offsetof(" + tag + ", " + field->getName().str() +
          ") == " + std::to_string(offset) + ", \"C export aggregate field offset\");\n";
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
  llvm::SmallPtrSet<const clang::EnumDecl *, 8> enums_;
};

std::optional<CExportType> ExportType(clang::ASTContext &context,
                                     clang::QualType type, llvm::StringRef alias,
                                     llvm::raw_ostream &declarations,
                                     CExportValues &values,
                                     bool nullable_result) {
  if (auto value = PortableValueType(context, type, nullable_result))
    return CExportType{std::move(*value)};
  if (clang::containsObjC3BorrowedType(context, type) ||
      clang::getObjC3ResourceTypeContract(context, type).Cleanup ||
      type.hasAddressSpace())
    return std::nullopt;

  const auto canonical = type.getCanonicalType();
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.Bool = true;
  policy.SuppressTagKeyword = false;
  policy.SuppressLifetimeQualifiers = true;
  if (IsPortableVector(context, type)) {
    declarations << "typedef ";
    CExportSpellingType(context, type).print(declarations, policy, alias);
    declarations << ";\n";
    return CExportType{alias.str()};
  }
  if (canonical->isFunctionPointerType()) {
    const auto *signature = type->getPointeeType()->getAs<clang::FunctionProtoType>();
    if (!values.SupportsCallback(signature) ||
        signature->getExtInfo() != clang::FunctionType::ExtInfo(clang::CC_C) ||
        signature->getExceptionSpecType() != clang::EST_None)
      return std::nullopt;
    declarations << "typedef ";
    CExportSpellingType(context, type, nullable_result).print(declarations, policy, alias);
    declarations << ";\n";
    return CExportType{alias.str()};
  }
  const char *kind;
  if (const auto *object = canonical->getAs<clang::ObjCObjectPointerType>()) {
    if (object->isSpecialized()) return std::nullopt;
    kind = "OBJECT";
    declarations << "#ifdef __OBJC__\n";
    if (const auto *interface = object->getInterfaceDecl())
      declarations << "@class " << interface->getName() << ";\n";
    for (const auto *protocol : object->quals())
      declarations << "@protocol " << protocol->getName() << ";\n";
  } else if (const auto *block = canonical->getAs<clang::BlockPointerType>()) {
    if (!values.SupportsCallback(block->getPointeeType()->getAs<clang::FunctionProtoType>()))
      return std::nullopt;
    kind = "BLOCK";
    declarations << "#ifdef __BLOCKS__\n";
  } else {
    if (auto value = values.Render(type, nullable_result))
      return CExportType{std::move(*value)};
    return std::nullopt;
  }

  // Ownership belongs to the bridge. Its aliases must also parse in C
  // consumers using Blocks, where ARC qualifiers are unavailable.
  type = CExportSpellingType(context, type, nullable_result);
  declarations << "typedef ";
  type.print(declarations, policy, alias);
  declarations << ";\n#else\ntypedef void *";
  if (const auto nullability = type->getNullability())
    declarations << ' ' << clang::getNullabilitySpelling(*nullability);
  declarations << ' ' << alias << ";\n#endif\n";
  return CExportType{alias.str(), kind};
}

std::string RetainedInput(llvm::StringRef type, llvm::StringRef argument, bool block) {
  return "(" + type.str() + ")__objc3_c_export_retain((void *)" + argument.str() +
      (block ? ", 1)" : ", 0)");
}

std::optional<std::string> NativeInput(
    const clang::ParmVarDecl *parameter, clang::FunctionProtoType::ExtParameterInfo attributes,
    llvm::StringRef type, llvm::StringRef argument, bool arc) {
  const bool consumed = attributes.isConsumed() || parameter->hasAttr<clang::NSConsumedAttr>();
  const auto native_type = parameter->getType();
  const bool block = native_type->isBlockPointerType();
  if (attributes.withIsConsumed(false).getOpaqueValue() || parameter->hasAttr<clang::CFConsumedAttr>() ||
      (consumed && !native_type->isObjCObjectPointerType() && !block))
    return std::nullopt;
  return consumed && !arc ? RetainedInput(type, argument, block) : argument.str();
}

std::string SwiftScalar(clang::QualType type, llvm::StringRef module) {
  // C admission already checked that this is a named, fixed-underlying enum
  // and published its exact tag in the companion header.
  if (const auto *enumeration = type->getAs<clang::EnumType>())
    return "`" + module.str() + "`.`" + enumeration->getDecl()->getName().str() + "`";
  const auto *builtin = type.getCanonicalType()->getAs<clang::BuiltinType>();
  if (!builtin) return {};
  switch (builtin->getKind()) {
  case clang::BuiltinType::Void: return "Void";
  case clang::BuiltinType::Bool: return "Bool";
  case clang::BuiltinType::Char_S:
  case clang::BuiltinType::Char_U: return "CChar";
  case clang::BuiltinType::SChar: return "Int8";
  case clang::BuiltinType::UChar: return "UInt8";
  case clang::BuiltinType::Short: return "CShort";
  case clang::BuiltinType::UShort: return "CUnsignedShort";
  case clang::BuiltinType::Int: return "CInt";
  case clang::BuiltinType::UInt: return "CUnsignedInt";
  case clang::BuiltinType::Long: return "CLong";
  case clang::BuiltinType::ULong: return "CUnsignedLong";
  case clang::BuiltinType::LongLong: return "CLongLong";
  case clang::BuiltinType::ULongLong: return "CUnsignedLongLong";
  case clang::BuiltinType::Float: return "Float";
  case clang::BuiltinType::Double: return "Double";
  default: return {};
  }
}

std::string SwiftValue(clang::ASTContext &context, clang::QualType type, llvm::StringRef module,
                       llvm::StringRef c_alias) {
  if (type.isVolatileQualified()) return {};
  if (auto scalar = SwiftScalar(type, module); !scalar.empty()) return scalar;
  auto is_simd = [&](clang::QualType element) {
    const auto *vector = element->getAs<clang::VectorType>();
    return vector && vector->getNumElements() >= 2 && IsPortableVector(context, element) &&
           !SwiftScalar(vector->getElementType(), module).empty();
  };
  // Refer to the imported typedef so the C module owns the exact Swift SIMD
  // element spelling, even for platform-specific C integer typedefs.
  if (is_simd(type)) return "`" + module.str() + "`.`" + c_alias.str() + "`";
  const auto *record_type = type->getAs<clang::RecordType>();
  const auto *record = record_type ? record_type->getDecl()->getDefinition() : nullptr;
  // The C boundary has already verified field layout and trivial transport.
  // A Swift continuation also needs the complete by-value graph to be Sendable.
  llvm::SmallPtrSet<const clang::RecordDecl *, 16> visited;
  auto can_send = [&](auto &&walk, const clang::RecordDecl *definition) -> bool {
    if (!definition || !definition->isStruct()) return false;
    if (!visited.insert(definition).second) return true;
    for (const auto *field : definition->fields()) {
      auto element = field->getType();
      while (const auto *array = context.getAsConstantArrayType(element))
        element = array->getElementType();
      if (element.isVolatileQualified()) return false;
      if (!SwiftScalar(element, module).empty() || is_simd(element)) continue;
      const auto *nested = element->getAs<clang::RecordType>();
      if (!nested || !walk(walk, nested->getDecl()->getDefinition())) return false;
    }
    return true;
  };
  if (!can_send(can_send, record)) return {};
  return "`" + module.str() + "`.`" + record->getName().str() + "`";
}

void WriteSwiftSupport(llvm::raw_ostream &out, llvm::StringRef module) {
  out << "import `" << module << "`\nimport Synchronization\n\n";
  out << "public final class " << module << "NativeError: Error, @unchecked Sendable {\n";
  out << R"swift(    private let pointer: UnsafeMutableRawPointer

    fileprivate init(_ error: UnsafeMutableRawPointer) {
        self.pointer = objc3_async_c_error_retain(error)!
    }

    deinit { objc3_async_c_error_release(pointer) }

    public func withBorrowedNativeError<T>(_ body: (UnsafeMutableRawPointer) throws -> T) rethrows -> T {
        try withExtendedLifetime(self) { try body(pointer) }
    }
}

private enum ObjC3CompletionContractError: Error { case invalidCompletion }

private final class ObjC3NativeTask: @unchecked Sendable {
    let handle: OpaquePointer

    init(_ handle: OpaquePointer) { self.handle = handle }
    deinit { objc3_task_release(handle) }

    func cancel() { objc3_task_request_cancel(handle) }
}

private final class ObjC3Cancellation: Sendable {
    private struct State {
        var task: ObjC3NativeTask?
        var requested = false
        var finished = false
    }

    private let state = Mutex(State())

    func install(_ handle: OpaquePointer?) {
        guard let handle else { return }
        let task = ObjC3NativeTask(handle)
        let requested = state.withLock { state in
            guard !state.finished else { return false }
            state.task = task
            return state.requested
        }
        if requested { task.cancel() }
    }

    func cancel() {
        let task = state.withLock { state in
            state.requested = true
            return state.task
        }
        // Dispatch failure can invoke the callback synchronously.
        task?.cancel()
    }

    func finish() {
        let task = state.withLock { state in
            state.finished = true
            let task = state.task
            state.task = nil
            return task
        }
        withExtendedLifetime(task) {}
    }
}

private final class ObjC3Pending<Value: Sendable>: Sendable {
    let continuation: CheckedContinuation<Value, any Error>
    let cancellation: ObjC3Cancellation

    init(_ continuation: CheckedContinuation<Value, any Error>, _ cancellation: ObjC3Cancellation) {
        self.continuation = continuation
        self.cancellation = cancellation
    }
}

)swift";
  out << "public enum " << module << "Native {\n";
}

bool WriteSwiftEntry(clang::ASTContext &context, llvm::raw_ostream &out, llvm::StringRef module,
                     llvm::StringRef name, llvm::StringRef symbol, llvm::StringRef type_prefix,
                     clang::QualType result, llvm::ArrayRef<clang::ParmVarDecl *> parameters,
                     bool is_async, bool actor, std::string &failure) {
  const auto result_type = SwiftValue(context, result, module, (type_prefix + "result_t").str());
  if (result_type.empty()) {
    failure = "Swift overlay requires a portable scalar, SIMD vector, pointer-free struct or void result";
    return false;
  }
  const bool is_void = result->isVoidType();
  std::string declaration, arguments;
  llvm::raw_string_ostream signature(declaration), call(arguments);
  if (actor) {
    signature << "_ receiver: UnsafeMutableRawPointer";
    call << "receiver, ";
  }
  for (unsigned index = 0; index < parameters.size(); ++index) {
    const auto type = SwiftValue(context, parameters[index]->getType(), module,
                                 (type_prefix + "arg" + llvm::Twine(index) + "_t").str());
    if (type.empty()) {
      failure = "Swift overlay has no portable scalar, SIMD vector or pointer-free struct type for parameter " +
                std::to_string(index + 1);
      return false;
    }
    if (index || actor) signature << ", ";
    signature << "_ argument" << index << ": " << type;
    call << "argument" << index << ", ";
  }
  out << "    public static " << (is_async ? "nonisolated(nonsending) " : "")
      << "func `" << name << "`(" << declaration << ") "
      << (is_async ? "async " : "") << "throws -> " << result_type << " {\n";
  if (!is_async) {
    out << R"swift(        let pool = objc3_c_autorelease_pool_push()
        defer { objc3_c_autorelease_pool_pop(pool) }
        var error: UnsafeMutableRawPointer?
)swift";
    out << "        " << (is_void ? "" : "let value = ") << '`' << module << "`." << symbol << '('
        << arguments << "&error)\n"
        << "        if let error { throw " << module << "NativeError(error) }\n"
        << "        return " << (is_void ? "()" : "value") << "\n    }\n\n";
    return true;
  }
  out << R"swift(        let cancellation = ObjC3Cancellation()
        return try await withTaskCancellationHandler {
)swift";
  out << "            try await withCheckedThrowingContinuation { (continuation: CheckedContinuation<"
      << result_type << ", any Error>) in\n"
      << "                let context = Unmanaged.passRetained(ObjC3Pending(continuation, cancellation)).toOpaque()\n";
  out << "                let task = `" << module << "`." << symbol << '(' << arguments << "context) { context, status, "
      << (is_void ? "" : "value, ") << "error in\n";
  out << "                    guard let context else { preconditionFailure(\"missing native completion context\") }\n"
      << "                    let pending = Unmanaged<ObjC3Pending<" << result_type
      << ">>.fromOpaque(context).takeRetainedValue()\n";
  out << R"swift(                    pending.cancellation.finish()
                    switch status {
                    case OBJC_ASYNC_COMPLETION_SUCCESS where error == nil:
)swift";
  out << "                        pending.continuation.resume(returning: " << (is_void ? "()" : "value") << ")\n";
  out << R"swift(                    case OBJC_ASYNC_COMPLETION_CANCELLED where objc3_async_c_error_is_cancellation(error) != 0:
                        pending.continuation.resume(throwing: CancellationError())
                    case OBJC_ASYNC_COMPLETION_ERROR:
                        if let error, objc3_async_c_error_is_cancellation(error) == 0 {
)swift";
  out << "                            pending.continuation.resume(throwing: " << module << "NativeError(error))\n";
  out << R"swift(                        } else {
                            pending.continuation.resume(throwing: ObjC3CompletionContractError.invalidCompletion)
                        }
                    default:
                        pending.continuation.resume(throwing: ObjC3CompletionContractError.invalidCompletion)
                    }
                }
                cancellation.install(task)
            }
        } onCancel: {
            cancellation.cancel()
        }
    }

)swift";
  return true;
}

}  // namespace

CExportSource GenerateCExports(clang::ASTContext &context,
                               std::span<const std::string> async_symbols,
                               std::span<const std::string> throws_symbols,
                               std::string &failure, std::string_view swift_module_name) {
  failure.clear();
  CExportSource output;
  auto &language = context.getLangOpts();
  if (!language.ObjC3 || !context.getTargetInfo().getTriple().isOSLinux() ||
      (!async_symbols.empty() && !language.Blocks)) {
    failure = "C exports require Linux Objective-C 3.0, with Blocks enabled for async entries";
    return {};
  }

  std::string entries;
  llvm::raw_string_ostream header(entries), definitions(output.definitions);
  const llvm::StringRef swift_module(swift_module_name);
  llvm::raw_string_ostream swift(output.swift_overlay);
  if (!swift_module.empty()) {
    if ((async_symbols.empty() && throws_symbols.empty()) || !IsSourceIdentifier(swift_module)) {
      failure = "Swift overlay requires selected native entries and a C module identifier";
      return {};
    }
    WriteSwiftSupport(swift, swift_module);
  }
  definitions << "\n\n#include <stdbool.h>\n";
  llvm::StringSet<> selected, swift_names;
  auto export_swift = [&](llvm::StringRef selection, llvm::StringRef name, llvm::StringRef symbol,
                          llvm::StringRef type_prefix,
                          clang::QualType result, llvm::ArrayRef<clang::ParmVarDecl *> parameters,
                          bool is_async, bool actor) {
    if (swift_module.empty()) return true;
    if (!swift_names.insert(name).second) {
      failure = "Duplicate Swift overlay entry " + name.str();
      return false;
    }
    if (WriteSwiftEntry(context, swift, swift_module, name, symbol, type_prefix, result, parameters,
                        is_async, actor, failure))
      return true;
    failure = "Cannot generate Swift overlay for " + selection.str() + ": " + failure;
    return false;
  };
  auto *tu = context.getTranslationUnitDecl();
  CExportValues values(context, output.interface_exports);
  auto export_type = [&](clang::QualType type, const std::string &alias,
                         llvm::raw_ostream &declarations, bool nullable_result = false) {
    auto emitted = ExportType(context, type, alias, declarations, values, nullable_result);
    if (emitted && emitted->spelling == alias) {
      if (!tu->lookup(&context.Idents.get(alias)).empty())
        return std::optional<CExportType>{};
      output.interface_exports.push_back(alias);
    }
    return emitted;
  };
  for (const auto &selection : throws_symbols) {
    auto reject = [&](llvm::StringRef reason) {
      failure = "Cannot export C throwing entry " + selection + ": " + reason.str();
      return CExportSource{};
    };
    const auto selected_function = SelectFreeFunction(context, selection);
    const auto *function = selected_function ? selected_function->function : nullptr;
    const auto *prototype = function ? function->getType()->getAs<clang::FunctionProtoType>() : nullptr;
    if (!prototype || prototype->isVariadic())
      return reject("expected one nonvariadic free function with a native prototype");
    const auto effects = prototype->getObjC3FunctionEffects();
    if (effects.Async || effects.Throws == clang::ObjC3ThrowsKind::None ||
        clang::getObjC3Executor(function) || prototype->hasObjC3BorrowedResult() ||
        prototype->getCallConv() != clang::CC_C || prototype->getExtInfo().getProducesResult() ||
        function->hasAttr<clang::CFReturnsRetainedAttr>() ||
        function->hasAttr<clang::CFReturnsNotRetainedAttr>())
      return reject("expected a synchronous throwing entry with a portable result lifetime and C calling convention");
    if (!function->isExternallyVisible() && !function->getDefinition())
      return reject("the internal throwing declaration needs a definition in this translation unit");
    if (language.CPlusPlus && !prototype->isNothrow() &&
        !function->hasAttr<clang::NoThrowAttr>())
      return reject("Objective-C++ entries must declare a nonthrowing native exception contract");
    const auto &name = selected_function->symbol;
    const auto thunk = name + "_throws_c";
    const auto bridge = "__objc3_c_error_bridge_" + name;
    if (!selected.insert(thunk).second || !tu->lookup(&context.Idents.get(thunk)).empty() ||
        !tu->lookup(&context.Idents.get(bridge)).empty())
      return reject("the generated C symbol or native bridge is already declared or selected");
    std::string types;
    llvm::raw_string_ostream declarations(types);
    const auto prefix = thunk + '_';
    const auto result = export_type(prototype->getReturnType(), prefix + "result_t", declarations, true);
    if (!result) return reject("the result has no self-contained C value declaration");
    std::string parameters, inputs;
    for (unsigned index = 0; index < prototype->getNumParams(); ++index) {
      const auto type = export_type(function->getParamDecl(index)->getType(),
                                    prefix + "arg" + std::to_string(index) + "_t", declarations);
      const auto *parameter = function->getParamDecl(index);
      const auto argument = "__objc3_c_argument_" + std::to_string(index);
      auto input = type ? NativeInput(parameter, prototype->getExtParameterInfo(index),
                                     type->spelling, argument, language.ObjCAutoRefCount) : std::nullopt;
      if (!input)
        return reject("parameter " + std::to_string(index + 1) +
                      " has no self-contained C value declaration");
      if (index) { parameters += ", "; inputs += ", "; }
      parameters += type->spelling + ' ' + argument;
      inputs += *input;
    }
    constexpr llvm::StringLiteral linkage = "__OBJC3_ASYNC_C_LINKAGE ";
    const auto declaration = linkage.str() + result->spelling + ' ' + thunk + '(' +
        (parameters.empty() ? "" : parameters + ", ") + "objc3_async_c_error _Nullable * _Nullable);\n";
    header << types << declaration;
    output.interface_exports.push_back(thunk);
    for (const auto identifier : selected_function->identifiers)
      definitions << "#undef " << identifier << '\n';
    definitions << "#undef " << thunk << "\n#undef " << bridge << '\n'
                << types << declaration;
    const auto native = linkage.str() + result->spelling + ' ' + bridge + '(' +
        (parameters.empty() ? "void" : parameters) + ')';
    // The native checked entry implements exactly the trailing error-out
    // parameter in the public C declaration, including null and +0 ownership.
    definitions << native;
    PrintThrows(definitions, context, effects);
    definitions << " __asm__(\"" << thunk << "\");\n" << native;
    PrintThrows(definitions, context, effects);
    definitions << " { ";
    if (!prototype->getReturnType()->isVoidType()) definitions << "return ";
    definitions << "__objc3_try(" << (language.CPlusPlus ? "::" : "") << selection
                << '(' << inputs << ")); }\n";
    if (!export_swift(selection, name, thunk, prefix, prototype->getReturnType(), function->parameters(), false, false))
      return {};
  }
  for (const auto &selection : async_symbols) {
    auto reject = [&](llvm::StringRef reason) {
      failure = "Cannot export C async entry " + selection + ": " + reason.str();
      return CExportSource{};
    };
    std::optional<CExportActorMethod> actor_method;
    const clang::Decl *source = nullptr;
    const clang::FunctionProtoType *prototype = nullptr;
    clang::QualType result_type;
    llvm::ArrayRef<clang::ParmVarDecl *> source_parameters;
    clang::ObjC3FunctionEffectInfo effects;
    std::string name;
    const auto selected_function = SelectFreeFunction(context, selection);
    if (!selected_function && llvm::StringRef(selection).contains("::")) {
      actor_method = SelectActorMethod(context, selection);
      if (!actor_method)
        return reject("expected one qualified free function or an isolated instance selector in the named actor class hierarchy");
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
      if (!selected_function) return reject("expected one nonvariadic free function with a native prototype");
      name = selected_function->symbol;
      const auto *function = selected_function->function;
      prototype = function ? function->getType()->getAs<clang::FunctionProtoType>() : nullptr;
      if (!prototype || prototype->isVariadic())
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
    const auto thunk = name + "_async_c";
    const auto cancellable = thunk + "_task";
    const auto callback = name + "_completion_t";
    const auto submit = "__objc3_c_submit_" + name;
    for (const auto &symbol : {thunk, cancellable, callback, submit}) {
      if (!selected.insert(symbol).second || !tu->lookup(&context.Idents.get(symbol)).empty())
        return reject("the generated name " + symbol + " is already declared or selected");
    }

    std::string types;
    llvm::raw_string_ostream declarations_text(types);
    const auto type_prefix = name + "_async_c_";
    auto result = export_type(result_type, type_prefix + "result_t", declarations_text, true);
    if (!result)
      return reject("the result has no self-contained C value declaration");
    std::string parameters, inputs, native_inputs;
    bool owning_bridge = actor_method.has_value();
    constexpr llvm::StringLiteral receiver = "__objc3_c_receiver";
    constexpr llvm::StringLiteral opaque_receiver_type = "void * _Nonnull ";
    if (actor_method) {
      // The C signature can be imported without exporting the actor's class
      // API. The private frame alone needs its typed receiver.
      parameters = ", " + opaque_receiver_type.str() + receiver.str();
      inputs = receiver.str();
    }
    std::string message = "[" + receiver.str();
    if (actor_method && actor_method->method->hasAttr<clang::NSConsumesSelfAttr>() &&
        !language.ObjCAutoRefCount)
      message = '[' + RetainedInput(actor_method->actor->getName().str() + " *", receiver, false);
    for (unsigned index = 0; index < source_parameters.size(); ++index) {
      const auto *parameter = source_parameters[index];
      const auto parameter_type = parameter->getType();
      auto type = export_type(parameter_type,
                              type_prefix + "arg" + std::to_string(index) + "_t", declarations_text);
      const auto argument = "__objc3_c_argument_" + std::to_string(index);
      const auto attributes = prototype ? prototype->getExtParameterInfo(index)
                                        : clang::FunctionProtoType::ExtParameterInfo{};
      auto input = type ? NativeInput(parameter, attributes, type->spelling, argument,
                                     language.ObjCAutoRefCount) : std::nullopt;
      if (!input)
        return reject("parameter " + std::to_string(index + 1) +
                      " has no self-contained C value declaration");
      owning_bridge |= parameter_type->isObjCRetainableType();
      parameters += ", " + type->spelling + " " + argument;
      if (!inputs.empty()) inputs += ", ";
      inputs += argument;
      if (actor_method) {
        message += " " + actor_method->method->getSelector().getNameForSlot(index).str() + ':' + *input;
      } else {
        if (!native_inputs.empty()) native_inputs += ", ";
        native_inputs += *input;
      }
    }

    const bool returns_void = result_type->isVoidType();
    const auto declaration = (returns_void
        ? "OBJC3_ASYNC_C_DECLARE_VOID(" + name
        : "OBJC3_ASYNC_C_DECLARE(" + name + ", " + result->spelling) + parameters + ");\n";
    const auto c_parameters = (parameters.empty() ? "" : parameters.substr(2) + ", ") +
        "void * _Nullable __objc3_context, " + callback + " _Nonnull __objc3_completion";
    const auto task_entry = "__OBJC3_ASYNC_C_LINKAGE objc3_task_t * _Nullable " +
        cancellable + '(' + c_parameters + ')';
    header << types << declaration << task_entry << ";\n";
    output.interface_exports.push_back(thunk);
    output.interface_exports.push_back(cancellable);
    output.interface_exports.push_back(callback);
    if (actor_method && source_parameters.empty())
      message += " " + actor_method->method->getSelector().getAsString();
    const auto call = actor_method ? message + ']' :
        (language.CPlusPlus ? "::" : "") + selection + '(' + native_inputs + ')';
    auto frame = call;
    // The unqualified C preprocessing spelling must select the original
    // declaration even when the provider reuses a name as a macro later.
    if (actor_method) {
      definitions << "#undef " << actor_method->actor->getName() << '\n';
      const auto selector = actor_method->method->getSelector();
      for (unsigned slot = 0; slot < std::max(1u, selector.getNumArgs()); ++slot)
        definitions << "#undef " << selector.getNameForSlot(slot) << '\n';
    } else {
      for (const auto identifier : selected_function->identifiers)
        definitions << "#undef " << identifier << '\n';
    }
    definitions << "#undef " << name << "\n#undef " << thunk << "\n#undef "
                << cancellable << "\n#undef " << submit << "\n#undef " << callback
                << "\n" << types << declaration;
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
      if (checked) PrintThrows(definitions, context, effects);
      definitions << " { ";
      if (!returns_void) definitions << "return ";
      if (checked) definitions << "__objc3_try(";
      definitions << "__objc3_await " << call;
      if (checked) definitions << ')';
      definitions << (owning_bridge ? "; })(); }\n" : "; }\n");
      frame = bridge + '(' + bridge_inputs + ')';
    }
    definitions << "static void " << submit << "(objc3_task_t **__objc3_task_out, "
                << c_parameters << ") { OBJC3_ASYNC_C_START_"
                << (returns_void ? "VOID" : result->kind) << '(';
    if (!returns_void) definitions << result->spelling << ", ";
    definitions << frame << ", "
                << (effects.Throws == clang::ObjC3ThrowsKind::None ? 0 : 1)
                << ", 0, 0, __objc3_task_out); }\n";
    const auto forwarded = (inputs.empty() ? "" : inputs + ", ") +
        "__objc3_context, __objc3_completion";
    definitions << "__OBJC3_ASYNC_C_LINKAGE void " << thunk << '(' << c_parameters
                << ") { " << submit << "(0, " << forwarded << "); }\n"
                << task_entry << " { objc3_task_t *__objc3_task = 0; " << submit
                << "(&__objc3_task, " << forwarded << "); return __objc3_task; }\n";
    if (!export_swift(selection, name, cancellable, type_prefix, result_type, source_parameters,
                      true, actor_method.has_value()))
      return {};
  }
  if (!swift_module.empty()) swift << "}\n";
  output.header = "#pragma once\n#include <stdbool.h>\n"
                  "#include <objc3c/runtime/objc3_async_c.h>\n\n";
  if (!values.declarations().empty()) {
    output.header += "#include <assert.h>\n#include <stddef.h>\n\n";
    output.header += values.declarations();
  }
  output.header += entries;
  return output;
}

}  // namespace objc3::clang_foundation
