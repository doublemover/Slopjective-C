typedef struct SurfaceValue { int value; } SurfaceValue;
struct HiddenSurface { int private_value; };
#ifdef __cplusplus
class ImplementationOnly {
  int value;
public:
  ImplementationOnly() : value(7) {}
};
template<class T> T private_template(T value) { return value; }
#endif
int surface_hidden(void) { return 1; }
SurfaceValue surface_value(void) {
  struct BodyOnly { int value; } local = {42};
  SurfaceValue result = {local.value};
  return result;
}
