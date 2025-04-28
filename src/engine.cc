module;
#include <rfl/json.hpp>
#include <cstddef>
#include <expected>
#include <format>
#include <map>
#include <ranges>
#include <rfl.hpp>
#include <string>
#include <vector>
export module moderna.type_check:engine;
import :matcher;
import :types;
import :parser;

namespace moderna::type_check {

  export struct multi_nameless_record;
  export struct nameless_record;
  export struct source_match {
    std::string name;
    generic_type source;
    size_t id;
  };

  export struct record_match {
    generic_type type;
    std::string target;
    std::vector<source_match> sources;

    bool is_empty() const {
      return sources.empty();
    }
    bool is_not_empty() const {
      return !is_empty();
    }
    bool is_complete() const {
      return allow_empty(type) || is_not_empty();
    }
    /*
      If the current record match can still accept new sources. Generally, if the type allows
      multiple, this record match will always accept a new source and if a type is empty, it will
      always allow a new source.
    */
    bool is_connectable() const {
      return allow_multiple(type) || is_empty();
    }
    /*
      Get the first match.
    */
    std::optional<std::reference_wrapper<const source_match>> first() const {
      if (is_empty()) {
        return std::nullopt;
      } else {
        return std::cref(sources.front());
      }
    }
    size_t size() const {
      return sources.size();
    }
    auto begin() {
      return sources.begin();
    }
    auto end() {
      return sources.end();
    }
    auto begin() const {
      return sources.begin();
    }
    auto end() const {
      return sources.end();
    }
    auto cbegin() const {
      return sources.cbegin();
    }
    auto cend() const {
      return sources.cend();
    }
  };

  export struct record_matches {
    std::vector<record_match> matches;
    std::optional<std::reference_wrapper<const record_match>> get_match(std::string_view v) const {
      auto it = std::ranges::find_if(matches, [&](const auto &m) { return m.target == v; });
      if (it == matches.end()) {
        return std::nullopt;
      } else {
        return std::cref(*it);
      }
    }
    bool is_complete() const {
      if (matches.size() == 0) return false;
      return std::ranges::all_of(matches, [](const auto &m) { return m.is_complete(); });
    }
    bool is_partial() const {
      return !is_empty() && !is_complete();
    }
    bool is_empty() const {
      return std::ranges::all_of(matches, [](const auto &m) { return m.is_empty(); });
    }
    std::vector<record_match> get_connectable() const {
      auto conn_view =
        std::ranges::filter_view{matches, [](const auto &m) { return m.is_connectable(); }};
      return std::vector<record_match>{conn_view.begin(), conn_view.end()};
    }
    auto begin() const {
      return matches.begin();
    }
    auto end() const {
      return matches.end();
    }
    auto cbegin() const {
      return matches.cbegin();
    }
    auto cend() const {
      return matches.cend();
    }
    size_t size() const {
      return matches.size();
    }
    static const record_matches create(
      const multi_nameless_record &source, const nameless_record &target
    );
    static const record_matches create(
      const nameless_record &source, const nameless_record &target
    );
  };

  struct nameless_record {
    constexpr nameless_record() = default;
    constexpr nameless_record(std::vector<std::pair<std::string, generic_type>> r) :
      __records{std::move(r)} {}

    auto begin() {
      return __records.begin();
    }
    auto end() {
      return __records.end();
    }
    auto begin() const {
      return __records.begin();
    }
    auto end() const {
      return __records.end();
    }
    size_t size() const {
      return __records.size();
    }
    bool is_empty() const {
      return __records.empty();
    }

    nameless_record &add_record(std::string name, generic_type type) {
      auto ref = get(name);
      if (ref.has_value()) {
        ref.value().get() = std::move(type);
      } else {
        __records.emplace_back(std::move(name), std::move(type));
      }
      return *this;
    }
    std::optional<std::reference_wrapper<generic_type>> get(std::string_view name) {
      auto it = std::ranges::find_if(__records, [&](const auto &p) { return p.first == name; });
      if (it == __records.end()) {
        return std::nullopt;
      } else {
        return std::ref(it->second);
      }
    }
    std::optional<std::reference_wrapper<const generic_type>> get(std::string_view name) const {
      auto it = std::ranges::find_if(__records, [&](const auto &p) { return p.first == name; });
      if (it == __records.end()) {
        return std::nullopt;
      } else {
        return std::cref(it->second);
      }
    }
    std::optional<std::reference_wrapper<generic_type>> get(const std::string &name) {
      return get(std::string_view{name});
    }
    std::optional<std::reference_wrapper<const generic_type>> get(const std::string &name) const {
      return get(std::string_view{name});
    }
    std::optional<std::reference_wrapper<generic_type>> get(const source_match &match) {
      return get(match.name);
    }
    std::optional<std::reference_wrapper<const generic_type>> get(const source_match &match) const {
      return get(match.name);
    }

    /*
      Forward Declared
    */
    multi_nameless_record operator+(const nameless_record &other) const;
    multi_nameless_record operator+(const multi_nameless_record &other) const;
    multi_nameless_record to_multiple() const;

    static nameless_record empty() {
      return nameless_record{};
    }

    static std::expected<nameless_record, parse_error_trace> from_json(const std::string &v) {
      auto y = rfl::json::read<std::map<std::string, std::string>>(v);
      if (!y) {
        auto x = rfl::json::read<nameless_record>(v);
        if (!x) {
          return std::unexpected{parse_error_trace::make_trace(v, parse_error{x.error().what()})};
        }
        return x.value();
      }
      nameless_record r;
      for (const auto &[k, v] : y.value()) {
        auto t = from_string(v);
        if (!t) {
          return std::unexpected{std::move(t.error())};
        }
        r.add_record(k, std::move(t.value()));
      }
      return r;
    }

  private:
    std::vector<std::pair<std::string, generic_type>> __records;
    friend struct rfl::Reflector<nameless_record>;
  };

  struct multi_nameless_record {
    std::vector<nameless_record> records;
    size_t size() const {
      return records.size();
    }
    auto begin() {
      return records.begin();
    }
    auto end() {
      return records.end();
    }
    auto begin() const {
      return records.begin();
    }
    auto end() const {
      return records.end();
    }
    multi_nameless_record &add_record(const nameless_record &record) {
      records.push_back(record);
      return *this;
    }
    std::optional<std::reference_wrapper<nameless_record>> get(size_t id) {
      if (id < records.size()) {
        return std::ref(records[id]);
      } else {
        return std::nullopt;
      }
    }
    std::optional<std::reference_wrapper<const nameless_record>> get(size_t id) const {
      if (id < records.size()) {
        return std::cref(records[id]);
      } else {
        return std::nullopt;
      }
    }
    std::optional<std::reference_wrapper<generic_type>> get(std::string_view name, size_t id) {
      return get(id).and_then([&](nameless_record &record) { return record.get(name); });
    }
    std::optional<std::reference_wrapper<const generic_type>> get(std::string_view name, size_t id)
      const {
      return get(id).and_then([&](const nameless_record &record) { return record.get(name); });
    }
    multi_nameless_record operator+(const nameless_record &record) const {
      multi_nameless_record new_records{*this};
      new_records.add_record(record);
      return new_records;
    }
    multi_nameless_record operator+(nameless_record &&record) const {
      multi_nameless_record new_records{*this};
      new_records.add_record(std::move(record));
      return new_records;
    }
  };
}

namespace tc = moderna::type_check;
template <> struct rfl::Reflector<tc::nameless_record> {
  using ReflType = std::map<std::string, tc::generic_type>;
  static ReflType from(const tc::nameless_record &t) {
    return ReflType{t.begin(), t.end()};
  }
  static tc::nameless_record to(const ReflType &t) {
    return tc::nameless_record{
      std::vector<std::pair<std::string, tc::generic_type>>{t.begin(), t.end()}
    };
  }
};

tc::multi_nameless_record tc::nameless_record::operator+(const nameless_record &other) const {
  return multi_nameless_record{std::vector{nameless_record{*this}, other}};
}
tc::multi_nameless_record tc::nameless_record::operator+(const multi_nameless_record &other) const {
  multi_nameless_record new_records = other;
  new_records.add_record(*this);
  return new_records;
}
tc::multi_nameless_record tc::nameless_record::to_multiple() const {
  return multi_nameless_record{std::vector{nameless_record{*this}}};
}

const tc::record_matches tc::record_matches::create(
  const multi_nameless_record &sources, const nameless_record &target
) {
  record_matches matches;
  for (const auto &[target_name, target_type] : target) {
    record_match match{target_type, target_name, {}};
    bool is_added = false;
    bool multiple = allow_multiple(target_type);
    for (size_t i = 0; i < sources.size(); i += 1) {
      auto source = sources.get(i).value();
      for (const auto &[source_name, source_type] : source.get()) {
        if (is_connectable(source_type, target_type)) {
          match.sources.push_back(source_match{source_name, source_type, i});
          is_added = true;
          if (!multiple) break;
        }
      }
      if (is_added && !multiple) break;
    }
    matches.matches.emplace_back(std::move(match));
  }
  return matches;
}

const tc::record_matches tc::record_matches::create(
  const nameless_record &sources, const nameless_record &target
) {
  return create(sources.to_multiple(), target);
}

template <> struct std::formatter<tc::nameless_record> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_parse_context>
  constexpr auto format(const tc::nameless_record &t, format_parse_context &ctx) const {
    std::format_to(ctx.out(), "nameless_record[");
    size_t i = 0;
    for (const auto &[name, record] : t) {
      if (i == t.size() - 1) [[unlikely]] {
        std::format_to(ctx.out(), "{}({})", name, record);
      } else {
        std::format_to(ctx.out(), "{}({}), ", name, record);
      }
      i += 1;
    }
    return std::format_to(ctx.out(), "]");
  }
};

template <> struct std::formatter<tc::multi_nameless_record> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_parse_context>
  constexpr auto format(const tc::multi_nameless_record &t, format_parse_context &ctx) const {
    std::format_to(ctx.out(), "multi_nameless_record[");
    size_t ctr = 0;
    for (const auto &record : t) {
      if (ctr == record.size() - 1) [[unlikely]] {
        std::format_to(ctx.out(), "{} : {}]", ctr, record);
      } else {
        std::format_to(ctx.out(), "{} : {},\n", ctr, record);
      }
      ctr += 1;
    }
    return ctx.out();
  }
};

template <> struct std::formatter<tc::source_match> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_parse_context>
  constexpr auto format(const tc::source_match &t, format_parse_context &ctx) const {
    return std::format_to(ctx.out(), "source_match[{}_{}({})]", t.id, t.name, t.source);
  }
};

template <> struct std::formatter<tc::record_match> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_parse_context>
  constexpr auto format(const tc::record_match &t, format_parse_context &ctx) const {
    std::format_to(ctx.out(), "record_match[(");
    size_t i = 0;
    for (const auto &source : t) {
      if (i == t.size() - 1) [[unlikely]] {
        std::format_to(ctx.out(), "{}", source);
      } else {
        std::format_to(ctx.out(), "{}, ", source);
      }
      i += 1;
    }
    std::format_to(ctx.out(), ")->{}({})", t.target, t.type);
    return std::format_to(ctx.out(), "]");
  }
};

template <> struct std::formatter<tc::record_matches> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_parse_context>
  constexpr auto format(const tc::record_matches &t, format_parse_context &ctx) const {
    std::format_to(ctx.out(), "record_matches[\n");
    size_t ctr = 0;
    for (const auto &match : t) {
      if (ctr == t.size() - 1) [[unlikely]] {
        std::format_to(ctx.out(), "{}", match);
      } else {
        std::format_to(ctx.out(), "{},\n", match);
      }
      ctr += 1;
    }
    return std::format_to(ctx.out(), "]");
  }
};