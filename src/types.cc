module;
#include <rfl/json.hpp>
#include <expected>
#include <format>
#include <ranges>
#include <rfl.hpp>
#include <string>
#include <variant>
#include <vector>
export module moderna.type_check:types;
import :constraints;
import :main_types;
import :matcher;

namespace moderna::type_check {
  export struct generic_type {
    struct basic_type {
      std::string type;
      using viewer_t = basic_type_view;

      friend bool operator==(const basic_type &a, const basic_type &b) {
        return a.type == b.type;
      }

      std::string name() const {
        return std::format("\"{}\"", type);
      }
    };
    struct list_type {
      rfl::Literal<"list", "List"> type;
      rfl::Box<generic_type> child;
      using viewer_t = list_type_view<const generic_type &>;

      friend bool operator==(const list_type &a, const list_type &b) {
        return *a.child == *b.child;
      }

      list_type clone() const {
        return list_type{.type{"list"}, .child{rfl::make_box<generic_type>(deep_copy(*child))}};
      }

      std::string name() const {
        return std::format("List[{}]", child->name());
      }

      template <typename type_t> static list_type make(type_t type) {
        return list_type{.type{"list"}, .child{rfl::make_box<generic_type>(std::move(type))}};
      }
    };
    struct union_type {
      rfl::Literal<"union", "Union"> type;
      std::vector<generic_type> child;

      using viewer_t = union_type_view<const decltype(child) &>;
      using viewer_children_t = const decltype(child) &;

      template <typename... types> static union_type make(types... type) {
        auto t = union_type{.type{"union"}, .child{}};
        (t.child.emplace_back(std::move(type)), ...);
        return t;
      }

      friend bool operator==(const union_type &a, const union_type &b) {
        for (const auto &[a, b] : std::ranges::views::zip(a.child, b.child)) {
          if (a != b) {
            return false;
          }
        }
        return true;
      }

      std::string name() const {
        std::string n;
        auto ctx = std::back_inserter(n);
        std::format_to(ctx, "Union[");
        for (const auto &c : child)
          std::format_to(ctx, "{}, ", c.name());
        std::format_to(ctx, "]");
        return n;
      }
    };
    struct optional_type {
      rfl::Literal<"optional", "Optional"> type;
      rfl::Box<generic_type> child;
      using viewer_t = optional_type_view<const generic_type &>;

      optional_type clone() const {
        return optional_type{
          .type{"optional"}, .child{rfl::make_box<generic_type>(deep_copy(*child))}
        };
      }

      friend bool operator==(const optional_type &a, const optional_type &b) {
        return *a.child == *b.child;
      }

      std::string name() const {
        return std::format("Optional[{}]", child->name());
      }

      template <typename type_t> static optional_type make(type_t type) {
        return optional_type{
          .type{"optional"}, .child{rfl::make_box<generic_type>(std::move(type))}
        };
      }
    };

    struct variadic_type {
      rfl::Literal<"variadic", "Variadic"> type;
      rfl::Box<generic_type> child;
      using viewer_t = variadic_type_view<const generic_type &>;

      variadic_type clone() const {
        return variadic_type{
          .type{"variadic"}, .child{rfl::make_box<generic_type>(deep_copy(*child))}
        };
      }

      friend bool operator==(const variadic_type &a, const variadic_type &b) {
        return *a.child == *b.child;
      }

      std::string name() const {
        return std::format("Variadic[{}]", child->name());
      }

      template <typename type_t> static variadic_type make(type_t type) {
        return variadic_type{
          .type{"variadic"}, .child{rfl::make_box<generic_type>(std::move(type))}
        };
      }
    };
    struct tuple_type {
      rfl::Literal<"tuple", "Tuple"> type;
      std::vector<generic_type> child;
      using viewer_t = tuple_type_view<const decltype(child) &>;
      using viewer_children_t = const decltype(child) &;

      template <typename... types> static tuple_type make(types... type) {
        auto t = tuple_type{.type{"tuple"}, .child{}};
        (t.child.emplace_back(std::move(type)), ...);
        return t;
      }

      friend bool operator==(const tuple_type &a, const tuple_type &b) {
        for (const auto &[a, b] : std::ranges::views::zip(a.child, b.child)) {
          if (a != b) {
            return false;
          }
        }
        return true;
      }

      std::string name() const {
        std::string n;
        auto ctx = std::back_inserter(n);
        std::format_to(ctx, "Tuple[");
        for (const auto &c : child)
          std::format_to(ctx, "{}, ", c.name());
        std::format_to(ctx, "]");
        return n;
      }
    };
    struct any_type {
      rfl::Literal<"any", "Any"> type;
      using viewer_t = any_type_view;

      friend bool operator==(const any_type &a, const any_type &b) {
        return true;
      }

      std::string name() const {
        return "Any";
      }
    };

    using variant_t = std::variant<
      list_type,
      union_type,
      optional_type,
      variadic_type,
      tuple_type,
      any_type,
      basic_type>;
    variant_t type;

    using ReflectionType = std::variant<
      rfl::TaggedUnion<
        "type",
        list_type,
        union_type,
        optional_type,
        variadic_type,
        tuple_type,
        any_type>,
      basic_type,
      std::string>;

    std::string name() const {
      return std::visit([](const auto &t) { return t.name(); }, type);
    }

    template <typename type_t> generic_type(type_t type) : type{std::move(type)} {}
    generic_type(const ReflectionType &var) :
      type{std::visit(
        [](const auto &t) {
          using obj_t = std::remove_cvref_t<decltype(t)>;
          if constexpr (std::same_as<obj_t, std::string>) return variant_t{basic_type{t}};
          else if constexpr (std::same_as<obj_t, basic_type>)
            return variant_t{deep_copy(t)};
          else
            return rfl::visit([](const auto &t) { return variant_t{deep_copy(t)}; }, t.variant());
        },
        var
      )} {}
    generic_type(const generic_type &other) :
      type{std::visit([](const auto &e) { return variant_t{deep_copy(e)}; }, other.type)} {}

    generic_type(generic_type &&other) : type{std::move(other.type)} {}
    generic_type &operator=(const generic_type &other) {
      type = std::visit([](const auto &e) { return variant_t{deep_copy(e)}; }, other.type);
      return *this;
    }
    generic_type &operator=(generic_type &&other) {
      type = std::move(other.type);
      return *this;
    }
    friend bool operator==(const generic_type &a, const generic_type &b) {
      return std::visit(
        [&](const auto &a_type) {
          return std::visit(
            [&](const auto &b_type) {
              using type_a_t = std::decay_t<decltype(a_type)>;
              using type_b_t = std::decay_t<decltype(b_type)>;
              if constexpr (!std::same_as<type_a_t, type_b_t>) {
                return false;
              } else {
                return a_type == b_type;
              }
            },
            b.type
          );
        },
        a.type
      );
    }
    ReflectionType reflection() const {
      return std::visit(
        [](auto &&entry) { return ReflectionType{std::move(entry)}; }, deep_copy(*this).type
      );
    }

    static std::expected<generic_type, rfl::Error> from_json(const std::string &v) {
      auto res = rfl::json::read<generic_type>(v);
      if (res) return std::move(res.value());
      else
        return std::unexpected{res.error().value()};
    }
  };
  /*
    Specializations of basic type.
  */

  export template <> struct union_type_view<const std::vector<generic_type> &> {
    const std::vector<generic_type> &children;
  };
  export template <> struct tuple_type_view<const std::vector<generic_type> &> {
    const std::vector<generic_type> &children;
  };
  /*
    IMPLEMENTATION
  */
  using list_type = generic_type::list_type;
  using optional_type = generic_type::optional_type;
  using variadic_type = generic_type::variadic_type;
  using union_type = generic_type::union_type;
  using tuple_type = generic_type::tuple_type;
  using basic_type = generic_type::basic_type;
  using any_type = generic_type::any_type;

  /*
    VIEWER TRANSFORMATION
  */
  template <> struct owner_viewer<basic_type, basic_type_view> {
    constexpr static basic_type_view as_view(const basic_type &owner) {
      return basic_type_view{owner.type};
    }
  };
  template <> struct owner_viewer<list_type, list_type::viewer_t> {
    constexpr static list_type::viewer_t as_view(const list_type &owner) {
      return list_type::viewer_t{.child{*owner.child}};
    }
  };
  template <> struct owner_viewer<union_type, union_type::viewer_t> {
    constexpr static union_type::viewer_t as_view(const union_type &owner) {
      return union_type::viewer_t{owner.child};
    }
  };
  template <> struct owner_viewer<optional_type, optional_type::viewer_t> {
    constexpr static optional_type::viewer_t as_view(const optional_type &owner) {
      return optional_type::viewer_t{.child{*owner.child}};
    }
  };
  template <> struct owner_viewer<tuple_type, tuple_type::viewer_t> {
    constexpr static tuple_type::viewer_t as_view(const tuple_type &owner) {
      return tuple_type::viewer_t{owner.child};
    }
  };
  template <> struct owner_viewer<variadic_type, variadic_type::viewer_t> {
    constexpr static variadic_type::viewer_t as_view(const variadic_type &owner) {
      return variadic_type::viewer_t{.child{*owner.child}};
    }
  };
  template <> struct owner_viewer<any_type, any_type::viewer_t> {
    constexpr static any_type::viewer_t as_view(const any_type &owner) {
      return any_type::viewer_t{};
    }
  };
  /*
    Generic Matching
  */
  /*
    Compares a generic with another generic or not another generic.
  */
  template <typename source_t, typename target_t>
    requires(std::same_as<generic_type, source_t> || std::same_as<generic_type, target_t>)
  struct type_matcher<source_t, target_t> {
    static constexpr bool match(const source_t &s, const target_t &t);
  };

  template <> struct type_options<generic_type> {
    static constexpr bool allow_empty(const generic_type &t) {
      return std::visit(
        [](const auto &visited) { return ::moderna::type_check::allow_empty(visited); }, t.type
      );
    }
    static constexpr bool allow_multiple(const generic_type &t) {
      return std::visit(
        [](const auto &visited) { return ::moderna::type_check::allow_multiple(visited); }, t.type
      );
    }
  };
  template <> struct type_matcher<union_type::viewer_t, union_type::viewer_t> {
    static constexpr bool match(const union_type::viewer_t &s, const union_type::viewer_t &t);
  };
  template <typename source_t> struct type_matcher<source_t, union_type::viewer_t> {
    static constexpr bool match(const source_t &s, const union_type::viewer_t &t);
  };
  template <> struct type_matcher<tuple_type::viewer_t, tuple_type::viewer_t> {
    static constexpr bool match(const tuple_type::viewer_t &s, const tuple_type::viewer_t &t);
  };

  template <typename source_t, typename target_t>
    requires(std::same_as<generic_type, source_t> || std::same_as<generic_type, target_t>)
  constexpr bool type_matcher<source_t, target_t>::match(const source_t &s, const target_t &t) {
    if constexpr (std::same_as<generic_type, source_t> && std::same_as<generic_type, target_t>) {
      return std::visit(
        [&](const auto &s_type) {
          return std::visit(
            [&](const auto &t_type) { return is_connectable(s_type, t_type); }, t.type
          );
        },
        s.type
      );
    } else if constexpr (std::same_as<generic_type, source_t>) {
      return std::visit([&](const auto &s_type) { return is_connectable(s_type, t); }, s.type);
    } else {
      return std::visit([&](const auto &t_type) { return is_connectable(s, t_type); }, t.type);
    }
  }

  constexpr bool type_matcher<union_type::viewer_t, union_type::viewer_t>::match(
    const union_type::viewer_t &s, const union_type::viewer_t &t
  ) {
    return std::ranges::find_if_not(s.children, [&](const auto &s_type) {
             return is_connectable(s_type, t);
           }) == s.children.end();
  }
  template <typename source_t>
  constexpr bool type_matcher<source_t, union_type::viewer_t>::match(
    const source_t &s, const union_type::viewer_t &t
  ) {
    return std::ranges::find_if(t.children, [&](const auto &t_type) {
             return is_connectable(s, t_type);
           }) != t.children.end();
  }
  constexpr bool type_matcher<tuple_type::viewer_t, tuple_type::viewer_t>::match(
    const tuple_type::viewer_t &s, const tuple_type::viewer_t &t
  ) {
    if (t.children.size() != s.children.size()) {
      return false;
    }
    auto s_t_pair = std::ranges::zip_view(s.children, t.children);
    return std::ranges::find_if_not(s_t_pair, [](const auto &p) {
             const auto &[s_type, t_type] = p;
             return is_connectable(s_type, t_type);
           }) == s_t_pair.end();
  }
}