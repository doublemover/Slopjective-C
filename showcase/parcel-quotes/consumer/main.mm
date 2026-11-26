#include <array>
#include <iomanip>
#include <iostream>
#include <span>
#include <string_view>

@import Parcel.Quotes;

struct Parcel {
  std::string_view label;
  int grams;
  bool express;
};

static std::string_view error_message(QuoteError error) {
  switch (error) {
    case QuoteError::invalid_weight: return "weight must be positive";
    case QuoteError::above_limit: return "weight exceeds 20 kg";
  }
  return "unknown quote error";
}

static void print_quotes(std::span<const Parcel> parcels) {
  for (const Parcel &parcel : parcels) {
    std::cout << parcel.label << " (" << parcel.grams << " g, "
              << (parcel.express ? "express" : "standard") << "): ";
    auto result = @Parcel.Quotes.quote_shipment(parcel.grams, parcel.express);
    match (result) {
      case .Ok(let quote): {
        std::cout << '$' << quote.cents / 100 << '.' << std::setfill('0')
                  << std::setw(2) << quote.cents % 100 << std::setfill(' ')
                  << ", " << quote.business_days << " business days\n";
      }
      case .Err(let error): {
        std::cout << "rejected, " << error_message(error) << '\n';
      }
    }
  }
}

int main() {
  constexpr std::array parcels{
      Parcel{"Books", 1250, false},
      Parcel{"Glassware", 750, true},
      Parcel{"Empty parcel", 0, false},
      Parcel{"Oversized carton", 22000, true},
  };
  print_quotes(parcels);
}
