#include "io/objc3_diagnostics_artifact_document.h"

#include <cctype>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "diag/objc3_diag_utils.h"
#include "io/json/json_writer.h"
#include "io/objc3_file_io.h"

namespace {

using objc3::io::json::JsonObjectWriter;
using objc3::io::json::JsonValue;

struct ParsedDiagnosticFixIt {
  unsigned start_line = 1;
  unsigned start_column = 1;
  unsigned end_line = 1;
  unsigned end_column = 1;
  std::string label;
  std::string replacement;
  bool machine_applicable = true;
};

struct ParsedDiagnosticRecovery {
  bool present = false;
  std::string strategy;
  std::string boundary;
  bool deterministic = true;
  bool recovery_counts_as_success = false;
};

struct ParsedDiagnosticMetadata {
  std::string message;
  std::vector<ParsedDiagnosticFixIt> fixits;
  ParsedDiagnosticRecovery recovery;
};

bool StartsWithAt(std::string_view text,
                  std::size_t position,
                  std::string_view prefix) {
  return position <= text.size() && prefix.size() <= text.size() - position &&
         text.substr(position, prefix.size()) == prefix;
}

std::string TrimAscii(std::string_view value) {
  std::size_t begin = 0;
  while (begin < value.size() &&
         std::isspace(static_cast<unsigned char>(value[begin])) != 0) {
    ++begin;
  }

  std::size_t end = value.size();
  while (end > begin &&
         std::isspace(static_cast<unsigned char>(value[end - 1u])) != 0) {
    --end;
  }

  return std::string(value.substr(begin, end - begin));
}

std::string UnescapeDiagnosticMetadataValue(std::string_view value) {
  std::string unescaped;
  unescaped.reserve(value.size());
  bool escaped = false;
  for (const char ch : value) {
    if (escaped) {
      unescaped.push_back(ch);
      escaped = false;
      continue;
    }
    if (ch == '\\') {
      escaped = true;
      continue;
    }
    unescaped.push_back(ch);
  }
  if (escaped) {
    unescaped.push_back('\\');
  }
  return unescaped;
}

std::map<std::string, std::string> ParseDiagnosticMetadataPairs(
    std::string_view text) {
  std::map<std::string, std::string> pairs;
  std::size_t index = 0;

  while (index < text.size()) {
    while (index < text.size() &&
           (std::isspace(static_cast<unsigned char>(text[index])) != 0 ||
            text[index] == ';')) {
      ++index;
    }
    if (index >= text.size()) {
      break;
    }

    const std::size_t key_begin = index;
    while (index < text.size() && text[index] != '=' && text[index] != ';') {
      ++index;
    }
    if (index >= text.size() || text[index] != '=') {
      break;
    }

    const std::string key = TrimAscii(text.substr(key_begin, index - key_begin));
    ++index;

    std::string value;
    if (index < text.size() && text[index] == '\'') {
      ++index;
      const std::size_t value_begin = index;
      bool escaped = false;
      while (index < text.size()) {
        const char ch = text[index];
        if (escaped) {
          escaped = false;
          ++index;
          continue;
        }
        if (ch == '\\') {
          escaped = true;
          ++index;
          continue;
        }
        if (ch == '\'') {
          break;
        }
        ++index;
      }
      value = UnescapeDiagnosticMetadataValue(
          text.substr(value_begin, index - value_begin));
      if (index < text.size() && text[index] == '\'') {
        ++index;
      }
    } else {
      const std::size_t value_begin = index;
      while (index < text.size() && text[index] != ';') {
        ++index;
      }
      value = TrimAscii(text.substr(value_begin, index - value_begin));
    }

    if (!key.empty()) {
      pairs[key] = value;
    }

    if (index < text.size() && text[index] == ';') {
      ++index;
    }
  }

  return pairs;
}

bool TryParseUnsigned(std::string_view text, unsigned &value) {
  if (text.empty()) {
    return false;
  }
  unsigned parsed = 0;
  for (const char ch : text) {
    if (ch < '0' || ch > '9') {
      return false;
    }
    const unsigned digit = static_cast<unsigned>(ch - '0');
    if (parsed > (std::numeric_limits<unsigned>::max() - digit) / 10u) {
      return false;
    }
    parsed = parsed * 10u + digit;
  }
  value = parsed;
  return true;
}

bool TryParseDiagnosticRange(std::string_view text,
                             unsigned &start_line,
                             unsigned &start_column,
                             unsigned &end_line,
                             unsigned &end_column) {
  const std::size_t first_colon = text.find(':');
  const std::size_t dash = text.find('-', first_colon == std::string_view::npos
                                              ? 0
                                              : first_colon + 1u);
  const std::size_t second_colon = dash == std::string_view::npos
                                       ? std::string_view::npos
                                       : text.find(':', dash + 1u);
  if (first_colon == std::string_view::npos ||
      dash == std::string_view::npos ||
      second_colon == std::string_view::npos) {
    return false;
  }
  return TryParseUnsigned(text.substr(0, first_colon), start_line) &&
         TryParseUnsigned(text.substr(first_colon + 1u,
                                      dash - first_colon - 1u),
                          start_column) &&
         TryParseUnsigned(text.substr(dash + 1u, second_colon - dash - 1u),
                          end_line) &&
         TryParseUnsigned(text.substr(second_colon + 1u), end_column);
}

bool TryBuildParsedFixIt(const std::map<std::string, std::string> &pairs,
                         ParsedDiagnosticFixIt &fixit) {
  const auto range = pairs.find("range");
  if (range == pairs.end() ||
      !TryParseDiagnosticRange(range->second,
                               fixit.start_line,
                               fixit.start_column,
                               fixit.end_line,
                               fixit.end_column)) {
    return false;
  }

  const auto label = pairs.find("label");
  fixit.label =
      label == pairs.end() ? "compiler-provided-fix-it" : label->second;

  const auto replacement = pairs.find("replacement");
  fixit.replacement = replacement == pairs.end() ? "" : replacement->second;

  const auto applicable = pairs.find("machine-applicable");
  fixit.machine_applicable =
      applicable == pairs.end() || applicable->second == "true";
  return true;
}

bool TryBuildParsedRecovery(const std::map<std::string, std::string> &pairs,
                            ParsedDiagnosticRecovery &recovery) {
  const auto strategy = pairs.find("strategy");
  const auto boundary = pairs.find("boundary");
  if (strategy == pairs.end() || strategy->second.empty() ||
      boundary == pairs.end() || boundary->second.empty()) {
    return false;
  }

  recovery.present = true;
  recovery.strategy = strategy->second;
  recovery.boundary = boundary->second;

  const auto deterministic = pairs.find("deterministic");
  recovery.deterministic =
      deterministic == pairs.end() || deterministic->second == "true";

  const auto counts_as_success = pairs.find("recovery-counts-as-success");
  recovery.recovery_counts_as_success =
      counts_as_success != pairs.end() && counts_as_success->second == "true";
  return true;
}

void ConsumeDiagnosticMetadataBlock(std::string_view block,
                                    ParsedDiagnosticMetadata &parsed) {
  const std::size_t colon = block.find(':');
  if (colon == std::string_view::npos) {
    return;
  }

  const std::string kind = TrimAscii(block.substr(0, colon));
  const std::map<std::string, std::string> pairs =
      ParseDiagnosticMetadataPairs(block.substr(colon + 1u));
  if (kind == "fix-it") {
    ParsedDiagnosticFixIt fixit;
    if (TryBuildParsedFixIt(pairs, fixit)) {
      parsed.fixits.push_back(std::move(fixit));
    }
    return;
  }
  if (kind == "recovery") {
    ParsedDiagnosticRecovery recovery;
    if (TryBuildParsedRecovery(pairs, recovery)) {
      parsed.recovery = std::move(recovery);
    }
  }
}

ParsedDiagnosticMetadata ParseDiagnosticMetadata(std::string message) {
  ParsedDiagnosticMetadata parsed;
  parsed.message = std::move(message);

  std::size_t search = 0;
  while (search < parsed.message.size()) {
    const std::size_t block_begin = parsed.message.find(" {", search);
    if (block_begin == std::string::npos) {
      break;
    }
    const std::size_t payload_begin = block_begin + 2u;
    if (!StartsWithAt(parsed.message, payload_begin, "fix-it:") &&
        !StartsWithAt(parsed.message, payload_begin, "recovery:")) {
      search = payload_begin;
      continue;
    }

    const std::size_t block_end = parsed.message.find('}', payload_begin);
    if (block_end == std::string::npos) {
      break;
    }

    ConsumeDiagnosticMetadataBlock(
        std::string_view(parsed.message).substr(payload_begin,
                                                block_end - payload_begin),
        parsed);
    parsed.message.erase(block_begin, block_end - block_begin + 1u);
    search = block_begin;
  }

  parsed.message = TrimAscii(parsed.message);
  return parsed;
}

JsonValue PositionValue(unsigned line, unsigned column) {
  return JsonValue::ObjectValue(
      {{"line", JsonValue::Number(static_cast<double>(line))},
       {"column", JsonValue::Number(static_cast<double>(column))}});
}

JsonValue RangeValue(unsigned start_line,
                     unsigned start_column,
                     unsigned end_line,
                     unsigned end_column) {
  return JsonValue::ObjectValue(
      {{"start", PositionValue(start_line, start_column)},
       {"end", PositionValue(end_line, end_column)}});
}

const char *DiagnosticPhaseForCode(const std::string &code) {
  if (code.size() < 3) {
    return "unknown";
  }
  switch (code[2]) {
    case 'C':
    case 'P':
      return "parse";
    case 'S':
      return "sema";
    case 'L':
      return "lex";
    case 'A':
    case 'T':
      return "post-pipeline";
    case 'R':
      return "runtime";
    case 'E':
      return "frontend-api";
    default:
      return "unknown";
  }
}

unsigned DiagnosticSpanEndColumn(const std::string &code,
                                 unsigned column) {
  if (column == 0U) {
    return 0U;
  }
  if (code == "O3C004") {
    return column + 8U;
  }
  return column + 1U;
}

JsonValue FixItValue(const ParsedDiagnosticFixIt &fixit) {
  const bool insertion = fixit.start_line == fixit.end_line &&
                         fixit.start_column == fixit.end_column;
  return JsonValue::ObjectValue(
      {{"applicability",
        JsonValue::String(fixit.machine_applicable ? "machine-applicable"
                                                   : "maybe-applicable")},
       {"kind", JsonValue::String(insertion ? "insert-text" : "replace-token")},
       {"label", JsonValue::String(fixit.label)},
       {"range",
        RangeValue(fixit.start_line,
                   fixit.start_column,
                   fixit.end_line,
                   fixit.end_column)},
       {"replacement", JsonValue::String(fixit.replacement)},
       {"title", JsonValue::String(fixit.label)}});
}

JsonValue::Array BuildDiagnosticFixits(
    const std::string &code,
    unsigned line,
    unsigned column,
    const std::vector<ParsedDiagnosticFixIt> &metadata_fixits) {
  JsonValue::Array fixits;
  fixits.reserve(metadata_fixits.size());
  for (const ParsedDiagnosticFixIt &fixit : metadata_fixits) {
    fixits.push_back(FixItValue(fixit));
  }
  if (!fixits.empty()) {
    return fixits;
  }

  if (line == 0U || column == 0U) {
    return fixits;
  }

  if (code == "O3C004") {
    fixits.push_back(JsonValue::ObjectValue(
        {{"applicability", JsonValue::String("machine-applicable")},
         {"kind", JsonValue::String("replace-token")},
         {"range", RangeValue(line, column, line, column + 8U)},
         {"replacement", JsonValue::String("Optional")},
         {"title", JsonValue::String(
                       "replace optional<T> with canonical Optional<T>")}}));
  }

  return fixits;
}

JsonValue BuildDiagnosticRecovery(const std::string &code,
                                  const ParsedDiagnosticRecovery &metadata_recovery) {
  if (metadata_recovery.present) {
    return JsonValue::ObjectValue(
        {{"accepts_invalid_program", JsonValue::Bool(false)},
         {"available", JsonValue::Bool(true)},
         {"boundary", JsonValue::String(metadata_recovery.boundary)},
         {"deterministic", JsonValue::Bool(metadata_recovery.deterministic)},
         {"recovery_counts_as_success",
          JsonValue::Bool(metadata_recovery.recovery_counts_as_success)},
         {"strategy", JsonValue::String(metadata_recovery.strategy)}});
  }

  std::string strategy = "diagnostic-artifact-boundary";
  std::string boundary = "single diagnostic";
  bool available = false;

  if (code == "O3P104") {
    strategy = "parser-statement-boundary-synchronization";
    boundary = "next statement token";
    available = true;
  } else if (code == "O3C004") {
    strategy = "parser-canonical-spelling-rejection";
    boundary = "canonical type spelling";
    available = true;
  } else if (code == "O3P100") {
    strategy = "parser-local-rejection-boundary";
    boundary = "unsupported top-level construct";
    available = true;
  } else if (code == "O3S203") {
    strategy = "semantic-symbol-resolution-boundary";
    boundary = "unresolved callee";
    available = true;
  } else if (code == "O3S211") {
    strategy = "semantic-return-contract-boundary";
    boundary = "function return statement";
    available = true;
  }

  return JsonValue::ObjectValue(
      {{"accepts_invalid_program", JsonValue::Bool(false)},
       {"available", JsonValue::Bool(available)},
       {"boundary", JsonValue::String(boundary)},
       {"deterministic", JsonValue::Bool(true)},
       {"recovery_counts_as_success", JsonValue::Bool(false)},
       {"strategy", JsonValue::String(strategy)}});
}

std::string DiagnosticExplanationForCode(const std::string &code,
                                         const std::string &message,
                                         const ParsedDiagnosticRecovery &recovery) {
  if (recovery.present) {
    return "The compiler emitted a deterministic recovery boundary for " + code +
           " while preserving fail-closed compilation.";
  }
  if (code == "O3P104") {
    return "The parser expected a statement or declaration terminator and kept "
           "the invalid program rejected.";
  }
  if (code == "O3C004") {
    return "The parser rejected a removed Objective-C 2 compatibility spelling "
           "and provides the canonical replacement.";
  }
  if (code == "O3S205") {
    return "Semantic analysis proved that a value-returning callable can exit "
           "without returning a value.";
  }
  if (!message.empty()) {
    return message;
  }
  return "Structured Objective-C 3.0 diagnostic payload.";
}

JsonValue BuildDiagnosticJsonValue(const std::string &diagnostic_text) {
  const DiagSortKey key = ParseDiagSortKey(diagnostic_text);
  const ParsedDiagnosticMetadata metadata =
      ParseDiagnosticMetadata(key.message);
  const unsigned line =
      key.line == std::numeric_limits<unsigned>::max() ? 0U : key.line;
  const unsigned column =
      key.column == std::numeric_limits<unsigned>::max() ? 0U : key.column;
  const unsigned end_column = DiagnosticSpanEndColumn(key.code, column);
  const Objc3DiagnosticCategory category = DiagnosticCategoryForCode(key.code);

  return JsonValue::ObjectValue(
      {{"category", JsonValue::String(DiagnosticCategoryName(category))},
       {"code", JsonValue::String(key.code)},
       {"column", JsonValue::Number(static_cast<double>(column))},
       {"explanation",
        JsonValue::String(DiagnosticExplanationForCode(
            key.code, metadata.message, metadata.recovery))},
       {"fixits",
        JsonValue::ArrayValue(
            BuildDiagnosticFixits(key.code, line, column, metadata.fixits))},
       {"line", JsonValue::Number(static_cast<double>(line))},
       {"message", JsonValue::String(metadata.message)},
       {"phase", JsonValue::String(DiagnosticPhaseForCode(key.code))},
       {"raw", JsonValue::String(diagnostic_text)},
       {"recovery", BuildDiagnosticRecovery(key.code, metadata.recovery)},
       {"severity", JsonValue::String(ToLower(key.severity))},
       {"span", RangeValue(line, column, line, end_column)}});
}

}  // namespace

std::string BuildDiagnosticsTextArtifact(
    const std::vector<std::string> &diagnostics) {
  return JoinLines(diagnostics);
}

std::string BuildDiagnosticsJsonArtifact(
    const std::vector<std::string> &diagnostics) {
  JsonValue::Array diagnostic_values;
  diagnostic_values.reserve(diagnostics.size());
  for (const std::string &diagnostic_text : diagnostics) {
    diagnostic_values.push_back(BuildDiagnosticJsonValue(diagnostic_text));
  }

  std::ostringstream out;
  JsonObjectWriter artifact(out);
  artifact.StringField("schema_version", "1.0.0");
  artifact.RawJsonField(
      "diagnostics",
      objc3::io::json::RenderJson(
          JsonValue::ArrayValue(std::move(diagnostic_values))));
  artifact.End();
  out << '\n';
  return out.str();
}
