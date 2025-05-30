from __future__ import annotations
from typing import List, Union, Optional, Iterator, Tuple

class GenericType:
    def __init__(
        self,
        t: Union[
            BasicType,
            ListType,
            OptionalType,
            UnionType,
            TupleType,
            VariadicType,
            NoneType,
        ],
    ): ...
    def is_basic_type(self) -> bool: ...
    def is_list_type(self) -> bool: ...
    def is_optional_type(self) -> bool: ...
    def is_union_type(self) -> bool: ...
    def is_tuple_type(self) -> bool: ...
    def is_variadic_type(self) -> bool: ...
    def is_none_type(self) -> bool: ...
    def as_basic_type(self) -> BasicType: ...
    def as_list_type(self) -> ListType: ...
    def as_optional_type(self) -> OptionalType: ...
    def as_union_type(self) -> UnionType: ...
    def as_tuple_type(self) -> TupleType: ...
    def as_variadic_type(self) -> VariadicType: ...
    def as_none_type(self) -> NoneType: ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def __eq__(self, other: GenericType) -> bool: ...
    def allow_empty(self) -> bool: ...
    def allow_multiple(self) -> bool: ...
    def clone(self) -> GenericType: ...
    @staticmethod
    def from_json(json: str) -> GenericType: ...

class BasicType:
    type: str
    def __init__(self, name: str): ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def allow_empty(self) -> bool: ...
    def allow_multiple(self) -> bool: ...

class ListType:
    child: GenericType
    def __init__(self, child: GenericType): ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def allow_empty(self) -> bool: ...
    def allow_multiple(self) -> bool: ...

class UnionType:
    child: List[GenericType]
    def __init__(self, child: List[GenericType]): ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def allow_empty(self) -> bool: ...
    def allow_multiple(self) -> bool: ...

class TupleType:
    child: List[GenericType]
    def __init__(self, child: List[GenericType]): ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def allow_empty(self) -> bool: ...
    def allow_multiple(self) -> bool: ...

class OptionalType:
    child: GenericType
    def __init__(self, child: GenericType): ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def allow_empty(self) -> bool: ...
    def allow_multiple(self) -> bool: ...

class VariadicType:
    child: GenericType
    def __init__(self, child: GenericType): ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def allow_empty(self) -> bool: ...
    def allow_multiple(self) -> bool: ...

class NoneType:
    def __init__(self): ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def allow_empty(self) -> bool: ...
    def allow_multiple(self) -> bool: ...

class NamelessRecord:
    def __init__(self): ...
    def add_record(self, name: str, record: GenericType) -> NamelessRecord: ...
    def to_multiple(self) -> MultiNamelessRecord: ...
    def get(self, name: str) -> Optional[GenericType]: ...
    def __add__(
        self, other: Union[NamelessRecord, MultiNamelessRecord]
    ) -> MultiNamelessRecord: ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def __iter__(self) -> Iterator[Tuple[str, GenericType]]: ...
    def size(self) -> int: ...
    @staticmethod
    def from_json(json: str) -> NamelessRecord: ...

class MultiNamelessRecord:
    def __init__(self): ...
    def add_record(self, record: NamelessRecord) -> MultiNamelessRecord: ...
    def __add__(self, other: NamelessRecord) -> MultiNamelessRecord: ...
    def __iter__(self) -> Iterator[NamelessRecord]: ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...

class RecordMatches:
    @staticmethod
    def create(
        record: Union[MultiNamelessRecord, NamelessRecord],
        target: NamelessRecord,
    ) -> RecordMatches: ...
    def get_connectable(self) -> List[RecordMatch]: ...
    def get_match(self, target: str) -> RecordMatch: ...
    def is_complete(self) -> bool: ...
    def is_partial(self) -> bool: ...
    def is_empty(self) -> bool: ...
    def __iter__(self) -> Iterator[RecordMatch]: ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def size(self) -> int: ...

class RecordMatch:
    type: GenericType
    target: str
    sources: List[SourceMatch]
    def __iter__(self) -> Iterator[SourceMatch]: ...
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...
    def is_empty(self) -> bool: ...
    def is_not_empty(self) -> bool: ...
    def is_connectable(self) -> bool: ...
    def first(self) -> Optional[SourceMatch]: ...

class SourceMatch:
    source: GenericType
    name: str
    id: int
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...

def from_string(v: str) -> GenericType: ...
def is_connectable(l: GenericType, r: GenericType) -> bool: ...
