#include "parse/objc3_parser.h"
#include "parse/objc3_ast_builder.h"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace {

using Token = Objc3LexToken;
using TokenKind = Objc3LexTokenKind;

static bool IsHexDigit(char c) {
  return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

static bool IsBinaryDigit(char c) {
  return c == '0' || c == '1';
}

static bool IsOctalDigit(char c) {
  return c >= '0' && c <= '7';
}

static bool IsDigitSeparator(char c) {
  return c == '_';
}

static bool IsDigitForBase(char c, int base) {
  switch (base) {
  case 2:
    return IsBinaryDigit(c);
  case 8:
    return IsOctalDigit(c);
  case 10:
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
  case 16:
    return IsHexDigit(c);
  default:
    return false;
  }
}

static bool NormalizeIntegerDigits(const std::string &digits, int base, std::string &normalized) {
  normalized.clear();
  if (digits.empty()) {
    return false;
  }

  bool previous_was_digit = false;
  for (std::size_t i = 0; i < digits.size(); ++i) {
    const char c = digits[i];
    if (IsDigitSeparator(c)) {
      if (!previous_was_digit || i + 1 >= digits.size() || !IsDigitForBase(digits[i + 1], base)) {
        return false;
      }
      previous_was_digit = false;
      continue;
    }

    if (!IsDigitForBase(c, base)) {
      return false;
    }
    normalized.push_back(c);
    previous_was_digit = true;
  }

  return !normalized.empty() && previous_was_digit;
}

static bool ParseIntegerLiteralValue(const std::string &text, int &value) {
  if (text.empty()) {
    return false;
  }

  int base = 10;
  std::string digit_text = text;
  if (text.size() > 2 && text[0] == '0' && (text[1] == 'b' || text[1] == 'B')) {
    base = 2;
    digit_text = text.substr(2);
  } else if (text.size() > 2 && text[0] == '0' && (text[1] == 'o' || text[1] == 'O')) {
    base = 8;
    digit_text = text.substr(2);
  } else if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
    base = 16;
    digit_text = text.substr(2);
  }

  std::string normalized_digits;
  if (!NormalizeIntegerDigits(digit_text, base, normalized_digits)) {
    return false;
  }

  errno = 0;
  char *end = nullptr;
  const char *raw = normalized_digits.c_str();
  const long parsed = std::strtol(raw, &end, base);
  if (end == raw || end == nullptr || *end != '\0' || errno == ERANGE ||
      parsed < std::numeric_limits<int>::min() || parsed > std::numeric_limits<int>::max()) {
    return false;
  }

  value = static_cast<int>(parsed);
  return true;
}

static std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

static Objc3SemaTokenMetadata MakeSemaTokenMetadata(Objc3SemaTokenKind kind, const Token &token) {
  return MakeObjc3SemaTokenMetadata(kind, token.text, token.line, token.column);
}

static bool TryParseVectorTypeSpelling(const Token &type_token,
                                       ValueType &vector_type,
                                       std::string &vector_base_spelling,
                                       unsigned &vector_lane_count) {
  const std::string &text = type_token.text;
  const bool is_i32_vector = text.rfind("i32x", 0) == 0;
  const bool is_bool_vector = text.rfind("boolx", 0) == 0;
  if (!is_i32_vector && !is_bool_vector) {
    return false;
  }

  const std::size_t prefix_length = is_i32_vector ? 4u : 5u;
  if (text.size() <= prefix_length) {
    return false;
  }

  unsigned lane_count = 0;
  for (std::size_t i = prefix_length; i < text.size(); ++i) {
    const char c = text[i];
    if (!std::isdigit(static_cast<unsigned char>(c))) {
      return false;
    }
    lane_count = (lane_count * 10u) + static_cast<unsigned>(c - '0');
    if (lane_count > 1024u) {
      return false;
    }
  }

  if (lane_count != 2u && lane_count != 4u && lane_count != 8u && lane_count != 16u) {
    return false;
  }

  vector_type = is_i32_vector ? ValueType::I32 : ValueType::Bool;
  vector_base_spelling = is_i32_vector ? "i32" : "bool";
  vector_lane_count = lane_count;
  return true;
}

static std::string BuildNormalizedObjcSelector(const std::vector<Objc3MethodDecl::SelectorPiece> &pieces) {
  std::string normalized;
  for (const auto &piece : pieces) {
    normalized += piece.keyword;
    if (piece.has_parameter) {
      normalized += ":";
    }
  }
  return normalized;
}

static std::string BuildMessageSendFormSymbol(Expr::MessageSendForm form) {
  switch (form) {
  case Expr::MessageSendForm::Unary:
    return "message-send-form:unary";
  case Expr::MessageSendForm::Keyword:
    return "message-send-form:keyword";
  case Expr::MessageSendForm::None:
  default:
    return "message-send-form:none";
  }
}

static std::string BuildMessageSendSelectorLoweringSymbol(
    const std::vector<Expr::MessageSendSelectorPiece> &pieces) {
  std::string normalized_selector;
  for (const auto &piece : pieces) {
    normalized_selector += piece.keyword;
    if (piece.has_argument) {
      normalized_selector += ":";
    }
  }
  return "selector-lowering:" + normalized_selector;
}

static std::string BuildAutoreleasePoolScopeSymbol(unsigned serial, unsigned depth) {
  std::ostringstream out;
  out << "autoreleasepool-scope:" << serial << ";depth=" << depth;
  return out.str();
}

constexpr unsigned kDispatchAbiMarshallingRuntimeArgSlots = 4u;
constexpr const char *kRuntimeShimHostLinkDispatchSymbol = "objc3_msgsend_i32";

static unsigned ComputeDispatchAbiArgumentPaddingSlots(std::size_t argument_count,
                                                       unsigned runtime_arg_slots) {
  if (runtime_arg_slots == 0u) {
    return 0u;
  }
  const std::size_t remainder = argument_count % runtime_arg_slots;
  if (remainder == 0u) {
    return 0u;
  }
  return static_cast<unsigned>(runtime_arg_slots - static_cast<unsigned>(remainder));
}

static std::string BuildDispatchAbiMarshallingSymbol(unsigned receiver_slots,
                                                     unsigned selector_slots,
                                                     unsigned argument_value_slots,
                                                     unsigned argument_padding_slots,
                                                     unsigned argument_total_slots,
                                                     unsigned total_slots,
                                                     unsigned runtime_arg_slots) {
  std::ostringstream out;
  out << "dispatch-abi-marshalling:recv=" << receiver_slots << ";sel=" << selector_slots
      << ";arg-values=" << argument_value_slots << ";arg-padding=" << argument_padding_slots
      << ";arg-total=" << argument_total_slots << ";total=" << total_slots
      << ";runtime-slots=" << runtime_arg_slots;
  return out.str();
}

static std::string BuildNilReceiverFoldingSymbol(bool nil_receiver_foldable,
                                                 bool requires_runtime_dispatch,
                                                 Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "nil-receiver:foldable=" << (nil_receiver_foldable ? "true" : "false")
      << ";runtime-dispatch=" << (requires_runtime_dispatch ? "required" : "elided")
      << ";form=";
  switch (form) {
  case Expr::MessageSendForm::Unary:
    out << "unary";
    break;
  case Expr::MessageSendForm::Keyword:
    out << "keyword";
    break;
  case Expr::MessageSendForm::None:
  default:
    out << "none";
    break;
  }
  return out.str();
}

static bool IsSuperDispatchReceiver(const Expr &receiver) {
  return receiver.kind == Expr::Kind::Identifier && receiver.ident == "super";
}

static std::string ClassifyMethodFamilyFromSelector(const std::string &selector) {
  if (selector.rfind("mutableCopy", 0) == 0) {
    return "mutableCopy";
  }
  if (selector.rfind("copy", 0) == 0) {
    return "copy";
  }
  if (selector.rfind("init", 0) == 0) {
    return "init";
  }
  if (selector.rfind("new", 0) == 0) {
    return "new";
  }
  return "none";
}

static std::string BuildSuperDispatchSymbol(bool super_dispatch_enabled,
                                            bool super_dispatch_requires_class_context,
                                            Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "super-dispatch:enabled=" << (super_dispatch_enabled ? "true" : "false")
      << ";class-context=" << (super_dispatch_requires_class_context ? "required" : "not-required")
      << ";form=";
  switch (form) {
  case Expr::MessageSendForm::Unary:
    out << "unary";
    break;
  case Expr::MessageSendForm::Keyword:
    out << "keyword";
    break;
  case Expr::MessageSendForm::None:
  default:
    out << "none";
    break;
  }
  return out.str();
}

static std::string BuildMethodFamilySemanticsSymbol(const std::string &method_family_name,
                                                    bool returns_retained_result,
                                                    bool returns_related_result) {
  std::ostringstream out;
  out << "method-family:name=" << method_family_name
      << ";returns-retained=" << (returns_retained_result ? "true" : "false")
      << ";returns-related=" << (returns_related_result ? "true" : "false");
  return out.str();
}

static std::string BuildRuntimeShimHostLinkSymbol(bool runtime_shim_required,
                                                  bool runtime_shim_elided,
                                                  unsigned runtime_dispatch_arg_slots,
                                                  unsigned runtime_dispatch_declaration_parameter_count,
                                                  const std::string &runtime_dispatch_symbol,
                                                  Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "runtime-shim-host-link:required=" << (runtime_shim_required ? "true" : "false")
      << ";elided=" << (runtime_shim_elided ? "true" : "false")
      << ";runtime-slots=" << runtime_dispatch_arg_slots
      << ";decl-params=" << runtime_dispatch_declaration_parameter_count
      << ";symbol=" << runtime_dispatch_symbol
      << ";form=";
  switch (form) {
  case Expr::MessageSendForm::Unary:
    out << "unary";
    break;
  case Expr::MessageSendForm::Keyword:
    out << "keyword";
    break;
  case Expr::MessageSendForm::None:
  default:
    out << "none";
    break;
  }
  return out.str();
}

static std::vector<std::string> BuildScopePathLexicographic(std::string owner_symbol,
                                                             std::string entry_symbol) {
  std::vector<std::string> path;
  if (!owner_symbol.empty()) {
    path.push_back(std::move(owner_symbol));
  }
  if (!entry_symbol.empty()) {
    path.push_back(std::move(entry_symbol));
  }
  std::sort(path.begin(), path.end());
  path.erase(std::unique(path.begin(), path.end()), path.end());
  return path;
}

static std::string BuildObjcContainerScopeOwner(const std::string &container_kind,
                                                const std::string &name,
                                                bool has_category,
                                                const std::string &category_name) {
  std::string owner = container_kind + ":" + name;
  if (has_category) {
    owner += "(" + category_name + ")";
  }
  return owner;
}

static std::string BuildObjcMethodScopePathSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_method:" : "instance_method:") + method.selector;
}

static std::string BuildObjcPropertyScopePathSymbol(const Objc3PropertyDecl &property) {
  return "property:" + property.name;
}

static std::string BuildObjcPropertySynthesisSymbol(const Objc3PropertyDecl &property) {
  return "property_synthesis:" + property.name;
}

static std::string BuildObjcIvarBindingSymbol(const Objc3PropertyDecl &property) {
  return "ivar_binding:_" + property.name;
}

static std::string BuildObjcTypecheckParamFamilySymbol(const FuncParam &param) {
  if (param.id_spelling) {
    return "id";
  }
  if (param.class_spelling) {
    return "Class";
  }
  if (param.sel_spelling) {
    return "SEL";
  }
  if (param.object_pointer_type_spelling) {
    return "object-pointer:" + param.object_pointer_type_name;
  }
  return "";
}

static std::string BuildObjcTypecheckReturnFamilySymbol(const FunctionDecl &fn) {
  if (fn.return_id_spelling) {
    return "id";
  }
  if (fn.return_class_spelling) {
    return "Class";
  }
  if (fn.return_sel_spelling) {
    return "SEL";
  }
  if (fn.return_object_pointer_type_spelling) {
    return "object-pointer:" + fn.return_object_pointer_type_name;
  }
  return "";
}

static bool IsOwnershipQualifierSpelling(const std::string &text) {
  return text == "__strong" || text == "__weak" || text == "__autoreleasing" ||
         text == "__unsafe_unretained";
}

static std::string BuildOwnershipQualifierSymbol(const std::string &spelling, bool is_return_type) {
  if (spelling.empty()) {
    return "";
  }
  return std::string(is_return_type ? "return-ownership-qualifier:" : "ownership-qualifier:") + spelling;
}

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

static Objc3OwnershipOperationProfile BuildParamOwnershipOperationProfile(const std::string &spelling) {
  Objc3OwnershipOperationProfile profile;
  if (spelling == "__strong") {
    profile.insert_retain = true;
    profile.insert_release = true;
    profile.profile = "param-retain-release";
  } else if (spelling == "__weak") {
    profile.profile = "param-weak-side-table";
  } else if (spelling == "__autoreleasing") {
    profile.insert_autorelease = true;
    profile.profile = "param-autorelease-bridge";
  } else if (spelling == "__unsafe_unretained") {
    profile.profile = "param-unsafe-unretained";
  }
  return profile;
}

static Objc3OwnershipOperationProfile BuildReturnOwnershipOperationProfile(const std::string &spelling) {
  Objc3OwnershipOperationProfile profile;
  if (spelling == "__strong") {
    profile.insert_retain = true;
    profile.insert_release = true;
    profile.profile = "return-retain-release-transfer";
  } else if (spelling == "__weak") {
    profile.profile = "return-weak-load";
  } else if (spelling == "__autoreleasing") {
    profile.insert_autorelease = true;
    profile.profile = "return-autorelease-transfer";
  } else if (spelling == "__unsafe_unretained") {
    profile.profile = "return-unsafe-unretained";
  }
  return profile;
}

static Objc3WeakUnownedLifetimeProfile BuildWeakUnownedLifetimeProfile(const std::string &spelling,
                                                                       bool prefer_safe_unowned) {
  Objc3WeakUnownedLifetimeProfile profile;
  if (spelling == "__weak") {
    profile.is_weak_reference = true;
    profile.lifetime_profile = "weak";
    profile.runtime_hook_profile = "objc-weak-side-table";
  } else if (spelling == "__unsafe_unretained") {
    profile.is_unowned_reference = true;
    profile.is_unowned_safe_reference = prefer_safe_unowned;
    profile.lifetime_profile = prefer_safe_unowned ? "unowned-safe" : "unowned-unsafe";
    profile.runtime_hook_profile = prefer_safe_unowned ? "objc-unowned-safe-guard"
                                                       : "objc-unowned-unsafe-direct";
  } else if (spelling == "__strong") {
    profile.lifetime_profile = "strong-owned";
  } else if (spelling == "__autoreleasing") {
    profile.lifetime_profile = "autoreleasing";
  }
  return profile;
}

static Objc3WeakUnownedLifetimeProfile BuildPropertyWeakUnownedLifetimeProfile(
    const Objc3PropertyDecl &property) {
  if (property.is_weak) {
    return BuildWeakUnownedLifetimeProfile("__weak", false);
  }
  if (property.is_unowned) {
    return BuildWeakUnownedLifetimeProfile("__unsafe_unretained", true);
  }
  if (!property.ownership_qualifier_spelling.empty()) {
    return BuildWeakUnownedLifetimeProfile(property.ownership_qualifier_spelling, false);
  }
  if (property.is_assign) {
    return BuildWeakUnownedLifetimeProfile("__unsafe_unretained", false);
  }
  return Objc3WeakUnownedLifetimeProfile{};
}

static Objc3ArcDiagnosticFixitProfile BuildArcDiagnosticFixitProfile(const std::string &spelling,
                                                                     bool is_return_type,
                                                                     bool is_property_type,
                                                                     bool weak_unowned_conflict) {
  Objc3ArcDiagnosticFixitProfile profile;
  if (weak_unowned_conflict) {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = "arc-weak-unowned-conflict";
    profile.fixit_hint = "remove-weak-or-unowned-attribute";
    return profile;
  }

  if (spelling == "__unsafe_unretained") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = is_return_type ? "arc-return-unsafe-unretained" : "arc-unsafe-unretained";
    profile.fixit_hint = is_property_type ? "replace-with-weak-or-strong-attribute"
                                          : "replace-with-__weak-or-__strong";
    return profile;
  }

  if (spelling == "__autoreleasing") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = is_return_type ? "arc-return-autoreleasing-transfer" : "arc-autoreleasing-misuse";
    profile.fixit_hint = is_return_type ? "replace-return-qualifier-with-__strong"
                                        : "replace-with-__strong-or-out-parameter";
    return profile;
  }

  if (is_return_type && spelling == "__weak") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = "arc-return-weak-escape";
    profile.fixit_hint = "replace-return-qualifier-with-__strong";
  }
  return profile;
}

static std::vector<std::string> BuildSortedUniqueStrings(std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}

static std::vector<std::string> BuildProtocolSemanticLinkTargetsLexicographic(
    const std::vector<std::string> &protocol_names) {
  std::vector<std::string> targets;
  targets.reserve(protocol_names.size());
  for (const auto &name : protocol_names) {
    if (!name.empty()) {
      targets.push_back("protocol:" + name);
    }
  }
  return BuildSortedUniqueStrings(std::move(targets));
}

static std::string BuildObjcCategorySemanticLinkSymbol(const std::string &owner_name,
                                                       const std::string &category_name) {
  return "category:" + owner_name + "(" + category_name + ")";
}

static std::string BuildObjcMethodLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_lookup:" : "instance_lookup:") + method.selector;
}

static std::string BuildObjcMethodOverrideLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_override:" : "instance_override:") + method.selector;
}

static std::string BuildObjcMethodConflictLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_conflict:" : "instance_conflict:") + method.selector;
}

static std::vector<std::string> BuildObjcMethodLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.method_lookup_symbol.empty()) {
      symbols.push_back(method.method_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcMethodOverrideLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.override_lookup_symbol.empty()) {
      symbols.push_back(method.override_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcMethodConflictLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.conflict_lookup_symbol.empty()) {
      symbols.push_back(method.conflict_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcPropertySynthesisSymbolsLexicographic(
    const std::vector<Objc3PropertyDecl> &properties) {
  std::vector<std::string> symbols;
  symbols.reserve(properties.size());
  for (const auto &property : properties) {
    if (!property.property_synthesis_symbol.empty()) {
      symbols.push_back(property.property_synthesis_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcIvarBindingSymbolsLexicographic(
    const std::vector<Objc3PropertyDecl> &properties) {
  std::vector<std::string> symbols;
  symbols.reserve(properties.size());
  for (const auto &property : properties) {
    if (!property.ivar_binding_symbol.empty()) {
      symbols.push_back(property.ivar_binding_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

class Objc3Parser {
 public:
  explicit Objc3Parser(const std::vector<Token> &tokens) : tokens_(tokens) {}

  Objc3ParsedProgram Parse() {
    Objc3ParsedProgram program = ast_builder_.BeginProgram();
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::KwModule)) {
        ParseModule(program);
      } else if (Match(TokenKind::KwLet)) {
        auto decl = ParseGlobalLet();
        if (decl != nullptr) {
          ast_builder_.AddGlobalDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtInterface)) {
        auto decl = ParseObjcInterfaceDecl();
        if (decl != nullptr) {
          ast_builder_.AddInterfaceDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtImplementation)) {
        auto decl = ParseObjcImplementationDecl();
        if (decl != nullptr) {
          ast_builder_.AddImplementationDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtProtocol)) {
        auto decl = ParseObjcProtocolDecl();
        if (decl != nullptr) {
          ast_builder_.AddProtocolDecl(program, std::move(*decl));
        }
      } else if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwFn)) {
        ParseTopLevelFunctionDecl(program);
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100",
                                        "unsupported Objective-C 3 statement"));
        SynchronizeTopLevel();
      }
    }
    return program;
  }

  std::vector<std::string> TakeDiagnostics() { return diagnostics_; }

 private:
  bool At(TokenKind kind) const { return tokens_[index_].kind == kind; }

  const Token &Peek() const { return tokens_[index_]; }

  const Token &Previous() const { return tokens_[index_ - 1]; }

  const Token &Advance() {
    if (!At(TokenKind::Eof)) {
      ++index_;
    }
    return Previous();
  }

  bool Match(TokenKind kind) {
    if (At(kind)) {
      Advance();
      return true;
    }
    return false;
  }

  void ParseTopLevelFunctionDecl(Objc3ParsedProgram &program) {
    bool is_pure = false;
    bool is_extern = false;
    std::optional<TokenKind> trailing_qualifier = std::nullopt;

    while (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      if (Match(TokenKind::KwPure)) {
        if (is_pure) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'pure' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_pure = true;
        trailing_qualifier = TokenKind::KwPure;
        continue;
      }

      if (Match(TokenKind::KwExtern)) {
        if (is_extern) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'extern' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_extern = true;
        trailing_qualifier = TokenKind::KwExtern;
      }
    }

    if (!Match(TokenKind::KwFn)) {
      const Token &token = Peek();
      const std::string message = (trailing_qualifier.has_value() && trailing_qualifier.value() == TokenKind::KwExtern)
                                      ? "expected 'fn' after 'extern'"
                                      : "expected 'fn' after 'pure'";
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", message));
      SynchronizeTopLevel();
      return;
    }

    auto fn = ParseFunction();
    if (fn == nullptr) {
      return;
    }

    fn->is_pure = is_pure;
    if (is_extern && !fn->is_prototype) {
      diagnostics_.push_back(MakeDiag(fn->line, fn->column, "O3P104", "missing ';' after extern function declaration"));
      return;
    }

    ast_builder_.AddFunctionDecl(program, std::move(*fn));
  }

  bool AtIdentifierColon() const {
    return At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) &&
           tokens_[index_ + 1].kind == TokenKind::Colon;
  }

  static bool IsAssignmentOperatorToken(TokenKind kind) {
    return kind == TokenKind::Equal || kind == TokenKind::PlusEqual || kind == TokenKind::MinusEqual ||
           kind == TokenKind::StarEqual || kind == TokenKind::SlashEqual || kind == TokenKind::PercentEqual ||
           kind == TokenKind::AmpersandEqual || kind == TokenKind::PipeEqual || kind == TokenKind::CaretEqual ||
           kind == TokenKind::LessLessEqual || kind == TokenKind::GreaterGreaterEqual;
  }

  static bool IsUpdateOperatorToken(TokenKind kind) {
    return kind == TokenKind::PlusPlus || kind == TokenKind::MinusMinus;
  }

  bool AtIdentifierAssignment() const {
    return At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) &&
           IsAssignmentOperatorToken(tokens_[index_ + 1].kind);
  }

  bool AtIdentifierUpdate() const {
    return At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) &&
           IsUpdateOperatorToken(tokens_[index_ + 1].kind);
  }

  bool AtPrefixUpdate() const {
    return IsUpdateOperatorToken(Peek().kind) && (index_ + 1 < tokens_.size()) &&
           tokens_[index_ + 1].kind == TokenKind::Identifier;
  }

  bool MatchAssignmentOperator(std::string &op) {
    if (Match(TokenKind::Equal)) {
      op = "=";
      return true;
    }
    if (Match(TokenKind::PlusEqual)) {
      op = "+=";
      return true;
    }
    if (Match(TokenKind::MinusEqual)) {
      op = "-=";
      return true;
    }
    if (Match(TokenKind::StarEqual)) {
      op = "*=";
      return true;
    }
    if (Match(TokenKind::SlashEqual)) {
      op = "/=";
      return true;
    }
    if (Match(TokenKind::PercentEqual)) {
      op = "%=";
      return true;
    }
    if (Match(TokenKind::AmpersandEqual)) {
      op = "&=";
      return true;
    }
    if (Match(TokenKind::PipeEqual)) {
      op = "|=";
      return true;
    }
    if (Match(TokenKind::CaretEqual)) {
      op = "^=";
      return true;
    }
    if (Match(TokenKind::LessLessEqual)) {
      op = "<<=";
      return true;
    }
    if (Match(TokenKind::GreaterGreaterEqual)) {
      op = ">>=";
      return true;
    }
    return false;
  }

  bool MatchUpdateOperator(std::string &op) {
    if (Match(TokenKind::PlusPlus)) {
      op = "++";
      return true;
    }
    if (Match(TokenKind::MinusMinus)) {
      op = "--";
      return true;
    }
    return false;
  }

  void ParseModule(Objc3ParsedProgram &program) {
    const Token &name_token = Peek();
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P101", "invalid module identifier"));
      SynchronizeTopLevel();
      return;
    }
    const std::string module_name = Advance().text;
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P104", "missing ';' after module declaration"));
      SynchronizeTopLevel();
      return;
    }
    if (saw_module_declaration_) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3S200", "duplicate module '" + module_name + "'"));
      return;
    }
    saw_module_declaration_ = true;
    ast_builder_.SetModuleName(program, module_name);
  }

  std::unique_ptr<GlobalDecl> ParseGlobalLet() {
    auto decl = std::make_unique<GlobalDecl>();
    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                      "invalid declaration identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;
    decl->scope_owner_symbol = BuildObjcContainerScopeOwner("protocol", decl->name, false, "");
    decl->scope_path_lexicographic = BuildScopePathLexicographic(decl->scope_owner_symbol, "protocol:" + decl->name);
    decl->semantic_link_symbol = "protocol:" + decl->name;
    decl->line = Previous().line;
    decl->column = Previous().column;

    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
      SynchronizeTopLevel();
      return nullptr;
    }

    decl->value = ParseExpression();
    if (decl->value == nullptr) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104",
                                      "missing ';' after declaration"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  void CopyMethodReturnTypeFromFunctionDecl(const FunctionDecl &source, Objc3MethodDecl &target) {
    target.return_type = source.return_type;
    target.return_vector_spelling = source.return_vector_spelling;
    target.return_vector_base_spelling = source.return_vector_base_spelling;
    target.return_vector_lane_count = source.return_vector_lane_count;
    target.return_id_spelling = source.return_id_spelling;
    target.return_class_spelling = source.return_class_spelling;
    target.return_sel_spelling = source.return_sel_spelling;
    target.return_instancetype_spelling = source.return_instancetype_spelling;
    target.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
    target.return_object_pointer_type_name = source.return_object_pointer_type_name;
    target.return_typecheck_family_symbol = source.return_typecheck_family_symbol;
    target.has_return_generic_suffix = source.has_return_generic_suffix;
    target.return_generic_suffix_terminated = source.return_generic_suffix_terminated;
    target.return_generic_suffix_text = source.return_generic_suffix_text;
    target.return_generic_line = source.return_generic_line;
    target.return_generic_column = source.return_generic_column;
    target.has_return_pointer_declarator = source.has_return_pointer_declarator;
    target.return_pointer_declarator_depth = source.return_pointer_declarator_depth;
    target.return_pointer_declarator_tokens = source.return_pointer_declarator_tokens;
    target.return_nullability_suffix_tokens = source.return_nullability_suffix_tokens;
    target.has_return_ownership_qualifier = source.has_return_ownership_qualifier;
    target.return_ownership_qualifier_spelling = source.return_ownership_qualifier_spelling;
    target.return_ownership_qualifier_symbol = source.return_ownership_qualifier_symbol;
    target.return_ownership_qualifier_tokens = source.return_ownership_qualifier_tokens;
    target.return_ownership_insert_retain = source.return_ownership_insert_retain;
    target.return_ownership_insert_release = source.return_ownership_insert_release;
    target.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
    target.return_ownership_operation_profile = source.return_ownership_operation_profile;
    target.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
    target.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
    target.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
    target.return_ownership_lifetime_profile = source.return_ownership_lifetime_profile;
    target.return_ownership_runtime_hook_profile = source.return_ownership_runtime_hook_profile;
    target.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
    target.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
    target.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
    target.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
  }

  void CopyPropertyTypeFromParam(const FuncParam &source, Objc3PropertyDecl &target) {
    target.type = source.type;
    target.vector_spelling = source.vector_spelling;
    target.vector_base_spelling = source.vector_base_spelling;
    target.vector_lane_count = source.vector_lane_count;
    target.id_spelling = source.id_spelling;
    target.class_spelling = source.class_spelling;
    target.sel_spelling = source.sel_spelling;
    target.instancetype_spelling = source.instancetype_spelling;
    target.object_pointer_type_spelling = source.object_pointer_type_spelling;
    target.object_pointer_type_name = source.object_pointer_type_name;
    target.typecheck_family_symbol = source.typecheck_family_symbol;
    target.has_generic_suffix = source.has_generic_suffix;
    target.generic_suffix_terminated = source.generic_suffix_terminated;
    target.generic_suffix_text = source.generic_suffix_text;
    target.generic_line = source.generic_line;
    target.generic_column = source.generic_column;
    target.has_pointer_declarator = source.has_pointer_declarator;
    target.pointer_declarator_depth = source.pointer_declarator_depth;
    target.pointer_declarator_tokens = source.pointer_declarator_tokens;
    target.nullability_suffix_tokens = source.nullability_suffix_tokens;
    target.has_ownership_qualifier = source.has_ownership_qualifier;
    target.ownership_qualifier_spelling = source.ownership_qualifier_spelling;
    target.ownership_qualifier_symbol = source.ownership_qualifier_symbol;
    target.ownership_qualifier_tokens = source.ownership_qualifier_tokens;
    target.ownership_insert_retain = source.ownership_insert_retain;
    target.ownership_insert_release = source.ownership_insert_release;
    target.ownership_insert_autorelease = source.ownership_insert_autorelease;
    target.ownership_operation_profile = source.ownership_operation_profile;
    target.ownership_is_weak_reference = source.ownership_is_weak_reference;
    target.ownership_is_unowned_reference = source.ownership_is_unowned_reference;
    target.ownership_is_unowned_safe_reference = source.ownership_is_unowned_safe_reference;
    target.ownership_lifetime_profile = source.ownership_lifetime_profile;
    target.ownership_runtime_hook_profile = source.ownership_runtime_hook_profile;
    target.ownership_arc_diagnostic_candidate = source.ownership_arc_diagnostic_candidate;
    target.ownership_arc_fixit_available = source.ownership_arc_fixit_available;
    target.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;
    target.ownership_arc_fixit_hint = source.ownership_arc_fixit_hint;
  }

  void AssignObjcMethodLookupOverrideConflictSymbols(Objc3MethodDecl &method,
                                                     const std::string &lookup_owner_symbol,
                                                     const std::string &override_owner_symbol) {
    method.method_lookup_symbol = lookup_owner_symbol + "::" + BuildObjcMethodLookupSymbol(method);
    method.override_lookup_symbol = override_owner_symbol + "::" + BuildObjcMethodOverrideLookupSymbol(method);
    method.conflict_lookup_symbol = BuildObjcMethodConflictLookupSymbol(method);
  }

  void FinalizeObjcMethodLookupOverrideConflictPackets(
      const std::vector<Objc3MethodDecl> &methods,
      std::vector<std::string> &method_lookup_symbols_lexicographic,
      std::vector<std::string> &override_lookup_symbols_lexicographic,
      std::vector<std::string> &conflict_lookup_symbols_lexicographic) {
    method_lookup_symbols_lexicographic = BuildObjcMethodLookupSymbolsLexicographic(methods);
    override_lookup_symbols_lexicographic = BuildObjcMethodOverrideLookupSymbolsLexicographic(methods);
    conflict_lookup_symbols_lexicographic = BuildObjcMethodConflictLookupSymbolsLexicographic(methods);
  }

  void AssignObjcPropertySynthesisIvarBindingSymbols(Objc3PropertyDecl &property,
                                                     const std::string &synthesis_owner_symbol) {
    property.property_synthesis_symbol = synthesis_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property);
    property.ivar_binding_symbol = synthesis_owner_symbol + "::" + BuildObjcIvarBindingSymbol(property);
  }

  void FinalizeObjcPropertySynthesisIvarBindingPackets(
      const std::vector<Objc3PropertyDecl> &properties,
      std::vector<std::string> &property_synthesis_symbols_lexicographic,
      std::vector<std::string> &ivar_binding_symbols_lexicographic) {
    property_synthesis_symbols_lexicographic = BuildObjcPropertySynthesisSymbolsLexicographic(properties);
    ivar_binding_symbols_lexicographic = BuildObjcIvarBindingSymbolsLexicographic(properties);
  }

  void ConsumeBracedBodyTail() {
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::LBrace)) {
        ++depth;
        continue;
      }
      if (Match(TokenKind::RBrace)) {
        --depth;
        continue;
      }
      Advance();
    }
  }

  bool ParseObjcMethodParameterClause(FuncParam &param) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P106", "missing '(' before Objective-C method parameter type"));
      return false;
    }
    if (!ParseParameterType(param)) {
      return false;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C method parameter type"));
      return false;
    }
    const Token &name = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name.line, name.column, "O3P101", "invalid Objective-C method parameter identifier"));
      return false;
    }
    param.name = Previous().text;
    param.line = Previous().line;
    param.column = Previous().column;
    return true;
  }

  bool ParseObjcMethodDecl(Objc3MethodDecl &method, bool allow_body) {
    if (Match(TokenKind::Minus)) {
      method.is_class_method = false;
    } else if (Match(TokenKind::Plus)) {
      method.is_class_method = true;
    } else {
      return false;
    }
    const Token method_marker = Previous();
    method.line = method_marker.line;
    method.column = method_marker.column;

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P106", "missing '(' after Objective-C method marker"));
      return false;
    }

    FunctionDecl synthetic_fn;
    if (!ParseFunctionReturnType(synthetic_fn)) {
      return false;
    }
    CopyMethodReturnTypeFromFunctionDecl(synthetic_fn, method);

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C method return type"));
      return false;
    }

    const Token &selector_head = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(selector_head.line, selector_head.column, "O3P101", "invalid Objective-C selector identifier"));
      return false;
    }

    Objc3MethodDecl::SelectorPiece head_piece;
    head_piece.keyword = Previous().text;
    head_piece.line = Previous().line;
    head_piece.column = Previous().column;
    if (Match(TokenKind::Colon)) {
      head_piece.has_parameter = true;
      FuncParam first_param;
      if (!ParseObjcMethodParameterClause(first_param)) {
        return false;
      }
      head_piece.parameter_name = first_param.name;
      method.params.push_back(std::move(first_param));
      method.selector_pieces.push_back(std::move(head_piece));

      while (At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) && tokens_[index_ + 1].kind == TokenKind::Colon) {
        const Token keyword = Advance();
        (void)Match(TokenKind::Colon);
        Objc3MethodDecl::SelectorPiece keyword_piece;
        keyword_piece.keyword = keyword.text;
        keyword_piece.has_parameter = true;
        keyword_piece.line = keyword.line;
        keyword_piece.column = keyword.column;

        FuncParam keyword_param;
        if (!ParseObjcMethodParameterClause(keyword_param)) {
          return false;
        }
        keyword_piece.parameter_name = keyword_param.name;
        method.params.push_back(std::move(keyword_param));
        method.selector_pieces.push_back(std::move(keyword_piece));
      }
    } else {
      method.selector_pieces.push_back(std::move(head_piece));
    }

    method.selector = BuildNormalizedObjcSelector(method.selector_pieces);
    method.selector_is_normalized = true;

    if (Match(TokenKind::Semicolon)) {
      method.has_body = false;
      return true;
    }

    if (!allow_body) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104",
                                      "missing ';' after Objective-C interface method declaration"));
      return false;
    }

    if (!Match(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110",
                                      "missing '{' or ';' after Objective-C implementation method declaration"));
      return false;
    }
    method.has_body = true;
    ConsumeBracedBodyTail();
    return true;
  }

  std::string ParseObjcPropertyAttributeValueText() {
    std::string value_text;
    while (!At(TokenKind::Eof) && !At(TokenKind::Comma) && !At(TokenKind::RParen)) {
      value_text += Advance().text;
    }
    return value_text;
  }

  bool ParseObjcPropertyAttributes(std::vector<Objc3PropertyAttributeDecl> &attributes) {
    if (!Match(TokenKind::LParen)) {
      return true;
    }

    while (true) {
      const Token &name_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C @property attribute"));
        return false;
      }

      Objc3PropertyAttributeDecl attribute;
      attribute.name = Previous().text;
      attribute.line = Previous().line;
      attribute.column = Previous().column;
      if (Match(TokenKind::Equal)) {
        attribute.has_value = true;
        attribute.value = ParseObjcPropertyAttributeValueText();
        if (attribute.value.empty()) {
          const Token &token = Peek();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "missing Objective-C @property attribute value"));
          return false;
        }
      }
      attributes.push_back(std::move(attribute));

      if (Match(TokenKind::Comma)) {
        continue;
      }
      if (Match(TokenKind::RParen)) {
        return true;
      }

      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C @property attribute list"));
      return false;
    }
  }

  void ApplyObjcPropertyAttributes(Objc3PropertyDecl &property) {
    for (const auto &attribute : property.attributes) {
      if (attribute.name == "readonly") {
        property.is_readonly = true;
      } else if (attribute.name == "readwrite") {
        property.is_readwrite = true;
      } else if (attribute.name == "atomic") {
        property.is_atomic = true;
      } else if (attribute.name == "nonatomic") {
        property.is_nonatomic = true;
      } else if (attribute.name == "copy") {
        property.is_copy = true;
      } else if (attribute.name == "strong") {
        property.is_strong = true;
      } else if (attribute.name == "weak") {
        property.is_weak = true;
      } else if (attribute.name == "unowned") {
        property.is_unowned = true;
      } else if (attribute.name == "assign") {
        property.is_assign = true;
      } else if (attribute.name == "getter") {
        property.has_getter = true;
        property.getter_selector = attribute.value;
      } else if (attribute.name == "setter") {
        property.has_setter = true;
        property.setter_selector = attribute.value;
      }
    }
  }

  bool ParseObjcPropertyDecl(Objc3PropertyDecl &property) {
    if (!Match(TokenKind::KwAtProperty)) {
      return false;
    }
    const Token property_marker = Previous();
    property.line = property_marker.line;
    property.column = property_marker.column;

    if (!ParseObjcPropertyAttributes(property.attributes)) {
      return false;
    }

    FuncParam property_type;
    if (!ParseParameterType(property_type)) {
      return false;
    }
    CopyPropertyTypeFromParam(property_type, property);

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C @property identifier"));
      return false;
    }
    property.name = Previous().text;

    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P104", "missing ';' after Objective-C @property declaration"));
      return false;
    }

    ApplyObjcPropertyAttributes(property);
    const Objc3WeakUnownedLifetimeProfile property_lifetime_profile =
        BuildPropertyWeakUnownedLifetimeProfile(property);
    property.ownership_is_weak_reference = property_lifetime_profile.is_weak_reference;
    property.ownership_is_unowned_reference = property_lifetime_profile.is_unowned_reference;
    property.ownership_is_unowned_safe_reference = property_lifetime_profile.is_unowned_safe_reference;
    property.ownership_lifetime_profile = property_lifetime_profile.lifetime_profile;
    property.ownership_runtime_hook_profile = property_lifetime_profile.runtime_hook_profile;
    property.has_weak_unowned_conflict = property.is_weak && property.is_unowned;
    const Objc3ArcDiagnosticFixitProfile property_arc_diagnostic_profile =
        BuildArcDiagnosticFixitProfile(
            property.ownership_qualifier_spelling,
            false,
            true,
            property.has_weak_unowned_conflict);
    property.ownership_arc_diagnostic_candidate = property_arc_diagnostic_profile.diagnostic_candidate;
    property.ownership_arc_fixit_available = property_arc_diagnostic_profile.fixit_available;
    property.ownership_arc_diagnostic_profile = property_arc_diagnostic_profile.diagnostic_profile;
    property.ownership_arc_fixit_hint = property_arc_diagnostic_profile.fixit_hint;
    return true;
  }

  void SynchronizeObjcContainer() {
    while (!At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtEnd) || At(TokenKind::Minus) || At(TokenKind::Plus) || At(TokenKind::KwAtProperty)) {
        return;
      }
      if (At(TokenKind::KwAtInterface) || At(TokenKind::KwAtImplementation) || At(TokenKind::KwAtProtocol) ||
          At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern)) {
        return;
      }
      if (Match(TokenKind::Semicolon)) {
        return;
      }
      if (Match(TokenKind::LBrace)) {
        ConsumeBracedBodyTail();
        continue;
      }
      Advance();
    }
  }

  bool ParseObjcProtocolCompositionClause(std::vector<std::string> &protocols) {
    if (!Match(TokenKind::Less)) {
      return true;
    }

    while (true) {
      const Token &protocol_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(protocol_token.line, protocol_token.column, "O3P101",
                                        "invalid Objective-C protocol composition identifier"));
        return false;
      }
      protocols.push_back(Previous().text);

      if (Match(TokenKind::Comma)) {
        continue;
      }

      if (Match(TokenKind::Greater)) {
        return true;
      }

      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P112", "missing '>' after Objective-C protocol composition list"));
      return false;
    }
  }

  bool ParseObjcCategoryClause(std::string &category_name, bool &has_category) {
    if (!Match(TokenKind::LParen)) {
      return true;
    }
    has_category = true;
    if (Match(TokenKind::Identifier)) {
      category_name = Previous().text;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C category name"));
      return false;
    }
    return true;
  }

  std::unique_ptr<Objc3ProtocolDecl> ParseObjcProtocolDecl() {
    auto decl = std::make_unique<Objc3ProtocolDecl>();
    decl->line = Previous().line;
    decl->column = Previous().column;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C protocol identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (!ParseObjcProtocolCompositionClause(decl->inherited_protocols)) {
      SynchronizeObjcContainer();
    }
    decl->inherited_protocols_lexicographic =
        BuildProtocolSemanticLinkTargetsLexicographic(decl->inherited_protocols);
    decl->semantic_link_symbol = "protocol:" + decl->name;
    decl->scope_owner_symbol = BuildObjcContainerScopeOwner("protocol", decl->name, false, "");
    decl->scope_path_lexicographic =
        BuildScopePathLexicographic(decl->scope_owner_symbol, decl->semantic_link_symbol);

    if (Match(TokenKind::Semicolon)) {
      decl->is_forward_declaration = true;
      return decl;
    }

    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.scope_owner_symbol = decl->scope_owner_symbol;
          property.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);
          decl->properties.push_back(std::move(property));
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "unsupported token inside @protocol declaration"));
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, false)) {
        method.scope_owner_symbol = decl->scope_owner_symbol;
        method.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);
        AssignObjcMethodLookupOverrideConflictSymbols(method, decl->semantic_link_symbol, decl->semantic_link_symbol);
        decl->methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }

    FinalizeObjcMethodLookupOverrideConflictPackets(decl->methods,
                                                    decl->method_lookup_symbols_lexicographic,
                                                    decl->override_lookup_symbols_lexicographic,
                                                    decl->conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '@end' after @protocol"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  std::unique_ptr<Objc3InterfaceDecl> ParseObjcInterfaceDecl() {
    auto decl = std::make_unique<Objc3InterfaceDecl>();
    decl->line = Previous().line;
    decl->column = Previous().column;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C interface identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (Match(TokenKind::Colon)) {
      const Token &super_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            MakeDiag(super_token.line, super_token.column, "O3P101", "invalid Objective-C superclass identifier"));
        SynchronizeObjcContainer();
      } else {
        decl->super_name = Previous().text;
      }
    }

    if (!ParseObjcCategoryClause(decl->category_name, decl->has_category)) {
      SynchronizeObjcContainer();
    }

    if (!ParseObjcProtocolCompositionClause(decl->adopted_protocols)) {
      SynchronizeObjcContainer();
    }
    decl->adopted_protocols_lexicographic = BuildProtocolSemanticLinkTargetsLexicographic(decl->adopted_protocols);
    decl->semantic_link_symbol =
        BuildObjcContainerScopeOwner("interface", decl->name, decl->has_category, decl->category_name);
    if (!decl->super_name.empty()) {
      decl->semantic_link_super_symbol = "interface:" + decl->super_name;
    }
    if (decl->has_category) {
      decl->semantic_link_category_symbol = BuildObjcCategorySemanticLinkSymbol(decl->name, decl->category_name);
    }
    decl->scope_owner_symbol =
        BuildObjcContainerScopeOwner("interface", decl->name, decl->has_category, decl->category_name);
    decl->scope_path_lexicographic =
        BuildScopePathLexicographic(decl->scope_owner_symbol, "interface:" + decl->name);
    if (!decl->super_name.empty()) {
      decl->scope_path_lexicographic.push_back("super:" + decl->super_name);
      std::sort(decl->scope_path_lexicographic.begin(), decl->scope_path_lexicographic.end());
      decl->scope_path_lexicographic.erase(
          std::unique(decl->scope_path_lexicographic.begin(), decl->scope_path_lexicographic.end()),
          decl->scope_path_lexicographic.end());
    }

    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.scope_owner_symbol = decl->scope_owner_symbol;
          property.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);
          AssignObjcPropertySynthesisIvarBindingSymbols(property, decl->semantic_link_symbol);
          decl->properties.push_back(std::move(property));
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "unsupported token inside @interface declaration"));
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, false)) {
        method.scope_owner_symbol = decl->scope_owner_symbol;
        method.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);
        const std::string override_owner_symbol =
            decl->semantic_link_super_symbol.empty() ? decl->semantic_link_symbol : decl->semantic_link_super_symbol;
        AssignObjcMethodLookupOverrideConflictSymbols(method, decl->semantic_link_symbol, override_owner_symbol);
        decl->methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }

    FinalizeObjcPropertySynthesisIvarBindingPackets(decl->properties,
                                                    decl->property_synthesis_symbols_lexicographic,
                                                    decl->ivar_binding_symbols_lexicographic);
    FinalizeObjcMethodLookupOverrideConflictPackets(decl->methods,
                                                    decl->method_lookup_symbols_lexicographic,
                                                    decl->override_lookup_symbols_lexicographic,
                                                    decl->conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '@end' after @interface"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  std::unique_ptr<Objc3ImplementationDecl> ParseObjcImplementationDecl() {
    auto decl = std::make_unique<Objc3ImplementationDecl>();
    decl->line = Previous().line;
    decl->column = Previous().column;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C implementation identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (!ParseObjcCategoryClause(decl->category_name, decl->has_category)) {
      SynchronizeObjcContainer();
    }
    decl->semantic_link_symbol =
        BuildObjcContainerScopeOwner("implementation", decl->name, decl->has_category, decl->category_name);
    decl->semantic_link_interface_symbol = BuildObjcContainerScopeOwner("interface", decl->name, false, "");
    if (decl->has_category) {
      decl->semantic_link_category_symbol = BuildObjcCategorySemanticLinkSymbol(decl->name, decl->category_name);
    }
    decl->scope_owner_symbol =
        BuildObjcContainerScopeOwner("implementation", decl->name, decl->has_category, decl->category_name);
    decl->scope_path_lexicographic =
        BuildScopePathLexicographic(decl->scope_owner_symbol, "implementation:" + decl->name);

    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.scope_owner_symbol = decl->scope_owner_symbol;
          property.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);
          decl->properties.push_back(std::move(property));
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "unsupported token inside @implementation declaration"));
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, true)) {
        method.scope_owner_symbol = decl->scope_owner_symbol;
        method.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);
        AssignObjcMethodLookupOverrideConflictSymbols(
            method, decl->semantic_link_symbol, decl->semantic_link_interface_symbol);
        decl->methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }

    FinalizeObjcMethodLookupOverrideConflictPackets(decl->methods,
                                                    decl->method_lookup_symbols_lexicographic,
                                                    decl->override_lookup_symbols_lexicographic,
                                                    decl->conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '@end' after @implementation"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  std::unique_ptr<FunctionDecl> ParseFunction() {
    auto fn = std::make_unique<FunctionDecl>();
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after 'fn'"
                                      : "unexpected qualifier 'extern' after 'fn'";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                      "invalid function identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    fn->name = Previous().text;
    fn->line = Previous().line;
    fn->column = Previous().column;
    fn->scope_owner_symbol = "global";
    fn->scope_path_lexicographic =
        BuildScopePathLexicographic(fn->scope_owner_symbol, "function:" + fn->name);

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after function name"
                                      : "unexpected qualifier 'extern' after function name";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after function name"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseFunctionParameters(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after parameters"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after parameter list"
                                      : "unexpected qualifier 'extern' after parameter list";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Minus)) {
      const Token arrow_start = Previous();
      if (!Match(TokenKind::Greater)) {
        diagnostics_.push_back(
            MakeDiag(arrow_start.line, arrow_start.column, "O3P114", "missing '>' in function return annotation"));
        SynchronizeFunctionTail();
        return nullptr;
      }
      if (!ParseFunctionReturnType(*fn)) {
        SynchronizeFunctionTail();
        return nullptr;
      }
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after function return annotation"
                                      : "unexpected qualifier 'extern' after function return annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Semicolon)) {
      fn->is_prototype = true;
      return fn;
    }

    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      if (At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern) || At(TokenKind::KwAtInterface) || At(TokenKind::KwAtImplementation) ||
          At(TokenKind::KwAtProtocol) || At(TokenKind::KwAtProperty) ||
          At(TokenKind::Eof)) {
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P104", "missing ';' after function prototype declaration"));
      } else {
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      }
      SynchronizeTopLevel();
      return nullptr;
    }

    fn->body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      SynchronizeTopLevel();
      return nullptr;
    }
    return fn;
  }

  bool ParseFunctionParameters(FunctionDecl &fn) {
    if (At(TokenKind::RParen)) {
      return true;
    }

    while (true) {
      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
        const Token qualifier = Advance();
        const std::string message = qualifier.kind == TokenKind::KwPure
                                        ? "unexpected qualifier 'pure' in parameter identifier position"
                                        : "unexpected qualifier 'extern' in parameter identifier position";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P101", "invalid parameter identifier"));
        return false;
      }

      FuncParam param;
      param.name = Advance().text;
      param.line = Previous().line;
      param.column = Previous().column;

      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
        const Token qualifier = Advance();
        const std::string message = qualifier.kind == TokenKind::KwPure
                                        ? "unexpected qualifier 'pure' after parameter name"
                                        : "unexpected qualifier 'extern' after parameter name";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P107", "missing ':' after parameter name"));
        return false;
      }
      if (!ParseParameterType(param)) {
        return false;
      }

      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
        const Token qualifier = Advance();
        const std::string message = qualifier.kind == TokenKind::KwPure
                                        ? "unexpected qualifier 'pure' after parameter type annotation"
                                        : "unexpected qualifier 'extern' after parameter type annotation";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      fn.params.push_back(std::move(param));
      if (!Match(TokenKind::Comma)) {
        return true;
      }
    }
  }

  bool ParseFunctionReturnType(FunctionDecl &fn) {
    fn.return_id_spelling = false;
    fn.return_class_spelling = false;
    fn.return_sel_spelling = false;
    fn.return_instancetype_spelling = false;
    fn.return_object_pointer_type_spelling = false;
    fn.return_object_pointer_type_name.clear();
    fn.return_typecheck_family_symbol.clear();
    fn.return_vector_spelling = false;
    fn.return_vector_base_spelling.clear();
    fn.return_vector_lane_count = 1;
    fn.has_return_generic_suffix = false;
    fn.return_generic_suffix_terminated = true;
    fn.return_generic_suffix_text.clear();
    fn.return_generic_line = 1;
    fn.return_generic_column = 1;
    fn.has_return_pointer_declarator = false;
    fn.return_pointer_declarator_depth = 0;
    fn.return_pointer_declarator_tokens.clear();
    fn.return_nullability_suffix_tokens.clear();
    fn.has_return_ownership_qualifier = false;
    fn.return_ownership_qualifier_spelling.clear();
    fn.return_ownership_qualifier_symbol.clear();
    fn.return_ownership_qualifier_tokens.clear();
    fn.return_ownership_insert_retain = false;
    fn.return_ownership_insert_release = false;
    fn.return_ownership_insert_autorelease = false;
    fn.return_ownership_operation_profile.clear();
    fn.return_ownership_is_weak_reference = false;
    fn.return_ownership_is_unowned_reference = false;
    fn.return_ownership_is_unowned_safe_reference = false;
    fn.return_ownership_lifetime_profile.clear();
    fn.return_ownership_runtime_hook_profile.clear();
    fn.return_ownership_arc_diagnostic_candidate = false;
    fn.return_ownership_arc_fixit_available = false;
    fn.return_ownership_arc_diagnostic_profile.clear();
    fn.return_ownership_arc_fixit_hint.clear();

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in function return type annotation"
                                      : "unexpected qualifier 'extern' in function return type annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    while (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      fn.has_return_ownership_qualifier = true;
      fn.return_ownership_qualifier_spelling = qualifier.text;
      fn.return_ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }

    if (Match(TokenKind::KwI32)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwBool)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwBOOL)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwNSInteger) || Match(TokenKind::KwNSUInteger)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwVoid)) {
      fn.return_type = ValueType::Void;
    } else if (Match(TokenKind::KwId)) {
      fn.return_type = ValueType::I32;
      fn.return_id_spelling = true;
    } else if (Match(TokenKind::KwClass)) {
      fn.return_type = ValueType::I32;
      fn.return_class_spelling = true;
    } else if (Match(TokenKind::KwSEL)) {
      fn.return_type = ValueType::I32;
      fn.return_sel_spelling = true;
    } else if (Match(TokenKind::KwProtocol)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwInstancetype)) {
      fn.return_type = ValueType::I32;
      fn.return_instancetype_spelling = true;
    } else {
      if (At(TokenKind::Identifier)) {
        const Token type_token = Advance();
        ValueType vector_type = ValueType::Unknown;
        std::string vector_base_spelling;
        unsigned vector_lane_count = 1;
        if (TryParseVectorTypeSpelling(type_token, vector_type, vector_base_spelling, vector_lane_count)) {
          fn.return_type = vector_type;
          fn.return_vector_spelling = true;
          fn.return_vector_base_spelling = vector_base_spelling;
          fn.return_vector_lane_count = vector_lane_count;
        } else {
          fn.return_type = ValueType::I32;
          fn.return_object_pointer_type_spelling = true;
          fn.return_object_pointer_type_name = type_token.text;
        }
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P114",
                     "expected function return type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', 'void', 'id', "
                     "'Class', 'SEL', 'Protocol', 'instancetype', object pointer spelling, ownership qualifiers "
                     "'__strong/__weak/__autoreleasing/__unsafe_unretained', or vector forms "
                     "'i32x2/i32x4/i32x8/i32x16' and 'boolx2/boolx4/boolx8/boolx16'"));
        return false;
      }
    }

    fn.return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(fn);

    bool parsed_generic_suffix = false;
    while (true) {
      if (At(TokenKind::Less) && !parsed_generic_suffix) {
        Match(TokenKind::Less);
        const Token &open = Previous();
        fn.has_return_generic_suffix = true;
        fn.return_generic_suffix_terminated = false;
        fn.return_generic_line = open.line;
        fn.return_generic_column = open.column;
        fn.return_generic_suffix_text = "<";
        int depth = 1;
        while (depth > 0 && !At(TokenKind::Eof)) {
          if (Match(TokenKind::Less)) {
            fn.return_generic_suffix_text += "<";
            ++depth;
            continue;
          }
          if (Match(TokenKind::Greater)) {
            fn.return_generic_suffix_text += ">";
            --depth;
            if (depth == 0) {
              fn.return_generic_suffix_terminated = true;
            }
            continue;
          }
          fn.return_generic_suffix_text += Advance().text;
        }
        if (!fn.return_generic_suffix_terminated) {
          diagnostics_.push_back(
              MakeDiag(fn.return_generic_line, fn.return_generic_column, "O3P114",
                       "unterminated generic function return type suffix"));
          return false;
        }
        parsed_generic_suffix = true;
        continue;
      }

      if (Match(TokenKind::Star)) {
        fn.has_return_pointer_declarator = true;
        fn.return_pointer_declarator_depth += 1;
        fn.return_pointer_declarator_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));
        continue;
      }

      if (At(TokenKind::Question) || At(TokenKind::Bang)) {
        fn.return_nullability_suffix_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));
        continue;
      }

      if (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
        const Token qualifier = Advance();
        fn.has_return_ownership_qualifier = true;
        fn.return_ownership_qualifier_spelling = qualifier.text;
        fn.return_ownership_qualifier_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
        continue;
      }

      break;
    }

    fn.return_ownership_qualifier_symbol =
        BuildOwnershipQualifierSymbol(fn.return_ownership_qualifier_spelling, true);
    const Objc3OwnershipOperationProfile return_ownership_profile =
        BuildReturnOwnershipOperationProfile(fn.return_ownership_qualifier_spelling);
    fn.return_ownership_insert_retain = return_ownership_profile.insert_retain;
    fn.return_ownership_insert_release = return_ownership_profile.insert_release;
    fn.return_ownership_insert_autorelease = return_ownership_profile.insert_autorelease;
    fn.return_ownership_operation_profile = return_ownership_profile.profile;
    const Objc3WeakUnownedLifetimeProfile return_lifetime_profile =
        BuildWeakUnownedLifetimeProfile(fn.return_ownership_qualifier_spelling, false);
    fn.return_ownership_is_weak_reference = return_lifetime_profile.is_weak_reference;
    fn.return_ownership_is_unowned_reference = return_lifetime_profile.is_unowned_reference;
    fn.return_ownership_is_unowned_safe_reference = return_lifetime_profile.is_unowned_safe_reference;
    fn.return_ownership_lifetime_profile = return_lifetime_profile.lifetime_profile;
    fn.return_ownership_runtime_hook_profile = return_lifetime_profile.runtime_hook_profile;
    const Objc3ArcDiagnosticFixitProfile return_arc_diagnostic_profile =
        BuildArcDiagnosticFixitProfile(fn.return_ownership_qualifier_spelling, true, false, false);
    fn.return_ownership_arc_diagnostic_candidate = return_arc_diagnostic_profile.diagnostic_candidate;
    fn.return_ownership_arc_fixit_available = return_arc_diagnostic_profile.fixit_available;
    fn.return_ownership_arc_diagnostic_profile = return_arc_diagnostic_profile.diagnostic_profile;
    fn.return_ownership_arc_fixit_hint = return_arc_diagnostic_profile.fixit_hint;

    return true;
  }

  bool ParseParameterType(FuncParam &param) {
    param.vector_spelling = false;
    param.vector_base_spelling.clear();
    param.vector_lane_count = 1;
    param.id_spelling = false;
    param.class_spelling = false;
    param.sel_spelling = false;
    param.instancetype_spelling = false;
    param.object_pointer_type_spelling = false;
    param.object_pointer_type_name.clear();
    param.typecheck_family_symbol.clear();
    param.has_generic_suffix = false;
    param.generic_suffix_terminated = true;
    param.generic_suffix_text.clear();
    param.generic_line = 1;
    param.generic_column = 1;
    param.has_pointer_declarator = false;
    param.pointer_declarator_depth = 0;
    param.pointer_declarator_tokens.clear();
    param.nullability_suffix_tokens.clear();
    param.has_ownership_qualifier = false;
    param.ownership_qualifier_spelling.clear();
    param.ownership_qualifier_symbol.clear();
    param.ownership_qualifier_tokens.clear();
    param.ownership_insert_retain = false;
    param.ownership_insert_release = false;
    param.ownership_insert_autorelease = false;
    param.ownership_operation_profile.clear();
    param.ownership_is_weak_reference = false;
    param.ownership_is_unowned_reference = false;
    param.ownership_is_unowned_safe_reference = false;
    param.ownership_lifetime_profile.clear();
    param.ownership_runtime_hook_profile.clear();
    param.ownership_arc_diagnostic_candidate = false;
    param.ownership_arc_fixit_available = false;
    param.ownership_arc_diagnostic_profile.clear();
    param.ownership_arc_fixit_hint.clear();
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in parameter type annotation"
                                      : "unexpected qualifier 'extern' in parameter type annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    while (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      param.has_ownership_qualifier = true;
      param.ownership_qualifier_spelling = qualifier.text;
      param.ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }

    if (Match(TokenKind::KwI32)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwBool)) {
      param.type = ValueType::Bool;
    } else if (Match(TokenKind::KwBOOL)) {
      param.type = ValueType::Bool;
    } else if (Match(TokenKind::KwNSInteger) || Match(TokenKind::KwNSUInteger)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwId)) {
      param.type = ValueType::I32;
      param.id_spelling = true;
    } else if (Match(TokenKind::KwClass)) {
      param.type = ValueType::I32;
      param.class_spelling = true;
    } else if (Match(TokenKind::KwSEL)) {
      param.type = ValueType::I32;
      param.sel_spelling = true;
    } else if (Match(TokenKind::KwProtocol)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwInstancetype)) {
      param.type = ValueType::I32;
      param.instancetype_spelling = true;
    } else if (At(TokenKind::Identifier)) {
      const Token type_token = Advance();
      ValueType vector_type = ValueType::Unknown;
      std::string vector_base_spelling;
      unsigned vector_lane_count = 1;
      if (TryParseVectorTypeSpelling(type_token, vector_type, vector_base_spelling, vector_lane_count)) {
        param.type = vector_type;
        param.vector_spelling = true;
        param.vector_base_spelling = vector_base_spelling;
        param.vector_lane_count = vector_lane_count;
        ParseParameterTypeSuffix(param);
        if (!param.generic_suffix_terminated) {
          return false;
        }
        return true;
      }
      param.type = ValueType::I32;
      param.object_pointer_type_spelling = true;
      param.object_pointer_type_name = type_token.text;
    } else {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P108",
                                      "expected parameter type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', or "
                                      "'id', 'Class', 'SEL', 'Protocol', 'instancetype', object pointer spelling, "
                                      "ownership qualifiers '__strong/__weak/__autoreleasing/__unsafe_unretained', "
                                      "or vector forms "
                                      "'i32x2/i32x4/i32x8/i32x16' and 'boolx2/boolx4/boolx8/boolx16'"));
      return false;
    }

    param.typecheck_family_symbol = BuildObjcTypecheckParamFamilySymbol(param);

    ParseParameterTypeSuffix(param);
    if (!param.generic_suffix_terminated) {
      return false;
    }

    param.ownership_qualifier_symbol = BuildOwnershipQualifierSymbol(param.ownership_qualifier_spelling, false);
    const Objc3OwnershipOperationProfile param_ownership_profile =
        BuildParamOwnershipOperationProfile(param.ownership_qualifier_spelling);
    param.ownership_insert_retain = param_ownership_profile.insert_retain;
    param.ownership_insert_release = param_ownership_profile.insert_release;
    param.ownership_insert_autorelease = param_ownership_profile.insert_autorelease;
    param.ownership_operation_profile = param_ownership_profile.profile;
    const Objc3WeakUnownedLifetimeProfile param_lifetime_profile =
        BuildWeakUnownedLifetimeProfile(param.ownership_qualifier_spelling, false);
    param.ownership_is_weak_reference = param_lifetime_profile.is_weak_reference;
    param.ownership_is_unowned_reference = param_lifetime_profile.is_unowned_reference;
    param.ownership_is_unowned_safe_reference = param_lifetime_profile.is_unowned_safe_reference;
    param.ownership_lifetime_profile = param_lifetime_profile.lifetime_profile;
    param.ownership_runtime_hook_profile = param_lifetime_profile.runtime_hook_profile;
    const Objc3ArcDiagnosticFixitProfile param_arc_diagnostic_profile =
        BuildArcDiagnosticFixitProfile(param.ownership_qualifier_spelling, false, false, false);
    param.ownership_arc_diagnostic_candidate = param_arc_diagnostic_profile.diagnostic_candidate;
    param.ownership_arc_fixit_available = param_arc_diagnostic_profile.fixit_available;
    param.ownership_arc_diagnostic_profile = param_arc_diagnostic_profile.diagnostic_profile;
    param.ownership_arc_fixit_hint = param_arc_diagnostic_profile.fixit_hint;

    return true;
  }

  void ParseParameterTypeSuffix(FuncParam &param) {
    bool parsed_generic_suffix = false;
    while (true) {
      if (At(TokenKind::Less) && !parsed_generic_suffix) {
        Match(TokenKind::Less);
        const Token &open = Previous();
        param.has_generic_suffix = true;
        param.generic_suffix_terminated = false;
        param.generic_line = open.line;
        param.generic_column = open.column;
        param.generic_suffix_text = "<";
        int depth = 1;
        while (depth > 0 && !At(TokenKind::Eof)) {
          if (Match(TokenKind::Less)) {
            param.generic_suffix_text += "<";
            ++depth;
            continue;
          }
          if (Match(TokenKind::Greater)) {
            param.generic_suffix_text += ">";
            --depth;
            if (depth == 0) {
              param.generic_suffix_terminated = true;
            }
            continue;
          }
          param.generic_suffix_text += Advance().text;
        }
        if (!param.generic_suffix_terminated) {
          diagnostics_.push_back(MakeDiag(open.line, open.column, "O3P108",
                                          "unterminated generic parameter type suffix"));
          return;
        }
        parsed_generic_suffix = true;
        continue;
      }

      if (Match(TokenKind::Star)) {
        param.has_pointer_declarator = true;
        param.pointer_declarator_depth += 1;
        param.pointer_declarator_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));
        continue;
      }

      if (At(TokenKind::Question) || At(TokenKind::Bang)) {
        param.nullability_suffix_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));
        continue;
      }

      if (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
        const Token qualifier = Advance();
        param.has_ownership_qualifier = true;
        param.ownership_qualifier_spelling = qualifier.text;
        param.ownership_qualifier_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
        continue;
      }

      break;
    }
  }

  std::vector<std::unique_ptr<Stmt>> ParseBlock() {
    std::vector<std::unique_ptr<Stmt>> body;
    if (!Match(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      block_failed_ = true;
      return {};
    }

    while (!At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
      auto stmt = ParseStatement();
      if (stmt != nullptr) {
        body.push_back(std::move(stmt));
      } else {
        SynchronizeStatement();
      }
    }

    if (!Match(TokenKind::RBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '}' to end block"));
      block_failed_ = true;
      return {};
    }

    return body;
  }

  std::vector<std::unique_ptr<Stmt>> ParseControlBody() {
    if (At(TokenKind::LBrace)) {
      return ParseBlock();
    }
    std::vector<std::unique_ptr<Stmt>> body;
    auto stmt = ParseStatement();
    if (stmt == nullptr) {
      block_failed_ = true;
      return {};
    }
    body.push_back(std::move(stmt));
    return body;
  }

  void SynchronizeTopLevel() {
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::Semicolon)) {
        return;
      }
      if (At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern) || At(TokenKind::KwAtInterface) || At(TokenKind::KwAtImplementation) ||
          At(TokenKind::KwAtProtocol) || At(TokenKind::KwAtProperty)) {
        return;
      }
      Advance();
    }
  }

  void SynchronizeFunctionTail() {
    if (At(TokenKind::LBrace)) {
      int depth = 0;
      while (!At(TokenKind::Eof)) {
        if (Match(TokenKind::LBrace)) {
          ++depth;
          continue;
        }
        if (Match(TokenKind::RBrace)) {
          --depth;
          if (depth <= 0) {
            return;
          }
          continue;
        }
        Advance();
      }
      return;
    }
    SynchronizeTopLevel();
  }

  void SynchronizeStatement() {
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::Semicolon)) {
        return;
      }
      if (At(TokenKind::KwLet) || At(TokenKind::KwReturn) || At(TokenKind::KwIf) || At(TokenKind::KwDo) ||
          At(TokenKind::KwFor) || At(TokenKind::KwSwitch) || At(TokenKind::KwWhile) || At(TokenKind::KwBreak) ||
          At(TokenKind::KwContinue) || At(TokenKind::KwAtAutoreleasePool) || AtIdentifierAssignment() ||
          AtIdentifierUpdate() || AtPrefixUpdate() || At(TokenKind::RBrace)) {
        return;
      }
      Advance();
    }
  }

  std::unique_ptr<Stmt> ParseStatement() {
    if (At(TokenKind::LBrace)) {
      const Token open = Peek();
      auto body = ParseBlock();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Block;
      stmt->line = open.line;
      stmt->column = open.column;
      stmt->block_stmt = std::make_unique<BlockStmt>();
      stmt->block_stmt->line = open.line;
      stmt->block_stmt->column = open.column;
      stmt->block_stmt->body = std::move(body);
      return stmt;
    }

    if (Match(TokenKind::Semicolon)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Empty;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      return stmt;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in statement position"
                                      : "unexpected qualifier 'extern' in statement position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return nullptr;
    }

    if (Match(TokenKind::KwAtAutoreleasePool)) {
      const Token marker = Previous();
      const unsigned scope_depth = autoreleasepool_scope_depth_ + 1u;
      ++autoreleasepool_scope_depth_;
      const unsigned scope_serial = ++autoreleasepool_scope_serial_;
      auto body = ParseBlock();
      --autoreleasepool_scope_depth_;
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }

      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Block;
      stmt->line = marker.line;
      stmt->column = marker.column;
      stmt->block_stmt = std::make_unique<BlockStmt>();
      stmt->block_stmt->line = marker.line;
      stmt->block_stmt->column = marker.column;
      stmt->block_stmt->body = std::move(body);
      stmt->block_stmt->is_autoreleasepool_scope = true;
      stmt->block_stmt->autoreleasepool_scope_depth = scope_depth;
      stmt->block_stmt->autoreleasepool_scope_symbol =
          BuildAutoreleasePoolScopeSymbol(scope_serial, scope_depth);
      return stmt;
    }

    if (Match(TokenKind::KwLet)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Let;
      stmt->let_stmt = std::make_unique<LetStmt>();
      const Token &name_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                        "invalid declaration identifier"));
        return nullptr;
      }
      stmt->let_stmt->name = Previous().text;
      stmt->let_stmt->line = Previous().line;
      stmt->let_stmt->column = Previous().column;
      stmt->line = Previous().line;
      stmt->column = Previous().column;

      if (!Match(TokenKind::Equal)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
        return nullptr;
      }

      stmt->let_stmt->value = ParseExpression();
      if (stmt->let_stmt->value == nullptr) {
        return nullptr;
      }

      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104",
                                        "missing ';' after declaration"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwReturn)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Return;
      stmt->return_stmt = std::make_unique<ReturnStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->return_stmt->line = Previous().line;
      stmt->return_stmt->column = Previous().column;
      if (Match(TokenKind::Semicolon)) {
        return stmt;
      }
      stmt->return_stmt->value = ParseExpression();
      if (stmt->return_stmt->value == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after return"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwIf)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::If;
      stmt->if_stmt = std::make_unique<IfStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->if_stmt->line = Previous().line;
      stmt->if_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after if"));
        return nullptr;
      }
      stmt->if_stmt->condition = ParseExpression();
      if (stmt->if_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after if condition"));
        return nullptr;
      }

      stmt->if_stmt->then_body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      if (Match(TokenKind::KwElse)) {
        stmt->if_stmt->else_body = ParseControlBody();
        if (block_failed_) {
          block_failed_ = false;
          return nullptr;
        }
      }
      return stmt;
    }

    if (Match(TokenKind::KwDo)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::DoWhile;
      stmt->do_while_stmt = std::make_unique<DoWhileStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->do_while_stmt->line = Previous().line;
      stmt->do_while_stmt->column = Previous().column;

      stmt->do_while_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }

      if (!Match(TokenKind::KwWhile)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", "missing 'while' after do block"));
        return nullptr;
      }
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after while"));
        return nullptr;
      }
      stmt->do_while_stmt->condition = ParseExpression();
      if (stmt->do_while_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after do-while condition"));
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after do-while"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwFor)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::For;
      stmt->for_stmt = std::make_unique<ForStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->for_stmt->line = Previous().line;
      stmt->for_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after for"));
        return nullptr;
      }

      if (Match(TokenKind::Semicolon)) {
        stmt->for_stmt->init.kind = ForClause::Kind::None;
      } else {
        if (Match(TokenKind::KwLet)) {
          stmt->for_stmt->init.kind = ForClause::Kind::Let;
          const Token &name_token = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                            "invalid declaration identifier"));
            return nullptr;
          }
          stmt->for_stmt->init.name = Previous().text;
          stmt->for_stmt->init.line = Previous().line;
          stmt->for_stmt->init.column = Previous().column;

          if (!Match(TokenKind::Equal)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
            return nullptr;
          }

          stmt->for_stmt->init.value = ParseExpression();
          if (stmt->for_stmt->init.value == nullptr) {
            return nullptr;
          }
        } else if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
          stmt->for_stmt->init.kind = ForClause::Kind::Assign;
          const Token name = Advance();
          std::string op = "=";
          if (!MatchAssignmentOperator(op)) {
            (void)MatchUpdateOperator(op);
          }
          stmt->for_stmt->init.name = name.text;
          stmt->for_stmt->init.op = op;
          stmt->for_stmt->init.line = name.line;
          stmt->for_stmt->init.column = name.column;
          if (op == "++" || op == "--") {
            stmt->for_stmt->init.value = nullptr;
          } else {
            stmt->for_stmt->init.value = ParseExpression();
            if (stmt->for_stmt->init.value == nullptr) {
              return nullptr;
            }
          }
        } else if (AtPrefixUpdate()) {
          stmt->for_stmt->init.kind = ForClause::Kind::Assign;
          std::string op = "++";
          (void)MatchUpdateOperator(op);
          const Token name = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name.line, name.column, "O3P101",
                                            "invalid assignment target"));
            return nullptr;
          }
          stmt->for_stmt->init.name = name.text;
          stmt->for_stmt->init.op = op;
          stmt->for_stmt->init.line = name.line;
          stmt->for_stmt->init.column = name.column;
          stmt->for_stmt->init.value = nullptr;
        } else {
          stmt->for_stmt->init.kind = ForClause::Kind::Expr;
          stmt->for_stmt->init.line = Peek().line;
          stmt->for_stmt->init.column = Peek().column;
          stmt->for_stmt->init.value = ParseExpression();
          if (stmt->for_stmt->init.value == nullptr) {
            return nullptr;
          }
        }
        if (!Match(TokenKind::Semicolon)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after for init"));
          return nullptr;
        }
      }

      if (Match(TokenKind::Semicolon)) {
        stmt->for_stmt->condition = nullptr;
      } else {
        stmt->for_stmt->condition = ParseExpression();
        if (stmt->for_stmt->condition == nullptr) {
          return nullptr;
        }
        if (!Match(TokenKind::Semicolon)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after for condition"));
          return nullptr;
        }
      }

      if (Match(TokenKind::RParen)) {
        stmt->for_stmt->step.kind = ForClause::Kind::None;
      } else {
        if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
          stmt->for_stmt->step.kind = ForClause::Kind::Assign;
          const Token name = Advance();
          std::string op = "=";
          if (!MatchAssignmentOperator(op)) {
            (void)MatchUpdateOperator(op);
          }
          stmt->for_stmt->step.name = name.text;
          stmt->for_stmt->step.op = op;
          stmt->for_stmt->step.line = name.line;
          stmt->for_stmt->step.column = name.column;
          if (op == "++" || op == "--") {
            stmt->for_stmt->step.value = nullptr;
          } else {
            stmt->for_stmt->step.value = ParseExpression();
            if (stmt->for_stmt->step.value == nullptr) {
              return nullptr;
            }
          }
        } else if (AtPrefixUpdate()) {
          stmt->for_stmt->step.kind = ForClause::Kind::Assign;
          std::string op = "++";
          (void)MatchUpdateOperator(op);
          const Token name = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name.line, name.column, "O3P101",
                                            "invalid assignment target"));
            return nullptr;
          }
          stmt->for_stmt->step.name = name.text;
          stmt->for_stmt->step.op = op;
          stmt->for_stmt->step.line = name.line;
          stmt->for_stmt->step.column = name.column;
          stmt->for_stmt->step.value = nullptr;
        } else {
          stmt->for_stmt->step.kind = ForClause::Kind::Expr;
          stmt->for_stmt->step.line = Peek().line;
          stmt->for_stmt->step.column = Peek().column;
          stmt->for_stmt->step.value = ParseExpression();
          if (stmt->for_stmt->step.value == nullptr) {
            return nullptr;
          }
        }
        if (!Match(TokenKind::RParen)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after for clauses"));
          return nullptr;
        }
      }

      stmt->for_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwSwitch)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Switch;
      stmt->switch_stmt = std::make_unique<SwitchStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->switch_stmt->line = Previous().line;
      stmt->switch_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after switch"));
        return nullptr;
      }
      stmt->switch_stmt->condition = ParseExpression();
      if (stmt->switch_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after switch condition"));
        return nullptr;
      }
      if (!Match(TokenKind::LBrace)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' for switch body"));
        return nullptr;
      }

      while (!At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
        if (Match(TokenKind::KwCase)) {
          SwitchCase case_stmt;
          case_stmt.line = Previous().line;
          case_stmt.column = Previous().column;
          case_stmt.is_default = false;

          if (Match(TokenKind::Number)) {
            case_stmt.value_line = Previous().line;
            case_stmt.value_column = Previous().column;
            case_stmt.value = std::atoi(Previous().text.c_str());
          } else if (Match(TokenKind::Minus) || Match(TokenKind::Plus)) {
            const Token sign = Previous();
            if (!Match(TokenKind::Number)) {
              diagnostics_.push_back(MakeDiag(sign.line, sign.column, "O3P103", "invalid case label expression"));
              return nullptr;
            }
            case_stmt.value_line = sign.line;
            case_stmt.value_column = sign.column;
            const int magnitude = std::atoi(Previous().text.c_str());
            case_stmt.value = sign.kind == TokenKind::Minus ? -magnitude : magnitude;
          } else if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
            case_stmt.value_line = Previous().line;
            case_stmt.value_column = Previous().column;
            case_stmt.value = Previous().kind == TokenKind::KwTrue ? 1 : 0;
          } else if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
            const Token qualifier = Advance();
            const std::string message = qualifier.kind == TokenKind::KwPure
                                            ? "unexpected qualifier 'pure' in case label expression"
                                            : "unexpected qualifier 'extern' in case label expression";
            diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
            return nullptr;
          } else {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid case label expression"));
            return nullptr;
          }

          if (!Match(TokenKind::Colon)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' after case label"));
            return nullptr;
          }

          while (!At(TokenKind::KwCase) && !At(TokenKind::KwDefault) && !At(TokenKind::RBrace) &&
                 !At(TokenKind::Eof)) {
            std::unique_ptr<Stmt> body_stmt = ParseStatement();
            if (body_stmt != nullptr) {
              case_stmt.body.push_back(std::move(body_stmt));
              continue;
            }
            SynchronizeStatement();
            if (At(TokenKind::Eof)) {
              break;
            }
          }
          stmt->switch_stmt->cases.push_back(std::move(case_stmt));
          continue;
        }

        if (Match(TokenKind::KwDefault)) {
          SwitchCase default_stmt;
          default_stmt.line = Previous().line;
          default_stmt.column = Previous().column;
          default_stmt.is_default = true;
          default_stmt.value = 0;
          default_stmt.value_line = Previous().line;
          default_stmt.value_column = Previous().column;

          if (!Match(TokenKind::Colon)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' after default"));
            return nullptr;
          }

          while (!At(TokenKind::KwCase) && !At(TokenKind::KwDefault) && !At(TokenKind::RBrace) &&
                 !At(TokenKind::Eof)) {
            std::unique_ptr<Stmt> body_stmt = ParseStatement();
            if (body_stmt != nullptr) {
              default_stmt.body.push_back(std::move(body_stmt));
              continue;
            }
            SynchronizeStatement();
            if (At(TokenKind::Eof)) {
              break;
            }
          }
          stmt->switch_stmt->cases.push_back(std::move(default_stmt));
          continue;
        }

        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "expected 'case' or 'default' in switch body"));
        Advance();
      }

      if (!Match(TokenKind::RBrace)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '}' after switch body"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwWhile)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::While;
      stmt->while_stmt = std::make_unique<WhileStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->while_stmt->line = Previous().line;
      stmt->while_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after while"));
        return nullptr;
      }
      stmt->while_stmt->condition = ParseExpression();
      if (stmt->while_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after while condition"));
        return nullptr;
      }

      stmt->while_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwBreak)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Break;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after break"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwContinue)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Continue;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after continue"));
        return nullptr;
      }
      return stmt;
    }

    if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Assign;
      stmt->assign_stmt = std::make_unique<AssignStmt>();
      const Token name = Advance();
      std::string op = "=";
      if (!MatchAssignmentOperator(op)) {
        (void)MatchUpdateOperator(op);
      }
      stmt->line = name.line;
      stmt->column = name.column;
      stmt->assign_stmt->line = name.line;
      stmt->assign_stmt->column = name.column;
      stmt->assign_stmt->name = name.text;
      stmt->assign_stmt->op = op;
      if (op == "++" || op == "--") {
        stmt->assign_stmt->value = nullptr;
      } else {
        stmt->assign_stmt->value = ParseExpression();
        if (stmt->assign_stmt->value == nullptr) {
          return nullptr;
        }
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after assignment"));
        return nullptr;
      }
      return stmt;
    }

    if (AtPrefixUpdate()) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Assign;
      stmt->assign_stmt = std::make_unique<AssignStmt>();
      std::string op = "++";
      const Token op_token = Peek();
      (void)MatchUpdateOperator(op);
      const Token name = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(op_token.line, op_token.column, "O3P101", "invalid assignment target"));
        return nullptr;
      }
      stmt->line = name.line;
      stmt->column = name.column;
      stmt->assign_stmt->line = name.line;
      stmt->assign_stmt->column = name.column;
      stmt->assign_stmt->name = name.text;
      stmt->assign_stmt->op = op;
      stmt->assign_stmt->value = nullptr;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after assignment"));
        return nullptr;
      }
      return stmt;
    }

    auto stmt = std::make_unique<Stmt>();
    stmt->kind = Stmt::Kind::Expr;
    stmt->expr_stmt = std::make_unique<ExprStmt>();
    stmt->line = Peek().line;
    stmt->column = Peek().column;
    stmt->expr_stmt->line = Peek().line;
    stmt->expr_stmt->column = Peek().column;
    stmt->expr_stmt->value = ParseExpression();
    if (stmt->expr_stmt->value == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after expression"));
      return nullptr;
    }
    return stmt;
  }

  std::unique_ptr<Expr> ParseExpression() { return ParseConditional(); }

  std::unique_ptr<Expr> ParseConditional() {
    auto expr = ParseLogicalOr();
    if (expr == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Question)) {
      return expr;
    }

    const Token question = Previous();
    auto when_true = ParseExpression();
    if (when_true == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Colon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' in conditional expression"));
      return nullptr;
    }
    auto when_false = ParseConditional();
    if (when_false == nullptr) {
      return nullptr;
    }

    auto node = std::make_unique<Expr>();
    node->kind = Expr::Kind::Conditional;
    node->line = question.line;
    node->column = question.column;
    node->left = std::move(expr);
    node->right = std::move(when_true);
    node->third = std::move(when_false);
    return node;
  }

  std::unique_ptr<Expr> ParseLogicalOr() {
    auto expr = ParseLogicalAnd();
    while (expr != nullptr && Match(TokenKind::OrOr)) {
      const Token op = Previous();
      auto rhs = ParseLogicalAnd();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseLogicalAnd() {
    auto expr = ParseBitwiseOr();
    while (expr != nullptr && Match(TokenKind::AndAnd)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseOr();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseOr() {
    auto expr = ParseBitwiseXor();
    while (expr != nullptr && Match(TokenKind::Pipe)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseXor();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseXor() {
    auto expr = ParseBitwiseAnd();
    while (expr != nullptr && Match(TokenKind::Caret)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseAnd();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseAnd() {
    auto expr = ParseEquality();
    while (expr != nullptr && Match(TokenKind::Ampersand)) {
      const Token op = Previous();
      auto rhs = ParseEquality();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseEquality() {
    auto expr = ParseRelational();
    while (expr != nullptr && (Match(TokenKind::EqualEqual) || Match(TokenKind::BangEqual))) {
      const Token op = Previous();
      auto rhs = ParseRelational();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseRelational() {
    auto expr = ParseShift();
    while (expr != nullptr &&
           (Match(TokenKind::Less) || Match(TokenKind::LessEqual) || Match(TokenKind::Greater) ||
            Match(TokenKind::GreaterEqual))) {
      const Token op = Previous();
      auto rhs = ParseShift();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseShift() {
    auto expr = ParseAdditive();
    while (expr != nullptr && (Match(TokenKind::LessLess) || Match(TokenKind::GreaterGreater))) {
      const Token op = Previous();
      auto rhs = ParseAdditive();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseAdditive() {
    auto expr = ParseMultiplicative();
    while (expr != nullptr && (At(TokenKind::Plus) || At(TokenKind::Minus))) {
      const Token op = Advance();
      auto rhs = ParseMultiplicative();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseMultiplicative() {
    auto expr = ParseUnary();
    while (expr != nullptr && (At(TokenKind::Star) || At(TokenKind::Slash) || At(TokenKind::Percent))) {
      const Token op = Advance();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseUnary() {
    if (Match(TokenKind::Bang)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto zero = std::make_unique<Expr>();
      zero->kind = Expr::Kind::Number;
      zero->number = 0;
      zero->line = op.line;
      zero->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "==";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(rhs);
      node->right = std::move(zero);
      return node;
    }
    if (Match(TokenKind::Plus)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto zero = std::make_unique<Expr>();
      zero->kind = Expr::Kind::Number;
      zero->number = 0;
      zero->line = op.line;
      zero->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "+";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(zero);
      node->right = std::move(rhs);
      return node;
    }
    if (Match(TokenKind::Minus)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto zero = std::make_unique<Expr>();
      zero->kind = Expr::Kind::Number;
      zero->number = 0;
      zero->line = op.line;
      zero->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "-";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(zero);
      node->right = std::move(rhs);
      return node;
    }
    if (Match(TokenKind::Tilde)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto minus_one = std::make_unique<Expr>();
      minus_one->kind = Expr::Kind::Number;
      minus_one->number = -1;
      minus_one->line = op.line;
      minus_one->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "^";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(rhs);
      node->right = std::move(minus_one);
      return node;
    }
    return ParsePostfix();
  }

  std::unique_ptr<Expr> ParsePostfix() {
    auto expr = ParsePrimary();
    while (expr != nullptr && Match(TokenKind::LParen)) {
      const unsigned callee_line = expr->line;
      const unsigned callee_column = expr->column;
      auto call = std::make_unique<Expr>();
      call->kind = Expr::Kind::Call;
      call->line = callee_line;
      call->column = callee_column;
      if (expr->kind != Expr::Kind::Identifier) {
        diagnostics_.push_back(MakeDiag(expr->line, expr->column, "O3P112", "call target must be identifier"));
        return nullptr;
      }
      call->ident = expr->ident;
      if (!At(TokenKind::RParen)) {
        while (true) {
          auto arg = ParseExpression();
          if (arg == nullptr) {
            return nullptr;
          }
          call->args.push_back(std::move(arg));
          if (!Match(TokenKind::Comma)) {
            break;
          }
        }
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after call"));
        return nullptr;
      }
      expr = std::move(call);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParsePrimary() {
    if (Match(TokenKind::Number)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::Number;
      expr->line = Previous().line;
      expr->column = Previous().column;
      if (!ParseIntegerLiteralValue(Previous().text, expr->number)) {
        diagnostics_.push_back(MakeDiag(expr->line, expr->column, "O3P103",
                                        "invalid numeric literal '" + Previous().text + "'"));
        return nullptr;
      }
      return expr;
    }
    if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::BoolLiteral;
      expr->line = Previous().line;
      expr->column = Previous().column;
      expr->bool_value = Previous().kind == TokenKind::KwTrue;
      return expr;
    }
    if (Match(TokenKind::KwNil)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::NilLiteral;
      expr->line = Previous().line;
      expr->column = Previous().column;
      return expr;
    }
    if (Match(TokenKind::Identifier)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::Identifier;
      expr->line = Previous().line;
      expr->column = Previous().column;
      expr->ident = Previous().text;
      return expr;
    }
    if (Match(TokenKind::LParen)) {
      auto expr = ParseExpression();
      if (expr == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after expression"));
        return nullptr;
      }
      return expr;
    }

    if (Match(TokenKind::LBracket)) {
      return ParseMessageSendExpression();
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in expression position"
                                      : "unexpected qualifier 'extern' in expression position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return nullptr;
    }

    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid expression"));
    return nullptr;
  }

  std::unique_ptr<Expr> ParseMessageSendExpression() {
    const Token open = Previous();
    auto message = std::make_unique<Expr>();
    message->kind = Expr::Kind::MessageSend;
    message->line = open.line;
    message->column = open.column;

    const std::size_t receiver_diag_count = diagnostics_.size();
    message->receiver = ParsePostfix();
    if (message->receiver == nullptr) {
      if (diagnostics_.size() == receiver_diag_count) {
        diagnostics_.push_back(MakeDiag(open.line, open.column, "O3P113",
                                        "invalid receiver expression in message send"));
      }
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message_text =
          qualifier.kind == TokenKind::KwPure
              ? "unexpected qualifier 'pure' in message selector position"
              : "unexpected qualifier 'extern' in message selector position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message_text));
      return nullptr;
    }

    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                      "expected selector identifier in message send"));
      return nullptr;
    }

    const Token selector_head = Advance();
    message->selector = selector_head.text;
    Expr::MessageSendSelectorPiece head_piece;
    head_piece.keyword = selector_head.text;
    head_piece.line = selector_head.line;
    head_piece.column = selector_head.column;
    if (Match(TokenKind::Colon)) {
      message->message_send_form = Expr::MessageSendForm::Keyword;
      head_piece.has_argument = true;
      message->selector_lowering_pieces.push_back(head_piece);
      message->selector += ":";
      auto first_arg = ParseExpression();
      if (first_arg == nullptr) {
        return nullptr;
      }
      message->args.push_back(std::move(first_arg));

      while (true) {
        if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
          const Token qualifier = Advance();
          const std::string message_text =
              qualifier.kind == TokenKind::KwPure
                  ? "unexpected qualifier 'pure' in keyword selector segment position"
                  : "unexpected qualifier 'extern' in keyword selector segment position";
          diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message_text));
          return nullptr;
        }
        if (!At(TokenKind::Identifier)) {
          break;
        }
        const Token keyword = Advance();
        if (!Match(TokenKind::Colon)) {
          diagnostics_.push_back(MakeDiag(keyword.line, keyword.column, "O3P113",
                                          "missing ':' in keyword selector segment"));
          return nullptr;
        }
        Expr::MessageSendSelectorPiece keyword_piece;
        keyword_piece.keyword = keyword.text;
        keyword_piece.has_argument = true;
        keyword_piece.line = keyword.line;
        keyword_piece.column = keyword.column;
        message->selector_lowering_pieces.push_back(std::move(keyword_piece));
        message->selector += keyword.text;
        message->selector += ":";
        auto arg = ParseExpression();
        if (arg == nullptr) {
          return nullptr;
        }
        message->args.push_back(std::move(arg));
      }
    } else {
      message->message_send_form = Expr::MessageSendForm::Unary;
      message->selector_lowering_pieces.push_back(head_piece);
    }
    message->message_send_form_symbol = BuildMessageSendFormSymbol(message->message_send_form);
    message->selector_lowering_symbol = BuildMessageSendSelectorLoweringSymbol(message->selector_lowering_pieces);
    message->selector_lowering_is_normalized = true;
    message->dispatch_abi_receiver_slots_marshaled = 1u;
    message->dispatch_abi_selector_slots_marshaled = 1u;
    message->dispatch_abi_argument_value_slots_marshaled = static_cast<unsigned>(message->args.size());
    message->dispatch_abi_runtime_arg_slots = kDispatchAbiMarshallingRuntimeArgSlots;
    message->dispatch_abi_argument_padding_slots_marshaled = ComputeDispatchAbiArgumentPaddingSlots(
        message->args.size(), message->dispatch_abi_runtime_arg_slots);
    message->dispatch_abi_argument_total_slots_marshaled = message->dispatch_abi_argument_value_slots_marshaled +
                                                           message->dispatch_abi_argument_padding_slots_marshaled;
    message->dispatch_abi_total_slots_marshaled = message->dispatch_abi_receiver_slots_marshaled +
                                                  message->dispatch_abi_selector_slots_marshaled +
                                                  message->dispatch_abi_argument_total_slots_marshaled;
    message->dispatch_abi_marshalling_symbol = BuildDispatchAbiMarshallingSymbol(
        message->dispatch_abi_receiver_slots_marshaled, message->dispatch_abi_selector_slots_marshaled,
        message->dispatch_abi_argument_value_slots_marshaled, message->dispatch_abi_argument_padding_slots_marshaled,
        message->dispatch_abi_argument_total_slots_marshaled, message->dispatch_abi_total_slots_marshaled,
        message->dispatch_abi_runtime_arg_slots);
    message->dispatch_abi_marshalling_is_normalized = true;
    message->nil_receiver_semantics_enabled = message->receiver->kind == Expr::Kind::NilLiteral;
    message->nil_receiver_foldable = message->nil_receiver_semantics_enabled;
    message->nil_receiver_requires_runtime_dispatch = !message->nil_receiver_foldable;
    message->nil_receiver_folding_symbol = BuildNilReceiverFoldingSymbol(
        message->nil_receiver_foldable, message->nil_receiver_requires_runtime_dispatch, message->message_send_form);
    message->nil_receiver_semantics_is_normalized = true;
    message->super_dispatch_enabled = IsSuperDispatchReceiver(*message->receiver);
    message->super_dispatch_requires_class_context = message->super_dispatch_enabled;
    message->super_dispatch_symbol = BuildSuperDispatchSymbol(
        message->super_dispatch_enabled, message->super_dispatch_requires_class_context, message->message_send_form);
    message->super_dispatch_semantics_is_normalized = true;
    message->method_family_name = ClassifyMethodFamilyFromSelector(message->selector);
    message->method_family_returns_retained_result = message->method_family_name == "init" ||
                                                     message->method_family_name == "copy" ||
                                                     message->method_family_name == "mutableCopy" ||
                                                     message->method_family_name == "new";
    message->method_family_returns_related_result = message->method_family_name == "init";
    message->method_family_semantics_symbol = BuildMethodFamilySemanticsSymbol(
        message->method_family_name, message->method_family_returns_retained_result,
        message->method_family_returns_related_result);
    message->method_family_semantics_is_normalized = true;
    message->runtime_shim_host_link_required = message->nil_receiver_requires_runtime_dispatch;
    message->runtime_shim_host_link_elided = !message->runtime_shim_host_link_required;
    message->runtime_shim_host_link_declaration_parameter_count = message->dispatch_abi_runtime_arg_slots + 2u;
    message->runtime_dispatch_bridge_symbol = kRuntimeShimHostLinkDispatchSymbol;
    message->runtime_shim_host_link_symbol = BuildRuntimeShimHostLinkSymbol(
        message->runtime_shim_host_link_required, message->runtime_shim_host_link_elided,
        message->dispatch_abi_runtime_arg_slots, message->runtime_shim_host_link_declaration_parameter_count,
        message->runtime_dispatch_bridge_symbol, message->message_send_form);
    message->runtime_shim_host_link_is_normalized = true;

    if (!Match(TokenKind::RBracket)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                      "missing ']' after message send expression"));
      return nullptr;
    }
    return message;
  }

  const std::vector<Token> &tokens_;
  std::size_t index_ = 0;
  std::vector<std::string> diagnostics_;
  bool saw_module_declaration_ = false;
  bool block_failed_ = false;
  unsigned autoreleasepool_scope_depth_ = 0;
  unsigned autoreleasepool_scope_serial_ = 0;
  Objc3AstBuilder ast_builder_;
};

}  // namespace

Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens) {
  Objc3Parser parser(tokens);
  Objc3ParseResult result;
  result.program = parser.Parse();
  result.diagnostics = parser.TakeDiagnostics();
  return result;
}
