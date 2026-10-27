#include "clang_foundation/runtime_interface_types.h"
#include "clang_foundation/interface_codec.h"
#include "runtime_interface_header.h"

#include <array>
#include <cstdint>
#include <iterator>

#include "clang/AST/ASTContext.h"
#include "clang/AST/CanonicalType.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Module.h"
#include "clang/Basic/TargetInfo.h"
#include "llvm/ADT/STLExtras.h"

namespace objc3::clang_foundation {
namespace {

bool DeclarationHeaderMatches(const clang::NamedDecl *declaration,
                              const clang::ASTContext &context,
                              llvm::StringRef expected) {
  if (!declaration) return false;
  const auto &source=context.getSourceManager();
  const auto location=source.getSpellingLoc(declaration->getLocation());
  if (location.isInvalid()) return false;
  bool invalid=false;
  const auto bytes=source.getBufferData(source.getFileID(location),&invalid);
  return !invalid && ComputeInterfaceSourceSha256(bytes)==expected;
}

bool HeaderMatches(const clang::NamedDecl *declaration,
                   const clang::ASTContext &context) {
  if (!declaration ||
      !declaration->getDeclContext()->getRedeclContext()->isTranslationUnit())
    return false;
  const auto &source = context.getSourceManager();
  auto location = source.getSpellingLoc(declaration->getLocation());
  if (location.isInvalid())
    return false;
  bool invalid = false;
  auto bytes = source.getBufferData(source.getFileID(location), &invalid);
  return !invalid && ComputeInterfaceSourceSha256(bytes) ==
                         runtime_interface::kHeaderSha256;
}

bool ServiceContractMatches(const clang::ASTContext &context) {
  bool frame_contract=false;
  const auto *tu=context.getTranslationUnitDecl();
  for (const auto *frame:tu->lookup(&context.Idents.get("objc3_async_frame")))
    frame_contract |= IsQualifiedRuntimeFrameType(frame,context);
  if (!frame_contract) return false;
  for (const char *name:{"objc3_task","objc3_executor","objc3_error",
                         "objc3_task_group","objc3_actor"})
    for (const auto *declaration:tu->lookup(&context.Idents.get(name)))
      if (const auto *tag=llvm::dyn_cast<clang::TagDecl>(declaration))
        if (const auto *definition=tag->getDefinition())
          if (!DeclarationHeaderMatches(definition,context,runtime_interface::kHeaderSha256))
            return false;
  return true;
}

template <size_t N>
bool RecordMatches(const clang::RecordDecl *record,
                   const clang::ASTContext &context, int64_t size,
                   const std::array<const char *, N> &names,
                   const std::array<uint64_t, N> &offsets) {
  if (!record || !record->isCompleteDefinition() || record->isUnion() ||
      !HeaderMatches(record, context) ||
      static_cast<size_t>(
          std::distance(record->field_begin(), record->field_end())) != N)
    return false;
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record)) {
    if (!cxx->isTriviallyCopyable() || !cxx->isStandardLayout() ||
        cxx->getNumBases())
      return false;
    for (const auto *method : cxx->methods())
      if (!method->isImplicit())
        return false;
  }
  const auto &layout = context.getASTRecordLayout(record);
  if (layout.getSize().getQuantity() != size ||
      layout.getAlignment().getQuantity() != 8)
    return false;
  size_t index = 0;
  for (const auto *field : record->fields()) {
    if (field->isBitField() || field->getName() != names[index] ||
        layout.getFieldOffset(index) != offsets[index])
      return false;
    ++index;
  }
  return true;
}

bool UnsignedWidth(clang::QualType type, unsigned width,
                   const clang::ASTContext &context) {
  return !type->isAtomicType() && type->isUnsignedIntegerType() &&
         context.getTypeSize(type) == width;
}

const clang::RecordDecl *RecordNamed(clang::QualType type,
                                     llvm::StringRef name) {
  const auto *record_type = type->getAs<clang::RecordType>();
  if (!record_type ||
      record_type->getDecl()->getQualifiedNameAsString() != name)
    return nullptr;
  return record_type->getDecl()->getDefinition();
}

bool ResultDescriptorMatches(clang::QualType type,
                             const clang::ASTContext &context) {
  auto *record = RecordNamed(type, "objc3_async_result_type");
  if (!RecordMatches<9>(record, context, 56,
                        {"struct_size", "version", "storage_size",
                         "storage_alignment", "kind", "reserved", "identity",
                         "identity_size", "identity_domain"},
                        {0, 32, 64, 128, 192, 224, 256, 320, 384}))
    return false;
  std::array<clang::QualType, 9> fields;
  size_t index = 0;
  for (const auto *field : record->fields())
    fields[index++] = field->getType();
  const auto *kind = fields[4]->getAs<clang::EnumType>();
  return UnsignedWidth(fields[0], 32, context) &&
         UnsignedWidth(fields[1], 32, context) &&
         UnsignedWidth(fields[2], 64, context) &&
         UnsignedWidth(fields[3], 64, context) && kind &&
         kind->getDecl()->getQualifiedNameAsString() ==
             "objc3_async_result_kind" &&
         context.getTypeSize(fields[4]) == 32 &&
         UnsignedWidth(fields[5], 32, context) &&
         context.hasSameType(
             fields[6], context.getPointerType(context.CharTy.withConst())) &&
         UnsignedWidth(fields[7], 64, context) &&
         context.hasSameType(
             fields[8], context.getPointerType(context.VoidTy.withConst()));
}

} // namespace

bool IsQualifiedStandardLibraryImport(const clang::ImportDecl *declaration,
                                      const clang::ASTContext &context) {
  const auto *module = declaration ? declaration->getImportedModule() : nullptr;
  if (!module || !module->IsSystem)
    return false;
  const auto name = module->getFullModuleName();
  const auto &source = context.getSourceManager();
  for (const auto &contract : runtime_interface::kStandardLibraryHeaders) {
    if (name != contract.module)
      continue;
    // Canonical submodules each own exactly one public header. Do not admit a
    // module which adds, excludes, or redirects headers under a trusted name.
    const auto headers = module->getAllHeaders();
    if (headers.size() != 1 ||
        module->getHeaders(clang::Module::HK_Normal).size() != 1)
      return false;
    const auto file = source.translateFile(headers.front().Entry);
    if (file.isInvalid())
      return false;
    const auto bytes = source.getBufferDataOrNone(file);
    return bytes && ComputeInterfaceSourceSha256(*bytes) == contract.sha256;
  }
  return false;
}

llvm::StringRef RuntimeInterfaceSemantic() {
  return runtime_interface::kSemantic;
}

llvm::StringRef RuntimeInterfaceHeaderSource() {
  return runtime_interface::kHeaderSource;
}

llvm::StringRef RuntimeTaskHeaderSource() {
  return runtime_interface::kTaskHeaderSource;
}

bool IsQualifiedRuntimeTaskHeader(const clang::ASTContext &context) {
  if (!ServiceContractMatches(context)) return false;
  const auto &source = context.getSourceManager();
  for (const auto *declaration : context.getTranslationUnitDecl()->lookup(
           &context.Idents.get("__objc3_async_task"))) {
    if (!llvm::isa<clang::TypeAliasTemplateDecl>(declaration))
      continue;
    const auto location = source.getSpellingLoc(declaration->getLocation());
    if (location.isInvalid())
      continue;
    bool invalid = false;
    const auto bytes = source.getBufferData(source.getFileID(location), &invalid);
    if (!invalid && ComputeInterfaceSourceSha256(bytes) ==
                        runtime_interface::kTaskHeaderSha256)
      return true;
  }
  return false;
}

bool IsQualifiedRuntimeServiceDeclaration(const clang::NamedDecl *declaration,
                                          const clang::ASTContext &context) {
  if (!declaration) return false;
  const auto name=declaration->getQualifiedNameAsString();
  // These public C-header dependencies carry only a target builtin type. The
  // canonical reparse uses the compiler's corresponding integer/size typedefs;
  // ownership attributes or a different representation are never equivalent.
  if (const auto *alias=llvm::dyn_cast<clang::TypedefNameDecl>(declaration);
      alias && !alias->hasAttrs()) {
    clang::QualType expected;
    if (name=="size_t") expected=context.getSizeType();
    else if (name=="uint32_t") expected=context.UnsignedIntTy;
    else if (name=="uint64_t") expected=context.UnsignedLongTy;
    if (!expected.isNull())
      return context.hasSameType(alias->getUnderlyingType(),expected) &&
             ServiceContractMatches(context);
  }
  if (!llvm::StringRef(name).starts_with("objc3_") &&
      !llvm::StringRef(name).starts_with("OBJC3_")) return false;
  if (!context.getTargetInfo().getTriple().isOSLinux() ||
      context.getTargetInfo().getTriple().getArch()!=llvm::Triple::x86_64 ||
      !DeclarationHeaderMatches(declaration,context,runtime_interface::kHeaderSha256))
    return false;
  if (!ServiceContractMatches(context)) return false;
  // Opaque service types cannot acquire an application-provided definition.
  if (const auto *tag=llvm::dyn_cast<clang::TagDecl>(declaration))
    if (const auto *definition=tag->getDefinition())
      if (!DeclarationHeaderMatches(definition,context,runtime_interface::kHeaderSha256))
        return false;
  return true;
}

bool IsQualifiedRuntimeTaskDeclaration(const clang::NamedDecl *declaration,
                                       const clang::ASTContext &context) {
  if (!context.getLangOpts().CPlusPlus || !declaration)
    return false;
  const std::string qualified_name=declaration->getQualifiedNameAsString();
  llvm::StringRef name=qualified_name;
  if (!name.consume_front("objc3::") || name.contains("::promise_type"))
    return false;
  if (!DeclarationHeaderMatches(declaration,context,runtime_interface::kTaskHeaderSha256) ||
      !IsQualifiedRuntimeTaskHeader(context)) return false;
  for (const clang::DeclContext *scope=declaration->getDeclContext();scope;
       scope=scope->getParent())
    if (const auto *tag=llvm::dyn_cast<clang::TagDecl>(scope))
      if (const auto *definition=tag->getDefinition())
        if (!DeclarationHeaderMatches(definition,context,runtime_interface::kTaskHeaderSha256))
          return false;
  // Public task aliases retain this SDK-owned sugar in instantiated result
  // expressions. Admit the alias, not the private traits record or its members.
  if (llvm::isa<clang::TypedefNameDecl>(declaration) &&
      declaration->getName()=="abi_type") {
    const auto *owner=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(
        declaration->getDeclContext());
    if (owner && owner->getSpecializedTemplate()->getQualifiedNameAsString()==
                     "objc3::task_detail::result_traits" &&
        DeclarationHeaderMatches(owner,context,runtime_interface::kTaskHeaderSha256))
      return true;
  }
  const auto root=name.take_until([](char c) { return c=='<' || c==':'; });
  static constexpr const char *public_names[]={
      "basic_task", "basic_task_handle", "task", "task_handle",
      "task_group_handle", "task_group_next_request", "task_group_cleanup_request",
      "task_destination", "hop_to", "task_checkpoint", "task_checkpoint_t",
      "task_yield", "task_yield_t", "service_failure", "task_cancelled", "task_error",
      "join_task", "finish_task_group"};
  return llvm::is_contained(public_names,root);
}

void AddRuntimeInterfaceFiles(
    std::vector<std::pair<std::string, std::string>> &files,
    std::vector<std::string> &arguments, bool cxx_task_definitions) {
  constexpr const char *root = "/__objc3_canonical_runtime";
  files.emplace_back(std::string(root) +
                         "/objc3c/runtime/objc3_runtime_services.h",
                     runtime_interface::kHeaderSource);
  for (const auto &file : files)
    cxx_task_definitions |= file.second.find(
        "#include <objc3c/runtime/objc3_task.hpp>") != std::string::npos;
  if (cxx_task_definitions) {
    files.emplace_back(std::string(root) + "/objc3c/runtime/objc3_task.hpp",
                       runtime_interface::kTaskHeaderSource);
    // The complete C++ bridge needs its standard-library headers, rather than
    // the minimal C-only integer/size typedef projection below.
    arguments.insert(arguments.end(), {"-I", root});
    return;
  }
  files.emplace_back(std::string(root) + "/stddef.h",
                     "#ifndef OBJC3_CANONICAL_STDDEF_H\n"
                     "#define OBJC3_CANONICAL_STDDEF_H\n"
                     "typedef __SIZE_TYPE__ size_t;\n#endif\n");
  files.emplace_back(std::string(root) + "/stdint.h",
                     "#ifndef OBJC3_CANONICAL_STDINT_H\n"
                     "#define OBJC3_CANONICAL_STDINT_H\n"
                     "typedef __UINT32_TYPE__ uint32_t;\n"
                     "typedef __UINT64_TYPE__ uint64_t;\n#endif\n");
  arguments.insert(arguments.end(), {"-I", root});
}

bool IsQualifiedRuntimeFrameType(const clang::NamedDecl *declaration,
                                 const clang::ASTContext &constant_context) {
  const auto &triple = constant_context.getTargetInfo().getTriple();
  if (!triple.isOSLinux() || triple.getArch() != llvm::Triple::x86_64 ||
      !HeaderMatches(declaration, constant_context) ||
      declaration->getQualifiedNameAsString() != "objc3_async_frame")
    return false;
  const auto &context = constant_context;
  clang::QualType type;
  if (const auto *alias = llvm::dyn_cast<clang::TypedefNameDecl>(declaration))
    type = alias->getUnderlyingType();
  else if (const auto *record = llvm::dyn_cast<clang::RecordDecl>(declaration))
    type = context.getCanonicalTagType(record);
  else
    return false;
  auto *record = RecordNamed(type, "objc3_async_frame");
  if (!RecordMatches<6>(record, context, 40,
                        {"struct_size", "version", "context", "resume",
                         "destroy", "result_type"},
                        {0, 32, 64, 128, 192, 256}))
    return false;
  std::array<clang::QualType, 6> fields;
  size_t index = 0;
  for (const auto *field : record->fields())
    fields[index++] = field->getType();
  if (!UnsignedWidth(fields[0], 32, context) ||
      !UnsignedWidth(fields[1], 32, context) ||
      !context.hasSameType(fields[2], context.VoidPtrTy))
    return false;
  const auto *resume_pointer = fields[3]->getAs<clang::PointerType>();
  const auto *destroy_pointer = fields[4]->getAs<clang::PointerType>();
  const auto *result_pointer = fields[5]->getAs<clang::PointerType>();
  if (!resume_pointer || !destroy_pointer || !result_pointer ||
      !result_pointer->getPointeeType().isConstQualified() ||
      !ResultDescriptorMatches(result_pointer->getPointeeType(), context))
    return false;
  const auto *resume =
      resume_pointer->getPointeeType()->getAs<clang::FunctionProtoType>();
  if (!resume || resume->getNumParams() != 2)
    return false;
  const auto *task_pointer =
      resume->getParamType(0)->getAs<clang::PointerType>();
  if (!task_pointer)
    return false;
  const auto *task = task_pointer->getPointeeType()->getAs<clang::RecordType>();
  if (!task || task->getDecl()->getQualifiedNameAsString() != "objc3_task")
    return false;
  clang::FunctionProtoType::ExtProtoInfo info;
  const std::array<clang::QualType, 2> resume_args{resume->getParamType(0),
                                                   context.VoidPtrTy};
  const std::array<clang::QualType, 1> destroy_args{context.VoidPtrTy};
  return context.hasSameType(
             resume_pointer->getPointeeType(),
             context.getFunctionType(context.VoidTy, resume_args, info)) &&
         context.hasSameType(
             destroy_pointer->getPointeeType(),
             context.getFunctionType(context.VoidTy, destroy_args, info));
}

} // namespace objc3::clang_foundation
