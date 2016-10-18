binding.addGraph("", "graph", 64.0, 428.0)

binding_exec = binding.binding.getExec()
for i in range(binding_exec.getNodeCount()):
  print binding_exec.getNodeName(i)

binding.editNode("", "graph", "graph_New", "{}", "{\n  \"uiTooltip\" : \"\",\n  \"uiDocUrl\" : \"\",\n  \"uiNodeColor\" : \"{\\n  \\\"r\\\" : 121,\\n  \\\"g\\\" : 134,\\n  \\\"b\\\" : 143\\n  }\",\n  \"uiTextColor\" : \"{\\n  \\\"r\\\" : 20,\\n  \\\"g\\\" : 20,\\n  \\\"b\\\" : 20\\n  }\",\n  \"uiHeaderColor\" : \"\"\n  }")
for i in range(binding_exec.getNodeCount()):
  print binding_exec.getNodeName(i)
