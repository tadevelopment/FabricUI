binding.addGraph("", "graph", 888.0, 196.0)
# Result: graph
binding.addPort("graph", "a", "in", "Float32", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: a
binding.addPort("graph", "b", "in", "String", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: b
binding.addPort("graph", "c", "in", "Vec2", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: c
binding.addPort("graph", "d", "in", "Vec3", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: d
binding.addPort("graph", "e", "in", "Color", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: e
binding.addPort("graph", "f", "in", "Mat44", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: f
binding.addPort("graph", "g", "in", "Xfo", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: g
binding.addPort("graph", "h", "in", "Integer", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: h
binding.setPortDefaultValue("", "graph.a", "Float32", "2")
binding.setPortDefaultValue("", "graph.b", "String", "\"test\"")
binding.setPortDefaultValue("", "graph.c", "Vec2", "{\n  \"x\" : 1,\n  \"y\" : 0\n  }")
binding.setPortDefaultValue("", "graph.c", "Vec2", "{\n  \"x\" : 2,\n  \"y\" : 0\n  }")
binding.setPortDefaultValue("", "graph.d", "Vec3", "{\n  \"x\" : 0,\n  \"y\" : 2,\n  \"z\" : 0\n  }")
binding.setPortDefaultValue("", "graph.e", "Color", "{\n  \"r\" : 0,\n  \"g\" : 0,\n  \"b\" : 0,\n  \"a\" : 1\n  }")
binding.setPortDefaultValue("", "graph.f", "Mat44", "{\n  \"row0\" : {\n    \"x\" : 2,\n    \"y\" : 0,\n    \"z\" : 0,\n    \"t\" : 0\n    },\n  \"row1\" : {\n    \"x\" : 0,\n    \"y\" : 1,\n    \"z\" : 0,\n    \"t\" : 0\n    },\n  \"row2\" : {\n    \"x\" : 0,\n    \"y\" : 0,\n    \"z\" : 1,\n    \"t\" : 0\n    },\n  \"row3\" : {\n    \"x\" : 0,\n    \"y\" : 0,\n    \"z\" : 0,\n    \"t\" : 1\n    }\n  }")
binding.setPortDefaultValue("", "graph.g", "Xfo", "{\n  \"ori\" : {\n    \"v\" : {\n      \"x\" : 0,\n      \"y\" : 0,\n      \"z\" : 0\n      },\n    \"w\" : 1\n    },\n  \"tr\" : {\n    \"x\" : -1,\n    \"y\" : 0,\n    \"z\" : 0\n    },\n  \"sc\" : {\n    \"x\" : 1,\n    \"y\" : 1,\n    \"z\" : 1\n    }\n  }")
binding.setPortDefaultValue("", "graph.h", "Integer", "1")
binding_exec = binding.binding.getExec()
print binding_exec.getPortDefaultValue("graph.a", "Float32")
print binding_exec.getPortDefaultValue("graph.b", "String")
print binding_exec.getPortDefaultValue("graph.c", "Vec2")
print binding_exec.getPortDefaultValue("graph.d", "Vec3")
print binding_exec.getPortDefaultValue("graph.e", "Color")
print binding_exec.getPortDefaultValue("graph.f", "Mat44")
print binding_exec.getPortDefaultValue("graph.g", "Xfo")
print binding_exec.getPortDefaultValue("graph.h", "Integer")

