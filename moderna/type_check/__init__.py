from __future__ import annotations
from .moderna_type_check import (
    NamelessRecord as nameless_record,
    MultiNamelessRecord as multi_nameless_record,
    SourceMatch as source_match,
    RecordMatch as record_match,
    RecordMatches as record_matches,
    GenericType as generic_type,
)
from typing_extensions import Callable, Dict, List, Union


class GenericType(generic_type):
    name: Callable[[], str]
    from_json: Callable[[str], "GenericType"]
    allow_multiple : Callable[[], bool]
    allow_empty: Callable[[], bool]


class NamelessRecord(nameless_record):
    from_json: Callable[[str], "NamelessRecord"]
    make_empty: Callable[[], "NamelessRecord"]
    add_record: Callable[[str, GenericType], "NamelessRecord"]
    to_multiple: Callable[[], "MultiNamelessRecord"]
    get_name_pairs: Callable[[], Dict[str, str]]
    size: Callable[[], int]
    __len__: Callable[[], int]
    is_empty: Callable[[], bool]
    __add__: Callable[["NamelessRecord"], "MultiNamelessRecord"]
    __str__: Callable[[], str]
    __repr__: Callable[[], str]


class MultiNamelessRecord(multi_nameless_record):
    make_empty: Callable[[], "MultiNamelessRecord"]
    __add__: Callable[["NamelessRecord"], "MultiNamelessRecord"]
    __str__: Callable[[], str]
    __repr__: Callable[[], str]


class SourceMatch(source_match):
    type: GenericType
    id: int
    name: str
    __str__: Callable[[], str]
    __repr__: Callable[[], str]


class RecordMatch(record_match):
    is_empty: Callable[[], bool]
    type: GenericType
    sources: List[SourceMatch]
    target: str
    __str__: Callable[[], str]
    __repr__: Callable[[], str]
    size: int


class RecordMatches(record_matches):
    matches: List[RecordMatch]
    create: Callable[
        [Union["MultiNamelessRecord", "NamelessRecord"], "NamelessRecord"],
        "RecordMatches",
    ]
    get_match: Callable[[str], RecordMatch]
    is_complete: Callable[[], bool]
    is_partial: Callable[[], bool]
    is_empty: Callable[[], bool]
    __str__: Callable[[], str]
    __repr__: Callable[[], str]


__all__ = [
    "NamelessRecord",
    "MultiNamelessRecord",
    "SourceMatch",
    "RecordMatch",
    "RecordMatches",
    "GenericType",
]
