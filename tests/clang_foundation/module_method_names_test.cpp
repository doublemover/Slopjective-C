#include "Plugins/ExpressionParser/Clang/ObjC3MethodNames.h"
#include <cassert>

int main() {
  using lldb_private::ParseObjC3MethodNames;
  auto direct =
      ParseObjC3MethodNames("__objc3_method_c_R3_466f6f_C0__S4_6261723a_D");
  assert(direct && direct->Class == "Foo" && direct->Category.empty() &&
         direct->Selector == "bar:" && direct->IsClassMethod &&
         direct->IsDirect);
  auto instance = ParseObjC3MethodNames(
      "__objc3_method_i_R4_5f466f6f_C3_435f44_S4_615f623a_M");
  assert(instance && instance->Class == "_Foo" && instance->Category == "C_D" &&
         instance->Selector == "a_b:" && !instance->IsClassMethod &&
         !instance->IsDirect);
  for (const char *invalid :
       {"_c_Foo__bar_", "__objc3_method_x_R3_466f6f_C0__S1_61_M",
        "__objc3_method_c_R999999999999999999999999999999_00_C0__S1_61_M",
        "__objc3_method_c_R3_466f_C0__S1_61_M",
        "__objc3_method_c_R3_466fzz_C0__S1_61_M",
        "__objc3_method_c_R0__C0__S1_61_M",
        "__objc3_method_c_R3_466f6f_C0__S0__M",
        "__objc3_method_c_R3_466f6f_C0__S1_61_X",
        "__objc3_method_c_R3_466f6f_C0__S1_61_M_extra"})
    assert(!ParseObjC3MethodNames(invalid));
}
