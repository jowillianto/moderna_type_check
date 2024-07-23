import sys
import pathlib
sys.path.append((pathlib.Path(__file__).parent.parent / 'build').__str__())
from moderna_type_check import NamelessRecord, RecordMatches

with open("../assets/type_a.json") as f:
  a = f.read()
with open("../assets/type_b.json") as f:
  b = f.read()

record_a = NamelessRecord.from_json(a)
record_b = NamelessRecord.from_json(b)
matches = RecordMatches.create(record_b, record_a)
print (record_a)
print (record_b)
print(matches.matches)
print(matches.is_complete())
for match in matches.matches:
  for source in match.sources:
    print(source)