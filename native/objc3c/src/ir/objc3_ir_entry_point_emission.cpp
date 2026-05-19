#include "ir/objc3_ir_entry_point_emission.h"

#include <cstddef>
#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_type_model.h"

namespace {

const LoweredFunctionSignature *LookupEntryPointFunctionSignature(
    const std::map<std::string, LoweredFunctionSignature> &function_signatures,
    const std::string &name) {
  auto it = function_signatures.find(name);
  return it == function_signatures.end() ? nullptr : &it->second;
}

}  // namespace

void EmitObjc3IREntryPoint(
    const Objc3Program &program,
    const std::unordered_map<std::string, std::size_t> &function_arity,
    const std::map<std::string, LoweredFunctionSignature> &function_signatures,
    std::ostringstream &out) {
  auto main_it = function_arity.find("main");
  const bool has_zero_arity_main =
      main_it != function_arity.end() && main_it->second == 0;
  if (has_zero_arity_main) {
    out << "define i32 @objc3c_entry() {\n";
    out << "entry:\n";
    ValueType main_return_type = ValueType::I32;
    const LoweredFunctionSignature *main_signature =
        LookupEntryPointFunctionSignature(function_signatures, "main");
    if (main_signature != nullptr) {
      main_return_type = main_signature->return_type;
    }
    const bool main_throws =
        main_signature != nullptr && main_signature->throws_declared;
    if (main_throws) {
      out << "  %main.error.addr = alloca i32, align 4\n";
      out << "  store i32 0, ptr %main.error.addr, align 4\n";
    }
    if (main_return_type == ValueType::Void) {
      out << "  call void @main(";
      if (main_throws) {
        out << "ptr %main.error.addr";
      }
      out << ")\n";
      out << "  ret i32 0\n";
    } else {
      out << "  %call_main = call " << LLVMScalarType(main_return_type)
          << " @main(";
      if (main_throws) {
        out << "ptr %main.error.addr";
      }
      out << ")\n";
      if (main_return_type == ValueType::Bool) {
        out << "  %call_main_i32 = zext i1 %call_main to i32\n";
        out << "  ret i32 %call_main_i32\n";
      } else {
        out << "  ret i32 %call_main\n";
      }
    }
    out << "}\n";
    return;
  }

  out << "define internal i32 @objc3c_entry() {\n";
  out << "entry:\n";
  std::string previous = "0";
  for (std::size_t i = 0; i < program.globals.size(); ++i) {
    const auto &global = program.globals[i];
    const std::string load_name = "%entry_load_" + std::to_string(i);
    const std::string sum_name = "%entry_sum_" + std::to_string(i);
    out << "  " << load_name << " = load i32, ptr @" << global.name
        << ", align 4\n";
    out << "  " << sum_name << " = add i32 " << previous << ", "
        << load_name << "\n";
    previous = sum_name;
  }
  out << "  ret i32 " << previous << "\n";
  out << "}\n";
}
