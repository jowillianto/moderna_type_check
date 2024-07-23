module;
#include <concepts>
export module moderna.type_check:matcher;
import :constraints;

namespace moderna::type_check {
  template <typename source_t, typename target_t> struct type_matcher;
  template <typename type_t> struct type_options;
  /*
    If the type is matchable in a way such that
    left >> right is defined.
  */
  template <typename source_t, typename target_t>
  concept is_matchable = requires(const source_t &left, const target_t &right) {
    { type_matcher<source_t, target_t>::match(left, right) } -> std::same_as<bool>;
  };
  /*
    if the type require match
  */
  template <typename type_t>
  concept is_allow_empty = requires(const type_t &type) {
    { type_options<type_t>::allow_empty(type) } -> std::same_as<bool>;
  };
  /*
    If the type accepts multiple entries
  */
  template <typename type_t>
  concept is_allow_multiple = requires(const type_t &type) {
    { type_options<type_t>::allow_multiple(type) } -> std::same_as<bool>;
  };

  export template <typename source_t, typename target_t>
  constexpr bool is_connectable(const source_t &source, const target_t &target) {
    if constexpr (is_matchable<source_t, target_t>) {
      return type_matcher<source_t, target_t>::match(source, target);
    } else {
      return false;
    }
  }

  export template <typename type_t> constexpr bool allow_empty(const type_t &type) {
    if constexpr (is_allow_empty<type_t>) return type_options<type_t>::allow_empty(type);
    else
      return false;
  }
  export template <typename type_t> constexpr bool allow_multiple(const type_t &type) {
    if constexpr (is_allow_multiple<type_t>) return type_options<type_t>::allow_multiple(type);
    else
      return false;
  }
}