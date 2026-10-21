// Ordinary Objective-C++ with no Objective-C metadata still has to link.
// In the GNUstep COFF profile, an empty metadata section needs no registration.
static int destroyed;
struct Guard {
  ~Guard() { ++destroyed; }
};

static int throw_and_catch() {
  try {
    Guard guard;
    throw 42;
  } catch (int value) {
    return value;
  }
}

int main() {
  return throw_and_catch() == 42 && destroyed == 1 ? 0 : 1;
}
