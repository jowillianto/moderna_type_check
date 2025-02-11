module;
#include <format>
#include <string_view>
#include <tuple>
export module moderna.type_check:main_types;
import :matcher;
import :constraints;

namespace moderna::type_check {
  export struct basic_type_view {
    std::string_view type;
  };
  export template <typename type_t> struct list_type_view {
    type_t child;
  };
  export template <typename... type_t> struct union_type_view {
    std::tuple<type_t...> children;
    constexpr union_type_view(type_t &&...type) : children(std::forward<type_t>(type)...) {}
  };
  export template <typename type_t> struct optional_type_view {
    type_t child;
  };
  export template <typename... type_t> struct tuple_type_view {
    std::tuple<type_t...> children;
    constexpr tuple_type_view(type_t &&...type) : children(std::forward<type_t>(type)...) {}
  };
  export template <typename type_t> struct variadic_type_view {
    type_t child;
  };
  export struct any_type_view {};

  /*
    Type Matching Implementation
  */
  /*
    The following implements the standard functions for conversions between views and owners.
    and generics.
  */
  template <typename type_t>
    requires(is_viewable<type_t>)
  struct type_options<type_t> {
    static constexpr bool allow_empty(const type_t &t);
    static constexpr bool allow_multiple(const type_t &t);
  };

  /*
    Converts a viewable into its view. This is because only the view can be type matched.
  */
  template <typename source_t, typename target_t>
    requires(is_viewable<source_t> || is_viewable<target_t>)
  struct type_matcher<source_t, target_t> {
    static constexpr bool match(const source_t &s, const target_t &t);
  };
  /*
    Compares a basic view with another basic view
  */
  template <> struct type_matcher<basic_type_view, basic_type_view> {
    static constexpr bool match(const basic_type_view &s, const basic_type_view &t);
  };
  /*
    Compares a list view with another list view. Since generics are supported,
    then this supports a generic too and will run correctly.
  */
  template <typename source_t, typename target_t>
  struct type_matcher<list_type_view<source_t>, list_type_view<target_t>> {
    static constexpr bool match(
      const list_type_view<source_t> &s, const list_type_view<target_t> &t
    );
  };
  /*
    compares an optional with another optional. This compares the child of the optional.
  */
  template <typename source_t, typename target_t>
  struct type_matcher<optional_type_view<source_t>, optional_type_view<target_t>> {
    static constexpr bool match(
      const optional_type_view<source_t> &s, const optional_type_view<target_t> &t
    );
  };
  template <typename type_t> struct type_options<optional_type_view<type_t>> {
    static constexpr bool allow_empty(const optional_type_view<type_t> &);
  };
  template <typename type_t> struct type_options<variadic_type_view<type_t>> {
    static constexpr bool allow_multiple(const variadic_type_view<type_t> &);
  };
  /*
    is source -> optional, compare the source with the child of the optional.
  */
  template <typename source_t, typename target_t>
  struct type_matcher<source_t, optional_type_view<target_t>> {
    static constexpr bool match(const source_t &s, const optional_type_view<target_t> &t);
  };

  /*
    compares a union with another union, asserts that target union is a subset of the source union.
    i.e. all source union have a target
  */
  template <typename... source_t, typename... target_t>
  struct type_matcher<union_type_view<source_t...>, union_type_view<target_t...>> {
    static constexpr bool match(
      const union_type_view<source_t...> &s, const union_type_view<target_t...> &t
    );
  };

  /*
    compares a union with another union, asserts that target union is matchable with the source
    type.
  */
  template <typename source_t, typename... target_t>
  struct type_matcher<source_t, union_type_view<target_t...>> {
    static constexpr bool match(const source_t &s, const union_type_view<target_t...> &t);
  };

  /*
    Tuple types have to be exactly the same
  */
  template <typename... source_t, typename... target_t>
  struct type_matcher<tuple_type_view<source_t...>, tuple_type_view<target_t...>> {
    static constexpr bool match(
      const tuple_type_view<source_t...> &s, const tuple_type_view<target_t...> &t
    );
    template <size_t i, typename... left_t, typename... right_t>
    static constexpr bool tup_vec_match(
      const std::tuple<left_t...> &left, const std::tuple<right_t...> &right
    );
  };
  /*
    variadic types work the same way lists do.
  */
  template <typename source_t, typename target_t>
  struct type_matcher<variadic_type_view<source_t>, variadic_type_view<target_t>> {
    static constexpr bool match(
      const variadic_type_view<source_t> &s, const variadic_type_view<target_t> &t
    );
  };
  /*
    variadics will additionally match with children of itself.
  */
  template <typename source_t, typename target_t>
  struct type_matcher<source_t, variadic_type_view<target_t>> {
    static constexpr bool match(const source_t &s, const variadic_type_view<target_t> &t);
  };
  /*
    any type
  */
  template <typename source_t> struct type_matcher<source_t, any_type_view> {
    static constexpr bool match(const source_t &s, const any_type_view &t);
  };
  template <typename target_t> struct type_matcher<any_type_view, target_t> {
    static constexpr bool match(const any_type_view &s, const target_t &t);
  };

  /*
    IMPLEMENTATION SECTION
  */

  template <typename type_t>
    requires(is_viewable<type_t>)
  constexpr bool type_options<type_t>::allow_empty(const type_t &t) {
    return ::moderna::type_check::allow_empty(as_view(t));
  }
  template <typename type_t>
    requires(is_viewable<type_t>)
  constexpr bool type_options<type_t>::allow_multiple(const type_t &t) {
    return ::moderna::type_check::allow_multiple(as_view(t));
  }

  template <typename type_t>
  constexpr bool
  type_options<optional_type_view<type_t>>::allow_empty(const optional_type_view<type_t> &) {
    return true;
  }
  template <typename type_t>
  constexpr bool
  type_options<variadic_type_view<type_t>>::allow_multiple(const variadic_type_view<type_t> &) {
    return true;
  };

  template <typename source_t, typename target_t>
    requires(is_viewable<source_t> || is_viewable<target_t>)
  constexpr bool type_matcher<source_t, target_t>::match(const source_t &s, const target_t &t) {
    if constexpr (is_viewable<source_t> && is_viewable<target_t>)
      return is_connectable(as_view(s), as_view(t));
    else if constexpr (is_viewable<source_t>)
      return is_connectable(as_view(s), t);
    else
      return is_connectable(s, as_view(t));
  }

  constexpr bool type_matcher<basic_type_view, basic_type_view>::match(
    const basic_type_view &s, const basic_type_view &t
  ) {
    return s.type == t.type;
  }

  // Specialization for list_type_view
  template <typename source_t, typename target_t>
  constexpr bool type_matcher<list_type_view<source_t>, list_type_view<target_t>>::match(
    const list_type_view<source_t> &s, const list_type_view<target_t> &t
  ) {
    return is_connectable(s.child, t.child);
  }

  // Specialization for optional_type_view
  template <typename source_t, typename target_t>
  constexpr bool type_matcher<optional_type_view<source_t>, optional_type_view<target_t>>::match(
    const optional_type_view<source_t> &s, const optional_type_view<target_t> &t
  ) {
    return is_connectable(s.child, t.child);
  }

  // Specialization for source_t and optional_type_view<target_t>
  template <typename source_t, typename target_t>
  constexpr bool type_matcher<source_t, optional_type_view<target_t>>::match(
    const source_t &s, const optional_type_view<target_t> &t
  ) {
    return is_connectable(s, t.child);
  }

  // Specialization for union_type_view
  template <typename... source_t, typename... target_t>
  constexpr bool type_matcher<union_type_view<source_t...>, union_type_view<target_t...>>::match(
    const union_type_view<source_t...> &s, const union_type_view<target_t...> &t
  ) {
    using source_children_t = decltype(s.children);
    using target_children_t = decltype(t.children);
    return std::apply(
      [&](const auto &...s_type) { return (is_connectable(s_type, t) && ...); }, s.children
    );
    // }
  }

  // Specialization for source_t and union_type_view<target_t...>
  template <typename source_t, typename... target_t>
  constexpr bool type_matcher<source_t, union_type_view<target_t...>>::match(
    const source_t &s, const union_type_view<target_t...> &t
  ) {
    using target_children_t = decltype(t.children);
    return std::apply(
      [&](const auto &...t_type) { return (is_connectable(s, t_type) || ...); }, t.children
    );
  }
  template <typename... source_t, typename... target_t>
  constexpr bool type_matcher<tuple_type_view<source_t...>, tuple_type_view<target_t...>>::match(
    const tuple_type_view<source_t...> &s, const tuple_type_view<target_t...> &t
  ) {
    using source_children_t = decltype(s.children);
    using target_children_t = decltype(t.children);
    constexpr size_t source_size = sizeof...(source_t);
    constexpr size_t target_size = sizeof...(target_t);
    if constexpr (source_size != target_size) {
      return false;
    }
    return tup_vec_match<0>(s.children, t.children);
  }
  template <typename... source_t, typename... target_t>
  template <size_t i, typename... left_t, typename... right_t>
  constexpr bool
  type_matcher<tuple_type_view<source_t...>, tuple_type_view<target_t...>>::tup_vec_match(
    const std::tuple<left_t...> &left, const std::tuple<right_t...> &right
  ) {
    constexpr size_t tup_size = sizeof...(left_t);
    if (!is_connectable(std::get<i>(left), std::get<i>(right))) return false;
    else if constexpr (i + 1 == tup_size)
      return true;
    else
      return tup_vec_match<i + 1>(left, right);
  }
  template <typename source_t, typename target_t>
  constexpr bool type_matcher<variadic_type_view<source_t>, variadic_type_view<target_t>>::match(
    const variadic_type_view<source_t> &s, const variadic_type_view<target_t> &t
  ) {
    return is_connectable(s.child, t.child);
  }
  template <typename source_t, typename target_t>
  constexpr bool type_matcher<source_t, variadic_type_view<target_t>>::match(
    const source_t &s, const variadic_type_view<target_t> &t
  ) {
    return is_connectable(s, t.child);
  }
  template <typename source_t>
  constexpr bool type_matcher<source_t, any_type_view>::match(
    const source_t &s, const any_type_view &t
  ) {
    return true;
  }
  template <typename target_t>
  constexpr bool type_matcher<any_type_view, target_t>::match(
    const any_type_view &s, const target_t &t
  ) {
    return true;
  }

}
namespace tc = moderna::type_check;
/*
  Compile time sanity tests
*/
static_assert(tc::is_connectable(tc::basic_type_view{"A"}, tc::basic_type_view{"A"}));
static_assert(!tc::is_connectable(tc::basic_type_view{"A"}, tc::basic_type_view{"B"}));
static_assert(tc::is_connectable(
  tc::list_type_view{tc::basic_type_view{"A"}}, tc::list_type_view{tc::basic_type_view{"A"}}
));
static_assert(tc::is_connectable(
  tc::optional_type_view{tc::basic_type_view{"A"}}, tc::optional_type_view{tc::basic_type_view{"A"}}
));
static_assert(
  tc::is_connectable(tc::basic_type_view{"A"}, tc::optional_type_view{tc::basic_type_view{"A"}})
);
static_assert(
  !tc::is_connectable(tc::basic_type_view{"B"}, tc::optional_type_view{tc::basic_type_view{"A"}})
);

/*
  Basic[A] -> Union[A, B]
*/
static_assert(tc::is_connectable(
  tc::basic_type_view{"A"}, tc::union_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"})
));

/*
  Basic[C] -> Union[A, B]
*/
static_assert(!tc::is_connectable(
  tc::basic_type_view{"C"}, tc::union_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"})
));

/*
  Union[A, B] -> Union[A, B]
*/
static_assert(tc::is_connectable(
  tc::union_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"}),
  tc::union_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"})
));

/*
  Union[Basic[A], Basic[B]] -> Union[Basic[A], Basic[B], Basic[C]]
*/
static_assert(tc::is_connectable(
  tc::union_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"}),
  tc::union_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"}, tc::basic_type_view{"C"})
));

/*
  Union[List[A], Basic[B]] -> Union[Basic[A], Basic[B], Basic[C]]
*/
static_assert(!tc::is_connectable(
  tc::union_type_view(tc::list_type_view{tc::basic_type_view{"A"}}, tc::basic_type_view{"B"}),
  tc::union_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"}, tc::basic_type_view{"C"})
));

/*
  Tuple[Basic[A], Basic[B]] -> Tuple[Basic[A], Basic[B]]
*/
static_assert(tc::is_connectable(
  tc::tuple_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"}),
  tc::tuple_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"})
));

/*
  Tuple[Basic[A], Basic[B]] -> Tuple[Basic[A], Basic[B], Basic[C]]
*/
static_assert(!tc::is_connectable(
  tc::tuple_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"}),
  tc::tuple_type_view(tc::basic_type_view{"A"}, tc::basic_type_view{"B"}, tc::basic_type_view{"C"})
));

static_assert(tc::is_connectable(
  tc::variadic_type_view{tc::basic_type_view{"A"}}, tc::variadic_type_view{tc::basic_type_view{"A"}}
));

static_assert(
  tc::is_connectable(tc::basic_type_view{"A"}, tc::variadic_type_view{tc::basic_type_view{"A"}})
);
static_assert(tc::allow_multiple(tc::variadic_type_view{tc::basic_type_view{"A"}}));
static_assert(tc::is_allow_multiple<tc::variadic_type_view<tc::basic_type_view>>);
static_assert(tc::is_connectable(tc::basic_type_view{"A"}, tc::any_type_view{}));
static_assert(tc::is_connectable(tc::list_type_view{tc::basic_type_view{"A"}}, tc::any_type_view{})
);
static_assert(tc::is_connectable(
  tc::list_type_view{tc::basic_type_view{"A"}}, tc::list_type_view{tc::any_type_view{}}
));
static_assert(tc::is_connectable(
  tc::tuple_type_view{tc::basic_type_view{"A"}}, tc::tuple_type_view{tc::any_type_view{}}
));
