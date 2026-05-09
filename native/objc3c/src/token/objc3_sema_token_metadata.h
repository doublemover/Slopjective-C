#pragma once

#include <string>
#include <utility>

enum class Objc3SemaTokenKind {
  PointerDeclarator,
  NullabilitySuffix,
  OwnershipQualifier,
};

struct Objc3SemaTokenMetadata {
  Objc3SemaTokenKind kind = Objc3SemaTokenKind::PointerDeclarator;
  std::string text;
  unsigned line = 1;
  unsigned column = 1;
};

inline Objc3SemaTokenMetadata MakeObjc3SemaTokenMetadata(
    Objc3SemaTokenKind kind,
    std::string text,
    unsigned line,
    unsigned column) {
  Objc3SemaTokenMetadata metadata;
  metadata.kind = kind;
  metadata.text = std::move(text);
  metadata.line = line;
  metadata.column = column;
  return metadata;
}
