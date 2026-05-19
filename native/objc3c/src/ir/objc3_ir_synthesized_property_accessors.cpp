#include "ir/objc3_ir_synthesized_property_accessors.h"

#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_type_model.h"
#include "lower/objc3_lowering_contract.h"
#include "support/objc3_property_storage_profile_helpers.h"

#include <string>

bool EmitObjc3IRSynthesizedPropertyAccessorMethod(
    const Objc3IRMethodDefinition &method_def, std::ostringstream &out,
    Objc3IRSynthesizedPropertyAccessorEmissionStats &stats) {
  if (method_def.synthetic_method_kind !=
          Objc3IRSyntheticMethodKind::PropertyGetter &&
      method_def.synthetic_method_kind !=
          Objc3IRSyntheticMethodKind::PropertySetter) {
    return false;
  }

  const bool uses_weak_runtime_hooks =
      objc3c::support::UsesWeakCurrentPropertyRuntimeHelper(
          method_def.synthesized_ownership_runtime_hook_profile);
  const bool uses_strong_runtime_hooks =
      objc3c::support::UsesStrongOwnedCurrentPropertyExchange(
          method_def.synthesized_ownership_lifetime_profile,
          method_def.synthesized_accessor_ownership_profile);
  const char *llvm_value_type = LLVMScalarType(method_def.synthesized_value_type);
  if (method_def.synthetic_method_kind ==
      Objc3IRSyntheticMethodKind::PropertyGetter) {
    ++stats.getter_definition_count;
    out << "define " << llvm_value_type << " @" << method_def.symbol
        << "() {\n";
    out << "entry:\n";
    const std::string loaded_value = "%objc3_property_slot";
    out << "  " << loaded_value << " = call i32 @"
        << (uses_weak_runtime_hooks ? kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
                                    : kObjc3RuntimeReadCurrentPropertyI32Symbol)
        << "()\n";
    if (uses_weak_runtime_hooks) {
      ++stats.weak_current_property_load_helper_call_count;
    } else {
      ++stats.current_property_read_helper_call_count;
    }
    std::string returned_value = loaded_value;
    if (uses_strong_runtime_hooks) {
      const std::string retained_value = "%objc3_property_retained";
      const std::string autoreleased_value = "%objc3_property_autoreleased";
      out << "  " << retained_value << " = call i32 @"
          << kObjc3RuntimeRetainI32Symbol << "(i32 " << loaded_value << ")\n";
      out << "  " << autoreleased_value << " = call i32 @"
          << kObjc3RuntimeAutoreleaseI32Symbol << "(i32 " << retained_value
          << ")\n";
      ++stats.retain_helper_call_count;
      ++stats.autorelease_helper_call_count;
      returned_value = autoreleased_value;
    }
    if (method_def.synthesized_value_type == ValueType::Bool) {
      out << "  %objc3_property_value = icmp ne i32 %objc3_property_slot, 0\n";
      out << "  ret i1 %objc3_property_value\n";
    } else {
      out << "  ret i32 " << returned_value << "\n";
    }
    out << "}\n";
    return true;
  }

  ++stats.setter_definition_count;
  out << "define void @" << method_def.symbol << "(" << llvm_value_type
      << " %arg0) {\n";
  out << "entry:\n";
  std::string stored_value = "%arg0";
  if (method_def.synthesized_value_type == ValueType::Bool) {
    out << "  %objc3_property_value = zext i1 %arg0 to i32\n";
    stored_value = "%objc3_property_value";
  }
  if (uses_weak_runtime_hooks) {
    out << "  call void @" << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
        << "(i32 " << stored_value << ")\n";
    ++stats.weak_current_property_store_helper_call_count;
  } else if (uses_strong_runtime_hooks) {
    out << "  %objc3_property_retained = call i32 @"
        << kObjc3RuntimeRetainI32Symbol << "(i32 " << stored_value << ")\n";
    out << "  %objc3_property_previous = call i32 @"
        << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
        << "(i32 %objc3_property_retained)\n";
    out << "  %objc3_property_release = call i32 @"
        << kObjc3RuntimeReleaseI32Symbol
        << "(i32 %objc3_property_previous)\n";
    ++stats.retain_helper_call_count;
    ++stats.current_property_exchange_helper_call_count;
    ++stats.release_helper_call_count;
  } else {
    out << "  call void @" << kObjc3RuntimeWriteCurrentPropertyI32Symbol
        << "(i32 " << stored_value << ")\n";
    ++stats.current_property_write_helper_call_count;
  }
  out << "  ret void\n";
  out << "}\n";
  return true;
}
