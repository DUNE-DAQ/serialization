// The schema used by NetworkObjectSender

local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local s = moo.oschema.schema("dunedaq.serialization.networkobjectsender");

// Object structure used by NetworkObjectSender
local nos = {

  stype: s.string("SerializationString", doc="String describing serialization type"),
  
  ipmtype: s.string("IPMPluginType", doc="IPM plugin type"),
  
  address: s.string("Address", doc="Address to send to"),
  
  conf: s.record("Conf",  [
    s.field("stype", self.stype, "json",
      doc="Serialization type"),
    s.field("ipm_plugin_type", self.ipmtype, "ZmqSender",
      doc="IPM plugin type"),
    s.field("address", self.address, "inproc://default",
      doc="Address to send to")
  ], doc="NetworkObjectSender Configuration"),
  
};

moo.oschema.sort_select(nos)

