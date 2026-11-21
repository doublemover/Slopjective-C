#include "values.h"

int main(void) {
  if (inspect_values())
    return 1;
  if (inspect_scopes(true) != 44 || cleanup_events != 43)
    return 2;
  if (inspect_scopes(false) != 44 || cleanup_events != 4343)
    return 3;
  return 0;
}
