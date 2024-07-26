#include <emscripten/bind.h>
#include <concepts>
#include <format>
#include <string>
#include <map>
import moderna.type_check;

namespace tc = moderna::type_check;
using namespace emscripten;
using tcg = tc::generic_type;

template<typename T>
auto from_json(const std::string &t) {
  return T::from_json(t).value();
}
template <typename T> auto to_string(const T &t) {
  return std::format("{}", t);
}
template <typename T> auto get_data(const T &t) {
  return std::string(t.name.data());
}
template <typename T> auto get_target(const T &t) {
  return std::string(t.target.data());
}
template <typename T> auto get_match(const T &t, const std::string &name) {
  auto rec = t.get_match(name);
  return *t.get_match(name).value();
}

EMSCRIPTEN_BINDINGS(moderna_type_check) {
  register_vector<tc::record_match<tcg>>("RecordMatchArray");
  register_vector<tc::source_match<tcg>>("SourceMatchArray");
  register_vector<std::string>("StringArray");
  register_map<std::string, std::string>("StringRecord");
  class_<tcg>("GenericType")
    .class_function("from_json", from_json<tcg>)
    .function("name", &tcg::name)
    .function("to_string", &tcg::name);

  class_<tc::nameless_record<tcg>>("NamelessRecord")
    .class_function("make_empty", tc::nameless_record<tcg>::make_empty)
    .class_function("from_json", from_json<tc::nameless_record<tcg> >)
    .function("to_multiple", &tc::nameless_record<tcg>::to_multiple)
    .function("add_record", &tc::nameless_record<tcg>::add_record, return_value_policy::reference())
    .function("add", &tc::nameless_record<tcg>::operator+)
    .function("get_name_pairs", &tc::nameless_record<tcg>::get_name_pairs)
    .function("size", &tc::nameless_record<tcg>::size)
    .function("is_empty", &tc::nameless_record<tcg>::is_empty)
    .function("to_string", to_string<tc::nameless_record<tcg>>);

  class_<tc::multi_nameless_record<tcg>>("MultiNamelessRecord")
    .class_function("make_empty", tc::multi_nameless_record<tcg>::make_empty)
    .function("add", &tc::multi_nameless_record<tcg>::operator+)
    .function("to_string", to_string<tc::multi_nameless_record<tcg>>);

  class_<tc::source_match<tcg>>("SourceMatch")
    .property("type", &tc::source_match<tcg>::type, return_value_policy::reference())
    .property("id", &tc::source_match<tcg>::id)
    .property("name", get_data<tc::source_match<tcg>>)
    .function("to_string", to_string<tc::source_match<tcg>>);

  class_<tc::record_match<tcg>>("RecordMatch")
    .function("is_empty", &tc::record_match<tcg>::is_empty)
    .property("type", &tc::record_match<tcg>::type, return_value_policy::reference())
    .property("sources", &tc::record_match<tcg>::sources, return_value_policy::reference())
    .property("target", get_target<tc::record_match<tcg>>)
    .function("to_string", to_string<tc::record_match<tcg>>)
    .function("size", &tc::record_match<tcg>::size);

  class_<tc::record_matches<tcg>>("RecordMatches")
    .class_function(
      "create",
      select_overload<tc::record_matches<
        tcg>(const tc::multi_nameless_record<tcg> &, const tc::nameless_record<tcg> &)>(
        [](const tc::multi_nameless_record<tcg> &s, const tc::nameless_record<tcg> &t) {
          return tc::record_matches<tcg>::create(s, t);
        }
      )
    )
    .function("get_match", get_match<tc::record_matches<tcg>>, return_value_policy::reference())
    .function("is_complete", &tc::record_matches<tcg>::is_complete)
    .function("is_empty", &tc::record_matches<tcg>::is_empty)
    .function("is_partial", &tc::record_matches<tcg>::is_partial)
    .property("matches", &tc::record_matches<tcg>::matches, return_value_policy::reference())
    .function("to_string", to_string<tc::record_matches<tcg>>);
}