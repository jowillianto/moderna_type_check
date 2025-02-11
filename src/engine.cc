module;
#include <rfl/json.hpp>
#include <algorithm>
#include <expected>
#include <format>
#include <map>
#include <print>
#include <ranges>
#include <rfl.hpp>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
export module moderna.type_check:engine;
import :matcher;

namespace moderna::type_check {
  template <typename type_t>
  concept checkable_generic = is_matchable<type_t, type_t> && requires(const type_t &type) {
    { type.name() } -> std::same_as<std::string>;
  };

  /*
    Forward Declaration
  */
  export template <checkable_generic type_t> struct source_match;
  export template <checkable_generic type_t> struct record_match;
  export template <checkable_generic type_t> struct record_matches;
  export template <checkable_generic type_t> struct nameless_record;
  export template <checkable_generic type_t> struct multi_nameless_record;

  /*
    Definition
  */
  export template <checkable_generic type_t> struct source_match {
    type_t type;
    std::string_view name;
    size_t id;
  };
  export template <checkable_generic type_t> struct record_match {
    type_t type;
    std::string_view target;
    std::vector<source_match<type_t>> sources;

    bool is_empty() const;
    size_t size() const;
  };
  export template <checkable_generic type_t> struct record_matches {
    std::vector<record_match<type_t>> matches;

    /*
      operation. this returns a record_match pointer to the internal data structure
    */
    std::expected<const record_match<type_t> *, std::out_of_range> get_match(std::string_view v
    ) const;

    /*
      Checks if the current node is fully connected. This will return false even for empty nodes
      as it does not make any sense.
    */
    bool is_complete() const;
    /*
      Checks if the current node is partially connected, this will return false for emtpy node
    */
    bool is_partial() const;
    /*
      Checks if the current node is empty, this will return true for empty node.
    */
    bool is_empty() const;

    static record_matches create(
      const multi_nameless_record<type_t> &source, const nameless_record<type_t> &target
    );
    static record_matches create(
      const nameless_record<type_t> &source, const nameless_record<type_t> &target
    );
  };
  export template <checkable_generic type_t> struct nameless_record {
    std::map<std::string, type_t> records;

    /*
      Adds a record into the nameless record.
    */
    nameless_record &add_record(const std::string &record_name, const type_t &type);
    /*
      Adds two records together.
    */
    multi_nameless_record<type_t> operator+(const nameless_record<type_t> &record) const;
    multi_nameless_record<type_t> operator+(const multi_nameless_record<type_t>& record) const;
    /*
      Create multiple records from this one record.
    */
    multi_nameless_record<type_t> to_multiple() const;
    /*
      Get key type name pairs.
      Example : 
      a : List[Optional[Ya]]
    */
    std::map<std::string, std::string> get_name_pairs() const{
      std::map<std::string, std::string> v;
      for (const auto& [fdName, fdType] : records){
        v.emplace(fdName, fdType.name());
      }
      return v;
    }
    /*
      Returns the size of the record
    */
    size_t size() const {
      return records.size();
    }
    /*
      Returns if the record is empty
    */
    bool is_empty() const {
      return records.size() == 0;
    }

    /*
      Creates a record from json.
    */
    static std::expected<nameless_record, rfl::Error> from_json(const std::string &v) {
      auto map = rfl::json::read<decltype(records)>(v);
      if (map) return nameless_record{.records{std::move(map.value())}};
      else
        return std::unexpected{map.error().value()};
    }
    static nameless_record make_empty() {
      return nameless_record{};
    }
  };
  export template <checkable_generic type_t> struct multi_nameless_record {
    std::vector<nameless_record<type_t>> records;

    multi_nameless_record operator+(const nameless_record<type_t> &record) const;

    static multi_nameless_record make_empty() {
      return multi_nameless_record{};
    }
  };

  /*
    Implementation
  */
  template <checkable_generic type_t> bool record_match<type_t>::is_empty() const {
    return sources.empty();
  }
  template <checkable_generic type_t> size_t record_match<type_t>::size() const {
    return sources.size();
  }
  template <checkable_generic type_t>
  record_matches<type_t> record_matches<type_t>::create(
    const multi_nameless_record<type_t> &sources, const nameless_record<type_t> &target
  ) {
    auto matches = record_matches<type_t>{};
    for (const auto &[target_name, target_type] : target.records) {
      record_match<type_t> match{.type{target_type}, .target{target_name}, .sources{}};
      bool is_added = false;
      bool multiple = allow_multiple(target_type);
      for (size_t id = 0; id < sources.records.size(); id++) {
        const auto &source = sources.records[id];
        for (const auto &[source_name, source_type] : source.records) {
          if (is_connectable(source_type, target_type) && (!is_added || multiple)) {
            match.sources.emplace_back(source_match<type_t>{
              .type{source_type}, .name{source_name}, .id{static_cast<size_t>(id)}
            });
            is_added = true;
            if (!multiple) break;
          }
        }
        if (is_added && !multiple) {
          break;
        }
      }
      matches.matches.emplace_back(std::move(match));
    }
    return matches;
  }
  template <checkable_generic type_t>
  record_matches<type_t> record_matches<type_t>::create(
    const nameless_record<type_t> &source, const nameless_record<type_t> &target
  ) {
    return create(source.to_multiple(), target);
  }
  template <checkable_generic type_t> bool record_matches<type_t>::is_complete() const {
    /*
      Find anything that is not empty allowable and is empty
    */
    if (matches.size() == 0) return false;
    return std::ranges::find_if(matches, [&](const record_match<type_t> &match) {
             return !allow_empty(match.type) && match.is_empty();
           }) == matches.end();
  }
  template <checkable_generic type_t> bool record_matches<type_t>::is_empty() const {
    /*
      Find anything that is not empty.
    */
    if (matches.size() == 0) return true;
    return std::ranges::find_if(matches, [&](const record_match<type_t> &match) {
             return !match.is_empty();
           }) == matches.end();
  }
  template <checkable_generic type_t> bool record_matches<type_t>::is_partial() const {
    /*
      partial is not complete and not empty.
    */
    return !is_complete() && !is_empty();
  }
  template <checkable_generic type_t>
  std::expected<const record_match<type_t> *, std::out_of_range> record_matches<type_t>::get_match(
    std::string_view v
  ) const {
    auto it = std::ranges::find_if(matches, [&](const auto &p) { return p.target == v; });
    if (it == matches.end())
      return std::unexpected{std::out_of_range{std::format("{} not found", v)}};
    return &(*it);
  }

  template <checkable_generic type_t>
  nameless_record<type_t> &nameless_record<type_t>::add_record(
    const std::string &record_name, const type_t &type
  ) {
    records.emplace(record_name, type);
    return *this;
  }

  template <checkable_generic type_t>
  multi_nameless_record<type_t> nameless_record<type_t>::operator+(
    const nameless_record<type_t> &record
  ) const {
    return multi_nameless_record<type_t>{.records{*this, record}};
  }
  template <checkable_generic type_t>
  multi_nameless_record<type_t> nameless_record<type_t>::operator+(
    const multi_nameless_record<type_t> &record
  ) const {
    std::vector<nameless_record<type_t> > records;
    records.reserve(record.records.size() + 1);
    records.emplace_back(*this);
    for (const auto& r : record.records)
      records.emplace_back(r);
    return multi_nameless_record<type_t>{.records{records}};
  }
  template <checkable_generic type_t>
  multi_nameless_record<type_t> nameless_record<type_t>::to_multiple() const {
    return multi_nameless_record<type_t>{.records{{*this}}};
  }
  template <checkable_generic type_t>
  multi_nameless_record<type_t> multi_nameless_record<type_t>::operator+(
    const nameless_record<type_t> &record
  ) const {
    auto new_records = multi_nameless_record<type_t>{.records{records}};
    new_records.records.push_back(record);
    return new_records;
  }
}
namespace tc = moderna::type_check;
template <tc::checkable_generic type_t> struct std::formatter<tc::source_match<type_t>> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_context>
  constexpr auto format(const tc::source_match<type_t> &r, format_context &ctx) const {
    std::format_to(ctx.out(), "{}:{}[{}]", r.id, r.name, r.type.name());
    return ctx.out();
  }
};
template <tc::checkable_generic type_t> struct std::formatter<tc::record_match<type_t>> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_context>
  constexpr auto format(const tc::record_match<type_t> &r, format_context &ctx) const {
    std::format_to(ctx.out(), "[");
    for (const auto &source : r.sources) {
      std::format_to(ctx.out(), "{}, ", source);
    }
    std::format_to(ctx.out(), "] -> {}[{}]\n", r.target, r.type.name());
    return ctx.out();
  }
};
template <tc::checkable_generic type_t> struct std::formatter<tc::record_matches<type_t>> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_context>
  constexpr auto format(const tc::record_matches<type_t> &r, format_context &ctx) const {
    for (const auto &match : r.matches)
      std::format_to(ctx.out(), "{}", match);
    return ctx.out();
  }
};

template <tc::checkable_generic type_t> struct std::formatter<tc::nameless_record<type_t>> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_context>
  constexpr auto format(const tc::nameless_record<type_t> &r, format_context &ctx) const {
    for (const auto &[record_name, record_type] : r.records)
      std::format_to(ctx.out(), "{} : {}\n", record_name, record_type.name());
    return ctx.out();
  }
};

template <tc::checkable_generic type_t> struct std::formatter<tc::multi_nameless_record<type_t>> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }
  template <typename format_context>
  constexpr auto format(const tc::multi_nameless_record<type_t> &r, format_context &ctx) const {
    for (size_t i = 0; i < r.records.size(); i += 1) {
      std::format_to(ctx.out(), "{}: \n{}", i, r.records[i]);
    }
    return ctx.out();
  }
};