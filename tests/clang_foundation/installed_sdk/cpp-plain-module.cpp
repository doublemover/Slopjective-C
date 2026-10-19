using PlainScalar = long;
enum class PlainMode : unsigned { Copy = 1, Move = 2 };
struct PlainData { PlainScalar marker; PlainScalar values[2]; };
extern "C" PlainData c_plain_echo(PlainData value);
PlainData cpp_plain_echo(PlainData value);
int plain_overload(int value);
long plain_overload(long value);
PlainMode plain_mode(void);
