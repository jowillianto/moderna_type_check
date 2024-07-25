// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
declare namespace RuntimeExports {
    let HEAPF32: any;
    let HEAPF64: any;
    let HEAP_DATA_VIEW: any;
    let HEAP8: any;
    let HEAPU8: any;
    let HEAP16: any;
    let HEAPU16: any;
    let HEAP32: any;
    let HEAPU32: any;
    let HEAP64: any;
    let HEAPU64: any;
}
interface WasmModule {
}

type EmbindString = ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string;
export interface RecordMatchArray {
  size(): number;
  get(_0: number): RecordMatch | undefined;
  push_back(_0: RecordMatch): void;
  resize(_0: number, _1: RecordMatch): void;
  set(_0: number, _1: RecordMatch): boolean;
  delete(): void;
}

export interface SourceMatchArray {
  size(): number;
  get(_0: number): SourceMatch | undefined;
  push_back(_0: SourceMatch): void;
  resize(_0: number, _1: SourceMatch): void;
  set(_0: number, _1: SourceMatch): boolean;
  delete(): void;
}

export interface StringArray {
  push_back(_0: EmbindString): void;
  resize(_0: number, _1: EmbindString): void;
  size(): number;
  get(_0: number): EmbindString | undefined;
  set(_0: number, _1: EmbindString): boolean;
  delete(): void;
}

export interface StringRecord {
  size(): number;
  get(_0: EmbindString): EmbindString | undefined;
  set(_0: EmbindString, _1: EmbindString): void;
  keys(): StringArray;
  delete(): void;
}

export interface GenericType {
  name(): string;
  to_string(): string;
  delete(): void;
}

export interface NamelessRecord {
  add_record(_0: EmbindString, _1: GenericType): NamelessRecord;
  get_name_pairs(): StringRecord;
  size(): number;
  is_empty(): boolean;
  to_string(): string;
  to_multiple(): MultiNamelessRecord;
  add(_0: NamelessRecord): MultiNamelessRecord;
  delete(): void;
}

export interface MultiNamelessRecord {
  add(_0: NamelessRecord): MultiNamelessRecord;
  to_string(): string;
  delete(): void;
}

export interface SourceMatch {
  readonly type: EmbindString;
  id: number;
  readonly name: EmbindString;
  to_string(): string;
  delete(): void;
}

export interface RecordMatch {
  sources: SourceMatchArray;
  readonly target: EmbindString;
  is_empty(): boolean;
  type(): string;
  to_string(): string;
  size(): number;
  delete(): void;
}

export interface RecordMatches {
  matches: RecordMatchArray;
  get_match(_0: EmbindString): RecordMatch;
  is_complete(): boolean;
  is_empty(): boolean;
  is_partial(): boolean;
  to_string(): string;
  delete(): void;
}

interface EmbindModule {
  RecordMatchArray: {new(): RecordMatchArray};
  SourceMatchArray: {new(): SourceMatchArray};
  StringArray: {new(): StringArray};
  StringRecord: {new(): StringRecord};
  GenericType: {from_json(_0: EmbindString): GenericType};
  NamelessRecord: {make_empty(): NamelessRecord; from_json(_0: EmbindString): NamelessRecord};
  MultiNamelessRecord: {make_empty(): MultiNamelessRecord};
  SourceMatch: {};
  RecordMatch: {};
  RecordMatches: {create(_0: MultiNamelessRecord, _1: NamelessRecord): RecordMatches};
}

export type MainModule = WasmModule & typeof RuntimeExports & EmbindModule;
