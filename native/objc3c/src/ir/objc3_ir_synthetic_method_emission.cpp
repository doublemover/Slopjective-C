#include "ir/objc3_ir_synthetic_method_emission.h"

#include <cstdint>
#include <sstream>

#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_synthesized_property_accessors.h"

namespace {

void AccumulateSynthesizedPropertyAccessorStats(
    Objc3IRSyntheticMethodEmissionStats &target,
    const Objc3IRSynthesizedPropertyAccessorEmissionStats &source) {
  target.getter_definition_count += source.getter_definition_count;
  target.setter_definition_count += source.setter_definition_count;
  target.current_property_read_helper_call_count +=
      source.current_property_read_helper_call_count;
  target.current_property_write_helper_call_count +=
      source.current_property_write_helper_call_count;
  target.current_property_exchange_helper_call_count +=
      source.current_property_exchange_helper_call_count;
  target.weak_current_property_load_helper_call_count +=
      source.weak_current_property_load_helper_call_count;
  target.weak_current_property_store_helper_call_count +=
      source.weak_current_property_store_helper_call_count;
  target.retain_helper_call_count += source.retain_helper_call_count;
  target.release_helper_call_count += source.release_helper_call_count;
  target.autorelease_helper_call_count += source.autorelease_helper_call_count;
}

}  // namespace

void EmitObjc3IRSyntheticMethod(
    const Objc3IRMethodDefinition &method_def, std::ostringstream &out,
    Objc3IRSyntheticMethodEmissionStats &stats) {
  if (method_def.synthetic_method_kind ==
      Objc3IRSyntheticMethodKind::MetaprogrammingDerivedEquality) {
    out << "define i32 @" << method_def.symbol << "(i32 %arg0) {\n";
    out << "entry:\n";
    out << "  ret i32 1\n";
    out << "}\n";
    return;
  }
  if (method_def.synthetic_method_kind ==
          Objc3IRSyntheticMethodKind::MetaprogrammingDerivedHash ||
      method_def.synthetic_method_kind ==
          Objc3IRSyntheticMethodKind::MetaprogrammingDerivedDebugDescription) {
    std::uint32_t seed = 2166136261u;
    for (unsigned char ch : method_def.symbol) {
      seed ^= static_cast<std::uint32_t>(ch);
      seed *= 16777619u;
    }
    out << "define i32 @" << method_def.symbol << "() {\n";
    out << "entry:\n";
    out << "  ret i32 " << static_cast<unsigned long long>(seed & 0x7fffffffu)
        << "\n";
    out << "}\n";
    return;
  }

  Objc3IRSynthesizedPropertyAccessorEmissionStats property_stats;
  if (!EmitObjc3IRSynthesizedPropertyAccessorMethod(method_def, out,
                                                    property_stats)) {
    return;
  }
  AccumulateSynthesizedPropertyAccessorStats(stats, property_stats);
}
