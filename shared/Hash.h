#pragma once

#include <string_view>
#include <utility>

namespace TracksAD {
struct string_equal {
  using is_transparent = std::true_type;

  bool operator()(std::string_view l, std::string_view r) const noexcept {
    return l == r;
  }
};

struct string_hash {
  using is_transparent = std::true_type;

  auto operator()(std::string_view str) const noexcept {
    return std::hash<std::string_view>()(str);
  }
};
} // namespace TracksAD
