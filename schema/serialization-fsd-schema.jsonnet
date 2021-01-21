// The schema used by classes in the appfwk code tests.
//
// It is an example of the lowest layer schema below that of the "cmd"
// and "app" and which defines the final command object structure as
// consumed by instances of specific DAQModule implementations (ie,
// the test/Fake* modules).

local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local s = moo.oschema.schema("dunedaq.serialization.fsd");

// Object structure used by the test/fake producer module
local fsd = {

  fakeness: s.enum("Fakeness",
    ["Unknown", "Fake", "SuperFake"],
    doc="The amount of fakeness"),

  count : s.number("Count", "i4",
    doc="A count of not too many things"),

  timestamp : s.number("Timestamp", "i8",
    doc="A fake timestamp"),
  
  fakedata: s.record("FakeData", [
    s.field("fake_count", self.count, -4,
      doc="A fake count of something"),
  ], doc="Fake Serializable data"),

  fakedatas: s.sequence("FakeDatas", self.fakedata,
    doc="A sequence of FakeData"),
  
  fakedata2: s.record("AnotherFakeData", [
    s.field("fake_count", self.count, -4,
      doc="A fake count of something"),
    s.field("fake_timestamp", self.timestamp, 0,
      doc="A fake timestamp for the data"),
    s.field("fake_datas", self.fakedatas),
    s.field("fakeness", self.fakeness)
  ], doc="Another fake Serializable data"),

};

moo.oschema.sort_select(fsd)

