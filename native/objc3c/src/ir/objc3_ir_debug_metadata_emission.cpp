#include "ir/objc3_ir_debug_metadata_emission.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_module_body_orchestration.h"

namespace {

struct Objc3IRDebugSourceSite {
  std::string display_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3IRDebugMetadataIds {
  Objc3IRDebugSourceSite site;
  std::size_t subprogram_id = 0;
  std::vector<std::size_t> instruction_location_ids;
};

struct Objc3IRDebugMetadataRootIds {
  std::size_t compile_unit_id = 0;
  std::size_t file_id = 0;
  std::size_t subroutine_type_id = 0;
  std::size_t type_list_id = 0;
  std::size_t retained_nodes_id = 0;
  std::size_t debug_info_version_flag_id = 0;
  bool emit_debug_info_version_flag = false;
  bool reference_debug_info_version_flag_in_module_flags = false;
  bool module_flags_present = false;
  std::size_t next_id = 0;
};

struct Objc3IRExistingMetadataScan {
  std::size_t next_id = 0;
  std::size_t debug_info_version_flag_id = 0;
  bool module_flags_present = false;
  bool debug_info_version_flag_present = false;
  bool debug_info_version_flag_referenced_by_module_flags = false;
};

unsigned ClampObjc3SourceCoordinate(unsigned value) {
  return value == 0 ? 1u : value;
}

std::string BuildObjc3IRDebugSourceFilename(const Objc3Program &program) {
  const std::string module_name =
      program.module_name.empty() ? "objc3_module" : program.module_name;
  return module_name + ".objc3";
}

std::string EscapeObjc3IRDebugMetadataString(const std::string &value) {
  std::ostringstream out;
  for (const char ch : value) {
    const unsigned char byte = static_cast<unsigned char>(ch);
    if (ch == '\\' || ch == '"') {
      out << '\\' << ch;
    } else if (std::iscntrl(byte)) {
      out << '_';
    } else {
      out << ch;
    }
  }
  return out.str();
}

std::string TrimLeft(std::string value) {
  value.erase(
      value.begin(),
      std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        return !std::isspace(ch);
      }));
  return value;
}

bool StartsWith(const std::string &value, const std::string &prefix) {
  return value.rfind(prefix, 0) == 0;
}

bool EndsWith(const std::string &value, char suffix) {
  return !value.empty() && value.back() == suffix;
}

bool IsWhitespaceOnly(const std::string &value) {
  return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
    return std::isspace(ch);
  });
}

std::string TrimRight(std::string value) {
  while (!value.empty() &&
         std::isspace(static_cast<unsigned char>(value.back()))) {
    value.pop_back();
  }
  return value;
}

std::size_t FindObjc3IRCommentStart(const std::string &line) {
  bool in_string = false;
  bool escaped = false;
  for (std::size_t index = 0; index < line.size(); ++index) {
    const char ch = line[index];
    if (in_string) {
      if (escaped) {
        escaped = false;
      } else if (ch == '\\') {
        escaped = true;
      } else if (ch == '"') {
        in_string = false;
      }
      continue;
    }
    if (ch == '"') {
      in_string = true;
      continue;
    }
    if (ch == ';') {
      return index;
    }
  }
  return std::string::npos;
}

std::string Objc3IRCodeBeforeComment(const std::string &line) {
  const std::size_t comment = FindObjc3IRCommentStart(line);
  return TrimRight(
      comment == std::string::npos ? line : line.substr(0, comment));
}

std::size_t FindObjc3IRNamedMetadataStart(const std::string &ir,
                                          const std::string &name) {
  std::size_t offset = 0;
  std::istringstream in(ir);
  std::string line;
  while (std::getline(in, line)) {
    const std::string trimmed = TrimLeft(line);
    if (StartsWith(trimmed, name)) {
      return offset + (line.size() - trimmed.size());
    }
    offset += line.size() + 1;
  }
  return std::string::npos;
}

std::size_t FindObjc3IRModuleFlagsListOpen(const std::string &ir,
                                           std::size_t start) {
  std::size_t line_begin = start;
  while (line_begin > 0 && ir[line_begin - 1] != '\n') {
    --line_begin;
  }
  std::size_t line_end = ir.find('\n', line_begin);
  if (line_end == std::string::npos) {
    line_end = ir.size();
  }
  const std::string first_line =
      ir.substr(line_begin, line_end - line_begin);
  const std::size_t same_line_open =
      Objc3IRCodeBeforeComment(first_line).find("!{");
  if (same_line_open != std::string::npos) {
    return line_begin + same_line_open;
  }

  std::size_t offset = line_end < ir.size() ? line_end + 1 : ir.size();
  while (offset < ir.size()) {
    std::size_t next_line_end = ir.find('\n', offset);
    if (next_line_end == std::string::npos) {
      next_line_end = ir.size();
    }
    const std::string line = ir.substr(offset, next_line_end - offset);
    const std::string trimmed = TrimLeft(Objc3IRCodeBeforeComment(line));
    if (trimmed.empty() || StartsWith(trimmed, ";")) {
      offset = next_line_end < ir.size() ? next_line_end + 1 : ir.size();
      continue;
    }
    if (StartsWith(trimmed, "!{")) {
      return offset + (line.size() - trimmed.size());
    }
    return std::string::npos;
  }
  return std::string::npos;
}

std::size_t FindObjc3IRMetadataListClose(const std::string &ir,
                                         std::size_t open) {
  bool in_string = false;
  bool escaped = false;
  bool in_comment = false;
  int depth = 0;
  for (std::size_t index = open; index < ir.size(); ++index) {
    const char ch = ir[index];
    if (in_comment) {
      if (ch == '\n') {
        in_comment = false;
      }
      continue;
    }
    if (in_string) {
      if (escaped) {
        escaped = false;
      } else if (ch == '\\') {
        escaped = true;
      } else if (ch == '"') {
        in_string = false;
      }
      continue;
    }
    if (ch == ';') {
      in_comment = true;
      continue;
    }
    if (ch == '"') {
      in_string = true;
      continue;
    }
    if (ch == '!' && index + 1 < ir.size() && ir[index + 1] == '{') {
      ++depth;
      ++index;
      continue;
    }
    if (ch == '}') {
      if (depth <= 0) {
        return std::string::npos;
      }
      --depth;
      if (depth == 0) {
        return index;
      }
    }
  }
  return std::string::npos;
}

std::string StripObjc3IRCommentsFromSegment(
    const std::string &ir,
    std::size_t begin,
    std::size_t end) {
  std::string out;
  bool in_string = false;
  bool escaped = false;
  bool in_comment = false;
  for (std::size_t index = begin; index < end; ++index) {
    const char ch = ir[index];
    if (in_comment) {
      if (ch == '\n') {
        in_comment = false;
        out += ch;
      }
      continue;
    }
    if (in_string) {
      out += ch;
      if (escaped) {
        escaped = false;
      } else if (ch == '\\') {
        escaped = true;
      } else if (ch == '"') {
        in_string = false;
      }
      continue;
    }
    if (ch == ';') {
      in_comment = true;
      continue;
    }
    if (ch == '"') {
      in_string = true;
    }
    out += ch;
  }
  return out;
}

bool ContainsObjc3IRMetadataReference(const std::string &value,
                                      std::size_t metadata_id) {
  const std::string target = "!" + std::to_string(metadata_id);
  std::size_t cursor = value.find(target);
  while (cursor != std::string::npos) {
    const std::size_t after = cursor + target.size();
    if (after >= value.size() ||
        !std::isdigit(static_cast<unsigned char>(value[after]))) {
      return true;
    }
    cursor = value.find(target, cursor + 1);
  }
  return false;
}

bool Objc3IRModuleFlagsReferenceMetadataId(const std::string &ir,
                                           std::size_t metadata_id) {
  const std::size_t start =
      FindObjc3IRNamedMetadataStart(ir, "!llvm.module.flags");
  if (start == std::string::npos) {
    return false;
  }
  const std::size_t open = FindObjc3IRModuleFlagsListOpen(ir, start);
  if (open == std::string::npos) {
    return false;
  }
  const std::size_t close = FindObjc3IRMetadataListClose(ir, open);
  if (close == std::string::npos) {
    return false;
  }
  const std::string entries =
      StripObjc3IRCommentsFromSegment(ir, open + 2, close);
  return ContainsObjc3IRMetadataReference(entries, metadata_id);
}

bool AppendObjc3IRDebugInfoVersionFlagToModuleFlags(
    const std::string &ir,
    std::size_t debug_info_version_flag_id,
    std::string &updated_ir) {
  const std::size_t start =
      FindObjc3IRNamedMetadataStart(ir, "!llvm.module.flags");
  if (start == std::string::npos) {
    return false;
  }
  const std::size_t open = FindObjc3IRModuleFlagsListOpen(ir, start);
  if (open == std::string::npos) {
    return false;
  }
  const std::size_t close = FindObjc3IRMetadataListClose(ir, open);
  if (close == std::string::npos) {
    return false;
  }
  const std::string entries =
      StripObjc3IRCommentsFromSegment(ir, open + 2, close);
  if (ContainsObjc3IRMetadataReference(entries, debug_info_version_flag_id)) {
    updated_ir = ir;
    return true;
  }
  const std::string trimmed_entries = TrimRight(TrimLeft(entries));
  std::string separator;
  if (!trimmed_entries.empty() && trimmed_entries.back() != ',') {
    separator = ", ";
  } else if (!trimmed_entries.empty()) {
    separator = " ";
  }
  updated_ir = ir.substr(0, close) + separator + "!" +
               std::to_string(debug_info_version_flag_id) + ir.substr(close);
  return true;
}

bool ParseObjc3IRNumberedMetadataDefinitionId(const std::string &trimmed,
                                             std::size_t &metadata_id) {
  if (trimmed.size() < 4 || trimmed[0] != '!' ||
      !std::isdigit(static_cast<unsigned char>(trimmed[1]))) {
    return false;
  }
  std::size_t cursor = 1;
  std::size_t parsed_id = 0;
  while (cursor < trimmed.size() &&
         std::isdigit(static_cast<unsigned char>(trimmed[cursor]))) {
    parsed_id =
        parsed_id * 10 + static_cast<std::size_t>(trimmed[cursor] - '0');
    ++cursor;
  }
  while (cursor < trimmed.size() &&
         std::isspace(static_cast<unsigned char>(trimmed[cursor]))) {
    ++cursor;
  }
  if (cursor >= trimmed.size() || trimmed[cursor] != '=') {
    return false;
  }
  metadata_id = parsed_id;
  return true;
}

bool HasExistingObjc3IRDebugMetadata(const std::string &ir) {
  return ir.find("!llvm.dbg.cu") != std::string::npos ||
         ir.find("llvm.dbg.") != std::string::npos ||
         ir.find("!dbg") != std::string::npos ||
         ir.find("DICompileUnit") != std::string::npos ||
         ir.find("DISubprogram") != std::string::npos ||
         ir.find("DILocation") != std::string::npos;
}

Objc3IRExistingMetadataScan ScanObjc3IRExistingMetadata(
    const std::string &ir) {
  Objc3IRExistingMetadataScan scan;
  bool saw_numbered_metadata = false;
  std::size_t highest_metadata_id = 0;
  std::istringstream in(ir);
  std::string line;
  std::size_t offset = 0;
  while (std::getline(in, line)) {
    const std::string trimmed = TrimLeft(line);
    if (StartsWith(trimmed, "!llvm.module.flags")) {
      scan.module_flags_present = true;
    }
    std::size_t metadata_id = 0;
    if (!ParseObjc3IRNumberedMetadataDefinitionId(trimmed, metadata_id)) {
      offset += line.size() + 1;
      continue;
    }
    saw_numbered_metadata = true;
    highest_metadata_id = std::max(highest_metadata_id, metadata_id);
    const std::size_t local_open = line.find("!{");
    const std::size_t open = local_open == std::string::npos
                                 ? std::string::npos
                                 : offset + local_open;
    const std::size_t close =
        open == std::string::npos ? std::string::npos
                                  : FindObjc3IRMetadataListClose(ir, open);
    if (open != std::string::npos && close != std::string::npos &&
        StripObjc3IRCommentsFromSegment(ir, open + 2, close)
            .find("!\"Debug Info Version\"") != std::string::npos) {
      scan.debug_info_version_flag_present = true;
      scan.debug_info_version_flag_id = metadata_id;
    }
    offset += line.size() + 1;
  }
  scan.module_flags_present =
      scan.module_flags_present ||
      FindObjc3IRNamedMetadataStart(ir, "!llvm.module.flags") !=
          std::string::npos;
  if (scan.debug_info_version_flag_present && scan.module_flags_present) {
    scan.debug_info_version_flag_referenced_by_module_flags =
        Objc3IRModuleFlagsReferenceMetadataId(
            ir, scan.debug_info_version_flag_id);
  }
  scan.next_id = saw_numbered_metadata ? highest_metadata_id + 1 : 0;
  return scan;
}

Objc3IRDebugMetadataRootIds AllocateObjc3IRDebugMetadataRootIds(
    const Objc3IRExistingMetadataScan &scan) {
  Objc3IRDebugMetadataRootIds ids;
  ids.module_flags_present = scan.module_flags_present;
  ids.compile_unit_id = scan.next_id;
  ids.file_id = ids.compile_unit_id + 1;
  ids.subroutine_type_id = ids.file_id + 1;
  ids.type_list_id = ids.subroutine_type_id + 1;
  ids.retained_nodes_id = ids.type_list_id + 1;
  ids.next_id = ids.retained_nodes_id + 1;
  if (scan.debug_info_version_flag_present) {
    ids.debug_info_version_flag_id = scan.debug_info_version_flag_id;
  } else {
    ids.debug_info_version_flag_id = ids.next_id++;
    ids.emit_debug_info_version_flag = true;
  }
  ids.reference_debug_info_version_flag_in_module_flags =
      ids.module_flags_present &&
      !scan.debug_info_version_flag_referenced_by_module_flags;
  return ids;
}

std::string AttachObjc3IRDebugMetadataToDefinition(
    const std::string &line,
    std::size_t subprogram_id) {
  if (line.find("!dbg") != std::string::npos) {
    return line;
  }
  const std::size_t brace = line.rfind('{');
  if (brace == std::string::npos) {
    return line;
  }
  std::string prefix = line.substr(0, brace);
  if (IsWhitespaceOnly(prefix)) {
    return line;
  }
  if (!prefix.empty() &&
      !std::isspace(static_cast<unsigned char>(prefix.back()))) {
    prefix += ' ';
  }
  return prefix + "!dbg !" + std::to_string(subprogram_id) + " " +
         line.substr(brace);
}

std::string AttachObjc3IRDebugMetadataToInstruction(
    const std::string &line,
    std::size_t location_id) {
  const std::size_t comment = FindObjc3IRCommentStart(line);
  std::string prefix =
      comment == std::string::npos ? line : line.substr(0, comment);
  prefix = TrimRight(std::move(prefix));
  if (prefix.empty()) {
    return line;
  }
  std::string result = prefix + ", !dbg !" + std::to_string(location_id);
  if (comment != std::string::npos) {
    result += " ";
    result += line.substr(comment);
  }
  return result;
}

bool ExtractObjc3IRDefinedSymbol(const std::string &line, std::string &symbol) {
  const std::string trimmed = TrimLeft(line);
  if (!StartsWith(trimmed, "define ")) {
    return false;
  }
  const std::size_t at = trimmed.find('@');
  if (at == std::string::npos || at + 1 >= trimmed.size()) {
    return false;
  }
  if (trimmed[at + 1] == '"') {
    std::size_t end = at + 2;
    bool escaped = false;
    for (; end < trimmed.size(); ++end) {
      const char ch = trimmed[end];
      if (escaped) {
        escaped = false;
        continue;
      }
      if (ch == '\\') {
        escaped = true;
        continue;
      }
      if (ch == '"') {
        break;
      }
    }
    if (end >= trimmed.size()) {
      return false;
    }
    symbol = trimmed.substr(at + 2, end - at - 2);
    return !symbol.empty();
  }
  const std::size_t open_paren = trimmed.find('(', at + 1);
  if (open_paren == std::string::npos || open_paren <= at + 1) {
    return false;
  }
  symbol = trimmed.substr(at + 1, open_paren - at - 1);
  return !symbol.empty();
}

struct Objc3IRDelimiterDepth {
  int paren_depth = 0;
  int bracket_depth = 0;
  int brace_depth = 0;
  bool underflow = false;
};

struct Objc3IRInstructionContinuationState {
  Objc3IRDelimiterDepth depth;
  bool active = false;
};

struct Objc3IRInstructionLineInfo {
  bool starts_instruction = false;
  bool attachable = false;
  bool continues = false;
  bool enters_continuation = false;
  Objc3IRDelimiterDepth depth_after_line;
};

bool Objc3IRDelimiterDepthIsOpen(const Objc3IRDelimiterDepth &depth) {
  return depth.paren_depth > 0 || depth.bracket_depth > 0 ||
         depth.brace_depth > 0;
}

void AccumulateObjc3IRDelimiterDepths(
    const std::string &code,
    Objc3IRDelimiterDepth &depth) {
  bool in_string = false;
  bool escaped = false;
  for (const char ch : code) {
    if (in_string) {
      if (escaped) {
        escaped = false;
      } else if (ch == '\\') {
        escaped = true;
      } else if (ch == '"') {
        in_string = false;
      }
      continue;
    }
    if (ch == '"') {
      in_string = true;
      continue;
    }
    switch (ch) {
      case '(':
        ++depth.paren_depth;
        break;
      case '[':
        ++depth.bracket_depth;
        break;
      case '{':
        ++depth.brace_depth;
        break;
      case ')':
        if (depth.paren_depth == 0) {
          depth.underflow = true;
        } else {
          --depth.paren_depth;
        }
        break;
      case ']':
        if (depth.bracket_depth == 0) {
          depth.underflow = true;
        } else {
          --depth.bracket_depth;
        }
        break;
      case '}':
        if (depth.brace_depth == 0) {
          depth.underflow = true;
        } else {
          --depth.brace_depth;
        }
        break;
      default:
        break;
    }
  }
}

bool Objc3IRLineImpliesContinuation(const std::string &code) {
  const std::string trimmed = TrimLeft(code);
  return EndsWith(trimmed, ',') || EndsWith(trimmed, '(') ||
         EndsWith(trimmed, '[') || EndsWith(trimmed, '{');
}

std::string Objc3IRInstructionTextWithoutResult(const std::string &trimmed) {
  if (trimmed.empty() || trimmed[0] != '%') {
    return trimmed;
  }
  const std::size_t equals = trimmed.find('=');
  if (equals == std::string::npos) {
    return trimmed;
  }
  const std::string result = TrimRight(trimmed.substr(0, equals));
  if (result.empty() || result[0] != '%' ||
      result.find(',') != std::string::npos ||
      result.find('(') != std::string::npos) {
    return trimmed;
  }
  return TrimLeft(trimmed.substr(equals + 1));
}

std::string ReadObjc3IRFirstToken(const std::string &value) {
  std::size_t cursor = 0;
  while (cursor < value.size() &&
         !std::isspace(static_cast<unsigned char>(value[cursor]))) {
    ++cursor;
  }
  return value.substr(0, cursor);
}

bool IsObjc3IRInstructionOpcode(const std::string &token) {
  static const char *const kOpcodes[] = {
      "ret",           "br",          "switch",      "indirectbr",
      "invoke",        "callbr",      "resume",      "catchswitch",
      "catchret",      "cleanupret",  "unreachable", "fneg",
      "add",           "fadd",        "sub",         "fsub",
      "mul",           "fmul",        "udiv",        "sdiv",
      "fdiv",          "urem",        "srem",        "frem",
      "shl",           "lshr",        "ashr",        "and",
      "or",            "xor",         "extractelement",
      "insertelement", "shufflevector", "extractvalue", "insertvalue",
      "alloca",        "load",        "store",       "fence",
      "cmpxchg",       "atomicrmw",   "getelementptr",
      "trunc",         "zext",        "sext",        "fptrunc",
      "fpext",         "fptoui",      "fptosi",      "uitofp",
      "sitofp",        "ptrtoint",    "inttoptr",    "bitcast",
      "addrspacecast", "icmp",        "fcmp",        "phi",
      "select",        "freeze",      "call",        "va_arg",
      "landingpad",    "catchpad",    "cleanuppad"};
  for (const char *const opcode : kOpcodes) {
    if (token == opcode) {
      return true;
    }
  }
  return false;
}

Objc3IRInstructionLineInfo ClassifyObjc3IRInstructionLine(
    const std::string &line) {
  Objc3IRInstructionLineInfo info;
  const std::string code = Objc3IRCodeBeforeComment(line);
  const std::string trimmed = TrimLeft(code);
  if (trimmed.empty() || StartsWith(trimmed, "}") ||
      StartsWith(trimmed, "!") || StartsWith(trimmed, "@") ||
      StartsWith(trimmed, "[") || StartsWith(trimmed, "]") ||
      StartsWith(trimmed, ",") || StartsWith(trimmed, "define ") ||
      StartsWith(trimmed, "declare ") || EndsWith(trimmed, ':') ||
      code.find("!dbg") != std::string::npos) {
    return info;
  }

  std::string instruction_text = Objc3IRInstructionTextWithoutResult(trimmed);
  std::string token = ReadObjc3IRFirstToken(instruction_text);
  while (token == "tail" || token == "musttail" || token == "notail") {
    instruction_text = TrimLeft(instruction_text.substr(token.size()));
    token = ReadObjc3IRFirstToken(instruction_text);
  }
  if (!IsObjc3IRInstructionOpcode(token)) {
    return info;
  }

  info.starts_instruction = true;
  AccumulateObjc3IRDelimiterDepths(code, info.depth_after_line);
  info.enters_continuation =
      Objc3IRDelimiterDepthIsOpen(info.depth_after_line) ||
      Objc3IRLineImpliesContinuation(code);
  info.continues =
      info.depth_after_line.underflow || info.enters_continuation;
  info.attachable = !info.continues;
  return info;
}

void AdvanceObjc3IRInstructionContinuation(
    const std::string &line,
    Objc3IRInstructionContinuationState &state) {
  const std::string code = Objc3IRCodeBeforeComment(line);
  AccumulateObjc3IRDelimiterDepths(code, state.depth);
  state.active = state.depth.underflow ||
                 Objc3IRDelimiterDepthIsOpen(state.depth) ||
                 Objc3IRLineImpliesContinuation(code);
  if (!state.active) {
    state.depth = Objc3IRDelimiterDepth{};
  }
}

std::map<std::string, Objc3IRDebugSourceSite> BuildObjc3IRDebugSourceSites(
    const Objc3IRModuleBodyOrchestrationOptions &options) {
  std::map<std::string, Objc3IRDebugSourceSite> sites;
  for (const FunctionDecl *function : options.function_definitions) {
    if (function == nullptr || function->name.empty()) {
      continue;
    }
    sites[function->name] = Objc3IRDebugSourceSite{
        function->name,
        ClampObjc3SourceCoordinate(function->line),
        ClampObjc3SourceCoordinate(function->column),
    };
  }
  for (const Objc3IRMethodDefinition &method_def : options.method_definitions) {
    if (method_def.method == nullptr || method_def.symbol.empty()) {
      continue;
    }
    const Objc3MethodDecl &method = *method_def.method;
    const std::string display_name =
        method.selector.empty() ? method_def.symbol : method.selector;
    sites[method_def.symbol] = Objc3IRDebugSourceSite{
        display_name,
        ClampObjc3SourceCoordinate(method.line),
        ClampObjc3SourceCoordinate(method.column),
    };
  }
  return sites;
}

void EmitObjc3IRDebugMetadataBlock(
    const std::string &source_filename,
    const Objc3IRDebugMetadataRootIds &root_ids,
    const std::vector<Objc3IRDebugMetadataIds> &metadata,
    std::ostringstream &out) {
  if (metadata.empty()) {
    return;
  }
  out << "\n!llvm.dbg.cu = !{!" << root_ids.compile_unit_id << "}\n";
  if (!root_ids.module_flags_present) {
    out << "!llvm.module.flags = !{!" << root_ids.debug_info_version_flag_id
        << "}\n";
  }
  out << "!objc3.ir.debug.actual_instruction_annotations = !{";
  bool first_instruction_location = true;
  for (const Objc3IRDebugMetadataIds &entry : metadata) {
    for (const std::size_t location_id : entry.instruction_location_ids) {
      if (!first_instruction_location) {
        out << ", ";
      }
      first_instruction_location = false;
      out << "!" << location_id;
    }
  }
  out << "}\n";
  out << "!" << root_ids.compile_unit_id
      << " = distinct !DICompileUnit(language: DW_LANG_C99, file: !"
      << root_ids.file_id
      << ", "
         "producer: \"objc3c-native\", isOptimized: false, runtimeVersion: 0, "
         "emissionKind: LineTablesOnly)\n";
  out << "!" << root_ids.file_id << " = !DIFile(filename: \""
      << EscapeObjc3IRDebugMetadataString(source_filename)
      << "\", directory: \".\")\n";
  out << "!" << root_ids.subroutine_type_id
      << " = !DISubroutineType(types: !" << root_ids.type_list_id << ")\n";
  out << "!" << root_ids.type_list_id << " = !{null}\n";
  out << "!" << root_ids.retained_nodes_id << " = !{}\n";
  if (root_ids.emit_debug_info_version_flag) {
    out << "!" << root_ids.debug_info_version_flag_id
        << " = !{i32 2, !\"Debug Info Version\", i32 3}\n";
  }
  for (const Objc3IRDebugMetadataIds &entry : metadata) {
    out << "!" << entry.subprogram_id
        << " = distinct !DISubprogram(name: \""
        << EscapeObjc3IRDebugMetadataString(entry.site.display_name)
        << "\", scope: !" << root_ids.file_id << ", file: !"
        << root_ids.file_id << ", line: " << entry.site.line
        << ", type: !" << root_ids.subroutine_type_id
        << ", scopeLine: " << entry.site.line
        << ", spFlags: DISPFlagDefinition, unit: !"
        << root_ids.compile_unit_id << ", retainedNodes: !"
        << root_ids.retained_nodes_id << ")\n";
    for (const std::size_t location_id : entry.instruction_location_ids) {
      out << "!" << location_id << " = !DILocation(line: "
          << entry.site.line << ", column: " << entry.site.column
          << ", scope: !" << entry.subprogram_id << ")\n";
    }
  }
}

bool HasObjc3IRInstructionDebugLocations(
    const std::vector<Objc3IRDebugMetadataIds> &metadata) {
  return std::any_of(
      metadata.begin(),
      metadata.end(),
      [](const Objc3IRDebugMetadataIds &entry) {
        return !entry.instruction_location_ids.empty();
      });
}

}  // namespace

std::string AttachObjc3IRSourceLineDebugMetadata(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    const std::string &ir) {
  if (HasExistingObjc3IRDebugMetadata(ir)) {
    return ir;
  }

  const std::map<std::string, Objc3IRDebugSourceSite> sites =
      BuildObjc3IRDebugSourceSites(options);
  if (sites.empty()) {
    return ir;
  }

  const Objc3IRDebugMetadataRootIds root_ids =
      AllocateObjc3IRDebugMetadataRootIds(ScanObjc3IRExistingMetadata(ir));
  std::istringstream in(ir);
  std::ostringstream out;
  std::vector<Objc3IRDebugMetadataIds> metadata;
  std::map<std::string, std::size_t> metadata_index_by_symbol;
  std::string active_symbol;
  std::string pending_symbol;
  std::size_t next_metadata_id = root_ids.next_id;
  Objc3IRInstructionContinuationState instruction_continuation;
  std::string line;
  while (std::getline(in, line)) {
    std::string defined_symbol;
    if (ExtractObjc3IRDefinedSymbol(line, defined_symbol)) {
      const auto site = sites.find(defined_symbol);
      if (site != sites.end()) {
        Objc3IRDebugMetadataIds ids;
        ids.site = site->second;
        ids.subprogram_id = next_metadata_id++;
        const std::size_t metadata_index = metadata.size();
        metadata.push_back(ids);
        metadata_index_by_symbol[defined_symbol] = metadata_index;
        if (line.rfind('{') != std::string::npos) {
          active_symbol = defined_symbol;
          pending_symbol.clear();
          instruction_continuation = Objc3IRInstructionContinuationState{};
          out << AttachObjc3IRDebugMetadataToDefinition(
                     line, ids.subprogram_id)
              << "\n";
          continue;
        }
        active_symbol.clear();
        pending_symbol = defined_symbol;
        instruction_continuation = Objc3IRInstructionContinuationState{};
      } else {
        active_symbol.clear();
        pending_symbol.clear();
        instruction_continuation = Objc3IRInstructionContinuationState{};
      }
    } else if (!pending_symbol.empty() &&
               line.rfind('{') != std::string::npos) {
      active_symbol = pending_symbol;
      pending_symbol.clear();
      instruction_continuation = Objc3IRInstructionContinuationState{};
      const auto metadata_index = metadata_index_by_symbol.find(active_symbol);
      if (metadata_index != metadata_index_by_symbol.end()) {
        out << AttachObjc3IRDebugMetadataToDefinition(
                   line, metadata[metadata_index->second].subprogram_id)
            << "\n";
        continue;
      }
    } else if (!instruction_continuation.active &&
               StartsWith(TrimLeft(line), "}")) {
      active_symbol.clear();
      pending_symbol.clear();
      instruction_continuation = Objc3IRInstructionContinuationState{};
    }

    if (!active_symbol.empty()) {
      if (instruction_continuation.active) {
        AdvanceObjc3IRInstructionContinuation(line, instruction_continuation);
      } else {
        const Objc3IRInstructionLineInfo instruction =
            ClassifyObjc3IRInstructionLine(line);
        if (instruction.starts_instruction && instruction.enters_continuation) {
          instruction_continuation.active = true;
          instruction_continuation.depth = instruction.depth_after_line;
        } else if (instruction.attachable) {
          const auto metadata_index =
              metadata_index_by_symbol.find(active_symbol);
          if (metadata_index != metadata_index_by_symbol.end()) {
            const std::size_t location_id = next_metadata_id++;
            metadata[metadata_index->second].instruction_location_ids.push_back(
                location_id);
            out << AttachObjc3IRDebugMetadataToInstruction(line, location_id)
                << "\n";
            continue;
          }
        }
      }
    }
    out << line << "\n";
  }

  if (metadata.empty() || !HasObjc3IRInstructionDebugLocations(metadata)) {
    return ir;
  }
  std::string rewritten_ir = out.str();
  if (root_ids.reference_debug_info_version_flag_in_module_flags) {
    std::string module_flags_updated_ir;
    if (!AppendObjc3IRDebugInfoVersionFlagToModuleFlags(
            rewritten_ir, root_ids.debug_info_version_flag_id,
            module_flags_updated_ir)) {
      return ir;
    }
    rewritten_ir = std::move(module_flags_updated_ir);
  }
  std::ostringstream final_ir;
  final_ir << rewritten_ir;
  EmitObjc3IRDebugMetadataBlock(
      BuildObjc3IRDebugSourceFilename(options.program), root_ids, metadata,
      final_ir);
  return final_ir.str();
}
