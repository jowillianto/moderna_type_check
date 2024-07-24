import moderna.test_lib;
import moderna.type_check;
import moderna.io;
namespace tc = moderna::type_check;
using tcg = moderna::type_check::generic_type;
namespace tl = moderna::test_lib;
namespace io = moderna::io;
#include <rfl/json.hpp>
#include <map>
#include <print>
#include <rfl.hpp>
#include <string>

auto match_tests =
  tl::make_tester("match_test")
    .add_test(
      R"(Basic["A"] >> Basic["A"])",
      []() {
        auto basic_a = tcg::basic_type{"A"};
        auto basic_b = tcg::basic_type{"A"};
        tl::assert_equal(tc::is_connectable(basic_a, basic_b), true);
      }
    )
    .add_test(
      R"(Basic["A"] >> Basic["B"])",
      []() {
        auto basic_a = tcg::basic_type{"A"};
        auto basic_b = tcg::basic_type{"B"};
        tl::assert_equal(tc::is_connectable(basic_a, basic_b), false);
      }
    )
    .add_test(
      R"(Union[List[A], Basic[B]] -> Union[Basic[A], Basic[B], Basic[C]])",
      []() {
        auto union_a =
          tcg::union_type::make(tcg::list_type::make(tcg::basic_type{"A"}), tcg::basic_type{"B"});
        auto union_b =
          tcg::union_type::make(tcg::basic_type{"B"}, tcg::basic_type{"A"}, tcg::basic_type{"C"});
        tl::assert_equal(tc::is_connectable(union_a, union_b), false);
      }
    )
    .add_test(
      R"(Basic[A] -> Union[A, List[A]])",
      []() {
        auto basic_a = tcg::basic_type{"A"};
        auto union_a =
          tcg::union_type::make(tcg::basic_type{"A"}, tcg::list_type::make(tcg::basic_type{"A"}));
        tl::assert_equal(tc::is_connectable(basic_a, union_a), true);
      }
    )
    .add_test(
      R"(List[Basic["A"]] >> List[Basic["B"]])",
      []() {
        auto list_basic_a = tcg::list_type::make(tcg::basic_type("A"));
        auto list_basic_b = tcg::list_type::make(tcg::basic_type("B"));
        tl::assert_equal(tc::is_connectable(list_basic_a, list_basic_b), false);
      }
    )
    .add_test(R"(List[Basic["A"]] >> List[Basic["A"]])", []() {
      auto list_basic_a = tcg::list_type::make(tcg::basic_type("A"));
      auto list_basic_b = tcg::list_type::make(tcg::basic_type("A"));
      tl::assert_equal(tc::is_connectable(list_basic_a, list_basic_b), true);
    });
auto engine_tests =
  tl::make_tester("engine_test")
    .add_test(
      "load_test",
      []() {
        auto file_a = io::readable_file::open(TYPE_A_FILE).value();
        auto file_b = io::readable_file::open(TYPE_B_FILE).value();
        auto file_c_a =
          rfl::json::read<std::map<std::string, std::map<std::string, tcg>>>(file_a.read().value())
            .value();
        auto file_c_b =
          rfl::json::read<std::map<std::string, std::map<std::string, tcg>>>(file_b.read().value())
            .value();
        auto rec_a_a = tc::nameless_record{file_c_a["a"]};
        auto rec_a_b = tc::nameless_record{file_c_a["b"]};
        auto rec_b_a = tc::nameless_record{file_c_b["a"]};
        auto rec_b_b = tc::nameless_record{file_c_b["b"]};
        auto matches_a = tc::record_matches<tcg>::create(rec_a_a, rec_b_a);
        auto matches_b = tc::record_matches<tcg>::create(rec_a_b, rec_b_b);
        tl::assert_equal(matches_a.is_complete(), true);
        tl::assert_equal(matches_b.is_complete(), true);
        std::print("{}", matches_a);
        std::print("{}", matches_b);
      }
    )
    .add_test("match_with_multiple", []() {
      auto file_a = io::readable_file::open(TYPE_A_FILE).value();
      auto file_b = io::readable_file::open(TYPE_B_FILE).value();
      auto file_c_a =
        rfl::json::read<std::map<std::string, std::map<std::string, tcg>>>(file_a.read().value())
          .value();
      auto file_c_b =
        rfl::json::read<std::map<std::string, std::map<std::string, tcg>>>(file_b.read().value())
          .value();
      auto rec_a_a = tc::nameless_record{file_c_a["a"]};
      auto rec_a_b = tc::nameless_record{file_c_a["c"]};
      auto rec_b_c = tc::nameless_record{file_c_b["c"]};
      auto matches = tc::record_matches<tcg>::create((rec_a_a + rec_a_b), rec_b_c);
      tl::assert_equal(matches.is_complete(), true);
      std::print("{}", matches);
    });

int main() {
  match_tests.print_or_exit();
  engine_tests.print_or_exit();
}