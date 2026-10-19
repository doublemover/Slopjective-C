@import RecordCarriers;
int main(void) {
  {
    struct RecordCall original=record_make(50), copy=record_echo(original);
    original.value=None(id);
    if (record_destroyed(50)) return 1;
    copy.value=None(id);
    if (record_destroyed(50)!=1 || copy.marker!=42) return 2;
  }
  struct RecordPair values[2] = {
    {0,Some(id,record_object(51)),None(id __weak),{}},
    {1,Some(id,record_object(52)),None(id __weak),{}}
  };
  return !hasValue(values[1].first);
}
