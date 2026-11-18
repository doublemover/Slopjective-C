import ForeignCxx

let before: MeterAPI.Reading = MeterAPI.initialReading()
let after: MeterAPI.Reading = MeterAPI.adjustedReading(before, 2)
let count: Int32 = after.count
let total: Int64 = after.total
precondition(MemoryLayout<MeterAPI.Reading>.size == 16)
precondition(MemoryLayout<MeterAPI.Reading>.alignment == 8)
precondition(before.count == 40 && before.total == 2147483688)
precondition(count == 42 && total == 2147483690)

@inline(never)
func checkOwner() -> (Int32, Int32) {
  let first: MeterAPI.ReadingOwner = MeterAPI.initialOwner(40)
  let adjusted: MeterAPI.ReadingOwner = MeterAPI.adjustedOwner(first, 2)
  let expected = (first.amount(), adjusted.amount())
  precondition(expected == (40, 42))
  withExtendedLifetime((first, adjusted)) {
    precondition(MeterAPI.ownerAllocations() - MeterAPI.ownerDestructions() >= 2)
    precondition(MeterAPI.ownerCopies() >= 1)
  }
  return expected
}

precondition(MeterAPI.ownerAllocations() == 0 && MeterAPI.ownerDestructions() == 0)
let (ownerBefore, ownerAfter) = checkOwner()
let allocations = MeterAPI.ownerAllocations()
let destructions = MeterAPI.ownerDestructions()
let copies = MeterAPI.ownerCopies()
precondition(allocations == destructions && copies >= 1)
print("{\"before\":\(before.count),\"count\":\(count),\"total\":\(total)," +
      "\"owner_before\":\(ownerBefore),\"owner_after\":\(ownerAfter),\"owner_allocations\":\(allocations)," +
      "\"owner_destructions\":\(destructions),\"owner_copies\":\(copies)}")
