#include <emscripten/bind.h>
import moderna.type_check


namespace py = pybind11;
namespace tc = moderna::type_check;
using tcg = tc::generic_type;

PYBIND11_MODULE(moderna_type_check, m) {
  py::class_<tc::nameless_record<tcg>>(m, "NamelessRecord")
    .def_static(
      "from_json",
      [](const std::string &s) { return tc::nameless_record<tcg>::from_json(s).value(); }
    )
    .def("to_multiple", &tc::nameless_record<tcg>::to_multiple)
    .def(
      "__add__",
      [](const tc::nameless_record<tcg> &x, const tc::nameless_record<tcg> &y) { return x + y; },
      py::is_operator()
    );
  py::class_<tc::multi_nameless_record<tcg>>(m, "MultiNamelessRecord")
    .def("__add__", [](const tc::multi_nameless_record<tcg> &x, const tc::nameless_record<tcg> &y) {
      return x + y;
    });
  py::class_<tc::source_match<tcg>>(m, "SourceMatch")
    .def_property_readonly("record", [](const tc::source_match<tcg> &t) { return t.record; })
    .def_property_readonly("id", [](const tc::source_match<tcg> &t) { return t.id; })
    .def_property_readonly("name", [](const tc::source_match<tcg> &t) { return t.name.data(); });
  py::class_<tc::record_match<tcg>>(m, "RecordMatch")
    .def("is_empty", &tc::record_match<tcg>::is_empty)
    .def_property_readonly("sources", [](const tc::record_match<tcg> &t) { return t.sources; })
    .def_property_readonly("target", [](const tc::record_match<tcg> &e) { return e.target.data(); })
    .def("size", &tc::record_match<tcg>::size);
  py::class_<tc::record_matches<tcg>>(m, "RecordMatches")
    .def_static("create", &tc::record_matches<tcg>::create)
    .def("get", &tc::record_matches<tcg>::get)
    .def("is_complete", &tc::record_matches<tcg>::is_complete)
    .def("is_empty", &tc::record_matches<tcg>::is_empty)
    .def("is_partial", &tc::record_matches<tcg>::is_partial)
    .def_property_readonly("matches", [](const tc::record_matches<tcg> &t) { return t.matches; })
    .def("__str__", [](const tc::record_matches<tcg>& t){
      return std::format("{}", t);
    })
    .def("__repr__", [](const tc::record_matches<tcg>& t){
      return std::format("{}", t);
    });
}