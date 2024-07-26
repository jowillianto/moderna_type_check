from moderna.type_check import NamelessRecord, RecordMatches, GenericType
from unittest import TestCase


class TypeCheckTests(TestCase):
    def test_match_with_empty(self):
        rec_a = NamelessRecord.make_empty()
        rec_b = NamelessRecord.make_empty()
        rec_b.add_record("b", GenericType.from_json('{"type": "b"}'))
        matches = RecordMatches.create(rec_a, rec_b)
        self.assertEqual(matches.is_empty(), True)
        self.assertEqual(matches.is_partial(), False)
        self.assertEqual(matches.is_complete(), False)

    def test_empty_empty(self):
        rec_a = NamelessRecord.make_empty()
        rec_b = NamelessRecord.make_empty()
        matches = RecordMatches.create(rec_a, rec_b)
        self.assertEqual(matches.is_empty(), True)
        self.assertEqual(matches.is_partial(), False)
        self.assertEqual(matches.is_complete(), False)

    def test_partial_match(self):
        rec_a = NamelessRecord.make_empty()
        rec_b = NamelessRecord.make_empty()
        rec_a.add_record("a", GenericType.from_json('{"type": "a"}'))
        rec_b.add_record("b", GenericType.from_json('{"type": "a"}')).add_record(
            "c", GenericType.from_json('{"type": "c"}')
        )
        matches = RecordMatches.create(rec_a, rec_b)
        self.assertEqual(matches.is_empty(), False)
        self.assertEqual(matches.is_partial(), True)
        self.assertEqual(matches.is_complete(), False)
