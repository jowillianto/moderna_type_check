import sys
import pathlib
sys.path.append((pathlib.Path(__file__).parent.parent / 'wrappers').__str__())
sys.path.append((pathlib.Path(__file__).parent.parent / 'build').__str__())
from graph_check import NamelessRecord, RecordMatches
import json

asset_fd = pathlib.Path(__file__).parent.parent / 'assets'

with open(asset_fd / 'type_a.json', 'r') as f:
  x = json.load(f)
  record_a = NamelessRecord.from_json(json.dumps(x['b']))

with open(asset_fd / 'type_b.json', 'r') as f:
    x = json.load(f)
    record_b = NamelessRecord.from_json(json.dumps(x['b']))

matches = RecordMatches.create(record_a, record_b)