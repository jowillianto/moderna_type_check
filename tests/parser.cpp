import moderna.test_lib;
import moderna.type_check;
#include <rfl/Box.hpp>
#include <array>
#include <format>
#include <memory>
#include <print>

namespace tc = moderna::type_check;
namespace tl = moderna::test_lib;

constexpr static auto test_stmt = std::array{
  "A",
  "Any",
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
  "Union[ABC_CBA_BAC, Variadic[ABC_CBA_BAC], List[Union[A, Variadic[List[Union[A, B]]]]]]]"
};

static auto test_answer = std::array{
  tc::generic_type{tc::generic_type::basic_type{"A"}},
  tc::generic_type{tc::generic_type::any_type{std::string{"Any"}}},
  tc::generic_type{tc::generic_type::list_type{
    std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"A"})
  }},
  tc::generic_type{tc::generic_type::optional_type{
    std::string{"Optional"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"A"})
  }},
  tc::generic_type{tc::generic_type::union_type{
    std::string{"Union"},
    std::vector{
      tc::generic_type{tc::generic_type::basic_type{"A"}},
      tc::generic_type{tc::generic_type::basic_type{"B"}}
    }
  }},
  tc::generic_type{tc::generic_type::tuple_type{
    std::string{"Tuple"},
    std::vector{
      tc::generic_type{tc::generic_type::basic_type{"A"}},
      tc::generic_type{tc::generic_type::basic_type{"B"}}
    }
  }},
  tc::generic_type{tc::generic_type::variadic_type{
    std::string{"Variadic"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"A"})
  }},
  tc::generic_type{tc::generic_type::basic_type{"B"}},
  tc::generic_type{tc::generic_type::list_type{
    std::string{"List"},
    rfl::make_box<tc::generic_type>(tc::generic_type::optional_type{
      std::string{"Optional"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"A"})
    })
  }},
  tc::generic_type{tc::generic_type::list_type{
    std::string{"List"},
    rfl::make_box<tc::generic_type>(tc::generic_type::union_type{
      std::string{"Union"},
      std::vector{
        tc::generic_type{tc::generic_type::basic_type{"A"}},
        tc::generic_type{tc::generic_type::basic_type{"B"}}
      }
    })
  }},
  tc::generic_type{tc::generic_type::optional_type{
    std::string{"Optional"},
    rfl::make_box<tc::generic_type>(tc::generic_type::union_type{
      std::string{"Union"},
      std::vector{
        tc::generic_type{tc::generic_type::basic_type{"A"}},
        tc::generic_type{tc::generic_type::basic_type{"B"}}
      }
    })
  }},
  tc::generic_type{tc::generic_type::list_type{
    std::string{"List"},
    rfl::make_box<tc::generic_type>(tc::generic_type::optional_type{
      std::string{"Optional"},
      rfl::make_box<tc::generic_type>(tc::generic_type::union_type{
        std::string{"Union"},
        std::vector{
          tc::generic_type{tc::generic_type::basic_type{"A"}},
          tc::generic_type{tc::generic_type::basic_type{"B"}}
        }
      })
    })
  }},
  tc::generic_type{tc::generic_type::variadic_type{
    std::string{"Variadic"},
    rfl::make_box<tc::generic_type>(tc::generic_type::list_type{
      std::string{"List"},
      rfl::make_box<tc::generic_type>(tc::generic_type::optional_type{
        std::string{"Optional"},
        rfl::make_box<tc::generic_type>(tc::generic_type::union_type{
          std::string{"Union"},
          std::vector{
            tc::generic_type{tc::generic_type::basic_type{"A"}},
            tc::generic_type{tc::generic_type::basic_type{"B"}}
          }
        })
      })
    })
  }},
  tc::generic_type{tc::generic_type::tuple_type{
    std::string{"Tuple"},
    std::vector{
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"A"})
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"B"})
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"C"})
      }}
    }
  }},
  tc::generic_type{tc::generic_type::tuple_type{
    std::string{"Tuple"},
    std::vector{
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"A"})
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"B"})
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"C"})
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"D"})
      }}
    }
  }},
  tc::generic_type{tc::generic_type::union_type{
    std::string{"Union"},
    std::vector{
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"A"})
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"B"})
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"C"})
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"}, rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"D"})
      }}
    }
  }},
  tc::generic_type{tc::generic_type::union_type{
    std::string{"Union"},
    std::vector{
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"},
        rfl::make_box<tc::generic_type>(tc::generic_type::optional_type{
          std::string{"Optional"},
          rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"A"})
        })
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"},
        rfl::make_box<tc::generic_type>(tc::generic_type::optional_type{
          std::string{"Optional"},
          rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"B"})
        })
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"},
        rfl::make_box<tc::generic_type>(tc::generic_type::optional_type{
          std::string{"Optional"},
          rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"C"})
        })
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"},
        rfl::make_box<tc::generic_type>(tc::generic_type::optional_type{
          std::string{"Optional"},
          rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"D"})
        })
      }}
    }
  }},
  tc::generic_type{tc::generic_type::variadic_type{
    std::string{"Variadic"},
    rfl::make_box<tc::generic_type>(tc::generic_type::list_type{
      std::string{"List"},
      rfl::make_box<tc::generic_type>(tc::generic_type::union_type{
        std::string{"Union"},
        std::vector{
          tc::generic_type{tc::generic_type::basic_type{"A"}},
          tc::generic_type{tc::generic_type::variadic_type{
            std::string{"Variadic"},
            rfl::make_box<tc::generic_type>(tc::generic_type::list_type{
              std::string{"List"},
              rfl::make_box<tc::generic_type>(tc::generic_type::union_type{
                std::string{"Union"},
                std::vector{
                  tc::generic_type{tc::generic_type::basic_type{"A"}},
                  tc::generic_type{tc::generic_type::basic_type{"B"}}
                }
              })
            })
          }}
        }
      })
    })
  }},
  tc::generic_type{tc::generic_type::optional_type{
    std::string{"Optional"},
    rfl::make_box<tc::generic_type>(tc::generic_type::union_type{
      std::string{"Union"},
      std::vector{
        tc::generic_type{tc::generic_type::basic_type{"A"}},
        tc::generic_type{tc::generic_type::basic_type{"B"}},
        tc::generic_type{tc::generic_type::basic_type{"C"}},
        tc::generic_type{tc::generic_type::basic_type{"D"}},
        tc::generic_type{tc::generic_type::basic_type{"E"}},
        tc::generic_type{tc::generic_type::basic_type{"F"}},
        tc::generic_type{tc::generic_type::basic_type{"G"}},
        tc::generic_type{tc::generic_type::basic_type{"H"}},
        tc::generic_type{tc::generic_type::basic_type{"I"}},
        tc::generic_type{tc::generic_type::basic_type{"J"}},
        tc::generic_type{tc::generic_type::basic_type{"K"}},
        tc::generic_type{tc::generic_type::basic_type{"L"}},
        tc::generic_type{tc::generic_type::basic_type{"M"}},
        tc::generic_type{tc::generic_type::basic_type{"N"}},
        tc::generic_type{tc::generic_type::basic_type{"O"}},
        tc::generic_type{tc::generic_type::basic_type{"P"}},
        tc::generic_type{tc::generic_type::basic_type{"Q"}},
        tc::generic_type{tc::generic_type::basic_type{"R"}},
        tc::generic_type{tc::generic_type::basic_type{"S"}},
        tc::generic_type{tc::generic_type::basic_type{"T"}},
        tc::generic_type{tc::generic_type::basic_type{"U"}},
        tc::generic_type{tc::generic_type::basic_type{"V"}},
        tc::generic_type{tc::generic_type::basic_type{"W"}},
        tc::generic_type{tc::generic_type::basic_type{"X"}},
        tc::generic_type{tc::generic_type::basic_type{"Y"}},
        tc::generic_type{tc::generic_type::basic_type{"Z"}}
      }
    })
  }},
  tc::generic_type{tc::generic_type::tuple_type{
    std::string{"Tuple"},
    std::vector{
      tc::generic_type{tc::generic_type::basic_type{"A_BC"}},
      tc::generic_type{tc::generic_type::basic_type{"BC_A"}}
    }
  }},
  tc::generic_type{tc::generic_type::tuple_type{
    std::string{"Tuple"},
    std::vector{
      tc::generic_type{tc::generic_type::basic_type{"ABC_CBA_BAC"}},
      tc::generic_type{tc::generic_type::optional_type{
        std::string{"Optional"},
        rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"ABC_CBA_BAC"})
      }}
    }
  }},
  tc::generic_type{tc::generic_type::tuple_type{
    std::string{"Tuple"},
    std::vector{
      tc::generic_type{tc::generic_type::basic_type{"ABC_CBA_BAC"}},
      tc::generic_type{tc::generic_type::optional_type{
        std::string{"Optional"},
        rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"ABC_CBA_BAC"})
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"},
        rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"ABC_CBA_BAC"})
      }}
    }
  }},
  tc::generic_type{tc::generic_type::union_type{
    std::string{"Union"},
    std::vector{
      tc::generic_type{tc::generic_type::basic_type{"ABC_CBA_BAC"}},
      tc::generic_type{tc::generic_type::variadic_type{
        std::string{"Variadic"},
        rfl::make_box<tc::generic_type>(tc::generic_type::basic_type{"ABC_CBA_BAC"})
      }},
      tc::generic_type{tc::generic_type::list_type{
        std::string{"List"},
        rfl::make_box<tc::generic_type>(tc::generic_type::union_type{
          std::string{"Union"},
          std::vector{
            tc::generic_type{tc::generic_type::basic_type{"A"}},
            tc::generic_type{tc::generic_type::variadic_type{
              std::string{"Variadic"},
              rfl::make_box<tc::generic_type>(tc::generic_type::list_type{
                std::string{"List"},
                rfl::make_box<tc::generic_type>(tc::generic_type::union_type{
                  std::string{"Union"},
                  std::vector{
                    tc::generic_type{tc::generic_type::basic_type{"A"}},
                    tc::generic_type{tc::generic_type::basic_type{"B"}}
                  }
                })
              })
            }}
          }
        })
      }}
    }
  }}
};

static_assert(test_stmt.size() == test_answer.size());

template <size_t i> struct run_ith_test {
  void operator()() const {
    auto stmt = test_stmt[i];
    auto gen_type = tc::from_string(stmt);
    if (!gen_type) {
      throw std::runtime_error(std::format("{}", gen_type.error()));
    } else {
      if (gen_type.value() != test_answer[i]) {
        throw std::runtime_error(
          std::format("Expected: {}\nGot: {}", test_answer[i].name(), gen_type.value().name())
        );
      }
    }
  }
};

template <size_t i = 0> static auto test_maker(auto prev_test) {
  if constexpr (i == test_stmt.size()) {
    return prev_test;
  } else {
    return test_maker<i + 1>(prev_test.add_test(test_stmt.at(i), run_ith_test<i>()));
  }
}

int main() {
  test_maker(tl::make_tester("Parser Test")).print_or_exit();
}