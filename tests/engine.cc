import moderna.test_lib;
import moderna.type_check;
namespace tc = moderna::type_check;
using tcg = moderna::type_check::generic_type;
namespace tl = moderna::test_lib;
#include <moderna/test_lib.hpp>
#include <rfl/json.hpp>
#include <print>
#include <rfl.hpp>
#include <string>

constexpr static auto A_JSON_FILE = R"({
  "a" : {
    "a" : "p",
    "b" : "f"
  },
  "b" : {
    "a" : "c",
    "b" : {
      "type" : "list",
      "child" : {
        "type" : "tuple",
        "child" : ["a", "b", "c", "d"]
      }
    }, 
    "c" : "a_b",
    "d" : "a_c",
    "e" : "a_d"
  },
  "c" : {
    "l" : "l"
  }
})";
constexpr static auto A_JSON_FILE_COMPACT = R"({
  "a" : "c",
  "b" : "List[Tuple[a, b, c, d]]",
  "c" : "Optional[a_b]"
})";
constexpr static auto A_JSON_FILE_NOT_COMPACT = R"({
  "a" : "c",
  "b" : {
      "type" : "List",
      "child" : {
          "type": "Tuple",
          "child" : ["a", "b", "c", "d"]
      }
  },
  "c" : {
      "type" : "Optional",
      "child" : "a_b"
  }
})";
constexpr static auto B_JSON_FILE_COMPACT = R"({
  "a" : "Optional[c]",
  "b" : "Optional[List[Tuple[a, b, c, d]]]",
  "c" : "Optional[Optional[a_b]]"
})";

constexpr static auto B_JSON_FILE_NOT_COMPACT = R"({
  "a" : {
    "type" : "Optional",
    "child" : "c"
  },
  "b" : {
    "type" : "Optional",
    "child" : {
        "type": "List",
        "child" : {
            "type": "Tuple",
            "child" : ["a", "b", "c", "d"]
        }
    }
  },
  "c" : {
    "type" : "Optional",
    "child" : {
        "type" : "Optional",
        "child" : "a_b"
    }
  }
})";

constexpr static auto B_JSON_FILE = R"({
  "a" : {
    "b" : {
      "type" : "optional",
      "child" : "p"
    },
    "c" : "f"
  }, 
  "c" : {
    "b" : {
      "type" : "optional",
      "child" : "p"
    },
    "c" : "f",
    "d" : "l"
  },
  "b" : {
    "e" : "c",
    "f" : {
      "type" : "list",
      "child" : {
        "type" : "tuple",
        "child" : ["a", "b", "c", "d"]
      }
    },
    "g" : {
      "type" : "Optional",
      "child" : "a_b"
    },
    "h" : {
      "type" : "Optional",
      "child" : "a_c"
    },
    "i" : {
      "type" : "optional",
      "child" : "a_d"
    }
  }
})";

MODERNA_ADD_TEST(nameless_format_record_test) {
  auto file_a = rfl::json::read<std::map<std::string, tc::nameless_record>>(A_JSON_FILE).value();
  auto record_a = file_a["a"];
  tl::assert_equal(std::format("{}", record_a), "nameless_record[a(p), b(f)]");
  auto record_b = file_a["b"];
  tl::assert_equal(
    std::format("{}", record_b),
    "nameless_record[a(c), b(List[Tuple[a, b, c, d]]), c(a_b), d(a_c), e(a_d)]"
  );
  auto record_c = file_a["c"];
  tl::assert_equal(std::format("{}", record_c), "nameless_record[l(l)]");
  auto file_b = rfl::json::read<std::map<std::string, tc::nameless_record>>(B_JSON_FILE).value();
  auto record_b_a = file_b["a"];
  tl::assert_equal(std::format("{}", record_b_a), "nameless_record[b(Optional[p]), c(f)]");
  auto record_b_b = file_b["b"];
  tl::assert_equal(
    std::format("{}", record_b_b),
    "nameless_record[e(c), f(List[Tuple[a, b, c, d]]), g(Optional[a_b]), h(Optional[a_c]), i(Optional[a_d])]"
  );
  auto record_b_c = file_b["c"];
  tl::assert_equal(std::format("{}", record_b_c), "nameless_record[b(Optional[p]), c(f), d(l)]");
}

MODERNA_ADD_TEST(source_match_format_test) {
  auto s_m = tc::source_match{"a", tc::generic_type{tc::basic_type{"A"}}, 0};
  tl::assert_equal(std::format("{}", s_m), "source_match[0_a(A)]");
  s_m.source = tc::generic_type{tc::list_type::make(tc::basic_type{"A"})};
  tl::assert_equal(std::format("{}", s_m), "source_match[0_a(List[A])]");
  s_m.source = tc::generic_type{tc::optional_type::make(tc::basic_type{"A"})};
  tl::assert_equal(std::format("{}", s_m), "source_match[0_a(Optional[A])]");
}

MODERNA_ADD_TEST(record_match_format_test) {
  auto r_m = tc::record_match{
    tc::generic_type{tc::basic_type{"A"}},
    "b",
    std::vector{
      tc::source_match{"a", tc::generic_type{tc::basic_type{"A"}}, 0},
      tc::source_match{"b", tc::generic_type{tc::basic_type{"B"}}, 0}
    }
  };
  tl::assert_equal(
    std::format("{}", r_m), "record_match[(source_match[0_a(A)], source_match[0_b(B)])->b(A)]"
  );
}

MODERNA_ADD_TEST(record_matches_format_test) {
  auto file_a = rfl::json::read<std::map<std::string, tc::nameless_record>>(A_JSON_FILE).value();
  auto record_a = file_a["a"];
  auto file_b = rfl::json::read<std::map<std::string, tc::nameless_record>>(B_JSON_FILE).value();
  auto record_b = file_b["a"];
  auto matches = tc::record_matches::create(record_a, record_b);
  tl::assert_equal(
    std::format("{}", matches),
    "record_matches[\nrecord_match[(source_match[0_a(p)])->b(Optional[p])],\nrecord_match[(source_match[0_b(f)])->c(f)]]"
  );
}

MODERNA_ADD_TEST(load_test) {
  auto file_c_a = rfl::json::read<std::map<std::string, tc::nameless_record>>(A_JSON_FILE).value();
  auto file_c_b = rfl::json::read<std::map<std::string, tc::nameless_record>>(B_JSON_FILE).value();
  auto rec_a_a = file_c_a["a"];
  auto rec_a_b = file_c_a["b"];
  auto rec_b_a = file_c_b["a"];
  auto rec_b_b = file_c_b["b"];
  auto matches_a = tc::record_matches::create(rec_a_a, rec_b_a);
  auto matches_b = tc::record_matches::create(rec_a_b, rec_b_b);
  tl::assert_equal(matches_a.is_complete(), true);
  tl::assert_equal(matches_b.is_complete(), true);
}

MODERNA_ADD_TEST(empty_match_test) {
  auto rec_a = tc::nameless_record::empty();
  auto rec_b = tc::nameless_record::empty();
  rec_b.add_record("b", rfl::json::read<tc::generic_type>(R"({"type" : "a"})").value());
  auto matches = tc::record_matches::create(rec_a, rec_b);
  tl::assert_equal(matches.is_empty(), true);
  tl::assert_equal(matches.is_partial(), false);
  tl::assert_equal(matches.is_complete(), false);
}

MODERNA_ADD_TEST(empty_empty_match_test) {
  auto rec_a = tc::nameless_record::empty();
  auto rec_b = tc::nameless_record::empty();
  auto matches = tc::record_matches::create(rec_a, rec_b);
  tl::assert_equal(matches.is_empty(), true);
  tl::assert_equal(matches.is_partial(), false);
  tl::assert_equal(matches.is_complete(), false);
}

MODERNA_ADD_TEST(match_with_multiple_test) {
  auto file_c_a = rfl::json::read<std::map<std::string, tc::nameless_record>>(A_JSON_FILE).value();
  auto file_c_b = rfl::json::read<std::map<std::string, tc::nameless_record>>(B_JSON_FILE).value();
  auto rec_a_a = file_c_a["a"];
  auto rec_a_b = file_c_a["c"];
  auto rec_b_c = file_c_b["c"];
  auto matches = tc::record_matches::create((rec_a_a + rec_a_b), rec_b_c);
  tl::assert_equal(matches.is_complete(), true);
  tl::assert_equal(matches.is_partial(), false);
  tl::assert_equal(matches.is_empty(), false);
}

MODERNA_ADD_TEST(partial_match_test) {
  auto rec_a = tc::nameless_record::empty();
  auto rec_b = tc::nameless_record::empty();
  rec_a.add_record("a", rfl::json::read<tc::generic_type>(R"({"type" : "a"})").value());
  rec_b.add_record("b", rfl::json::read<tc::generic_type>(R"({"type" : "a"})").value())
    .add_record("c", rfl::json::read<tc::generic_type>(R"({"type" : "c"})").value());
  auto matches = tc::record_matches::create(rec_a, rec_b);
  tl::assert_equal(matches.is_empty(), false);
  tl::assert_equal(matches.is_partial(), true);
  tl::assert_equal(matches.is_complete(), false);
}

MODERNA_ADD_TEST(match_output_empty_test) {
  auto rec_a = tc::nameless_record::empty();
  auto rec_b = tc::nameless_record::empty();
  rec_a.add_record("a", rfl::json::read<tc::generic_type>(R"({"type" : "a"})").value());
  rec_b.add_record("a", rfl::json::read<tc::generic_type>(R"({"type" : "b"})").value());
  auto matches = tc::record_matches::create(rec_a, rec_b);
  tl::assert_equal(matches.is_empty(), true);
  tl::assert_equal(matches.is_partial(), false);
  tl::assert_equal(matches.is_complete(), false);
}

MODERNA_ADD_TEST(compact_json_test) {
  auto source_r = tc::nameless_record::from_json(A_JSON_FILE_COMPACT).value();
  auto target_t = tc::nameless_record::from_json(B_JSON_FILE_COMPACT).value();
  auto matches = tc::record_matches::create(source_r, target_t);
  tl::assert_equal(matches.is_complete(), true);
  tl::assert_equal(matches.get_connectable().size(), 0);
}

MODERNA_ADD_TEST(compact_engine_json_test) {
  auto source_r = tc::nameless_record::from_json(A_JSON_FILE_NOT_COMPACT).value();
  auto target_t = tc::nameless_record::from_json(B_JSON_FILE_NOT_COMPACT).value();
  auto matches = tc::record_matches::create(source_r, target_t);
  tl::assert_equal(matches.is_complete(), true);
  tl::assert_equal(matches.get_connectable().size(), 0);
}