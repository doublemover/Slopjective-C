using Strong = __objc3_optional(id);
using Weak = __objc3_optional(id __weak);
enum class DataMode : unsigned { Copy = 1, Move = 2 };
struct CxxData { int marker; Strong value; Weak weak; Strong values[2]; };
extern "C" {
id record_object(int key) __attribute__((ns_returns_retained));
int record_destroyed(int key);
CxxData c_data_echo(CxxData value);
}
CxxData cpp_data_echo(CxxData value);
int data_overload(int value);
long data_overload(long value);
DataMode data_mode(void);
