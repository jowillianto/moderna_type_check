#include <pybind11/detail/common.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <format>
#include <ranges>
import moderna.type_check;

namespace py = pybind11;
namespace tc = moderna::type_check;
using tcg = tc::generic_type;

PYBIND11_MODULE(moderna_type_check, m) {
  py::class_<tcg>(m, "GenericType")
    .def("name", &tcg::name)
    .def("allow_multiple", [](const tcg& g) {
      return tc::allow_multiple(g);
    })
    .def("allow_empty", [](const tcg& g){
      return tc::allow_empty(g);
    })
    .def("__str__", &tcg::name)
    .def("__repr__", &tcg::name)
    .def_static("from_json", [](const std::string& s){
      return tcg::from_json(s).value();
    });
  py::class_<tc::nameless_record<tcg>>(m, "NamelessRecord")
    .def_static(
      "from_json",
      [](const std::string &s) { return tc::nameless_record<tcg>::from_json(s).value(); }
    )
    .def_static("make_empty", [](){return tc::nameless_record<tcg>::make_empty();})
    .def("add_record", &tc::nameless_record<tcg>::add_record)
    .def("to_multiple", &tc::nameless_record<tcg>::to_multiple)
    .def("get_name_pairs", &tc::nameless_record<tcg>::get_name_pairs)
    .def("size", &tc::nameless_record<tcg>::size)
    .def("__len__", &tc::nameless_record<tcg>::size)
    .def("is_empty", &tc::nameless_record<tcg>::is_empty)
    .def(
      "__add__",
      [](const tc::nameless_record<tcg> &x, const tc::nameless_record<tcg> &y) { return x + y; },
      py::is_operator()
    )
    .def("__str__", [](const tc::nameless_record<tcg> &t) { return std::format("{}", t); })
    .def("__repr__", [](const tc::nameless_record<tcg> &t) { return std::format("{}", t); });
  py::class_<tc::multi_nameless_record<tcg>>(m, "MultiNamelessRecord")
    .def_static("make_empty", tc::multi_nameless_record<tcg>::make_empty)
    .def(
      "__add__",
      [](const tc::multi_nameless_record<tcg> &x, const tc::nameless_record<tcg> &y) {
        return x + y;
      }
    )
    .def("__str__", [](const tc::multi_nameless_record<tcg> &t) { return std::format("{}", t); })
    .def("__repr__", [](const tc::multi_nameless_record<tcg> &t) { return std::format("{}", t); });
  py::class_<tc::source_match<tcg>>(m, "SourceMatch")
    .def_property_readonly("type", [](const tc::source_match<tcg>& t){return t.type;}, py::return_value_policy::reference)
    .def_property_readonly("id", [](const tc::source_match<tcg> &t) { return t.id; })
    .def_property_readonly("name", [](const tc::source_match<tcg> &t) { return t.name.data(); })
    .def("__str__", [](const tc::source_match<tcg> &t) { return std::format("{}", t); })
    .def("__repr__", [](const tc::source_match<tcg> &t) { return std::format("{}", t); });
  py::class_<tc::record_match<tcg>>(m, "RecordMatch")
    .def("is_empty", &tc::record_match<tcg>::is_empty)
    .def_property_readonly("type", [](const tc::record_match<tcg>& t){return t.type;}, py::return_value_policy::reference)
    .def_property_readonly(
      "sources",
      [](const tc::record_match<tcg> &t) { return t.sources; },
      py::return_value_policy::reference
    )
    .def_property_readonly("target", [](const tc::record_match<tcg> &e) { return e.target.data(); })
    .def("__str__", [](const tc::record_match<tcg> &t) { return std::format("{}", t); })
    .def("__repr__", [](const tc::record_match<tcg> &t) { return std::format("{}", t); })
    .def("size", &tc::record_match<tcg>::size);
  py::class_<tc::record_matches<tcg>>(m, "RecordMatches")
    .def_static(
      "create",
      [](const tc::multi_nameless_record<tcg> &s, const tc::nameless_record<tcg> &t) {
        return tc::record_matches<tcg>::create(s, t);
      }
    )
    .def_static(
      "create",
      [](const tc::nameless_record<tcg> &s, const tc::nameless_record<tcg> &t) {
        return tc::record_matches<tcg>::create(s, t);
      }
    )
    .def(
      "get_match",
      [](const tc::record_matches<tcg> &t, const std::string &name) {
        auto rec = t.get_match(name);
        return *t.get_match(name).value();
      },
      py::return_value_policy::reference
    )
    .def("is_complete", &tc::record_matches<tcg>::is_complete)
    .def("is_empty", &tc::record_matches<tcg>::is_empty)
    .def("is_partial", &tc::record_matches<tcg>::is_partial)
    .def_property_readonly(
      "matches",
      [](const tc::record_matches<tcg> &t) { return t.matches; },
      py::return_value_policy::reference
    )
    .def("__str__", [](const tc::record_matches<tcg> &t) { return std::format("{}", t); })
    .def("__repr__", [](const tc::record_matches<tcg> &t) { return std::format("{}", t); });
}