#include <pybind11/detail/common.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <rfl/json.hpp>
#include <concepts>
#include <expected>
#include <format>
#include <rfl.hpp>
#include <string>
#include <string_view>
import moderna.type_check;

namespace py = pybind11;
namespace tc = moderna::type_check;
using tcg = tc::generic_type;

template <class T> std::string to_string(const T &t) {
  return std::format("{}", t);
}
template <class T> tc::generic_type make_generic_type(const T &t) {
  return tc::generic_type{t.clone()};
}
template <class T, class V> bool is_equal(const T &a, const V &b) {
  return a == b;
}
template <class E>
concept is_exception = requires(const E ce) {
  { ce.what() } -> std::convertible_to<std::string>;
};

/*
  std::expected type casting
*/

namespace PYBIND11_NAMESPACE {
  namespace detail {
    template <class T, is_exception E> struct type_caster<std::expected<T, E>> {
    public:
      /**
       * This macro establishes the name 'inty' in
       * function signatures and declares a local variable
       * 'value' of type inty
       */
      using expected_t = std::expected<T, E>;
      PYBIND11_TYPE_CASTER(expected_t, const_name("expected"));
      static handle cast(expected_t src, return_value_policy policy, handle parent) {
        if (src)
          return pybind11::detail::make_caster<T>::cast(std::move(src.value()), policy, parent);
        else
          throw std::runtime_error{src.error().what()};
      }
    };
    template <class T> struct type_caster<rfl::Box<T>> {
      PYBIND11_TYPE_CASTER(rfl::Box<T>, const_name("Box"));
      static handle cast(const rfl::Box<T> &src, return_value_policy policy, handle parent) {
        return pybind11::detail::make_caster<T>::cast(*src, policy, parent);
      }
    };
    template <class T> struct type_caster<rfl::Result<T>> {
      PYBIND11_TYPE_CASTER(rfl::Result<T>, const_name("Result"));
      static handle cast(const rfl::Result<T> &src, return_value_policy policy, handle parent) {
        if (src) {
          return pybind11::detail::make_caster<T>::cast(src.value(), policy, parent);
        } else {
          throw std::runtime_error{src.error().value().what()};
        }
      }
    };
  }
}

PYBIND11_MODULE(moderna_type_check, m) {
  py::class_<tc::generic_type>(m, "GenericType")
    .def(py::init(&make_generic_type<tc::basic_type>))
    .def(py::init(&make_generic_type<tc::list_type>))
    .def(py::init(&make_generic_type<tc::optional_type>))
    .def(py::init(&make_generic_type<tc::union_type>))
    .def(py::init(&make_generic_type<tc::tuple_type>))
    .def(py::init(&make_generic_type<tc::variadic_type>))
    .def(py::init(&make_generic_type<tc::none_type>))
    .def("is_basic_type", &tc::generic_type::is_basic_type)
    .def("is_list_type", &tc::generic_type::is_list_type)
    .def("is_optional_type", &tc::generic_type::is_optional_type)
    .def("is_union_type", &tc::generic_type::is_union_type)
    .def("is_tuple_type", &tc::generic_type::is_tuple_type)
    .def("is_variadic_type", &tc::generic_type::is_variadic_type)
    .def("is_none_type", &tc::generic_type::is_none_type)
    .def("as_basic_type", &tc::generic_type::as_basic_type)
    .def("as_list_type", &tc::generic_type::as_list_type)
    .def("as_optional_type", &tc::generic_type::as_optional_type)
    .def("as_union_type", &tc::generic_type::as_union_type)
    .def("as_tuple_type", &tc::generic_type::as_tuple_type)
    .def("as_variadic_type", &tc::generic_type::as_variadic_type)
    .def("as_none_type", &tc::generic_type::as_none_type)
    .def("__str__", &to_string<tc::generic_type>)
    .def("__repr__", &to_string<tc::generic_type>)
    .def("__eq__", &is_equal<tc::generic_type, tc::generic_type>, py::is_operator())
    .def("allow_empty", &tc::allow_empty<tc::generic_type>)
    .def("allow_multiple", &tc::allow_multiple<tc::generic_type>)
    .def("clone", &tc::generic_type::clone)
    .def_static(
      "from_json", py::overload_cast<const std::string &>(&rfl::json::read<tc::generic_type>)
    );
  py::class_<tc::basic_type>(m, "BasicType")
    .def(py::init(&tc::basic_type::make))
    .def("__str__", &to_string<tc::basic_type>)
    .def("__repr__", &to_string<tc::basic_type>)
    .def("allow_empty", &tc::allow_empty<tc::basic_type>)
    .def("allow_multiple", &tc::allow_multiple<tc::basic_type>)
    .def_readonly("type", &tc::basic_type::type);
  py::class_<tc::list_type>(m, "ListType")
    .def(py::init(&tc::list_type::make))
    .def("allow_empty", &tc::allow_empty<tc::list_type>)
    .def("allow_multiple", &tc::allow_multiple<tc::list_type>)
    .def("__str__", &to_string<tc::list_type>)
    .def("__repr__", &to_string<tc::list_type>)
    .def_readonly("child", &tc::list_type::child, py::return_value_policy::reference);
  py::class_<tc::union_type>(m, "UnionType")
    .def(py::init(&tc::union_type::make))
    .def("allow_empty", &tc::allow_empty<tc::union_type>)
    .def("allow_multiple", &tc::allow_multiple<tc::union_type>)
    .def("__str__", &to_string<tc::union_type>)
    .def("__repr__", &to_string<tc::union_type>)
    .def_readonly("child", &tc::union_type::child);
  py::class_<tc::tuple_type>(m, "TupleType")
    .def(py::init(&tc::tuple_type::make))
    .def("allow_empty", &tc::allow_empty<tc::tuple_type>)
    .def("allow_multiple", &tc::allow_multiple<tc::tuple_type>)
    .def("__str__", &to_string<tc::tuple_type>)
    .def("__repr__", &to_string<tc::tuple_type>)
    .def_readonly("child", &tc::tuple_type::child);
  py::class_<tc::optional_type>(m, "OptionalType")
    .def(py::init(&tc::optional_type::make))
    .def("allow_empty", &tc::allow_empty<tc::optional_type>)
    .def("allow_multiple", &tc::allow_multiple<tc::optional_type>)
    .def("__str__", &to_string<tc::optional_type>)
    .def("__repr__", &to_string<tc::optional_type>)
    .def_readonly("child", &tc::optional_type::child);
  py::class_<tc::variadic_type>(m, "VariadicType")
    .def(py::init(&tc::variadic_type::make))
    .def("allow_empty", &tc::allow_empty<tc::variadic_type>)
    .def("allow_multiple", &tc::allow_multiple<tc::variadic_type>)
    .def_readonly("child", &tc::variadic_type::child);
  py::class_<tc::none_type>(m, "NoneType")
    .def(py::init(&tc::none_type::make))
    .def("allow_empty", &tc::allow_empty<tc::none_type>)
    .def("allow_multiple", &tc::allow_multiple<tc::none_type>)
    .def("__str__", &to_string<tc::none_type>)
    .def("__repr__", to_string<tc::none_type>);
  py::class_<tc::nameless_record>(m, "NamelessRecord")
    .def(py::init<>())
    .def(
      "add_record",
      py::overload_cast<std::string, tc::generic_type>(&tc::nameless_record::add_record),
      py::return_value_policy::reference
    )
    .def("to_multiple", &tc::nameless_record::to_multiple)
    .def("get", py::overload_cast<std::string_view>(&tc::nameless_record::get))
    .def(
      "__add__",
      static_cast<tc::multi_nameless_record (tc::nameless_record::*)(const tc::nameless_record &)
                    const>(&tc::nameless_record::operator+),
      py::is_operator()
    )
    .def(
      "__add__",
      static_cast<tc::multi_nameless_record (tc::nameless_record::*)(const tc::multi_nameless_record
                                                                       &) const>(
        &tc::nameless_record::operator+
      ),
      py::is_operator()
    )
    .def("__str__", &to_string<tc::nameless_record>)
    .def("__repr__", &to_string<tc::nameless_record>)
    .def("size", &tc::nameless_record::size)
    .def("__len__", &tc::nameless_record::size)
    .def_static("from_json", &tc::nameless_record::from_json);

  py::class_<tc::multi_nameless_record>(m, "MultiNamelessRecord")
    .def(py::init<>())
    .def("add_record", &tc::multi_nameless_record::add_record)
    .def("get", py::overload_cast<size_t>(&tc::multi_nameless_record::get))
    .def("get", py::overload_cast<std::string_view, size_t>(&tc::multi_nameless_record::get))
    .def(
      "__add__",
      static_cast<tc::multi_nameless_record (tc::multi_nameless_record::*)(const tc::nameless_record
                                                                             &) const>(
        &tc::multi_nameless_record::operator+
      )
    )
    .def("__str__", &to_string<tc::multi_nameless_record>)
    .def("__repr__", &to_string<tc::multi_nameless_record>);
  py::class_<tc::record_matches>(m, "RecordMatches")
    .def_static(
      "create",
      py::overload_cast<const tc::multi_nameless_record &, const tc::nameless_record &>(
        &tc::record_matches::create
      )
    )
    .def_static(
      "create",
      py::overload_cast<const tc::nameless_record &, const tc::nameless_record &>(
        &tc::record_matches::create
      )
    )
    .def("get_match", &tc::record_matches::get_match)
    .def("get_connectable", &tc::record_matches::get_connectable)
    .def("is_complete", &tc::record_matches::is_complete)
    .def("is_empty", &tc::record_matches::is_empty)
    .def("is_partial", &tc::record_matches::is_partial)
    .def("__str__", &to_string<tc::record_matches>)
    .def("__repr__", &to_string<tc::record_matches>)
    .def("size", &tc::record_matches::size);
  py::class_<tc::record_match>(m, "RecordMatch")
    .def_readonly("type", &tc::record_match::type, py::return_value_policy::reference)
    .def_readonly("target", &tc::record_match::target)
    .def_readonly("sources", &tc::record_match::sources, py::return_value_policy::reference)
    .def("is_empty", &tc::record_match::is_empty)
    .def("is_not_empty", &tc::record_match::is_not_empty)
    .def("is_connectable", &tc::record_match::is_connectable)
    .def("first", &tc::record_match::first)
    .def("__str__", &to_string<tc::record_match>)
    .def("__repr__", &to_string<tc::record_match>);
  py::class_<tc::source_match>(m, "SourceMatch")
    .def_readonly("source", &tc::source_match::source, py::return_value_policy::reference)
    .def_readonly("name", &tc::source_match::name)
    .def_readonly("id", &tc::source_match::id)
    .def("__str__", &to_string<tc::source_match>)
    .def("__repr__", &to_string<tc::source_match>);
  m.def("is_connectable", &tc::is_connectable<tc::generic_type, tc::generic_type>);
  m.def("from_string", py::overload_cast<std::string_view>(&tc::from_string));
}