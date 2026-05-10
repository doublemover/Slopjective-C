#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast_property_decl_nodes.h"

namespace objc3c::parse {

struct Objc3OwnershipOperationProfile {
  bool insert_retain = false;
  bool insert_release = false;
  bool insert_autorelease = false;
  std::string profile;
};

struct Objc3WeakUnownedLifetimeProfile {
  bool is_weak_reference = false;
  bool is_unowned_reference = false;
  bool is_unowned_safe_reference = false;
  std::string lifetime_profile;
  std::string runtime_hook_profile;
};

struct Objc3ArcDiagnosticFixitProfile {
  bool diagnostic_candidate = false;
  bool fixit_available = false;
  std::string diagnostic_profile;
  std::string fixit_hint;
};

std::string BuildProtocolQualifiedObjectTypeProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text);

bool IsProtocolQualifiedObjectTypeProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated);

bool IsOwnershipQualifierSpelling(const std::string &text);

std::string BuildOwnershipQualifierSymbol(const std::string &spelling,
                                          bool is_return_type);

Objc3OwnershipOperationProfile BuildParamOwnershipOperationProfile(
    const std::string &spelling);

Objc3OwnershipOperationProfile BuildReturnOwnershipOperationProfile(
    const std::string &spelling);

Objc3WeakUnownedLifetimeProfile BuildWeakUnownedLifetimeProfile(
    const std::string &spelling,
    bool prefer_safe_unowned);

Objc3WeakUnownedLifetimeProfile BuildPropertyWeakUnownedLifetimeProfile(
    const Objc3PropertyDecl &property);

Objc3ArcDiagnosticFixitProfile BuildArcDiagnosticFixitProfile(
    const std::string &spelling,
    bool is_return_type,
    bool is_property_type,
    bool weak_unowned_conflict);

std::vector<std::string> BuildSortedUniqueStrings(
    std::vector<std::string> values);

bool IsSortedUniqueStrings(const std::vector<std::string> &values);

}  // namespace objc3c::parse
