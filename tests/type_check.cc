import moderna.test_lib;
import moderna.type_check;
namespace tc = moderna::type_check;
using tcg = moderna::type_check::generic_type;
namespace tl = moderna::test_lib;
#include <moderna/test_lib.hpp>
#include <rfl/json.hpp>
#include <array>
#include <print>
#include <rfl.hpp>
#include <string>

MODERNA_ADD_TEST(basic_match_test) {
  auto basic_a = tcg::basic_type{"A"};
  auto basic_b = tcg::basic_type{"A"};
  tl::assert_equal(tc::is_connectable(basic_a, basic_b), true);
}

MODERNA_ADD_TEST(basic_mismatch_test) {
  auto basic_a = tcg::basic_type{"A"};
  auto basic_b = tcg::basic_type{"B"};
  tl::assert_equal(tc::is_connectable(basic_a, basic_b), false);
}

MODERNA_ADD_TEST(union_mismatch_test) {
  auto union_a =
    tcg::union_type::make({tcg::list_type::make(tcg::basic_type{"A"}), tcg::basic_type{"B"}});
  auto union_b =
    tcg::union_type::make({tcg::basic_type{"B"}, tcg::basic_type{"A"}, tcg::basic_type{"C"}});
  tl::assert_equal(tc::is_connectable(union_a, union_b), false);
}

MODERNA_ADD_TEST(union_match_test) {
  auto union_a = tcg::union_type::make({tcg::basic_type{"A"}, tcg::basic_type{"B"}});
  auto union_b =
    tcg::union_type::make({tcg::basic_type{"B"}, tcg::basic_type{"A"}, tcg::basic_type{"C"}});
  tl::assert_equal(tc::is_connectable(union_a, union_b), true);
}

MODERNA_ADD_TEST(list_mismatch_test) {
  auto list_basic_a = tcg::list_type::make(tcg::basic_type("A"));
  auto list_basic_b = tcg::list_type::make(tcg::basic_type("B"));
  tl::assert_equal(tc::is_connectable(list_basic_a, list_basic_b), false);
}
MODERNA_ADD_TEST(list_match_test) {
  auto list_basic_a = tcg::list_type::make(tcg::basic_type("A"));
  auto list_basic_b = tcg::list_type::make(tcg::basic_type("A"));
  tl::assert_equal(tc::is_connectable(list_basic_a, list_basic_b), true);
}

MODERNA_ADD_TEST(optionality_test) {
  auto optional_a = tcg::optional_type::make(tcg::basic_type("A"));
  tl::assert_equal(allow_empty(optional_a), true);
}
MODERNA_ADD_TEST(optional_match_test) {
  auto basic_a = tc::basic_type{"A"};
  auto optional_a = tc::optional_type::make(basic_a);
  tl::assert_equal(tc::is_connectable(basic_a, optional_a), true);
}
MODERNA_ADD_TEST(optional_mismatch_test) {
  auto optional_a = tc::optional_type::make(tc::basic_type{"A"});
  tl::assert_equal(tc::is_connectable(tc::none_type::make(), optional_a), true);
}

MODERNA_ADD_TEST(variadic_match_test) {
  auto variadic_a = tc::variadic_type::make(tcg::basic_type("A"));
  auto basic_a = tcg::basic_type("A");
  tl::assert_equal(tc::is_connectable(basic_a, variadic_a), true);
}

MODERNA_ADD_TEST(tuple_match_test) {
  auto tuple_a = tcg::tuple_type::make({tcg::basic_type("A"), tcg::basic_type("B")});
  auto tuple_b = tcg::tuple_type::make({tcg::basic_type("A"), tcg::basic_type("B")});
  tl::assert_true(tc::is_connectable(tuple_a, tuple_b));
}

MODERNA_ADD_TEST(tuple_mismatch_test) {
  auto tuple_a = tcg::tuple_type::make({tcg::basic_type("A"), tcg::basic_type("B")});
  auto tuple_b = tcg::tuple_type::make({tcg::basic_type("B"), tcg::basic_type("A")});
  tl::assert_true(!tc::is_connectable(tuple_a, tuple_b));
}

MODERNA_ADD_TEST(stringify_test) {
  std::array tests{
    "A",
    "List[A]",
    "Optional[A]",
    "List[Optional[A]]",
    "List[Union[A, B]]",
    "Union[A, B]",
    "Variadic[ABC]",
    "Variadic[List[Optional[Union[A, B, Optional[Variadic[C]]]]]]",
    "Tuple[ABC, Optional[ABC]]",
  };
  for (size_t i = 0; i < tests.size(); i += 1) {
    tl::assert_equal(std::format("{}", tc::from_string(tests[i]).value()), tests[i]);
  }
}