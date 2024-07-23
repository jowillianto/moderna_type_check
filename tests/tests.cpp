import moderna.test_lib;
import moderna.type_check;
import moderna.io;
namespace tc = moderna::type_check;
using tcg = moderna::type_check::generic_type;
namespace tl = moderna::test_lib;
namespace io = moderna::io;
#include <rfl/json.hpp>
#include <map>
#include <algorithm>
#include <rfl.hpp>
#include <string>
#include <print>

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
    .add_test(R"(Basic[A] -> Union[A, List[A]])", [](){
      auto basic_a = tcg::basic_type{"A"};
      auto union_a = tcg::union_type::make(tcg::basic_type{"A"}, tcg::list_type::make(tcg::basic_type{"A"}));
      tl::assert_equal(tc::is_connectable(basic_a, union_a), true);
    })
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
        auto record_a = tc::nameless_record<tcg>{
          .records{rfl::json::read<std::map<std::string, tcg>>(file_a.read().value()).value()}
        };
        auto record_b = tc::nameless_record<tcg>{
          .records{rfl::json::read<std::map<std::string, tcg>>(file_b.read().value()).value()}
        };
        auto matches = tc::record_matches<tcg>::create(record_b, record_a);
        auto entry = std::ranges::find_if(matches.matches, [](const auto& match){
          return match.target == "d";
        });
        tl::assert_equal((entry -> sources).size(), 3);
        tl::assert_equal(matches.is_complete(), true);
      }
    )
    .add_test("match_test", []() {
      
    });

int main() {
  match_tests.print_or_exit();
  engine_tests.print_or_exit();
}