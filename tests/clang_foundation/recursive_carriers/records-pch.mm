int main() {
  {
    CarrierRecord source = record_make(80);
    CarrierRecord copied = record_pch_copy(source);
    CarrierRecord moved = record_pch_move(source);
    CarrierAggregate aggregate{None(RecordLeaf), Some(RecordLeaf, RecordLeaf(82))};
    CarrierAggregate aggregate_copy = record_pch_copy(aggregate);
    if (record_live != 8 || record_copies != 3 || record_moves != 2 ||
        hasValue(aggregate_copy.first) || !hasValue(aggregate_copy.second))
      return 1;
  }
  return record_live || record_constructions + record_copies + record_moves !=
                            record_destructions ? 2 : 0;
}
