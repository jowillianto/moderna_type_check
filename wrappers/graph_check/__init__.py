from __future__ import annotations
from moderna_type_check import (
    NamelessRecord as nameless_record,
    MultiNamelessRecord as multi_nameless_record,
    SourceMatch as source_match,
    RecordMatch as record_match,
    RecordMatches as record_matches
)
from typing import List, Callable, Union

class NamelessRecord(nameless_record):
  from_json : Callable[[str], NamelessRecord]
  to_multiple: Callable[[], MultiNamelessRecord]
  __add__ : Callable[[NamelessRecord], MultiNamelessRecord]
  __str__ : str
  __repr__ : str

class MultiNamelessRecord(multi_nameless_record):
  __add__ : Callable[[NamelessRecord], MultiNamelessRecord]
  __str__ : str
  __repr__ : str

class SourceMatch(source_match):
  type : str
  id : int
  name : str
  __str__ : str
  __repr__ : str

class RecordMatch(record_match):
  is_empty : bool
  type : str
  sources : List[SourceMatch]
  size : int
  __str__ : str
  __repr__ : str

class RecordMatches(record_matches):
  create : Callable[[Union[MultiNamelessRecord, NamelessRecord], NamelessRecord], RecordMatches]
  get_match : Callable[[str], RecordMatch]
  is_complete : Callable[[], bool]
  is_empty : Callable[[], bool]
  is_partial : Callable[[], bool]
  matches : List[RecordMatch]
  sources : MultiNamelessRecord
  target : MultiNamelessRecord
  __str__ : str
  __repr__ : str