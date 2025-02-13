import moderna.test_lib;
import moderna.type_check;
#include <moderna/test_lib.hpp>
#include <rfl/Box.hpp>
#include <array>
#include <format>
#include <print>

namespace tc = moderna::type_check;
namespace tl = moderna::test_lib;

constexpr static auto test_stmt = std::array{
  "A",
  "List[A]",
  "Optional[A]",
  "Union[A, B]",
  "Tuple[A, B]",
  "Variadic[A]",
  "B",
  "List[Optional[A]]",
  "List[Union[A, B]]",
  "Optional[Union[A, B]]",
  "List[Optional[Union[A, B]]]",
  "Variadic[List[Optional[Union[A, B]]]]",
  "Tuple[List[A], List[B], List[C]]",
  "Tuple[List[A], List[B], List[C], List[D]]",
  "Union[List[A], List[B], List[C], List[D]]",
  "Union[List[Optional[A]], List[Optional[B]], List[Optional[C]], List[Optional[D]]]",
  "Variadic[List[Union[A, Variadic[List[Union[A, B]]]]]]]",
  "Optional[Union[A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z]]",
  "Tuple[A_BC, BC_A]",
  "Tuple[ABC_CBA_BAC, Optional[ABC_CBA_BAC]]",
  "Tuple[ABC_CBA_BAC, Optional[ABC_CBA_BAC], List[ABC_CBA_BAC]]",
  "Union[ABC_CBA_BAC, Variadic[ABC_CBA_BAC], List[Union[A, Variadic[List[Union[A, B]]]]]]]",
  R"("A")",
  R"({"type" : "List", "child" : "A"})",
  R"({"type" : "Optional", "child" : "A"})",
  R"({"type" : "Union", "child" : ["A", "B"]})",
  R"({"type" : "Tuple", "child" : ["A", "B"]})",
};

static auto test_answer = std::array{
  tc::generic_type{tc::generic_type::basic_type{"A"}},
  tc::generic_type{tc::generic_type::list_type::make(tc::generic_type::basic_type{"A"})},
  tc::generic_type{tc::generic_type::optional_type::make(tc::generic_type::basic_type{"A"})},
  tc::generic_type{tc::generic_type::union_type::make(
    {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"}}
  )},
  tc::generic_type{tc::generic_type::tuple_type::make(
    {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"}}
  )},
  tc::generic_type{tc::generic_type::variadic_type::make(tc::generic_type::basic_type{"A"})},
  tc::generic_type{tc::generic_type::basic_type{"B"}},
  tc::generic_type{tc::generic_type::list_type::make(
    tc::generic_type::optional_type::make(tc::generic_type::basic_type{"A"})
  )},
  tc::generic_type{tc::generic_type::list_type::make(tc::generic_type::union_type::make(
    {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"}}
  ))},
  tc::generic_type{tc::generic_type::optional_type::make(tc::generic_type::union_type::make(
    {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"}}
  ))},
  tc::generic_type{tc::generic_type::list_type::make(
    tc::generic_type::optional_type::make(tc::generic_type::union_type::make(
      {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"}}
    ))
  )},
  tc::generic_type{tc::generic_type::variadic_type::make(tc::generic_type::list_type::make(
    tc::generic_type::optional_type::make(tc::generic_type::union_type::make(
      {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"}}
    ))
  ))},
  tc::generic_type{tc::generic_type::tuple_type::make(
    {tc::generic_type::list_type::make(tc::generic_type::basic_type{"A"}),
     tc::generic_type::list_type::make(tc::generic_type::basic_type{"B"}),
     tc::generic_type::list_type::make(tc::generic_type::basic_type{"C"})}
  )},
  tc::generic_type{tc::generic_type::tuple_type::make(
    {tc::generic_type::list_type::make(tc::generic_type::basic_type{"A"}),
     tc::generic_type::list_type::make(tc::generic_type::basic_type{"B"}),
     tc::generic_type::list_type::make(tc::generic_type::basic_type{"C"}),
     tc::generic_type::list_type::make(tc::generic_type::basic_type{"D"})}
  )},
  tc::generic_type{tc::generic_type::union_type::make(
    {tc::generic_type::list_type::make(tc::generic_type::basic_type{"A"}),
     tc::generic_type::list_type::make(tc::generic_type::basic_type{"B"}),
     tc::generic_type::list_type::make(tc::generic_type::basic_type{"C"}),
     tc::generic_type::list_type::make(tc::generic_type::basic_type{"D"})}
  )},
  tc::generic_type{tc::generic_type::union_type::make(
    {tc::generic_type::list_type::make(
       tc::generic_type::optional_type::make(tc::generic_type::basic_type{"A"})
     ),
     tc::generic_type::list_type::make(
       tc::generic_type::optional_type::make(tc::generic_type::basic_type{"B"})
     ),
     tc::generic_type::list_type::make(
       tc::generic_type::optional_type::make(tc::generic_type::basic_type{"C"})
     ),
     tc::generic_type::list_type::make(
       tc::generic_type::optional_type::make(tc::generic_type::basic_type{"D"})
     )}
  )},
  tc::generic_type{tc::generic_type::variadic_type::make(
    tc::generic_type::list_type::make(tc::generic_type::union_type::make(
      {tc::generic_type::basic_type{"A"},
       tc::generic_type::variadic_type::make(
         tc::generic_type::list_type::make(tc::generic_type::union_type::make(
           {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"}}
         ))
       )}
    ))
  )},
  tc::generic_type{tc::generic_type::optional_type::make(tc::generic_type::union_type::make(
    {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"},
     tc::generic_type::basic_type{"C"}, tc::generic_type::basic_type{"D"},
     tc::generic_type::basic_type{"E"}, tc::generic_type::basic_type{"F"},
     tc::generic_type::basic_type{"G"}, tc::generic_type::basic_type{"H"},
     tc::generic_type::basic_type{"I"}, tc::generic_type::basic_type{"J"},
     tc::generic_type::basic_type{"K"}, tc::generic_type::basic_type{"L"},
     tc::generic_type::basic_type{"M"}, tc::generic_type::basic_type{"N"},
     tc::generic_type::basic_type{"O"}, tc::generic_type::basic_type{"P"},
     tc::generic_type::basic_type{"Q"}, tc::generic_type::basic_type{"R"},
     tc::generic_type::basic_type{"S"}, tc::generic_type::basic_type{"T"},
     tc::generic_type::basic_type{"U"}, tc::generic_type::basic_type{"V"},
     tc::generic_type::basic_type{"W"}, tc::generic_type::basic_type{"X"},
     tc::generic_type::basic_type{"Y"}, tc::generic_type::basic_type{"Z"}}
  ))},
  tc::generic_type{tc::generic_type::tuple_type::make(
    {tc::generic_type::basic_type{"A_BC"}, tc::generic_type::basic_type{"BC_A"}}
  )},
  tc::generic_type{tc::generic_type::tuple_type::make(
    {tc::generic_type::basic_type{"ABC_CBA_BAC"},
     tc::generic_type::optional_type::make(tc::generic_type::basic_type{"ABC_CBA_BAC"})}
  )},
  tc::generic_type{tc::generic_type::tuple_type::make(
    {tc::generic_type::basic_type{"ABC_CBA_BAC"},
     tc::generic_type::optional_type::make(tc::generic_type::basic_type{"ABC_CBA_BAC"}),
     tc::generic_type::list_type::make(tc::generic_type::basic_type{"ABC_CBA_BAC"})}
  )},
  tc::generic_type{tc::generic_type::union_type::make(
    {tc::generic_type::basic_type{"ABC_CBA_BAC"},
     tc::generic_type::variadic_type::make(tc::generic_type::basic_type{"ABC_CBA_BAC"}),
     tc::generic_type::list_type::make(tc::generic_type::union_type::make(
       {tc::generic_type::basic_type{"A"},
        tc::generic_type::variadic_type::make(
          tc::generic_type::list_type::make(tc::generic_type::union_type::make(
            {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"}}
          ))
        )}
     ))}
  )},
  tc::generic_type{tc::generic_type::basic_type{"A"}},
  tc::generic_type{tc::generic_type::list_type::make(tc::generic_type::basic_type{"A"})},
  tc::generic_type{tc::generic_type::optional_type::make(tc::generic_type::basic_type{"A"})},
  tc::generic_type{tc::generic_type::union_type::make(
    {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"}}
  )},
  tc::generic_type{tc::generic_type::tuple_type::make(
    {tc::generic_type::basic_type{"A"}, tc::generic_type::basic_type{"B"}}
  )}
};

static_assert(test_stmt.size() == test_answer.size());

struct run_ith_test {
  void operator()() const {
    auto stmt = test_stmt[i];
    auto gen_type = tc::from_string(stmt);
    if (!gen_type) {
      throw std::runtime_error(std::format("{}", gen_type.error()));
    } else {
      if (gen_type.value() != test_answer[i]) {
        throw std::runtime_error(
          std::format("Expected: {}\nGot: {}", test_answer[i], gen_type.value())
        );
      }
    }
  }
  size_t i;
};

MODERNA_SETUP(argc, argv) {
  for (size_t i = 0; i < test_stmt.size(); i += 1) {
    tl::add_test(run_ith_test{i}, std::format("Test {}", i));
  }
}