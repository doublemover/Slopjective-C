#include "carrier-record-call.h"
struct RecordCall record_make(int key) {
  struct RecordCall value={42,record_value(key),__objc3_none(id __weak)};
  return value;
}
struct RecordCall record_echo(struct RecordCall value) { return value; }
void record_replace(struct RecordCall value,int key) { value.value=record_value(key); }
void record_native(struct RecordCall value) { @throw record_object(39); }
