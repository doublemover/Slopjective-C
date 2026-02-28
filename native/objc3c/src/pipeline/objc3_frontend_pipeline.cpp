#include "pipeline/objc3_frontend_pipeline.h"

#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "lex/objc3_lexer.h"
#include "parse/objc3_ast_builder_contract.h"
#include "sema/objc3_sema_pass_manager.h"

namespace {

template <typename T, typename = void>
struct HasProtocolsMember : std::false_type {};

template <typename T>
struct HasProtocolsMember<T, std::void_t<decltype(std::declval<const T &>().protocols)>> : std::true_type {};

template <typename T, typename = void>
struct HasCategoriesMember : std::false_type {};

template <typename T>
struct HasCategoriesMember<T, std::void_t<decltype(std::declval<const T &>().categories)>> : std::true_type {};

template <typename T, typename = void>
struct HasMethodsMember : std::false_type {};

template <typename T>
struct HasMethodsMember<T, std::void_t<decltype(std::declval<const T &>().methods)>> : std::true_type {};

template <typename T, typename = void>
struct HasMethodsLexicographicMember : std::false_type {};

template <typename T>
struct HasMethodsLexicographicMember<T, std::void_t<decltype(std::declval<const T &>().methods_lexicographic)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasHasMatchingInterfaceMember : std::false_type {};

template <typename T>
struct HasHasMatchingInterfaceMember<T, std::void_t<decltype(std::declval<const T &>().has_matching_interface)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasProtocolsLexicographicMember : std::false_type {};

template <typename T>
struct HasProtocolsLexicographicMember<T, std::void_t<decltype(std::declval<const T &>().protocols_lexicographic)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasCategoriesLexicographicMember : std::false_type {};

template <typename T>
struct HasCategoriesLexicographicMember<T, std::void_t<decltype(std::declval<const T &>().categories_lexicographic)>>
    : std::true_type {};

template <typename T>
std::size_t CountProtocols(const T &value) {
  if constexpr (HasProtocolsMember<T>::value) {
    return value.protocols.size();
  }
  return 0;
}

template <typename T>
std::size_t CountCategories(const T &value) {
  if constexpr (HasCategoriesMember<T>::value) {
    return value.categories.size();
  }
  return 0;
}

template <typename Surface>
std::size_t CountProtocolMethodsFromSymbolTable(const Surface &surface) {
  if constexpr (HasProtocolsMember<Surface>::value) {
    std::size_t total = 0;
    for (const auto &entry : surface.protocols) {
      const auto &metadata = entry.second;
      if constexpr (HasMethodsMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Surface>
std::size_t CountCategoryMethodsFromSymbolTable(const Surface &surface) {
  if constexpr (HasCategoriesMember<Surface>::value) {
    std::size_t total = 0;
    for (const auto &entry : surface.categories) {
      const auto &metadata = entry.second;
      if constexpr (HasMethodsMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Surface>
std::size_t CountLinkedCategorySymbolsFromSymbolTable(const Surface &surface) {
  if constexpr (HasCategoriesMember<Surface>::value) {
    std::size_t total = 0;
    for (const auto &entry : surface.categories) {
      const auto &metadata = entry.second;
      if constexpr (HasHasMatchingInterfaceMember<std::decay_t<decltype(metadata)>>::value &&
                    HasMethodsMember<std::decay_t<decltype(metadata)>>::value) {
        if (metadata.has_matching_interface) {
          total += metadata.methods.size();
        }
      }
    }
    return total;
  }
  return 0;
}

template <typename Handoff>
std::size_t CountProtocolMethodsFromTypeMetadata(const Handoff &handoff) {
  if constexpr (HasProtocolsLexicographicMember<Handoff>::value) {
    std::size_t total = 0;
    for (const auto &metadata : handoff.protocols_lexicographic) {
      if constexpr (HasMethodsLexicographicMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods_lexicographic.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Handoff>
std::size_t CountCategoryMethodsFromTypeMetadata(const Handoff &handoff) {
  if constexpr (HasCategoriesLexicographicMember<Handoff>::value) {
    std::size_t total = 0;
    for (const auto &metadata : handoff.categories_lexicographic) {
      if constexpr (HasMethodsLexicographicMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods_lexicographic.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Handoff>
std::size_t CountLinkedCategorySymbolsFromTypeMetadata(const Handoff &handoff) {
  if constexpr (HasCategoriesLexicographicMember<Handoff>::value) {
    std::size_t total = 0;
    for (const auto &metadata : handoff.categories_lexicographic) {
      if constexpr (HasHasMatchingInterfaceMember<std::decay_t<decltype(metadata)>>::value &&
                    HasMethodsLexicographicMember<std::decay_t<decltype(metadata)>>::value) {
        if (metadata.has_matching_interface) {
          total += metadata.methods_lexicographic.size();
        }
      }
    }
    return total;
  }
  return 0;
}

Objc3FrontendProtocolCategorySummary BuildProtocolCategorySummary(
    const Objc3Program &program,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendProtocolCategorySummary summary;
  summary.declared_protocols = CountProtocols(program);
  summary.declared_categories = CountCategories(program);
  summary.resolved_protocol_symbols = CountProtocols(integration_surface);
  summary.resolved_category_symbols = CountCategories(integration_surface);
  summary.protocol_method_symbols = CountProtocolMethodsFromSymbolTable(integration_surface);
  summary.category_method_symbols = CountCategoryMethodsFromSymbolTable(integration_surface);
  summary.linked_category_symbols = CountLinkedCategorySymbolsFromSymbolTable(integration_surface);

  if (summary.protocol_method_symbols == 0) {
    summary.protocol_method_symbols = CountProtocolMethodsFromTypeMetadata(type_metadata_handoff);
  }
  if (summary.category_method_symbols == 0) {
    summary.category_method_symbols = CountCategoryMethodsFromTypeMetadata(type_metadata_handoff);
  }
  if (summary.linked_category_symbols == 0) {
    summary.linked_category_symbols = CountLinkedCategorySymbolsFromTypeMetadata(type_metadata_handoff);
  }

  summary.deterministic_protocol_category_handoff =
      summary.linked_category_symbols <= summary.category_method_symbols &&
      summary.resolved_protocol_symbols <= summary.declared_protocols &&
      summary.resolved_category_symbols <= summary.declared_categories;
  return summary;
}

}  // namespace

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                     const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  Objc3LexerOptions lexer_options;
  lexer_options.language_version = options.language_version;
  lexer_options.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy
                                         ? Objc3LexerCompatibilityMode::kLegacy
                                         : Objc3LexerCompatibilityMode::kCanonical;
  lexer_options.migration_assist = options.migration_assist;
  Objc3Lexer lexer(source, lexer_options);
  std::vector<Objc3LexToken> tokens = lexer.Run(result.stage_diagnostics.lexer);
  const Objc3LexerMigrationHints &lexer_hints = lexer.MigrationHints();
  const Objc3LexerLanguageVersionPragmaContract &pragma_contract = lexer.LanguageVersionPragmaContract();
  result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;
  result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;
  result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;
  result.language_version_pragma_contract.seen = pragma_contract.seen;
  result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;
  result.language_version_pragma_contract.duplicate = pragma_contract.duplicate;
  result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;
  result.language_version_pragma_contract.first_line = pragma_contract.first_line;
  result.language_version_pragma_contract.first_column = pragma_contract.first_column;
  result.language_version_pragma_contract.last_line = pragma_contract.last_line;
  result.language_version_pragma_contract.last_column = pragma_contract.last_column;

  Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);
  result.program = std::move(parse_result.program);
  result.stage_diagnostics.parser = std::move(parse_result.diagnostics);
  result.protocol_category_summary =
      BuildProtocolCategorySummary(Objc3ParsedProgramAst(result.program),
                                   result.integration_surface,
                                   result.sema_type_metadata_handoff);

  if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {
    Objc3SemanticValidationOptions semantic_options;
    semantic_options.max_message_send_args = options.lowering.max_message_send_args;

    Objc3SemaPassManagerInput sema_input;
    sema_input.program = &result.program;
    sema_input.validation_options = semantic_options;
    sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy
                                        ? Objc3SemaCompatibilityMode::Legacy
                                        : Objc3SemaCompatibilityMode::Canonical;
    sema_input.migration_assist = options.migration_assist;
    sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;
    sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;
    sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;
    sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;

    Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);
    result.integration_surface = std::move(sema_result.integration_surface);
    result.sema_type_metadata_handoff = std::move(sema_result.type_metadata_handoff);
    result.protocol_category_summary =
        BuildProtocolCategorySummary(Objc3ParsedProgramAst(result.program),
                                     result.integration_surface,
                                     result.sema_type_metadata_handoff);
    result.sema_diagnostics_after_pass = sema_result.diagnostics_after_pass;
    result.sema_parity_surface = sema_result.parity_surface;
    if (result.stage_diagnostics.semantic.empty() && !sema_result.diagnostics.empty()) {
      result.stage_diagnostics.semantic = std::move(sema_result.diagnostics);
    }
  }

  TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);
  return result;
}
