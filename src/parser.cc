module;
#include <rfl/Box.hpp>
#include <rfl/json.hpp>
#include <algorithm>
#include <expected>
#include <format>
#include <ranges>
#include <rfl.hpp>
#include <string>
#include <variant>
export module moderna.type_check:parser;
import :types;

namespace moderna::type_check {
  struct parse_error {
    std::string msg;
  };
  struct parse_error_trace {
    std::string parse_ctx;
    std::unique_ptr<std::variant<parse_error, parse_error_trace>> prev;

    parse_error_trace(const parse_error_trace &o) {
      parse_ctx = o.parse_ctx;
      prev = std::make_unique<std::variant<parse_error, parse_error_trace>>(std::visit(
        [](const auto &v) -> std::variant<parse_error, parse_error_trace> {
          using type_t = std::decay_t<decltype(v)>;
          if constexpr (std::same_as<type_t, parse_error>) {
            return v;
          } else {
            return parse_error_trace{v};
          }
        },
        *o.prev
      ));
    }
    parse_error_trace(parse_error_trace &&) = default;

    std::string what() const noexcept;

    static parse_error_trace make_trace(std::string_view parse_ctx, parse_error prev) {
      return parse_error_trace{
        std::string{parse_ctx},
        std::make_unique<std::variant<parse_error, parse_error_trace>>(std::move(prev))
      };
    }

    static parse_error_trace make_trace(std::string_view parse_ctx, parse_error_trace prev) {
      return parse_error_trace{
        std::string{parse_ctx},
        std::make_unique<std::variant<parse_error, parse_error_trace>>(std::move(prev))
      };
    }

  private:
    parse_error_trace(
      std::string parse_ctx, std::unique_ptr<std::variant<parse_error, parse_error_trace>> prev
    ) :
      parse_ctx{parse_ctx},
      prev{std::move(prev)} {}
  };

  /*
    In a string, v, find the first non space character as the start of the type name, ending it in
    a non alphabet character. On successful parsing, this will return :
    - type name string view
    - the first character AFTER the type name
  */
  std::expected<std::pair<std::string_view, const char *>, parse_error_trace> get_type_name(
    std::string_view v
  ) {
    auto type_name_start = std::ranges::find_if(v, [](auto c) { return std::isalpha(c); });
    auto type_name_end = std::ranges::find_if(type_name_start + 1, v.end(), [](auto c) {
      return !std::isalpha(c) && c != '_';
    });
    auto type_name = std::string_view{type_name_start, type_name_end};
    if (type_name.empty()) {
      return std::unexpected{parse_error_trace::make_trace(v, parse_error{"No type name found"})};
    }
    return std::pair{std::string_view{type_name_start, type_name_end}, type_name_end};
  }

  /*
    In a string v, find the first bracket and return the content within the brackets. This function
    will return :
    - The content within the brackets excluding the brackets.
    - The first character AFTER the closing bracket.
  */
  std::expected<std::pair<std::string_view, const char *>, parse_error_trace> get_bracket_content(
    std::string_view v
  ) {
    auto bracket_start = std::ranges::find(v, '[');
    if (bracket_start == v.end()) {
      return std::unexpected{parse_error_trace::make_trace(v, parse_error{"No bracket found"})};
    }
    int bracket_counter = 0;
    for (auto it = bracket_start; it != v.end(); it += 1) {
      if (*it == '[') {
        bracket_counter += 1;
      } else if (*it == ']') {
        if (bracket_counter == 0) {
          return std::unexpected{
            parse_error_trace::make_trace(v, parse_error{"Closing bracket cannot be found"})
          };
        }
        bracket_counter -= 1;
        if (bracket_counter == 0) {
          return std::pair{std::string_view{bracket_start + 1, it}, it + 1};
        }
      }
    }
    return std::unexpected{
      parse_error_trace::make_trace(v, parse_error{"Closing bracket cannot be found. "})
    };
  }

  /*
    Parses the generic type specified in v and returns
    - The generic type
    - the first character AFTER the character parsed
  */
  std::expected<std::pair<generic_type, const char *>, parse_error_trace> __from_string(
    std::string_view v
  ) {
    using return_t = std::expected<std::pair<generic_type, const char *>, parse_error_trace>;
    return get_type_name(v).and_then([&](auto &&p) -> return_t {
      auto [type_name, end] = p;
      auto type_params = std::string_view{end, v.end()};
      if (type_name == "List" || type_name == "Optional" || type_name == "Variadic") {
        return get_bracket_content(type_params).and_then([&](auto &&p) {
          auto [bracket_content, end] = p;
          return __from_string(bracket_content)
            .transform([](auto &&res) {
              auto [g, end] = std::move(res);
              return rfl::make_box<generic_type>(std::move(g));
            })
            .transform([&](auto &&g) {
              if (type_name == "List") {
                return std::pair{generic_type{list_type{std::string{"List"}, std::move(g)}}, end};
              } else if (type_name == "Optional") {
                return std::pair{
                  generic_type{optional_type{std::string{"Optional"}, std::move(g)}}, end
                };
              } else {
                return std::pair{
                  generic_type{variadic_type{std::string{"Variadic"}, std::move(g)}}, end
                };
              }
            });
        });
      } else if (type_name == "Union" || type_name == "Tuple") {
        return get_bracket_content(type_params).and_then([&](auto &&p) -> return_t {
          auto [bracket_content, end] = p;
          std::vector<generic_type> subtypes;
          auto cur_it = bracket_content.begin();
          while (cur_it != bracket_content.end()) {
            auto parsed = __from_string(std::string_view{cur_it, bracket_content.end()});
            if (!parsed.has_value()) {
              return std::unexpected{parsed.error()};
            } else {
              auto [g, new_it] = std::move(parsed.value());
              subtypes.push_back(std::move(g));
              cur_it = std::ranges::find_if(new_it, bracket_content.end(), [](auto c) {
                return c != ',';
              });
              if (cur_it == bracket_content.end()) {
                break;
              } else {
                cur_it += 1;
              }
            }
          }
          if (type_name == "Union") {
            return std::pair{
              generic_type{union_type{std::string{"Union"}, std::move(subtypes)}}, end
            };
          } else {
            return std::pair{
              generic_type{tuple_type{std::string{"Tuple"}, std::move(subtypes)}}, end
            };
          }
        });
      } else {
        return std::pair{generic_type{basic_type{std::string{type_name}}}, end};
      }
    });
  }

  /*
    This will only parse the first type definition found and disregard the rest.
  */
  export std::expected<generic_type, parse_error_trace> from_string(std::string_view v) {
    /*
      Now we try to find the type name. This means, parsing up until we see a space or a bracket.
    */
    using expected_t = std::expected<generic_type, parse_error_trace>;
    if (v.contains('{') || v.contains('}') || v.contains('\"')) {
      auto r = rfl::json::read<generic_type>(std::string{v.begin(), v.end()});
      std::optional<rfl::Error> err = r.error();
      if (!r) {
        return std::unexpected{parse_error_trace::make_trace(v, parse_error{err->what()})};
      } else {
        return std::move(r.value());
      }
    } else {
      return __from_string(v).transform([](auto &&p) {
        auto [g, end] = std::move(p);
        return std::move(g);
      });
    }
  }
}

namespace tc = moderna::type_check;
template <> struct std::formatter<tc::parse_error> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }

  template <typename format_context_t>
  auto format(const tc::parse_error &err, format_context_t &ctx) const {
    return std::format_to(ctx.out(), "ParseError({})", err.msg);
  }
};

template <> struct std::formatter<tc::parse_error_trace> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_context_t>
  auto format(const tc::parse_error_trace &trace, format_context_t &ctx) const {
    std::format_to(ctx.out(), "During the parsing of : {}.", trace.parse_ctx);
    if (std::holds_alternative<tc::parse_error>(*trace.prev)) {
      return std::format_to(ctx.out(), "Found : {}", std::get<tc::parse_error>(*trace.prev).msg);
    } else {
      return format(std::get<tc::parse_error_trace>(*trace.prev), ctx);
    }
    return ctx.out();
  }
};

std::string tc::parse_error_trace::what() const noexcept {
  return std::format("{}", *this);
}