module;
#include <rfl/Box.hpp>
#include <rfl/Literal.hpp>
#include <rfl/internal/has_reflector.hpp>
#include <algorithm>
#include <cstddef>
#include <expected>
#include <format>
#include <ranges>
#include <string>
#include <variant>
export module moderna.type_check:types;
import :matcher;

namespace moderna::type_check {
  export struct generic_type {
    /*
      This is a basic type determined by the typed stored in type.
    */
    struct basic_type {
      std::string type;

      basic_type clone() const {
        return basic_type{type};
      }
      friend bool operator==(const basic_type &a, const basic_type &b) {
        return a.type == b.type;
      }
      static basic_type make(std::string t) {
        return basic_type{std::move(t)};
      }
    };
    /*
      Is a list of types contained in the child.
    */
    struct list_type {
      rfl::Literal<"list", "List"> type;
      rfl::Box<generic_type> child;

      list_type clone() const {
        return list_type::make(child->clone());
      }

      friend bool operator==(const list_type &a, const list_type &b) {
        return *a.child == *b.child;
      }

      static list_type make(generic_type g) {
        return list_type{std::string{"List"}, rfl::make_box<generic_type>(std::move(g))};
      }
    };
    /*
      Can be matched with any of the typed contained in children.
    */

    struct union_type {
      rfl::Literal<"union", "Union"> type;
      std::vector<generic_type> child;

      union_type clone() const {
        std::vector<generic_type> new_children;
        new_children.reserve(child.size());
        for (const auto &child : child) {
          new_children.emplace_back(child.clone());
        }
        return union_type::make(std::move(new_children));
      }

      friend bool operator==(const union_type &a, const union_type &b) {
        for (const auto &[a_type, b_type] : std::ranges::zip_view(a.child, b.child)) {
          if (a_type != b_type) return false;
        }
        return true;
      }

      static union_type make(std::vector<generic_type> g) {
        return union_type{std::string{"Union"}, std::move(g)};
      }
    };

    /*
      Is a set of list of type where each type is stored in children.
    */
    struct tuple_type {
      rfl::Literal<"tuple", "Tuple"> type;
      std::vector<generic_type> child;

      tuple_type clone() const {
        std::vector<generic_type> new_children;
        new_children.reserve(child.size());
        for (const auto &child : child) {
          new_children.emplace_back(child.clone());
        }
        return tuple_type::make(std::move(new_children));
      }

      friend bool operator==(const tuple_type &a, const tuple_type &b) {
        for (const auto &[a_type, b_type] : std::ranges::views::zip(a.child, b.child)) {
          if (a_type != b_type) return false;
        }
        return true;
      }

      static tuple_type make(std::vector<generic_type> g) {
        return tuple_type{std::string{"Tuple"}, std::move(g)};
      }
    };

    /*
      Is a type that can be either the stored type or empty.
    */

    struct optional_type {
      rfl::Literal<"optional", "Optional"> type;
      rfl::Box<generic_type> child;

      optional_type clone() const {
        return optional_type::make(child->clone());
      }

      friend bool operator==(const optional_type &a, const optional_type &b) {
        return *a.child == *b.child;
      }

      static optional_type make(generic_type g) {
        return optional_type{std::string{"Optional"}, rfl::make_box<generic_type>(std::move(g))};
      }
    };

    /*
      Can store multiple types of the type stored in child.
    */
    struct variadic_type {
      rfl::Literal<"variadic", "Variadic"> type;
      rfl::Box<generic_type> child;

      variadic_type clone() const {
        return variadic_type::make(child->clone());
      }

      friend bool operator==(const variadic_type &a, const variadic_type &b) {
        return *a.child == *b.child;
      }

      static variadic_type make(generic_type g) {
        return variadic_type{std::string{"Variadic"}, rfl::make_box<generic_type>(std::move(g))};
      }
    };

    struct none_type {
      rfl::Literal<"none", "None"> type;

      friend bool operator==(const none_type &a, const none_type &b) {
        return true;
      }

      none_type clone() const {
        return make();
      }

      static none_type make() {
        return none_type{std::string{"None"}};
      }
    };

    std::variant<
      list_type,
      union_type,
      tuple_type,
      optional_type,
      variadic_type,
      none_type,
      basic_type>
      type;

    /*
      Compares two types together.
    */
    template <typename... Args>
      requires(std::is_constructible_v<decltype(type), Args...>)
    generic_type(Args &&...args) : type(std::forward<Args>(args)...) {}
    generic_type(generic_type &&o) : type(std::move(o.type)) {}
    generic_type(const generic_type &o) : type(o.clone().type) {}

    generic_type &operator=(const generic_type &o) {
      type = std::move(o.clone().type);
      return *this;
    }
    generic_type &operator=(generic_type &&o) {
      type = std::move(o.type);
      return *this;
    }
    friend bool operator==(const generic_type &a, const generic_type &b) {
      return std::visit(
        [&](const auto &t_a) {
          return std::visit(
            [&](const auto &t_b) {
              using type_a_t = std::decay_t<decltype(t_a)>;
              using type_b_t = std::decay_t<decltype(t_b)>;
              if constexpr (std::same_as<type_a_t, type_b_t>) {
                return t_a == t_b;
              } else {
                return false;
              }
            },
            b.type
          );
        },
        a.type
      );
    }

    generic_type clone() const {
      return std::visit([](const auto &t) { return generic_type(t.clone()); }, type);
    }

    template <class T> bool is() const {
      return std::holds_alternative<T>(type);
    }
    bool is_basic_type() const {
      return is<basic_type>();
    }
    bool is_list_type() const {
      return is<list_type>();
    }
    bool is_optional_type() const {
      return is<optional_type>();
    }
    bool is_union_type() const {
      return is<union_type>();
    }
    bool is_tuple_type() const {
      return is<tuple_type>();
    }
    bool is_variadic_type() const {
      return is<variadic_type>();
    }
    bool is_none_type() const {
      return is<none_type>();
    }
    template <class T>
    std::expected<std::reference_wrapper<const T>, std::bad_variant_access> as() const {
      using expected_t = std::expected<std::reference_wrapper<const T>, std::bad_variant_access>;
      try {
        return std::cref(std::get<T>(type));
      } catch (const std::bad_variant_access &e) {
        return std::unexpected{e};
      }
    }
    std::expected<std::reference_wrapper<const basic_type>, std::bad_variant_access> as_basic_type(
    ) const {
      return as<basic_type>();
    }
    std::expected<std::reference_wrapper<const list_type>, std::bad_variant_access> as_list_type(
    ) const {
      return as<list_type>();
    }
    std::expected<std::reference_wrapper<const optional_type>, std::bad_variant_access>
    as_optional_type() const {
      return as<optional_type>();
    }
    std::expected<std::reference_wrapper<const union_type>, std::bad_variant_access> as_union_type(
    ) const {
      return as<union_type>();
    }
    std::expected<std::reference_wrapper<const tuple_type>, std::bad_variant_access> as_tuple_type(
    ) const {
      return as<tuple_type>();
    }
    std::expected<std::reference_wrapper<const variadic_type>, std::bad_variant_access>
    as_variadic_type() const {
      return as<variadic_type>();
    }
    std::expected<std::reference_wrapper<const none_type>, std::bad_variant_access> as_none_type(
    ) const {
      return as<none_type>();
    }
  };

  export using basic_type = generic_type::basic_type;
  export using list_type = generic_type::list_type;
  export using optional_type = generic_type::optional_type;
  export using union_type = generic_type::union_type;
  export using tuple_type = generic_type::tuple_type;
  export using variadic_type = generic_type::variadic_type;
  export using none_type = generic_type::none_type;

};

namespace tc = moderna::type_check;

// Type matching overrides declaration.
template <typename source_t, typename target_t>
  requires(std::same_as<source_t, tc::generic_type> || std::same_as<target_t, tc::generic_type>)
struct tc::type_matcher<source_t, target_t> {
  constexpr static bool match(const source_t &s, const target_t &t);
};

template <> struct tc::type_matcher<tc::basic_type, tc::basic_type> {
  constexpr static bool match(const tc::basic_type &s, const tc::basic_type &t) {
    return s.type == t.type;
  }
};

template <> struct tc::type_matcher<tc::list_type, tc::list_type> {
  constexpr static bool match(const tc::list_type &s, const tc::list_type &t);
};

template <> struct tc::type_matcher<tc::union_type, tc::union_type> {
  constexpr static bool match(const tc::union_type &s, const tc::union_type &t);
};

template <typename source_type_t>
  requires(!std::same_as<source_type_t, tc::union_type>)
struct tc::type_matcher<source_type_t, tc::union_type> {
  constexpr static bool match(const source_type_t &s, const tc::union_type &t);
};

template <typename source_type_t> struct tc::type_matcher<source_type_t, tc::optional_type> {
  constexpr static bool match(const source_type_t &s, const tc::optional_type &t);
};

template <typename source_type_t> struct tc::type_matcher<source_type_t, tc::variadic_type> {
  constexpr static bool match(const source_type_t &s, const tc::variadic_type &t);
};

template <> struct tc::type_matcher<tc::tuple_type, tc::tuple_type> {
  constexpr static bool match(const tc::tuple_type &s, const tc::tuple_type &t);
};

// Type matching overrides implementation
template <typename source_t, typename target_t>
  requires(std::same_as<source_t, tc::generic_type> || std::same_as<target_t, tc::generic_type>)
constexpr bool tc::type_matcher<source_t, target_t>::match(const source_t &s, const target_t &t) {
  if constexpr (std::same_as<source_t, tc::generic_type>) {
    return std::visit(
      [&](const auto &s) {
        if constexpr (std::same_as<target_t, tc::generic_type>) {
          return std::visit([&](const auto &t) { return is_connectable(s, t); }, t.type);
        } else {
          return is_connectable(s, t);
        }
      },
      s.type
    );
  } else {
    return std::visit([&](const auto &t) { return is_connectable(s, t); }, t.type);
  }
}

constexpr bool tc::type_matcher<tc::list_type, tc::list_type>::match(
  const tc::list_type &s, const tc::list_type &t
) {
  return is_connectable(*s.child, *t.child);
}

template <typename source_type_t>
  requires(!std::same_as<source_type_t, tc::union_type>)
constexpr bool tc::type_matcher<source_type_t, tc::union_type>::match(
  const source_type_t &s, const tc::union_type &t
) {
  return std::ranges::any_of(t.child, [&](const auto &t_child) {
    return is_connectable(s, t_child);
  });
}

constexpr bool tc::type_matcher<tc::union_type, tc::union_type>::match(
  const tc::union_type &s, const tc::union_type &t
) {
  return std::ranges::all_of(s.child, [&](const auto &s_child) {
    return is_connectable(s_child, t);
  });
}

template <typename source_type_t>
constexpr bool tc::type_matcher<source_type_t, tc::optional_type>::match(
  const source_type_t &s, const tc::optional_type &t
) {
  if constexpr (std::same_as<source_type_t, none_type>) {
    return true;
  } else if constexpr (std::same_as<source_type_t, tc::optional_type>) {
    return is_connectable(*s.child, *t.child);
  } else {
    return is_connectable(s, *t.child);
  }
}

template <typename source_type_t>
constexpr bool tc::type_matcher<source_type_t, tc::variadic_type>::match(
  const source_type_t &t, const tc::variadic_type &v
) {
  if constexpr (std::same_as<source_type_t, variadic_type>) {
    return is_connectable(*t.child, *v.child);
  } else {
    return is_connectable(t, *v.child);
  }
}

constexpr bool tc::type_matcher<tc::tuple_type, tc::tuple_type>::match(
  const tc::tuple_type &s, const tc::tuple_type &t
) {
  if (s.child.size() != t.child.size()) return false;
  for (const auto &[s_child, t_child] : std::ranges::views::zip(s.child, t.child)) {
    if (!is_connectable(s_child, t_child)) return false;
  }
  return true;
}

// Type Options Override
template <class T>
  requires(std::is_constructible_v<tc::generic_type, T>)
struct tc::type_options<T> {
  constexpr static bool allow_empty(const T &t) {
    return is_connectable<none_type, T>(none_type::make(), t);
  }
};

template <> struct tc::type_options<tc::variadic_type> {
  constexpr static bool allow_multiple(const tc::variadic_type &t) {
    return true;
  }
};

template <> struct tc::type_options<tc::generic_type> {
  constexpr static bool allow_empty(const tc::generic_type &t) {
    return std::visit([](const auto &t) { return tc::allow_empty(t); }, t.type);
  }
  constexpr static bool allow_multiple(const tc::generic_type &t) {
    return std::visit([](const auto &t) { return tc::allow_multiple(t); }, t.type);
  }
};

// RFL Serialization Override
template <> struct rfl::Reflector<tc::basic_type> {
  struct ReflStruct {
    std::string type;
  };
  using ReflType = std::variant<std::string, ReflStruct>;
  static ReflType from(const tc::basic_type &t) {
    return t.type;
  }
  static tc::basic_type to(const ReflType &t) {
    return std::visit(
      [](const auto &v) {
        using type_t = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<type_t, std::string>) {
          return tc::basic_type{v};
        } else {
          return tc::basic_type{v.type};
        }
      },
      t
    );
  }
};

template <> struct rfl::Reflector<tc::generic_type> {
  using ReflType = decltype(tc::generic_type::type);
  static ReflType from(const tc::generic_type &t) {
    return t.clone().type;
  }
  static tc::generic_type to(const ReflType &t) {
    return std::visit([](const auto &t) { return tc::generic_type{t.clone()}; }, t);
  }
};

// Formatter Override
template <typename type_t, class char_type>
  requires(std::same_as<type_t, tc::basic_type> || std::same_as<type_t, tc::list_type> || std::same_as<type_t, tc::union_type> || std::same_as<type_t, tc::tuple_type> || std::same_as<type_t, tc::optional_type> || std::same_as<type_t, tc::variadic_type> || std::same_as<type_t, tc::none_type> || std::same_as<type_t, tc::generic_type>)
struct std::formatter<type_t, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  template <typename format_context_t>
  constexpr decltype(std::declval<format_context_t>().out()) format(
    const tc::basic_type &t, format_context_t &ctx
  ) const {
    return std::format_to(ctx.out(), "{}", t.type);
  }
  template <typename format_context_t>
  constexpr decltype(std::declval<format_context_t>().out()) format(
    const tc::list_type &t, format_context_t &ctx
  ) const {
    std::format_to(ctx.out(), "List[");
    format(*t.child, ctx);
    return std::format_to(ctx.out(), "]");
  }
  template <typename format_context_t>
  constexpr decltype(std::declval<format_context_t>().out()) format(
    const tc::union_type &t, format_context_t &ctx
  ) const {
    std::format_to(ctx.out(), "Union[");
    size_t i = 0;
    for (const auto &child : t.child) {
      format(child, ctx);
      if (i != t.child.size() - 1) {
        std::format_to(ctx.out(), ", ");
      }
      i += 1;
    }
    return std::format_to(ctx.out(), "]");
  }
  template <typename format_context_t>
  constexpr decltype(std::declval<format_context_t>().out()) format(
    const tc::tuple_type &t, format_context_t &ctx
  ) const {
    size_t i = 0;
    std::format_to(ctx.out(), "Tuple[");
    for (const auto &child : t.child) {
      format(child, ctx);
      if (i != t.child.size() - 1) {
        std::format_to(ctx.out(), ", ");
      }
      i += 1;
    }
    return std::format_to(ctx.out(), "]");
  }
  template <typename format_context_t>
  constexpr decltype(std::declval<format_context_t>().out()) format(
    const tc::optional_type &t, format_context_t &ctx
  ) const {
    std::format_to(ctx.out(), "Optional[");
    format(*t.child, ctx);
    return std::format_to(ctx.out(), "]");
  }
  template <typename format_context_t>
  constexpr decltype(std::declval<format_context_t>().out()) format(
    const tc::variadic_type &t, format_context_t &ctx
  ) const {
    std::format_to(ctx.out(), "Variadic[");
    format(*t.child, ctx);
    return std::format_to(ctx.out(), "]");
  }
  template <typename format_context_t>
  constexpr decltype(std::declval<format_context_t>().out()) format(
    const tc::generic_type &t, format_context_t &ctx
  ) const {
    return std::visit([&](const auto &t) { return format(t, ctx); }, t.type);
  }
};