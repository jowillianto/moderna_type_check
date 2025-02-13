from moderna.type_check import (
    GenericType,
    BasicType,
    ListType,
    UnionType,
    TupleType,
    OptionalType,
    VariadicType,
    NoneType,
    NamelessRecord,
    MultiNamelessRecord,
    RecordMatches,
    RecordMatch,
    SourceMatch,
    from_string,
)
from unittest import TestCase
import json


class FormatTests(TestCase):
    """
    Tests the type checker in a very basic way, most of the unit tests have been done in the C++
    side of things.
    """

    def test_basic_format(self):
        x = from_string(
            "Union[ABC_CBA_BAC, Variadic[ABC_CBA_BAC], List[Union[A, Variadic[List[Union[A, B]]]]]]]"
        )
        self.assertEqual(
            str(x),
            "Union[ABC_CBA_BAC, Variadic[ABC_CBA_BAC], List[Union[A, Variadic[List[Union[A, B]]]]]]",
        )


class EngineTests(TestCase):
    """
    Tests the type checker in a very basic way, most of the unit tests have been done in the C++
    side of things.
    """

    def test_engine(self):
        source_rec = (
            NamelessRecord()
            .add_record("a", GenericType(BasicType("p")))
            .add_record("b", GenericType(BasicType("f")))
        )
        target_rec = (
            NamelessRecord()
            .add_record("b", GenericType(OptionalType(GenericType(BasicType("p")))))
            .add_record("c", GenericType(BasicType("f")))
        )
        matches = RecordMatches.create(source_rec, target_rec)
        self.assertTrue(matches.is_complete())
        self.assertEqual(matches.size(), 2)

    def test_engine_double_now(self):
        source_rec_1 = (
            NamelessRecord()
            .add_record("a", GenericType(BasicType("a")))
            .add_record("b", GenericType(BasicType("b")))
        )
        source_rec_2 = (
            NamelessRecord()
            .add_record("c", GenericType(BasicType("c")))
            .add_record("d", GenericType(BasicType("d")))
        )
        target_rec = (
            NamelessRecord()
            .add_record("a", GenericType(BasicType("a")))
            .add_record("b", GenericType(BasicType("b")))
            .add_record("c", GenericType(BasicType("c")))
        )
        matches = RecordMatches.create(source_rec_1 + source_rec_2, target_rec)
        self.assertTrue(matches.is_complete())

    def test_engine_double(self):
        source_rec_1 = (
            NamelessRecord()
            .add_record("a", GenericType(BasicType("a")))
            .add_record("b", GenericType(BasicType("b")))
        )
        target_rec = (
            NamelessRecord()
            .add_record("a", GenericType(BasicType("a")))
            .add_record("b", GenericType(BasicType("b")))
            .add_record("c", GenericType(BasicType("c")))
        )
        matches = RecordMatches.create(source_rec_1, target_rec)
        connectable = matches.get_connectable()
        self.assertEqual(len(connectable), 1)
        self.assertEqual(connectable[0].type, GenericType(BasicType("c")))
        self.assertEqual(connectable[0].target, "c")

    def test_engine_from_json(self):
        source_x = {"a": "c", "b": "List[Tuple[a, b, c, d]]", "c": "Optional[a_b]"}
        target_x = {
            "a": "Optional[c]",
            "b": "Optional[List[Tuple[a, b, c, d]]]",
            "c": "Optional[Optional[a_b]]",
        }
        source_rec = NamelessRecord.from_json(json.dumps(source_x))
        target_rec = NamelessRecord.from_json(json.dumps(target_x))
        matches = RecordMatches.create(source_rec, target_rec)
        self.assertTrue(matches.is_complete())
        self.assertEqual(matches.size(), 3)
    
    def test_engine_from_json2(self):
        source_x = {
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
        }
        target_x = {
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
        }
        source_rec = NamelessRecord.from_json(json.dumps(source_x))
        target_rec = NamelessRecord.from_json(json.dumps(target_x))
        matches = RecordMatches.create(source_rec, target_rec)
        self.assertTrue(matches.is_complete())
        self.assertEqual(matches.size(), 3)