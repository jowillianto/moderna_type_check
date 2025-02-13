module;
#include <concepts>
export module moderna.type_check:matcher;

namespace moderna::type_check {
  template <typename source_t, typename target_t> struct type_matcher {
    constexpr static bool match(const source_t &source, const target_t &target) {
      return false;
    }
  };
  template <typename type_t> struct type_options {
    constexpr static bool allow_empty(const type_t &type) {
      return false;
    }
    constexpr static bool allow_multiple(const type_t &type) {
      return false;
    }
  };

  /*
    Compares two types and check if source can be connected to the target. In general, the source
    can be connected to the target if the source is a subtype of the target.
  */

  export template <typename source_t, typename target_t>
  constexpr bool is_connectable(const source_t &source, const target_t &target) {
    return type_matcher<source_t, target_t>::match(source, target);
  }

  export template <typename source_t>
  concept is_allow_empty_defined = requires(const source_t &source) {
    { type_options<source_t>::allow_empty(source) } -> std::same_as<bool>;
  };

  export template <typename source_t>
  concept is_allow_multiple_defined = requires(const source_t &source) {
    { type_options<source_t>::allow_multiple(source) } -> std::same_as<bool>;
  };

  /*
    Compares two types and check if the types can be matched with NULL.
  */
  export template <typename type_t> constexpr bool allow_empty(const type_t &type) {
    if constexpr (is_allow_empty_defined<type_t>) {
      return type_options<type_t>::allow_empty(type);
    } else {
      return false;
    }
  }

  /*
    Compares two types and checks if the current type is a variadic meaning, it can receive a pack
    of other types.
  */
  export template <typename type_t> constexpr bool allow_multiple(const type_t &type) {
    if constexpr (is_allow_multiple_defined<type_t>) {
      return type_options<type_t>::allow_multiple(type);
    } else {
      return false;
    }
  }
}