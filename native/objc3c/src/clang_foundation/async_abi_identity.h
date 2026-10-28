#pragma once

#include <string>

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/ObjC3KeyPath.h"
#include "llvm/ADT/SmallPtrSet.h"

namespace objc3::clang_foundation {

// An aggregate or parameter can contain several async callable ABIs. Preserve
// their positions instead of deriving their physical result from the consumer's
// source language. Canonical function types carry the AsyncFrame discriminator.
inline std::string ReadAsyncABIIdentity(clang::QualType type,
                                        const clang::ASTContext &context) {
  std::string result;
  llvm::SmallPtrSet<const clang::Type *, 16> active;
  auto field = [&](const std::string &value) {
    result += std::to_string(value.size()) + ":" + value;
  };
  auto visit = [&](auto &&self, clang::QualType current,
                   const std::string &position, unsigned depth) -> void {
    if (current.isNull())
      return;
    if (depth > 256) {
      result += "!unmodeled";
      return;
    }
    current = current.getDesugaredType(context);
    const auto *node = current.getTypePtr();
    if (!active.insert(node).second)
      return;
    clang::ObjC3KeyPathInfo keypath;
    if (clang::getObjC3KeyPathInfo(current, keypath)) {
      self(self, keypath.Root, position + ".root", depth + 1);
      self(self, keypath.Value, position + ".value", depth + 1);
    } else if (current->isPointerType() || current->isBlockPointerType() ||
               current->isReferenceType()) {
      self(self, current->getPointeeType(), position + ".pointee", depth + 1);
    } else if (const auto *member=current->getAs<clang::MemberPointerType>()) {
      self(self, member->getPointeeType(), position + ".member", depth + 1);
    } else if (const auto *array = context.getAsArrayType(current)) {
      self(self, array->getElementType(), position + ".element", depth + 1);
    } else if (const auto *optional =
                   llvm::dyn_cast<clang::ObjC3OptionalType>(node)) {
      self(self, optional->getElementType(), position + ".some", depth + 1);
    } else if (const auto *carrier =
                   llvm::dyn_cast<clang::ObjC3ResultType>(node)) {
      self(self, carrier->getSuccessType(), position + ".ok", depth + 1);
      self(self, carrier->getErrorType(), position + ".error", depth + 1);
    } else if (const auto *function =
                   llvm::dyn_cast<clang::FunctionProtoType>(node)) {
      const auto effects = function->getObjC3FunctionEffects();
      if (effects.Async) {
        if (result.empty())
          result = "v1:";
        field(position);
        field(effects.AsyncFrame ? "c-frame-v1" : "cxx-task-v1");
      }
      self(self, function->getReturnType(), position + ".return", depth + 1);
      for (unsigned i = 0; i < function->getNumParams(); ++i)
        self(self, function->getParamType(i),
             position + ".parameter[" + std::to_string(i) + "]", depth + 1);
      if (!effects.ErrorType.isNull())
        self(self, effects.ErrorType, position + ".error", depth + 1);
    } else if (const auto *record = current->getAs<clang::RecordType>()) {
      if (const auto *definition = record->getDecl()->getDefinition()) {
        unsigned index = 0;
        for (const auto *member : definition->fields())
          self(self, member->getType(),
               position + ".field[" + std::to_string(index++) + "]", depth + 1);
      }
    }
    active.erase(node);
  };
  visit(visit, type, "type", 0);
  return result;
}

} // namespace objc3::clang_foundation
