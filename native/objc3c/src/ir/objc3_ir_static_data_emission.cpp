#include "ir/objc3_ir_static_data_emission.h"

#include <cstddef>
#include <sstream>

#include "ir/objc3_ir_c_string.h"

bool ResolveGlobalInitializerValues(const std::vector<GlobalDecl> &globals,
                                    std::vector<int> &values);

bool EmitObjc3IRStaticData(
    const Objc3IRStaticDataEmissionOptions &options, std::ostringstream &out,
    std::string &error) {
  std::vector<int> resolved_global_values;
  if (!ResolveGlobalInitializerValues(options.globals, resolved_global_values) ||
      resolved_global_values.size() != options.globals.size()) {
    error = "global initializer failed const evaluation";
    return false;
  }

  options.global_const_values.clear();
  options.global_nil_proven_symbols.clear();
  for (std::size_t i = 0; i < options.globals.size(); ++i) {
    const auto &global = options.globals[i];
    if (options.mutable_global_symbols.find(global.name) ==
        options.mutable_global_symbols.end()) {
      options.global_const_values[global.name] = resolved_global_values[i];
    }
    out << "@" << global.name << " = global i32 " << resolved_global_values[i]
        << ", align 4\n";
  }
  for (const auto &global : options.globals) {
    if (options.mutable_global_symbols.find(global.name) !=
        options.mutable_global_symbols.end()) {
      continue;
    }
    if (options.is_compile_time_global_nil_expr(global.value.get())) {
      options.global_nil_proven_symbols.insert(global.name);
    }
  }
  if (!options.globals.empty()) {
    out << "\n";
  }

  for (const auto &artifact : options.metaprogramming_global_artifacts) {
    std::string payload = artifact.payload;
    payload.push_back('\0');
    out << "@" << artifact.symbol << " = private constant ["
        << payload.size() << " x i8] c\"" << EscapeCStringLiteral(payload)
        << "\", align 1\n";
  }
  if (!options.metaprogramming_global_artifacts.empty()) {
    out << "\n";
  }

  return true;
}
