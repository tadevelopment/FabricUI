ex = binding.binding.getExec()

# Rename a function with an empty name
binding.addFunc("", "customFunc", "dfgEntry {\n  // result = a + b;\n}\n", 496.0, 262.0)
binding.editNode("", "customFunc", "", "{}", "{\n  \"uiTooltip\" : \"\",\n  \"uiDocUrl\" : \"\",\n  \"uiNodeColor\" : \"{\\n  \\\"r\\\" : 121,\\n  \\\"g\\\" : 134,\\n  \\\"b\\\" : 143\\n  }\",\n  \"uiTextColor\" : \"{\\n  \\\"r\\\" : 20,\\n  \\\"g\\\" : 20,\\n  \\\"b\\\" : 20\\n  }\",\n  \"uiHeaderColor\" : \"\"\n  }")

print ex.getNodeName(0) # Should maintain the original name

# Rename a function
binding.editNode("", ex.getNodeName(0), "secondFuncName", "{}", "{\n  \"uiTooltip\" : \"\",\n  \"uiDocUrl\" : \"\",\n  \"uiNodeColor\" : \"{\\n  \\\"r\\\" : 121,\\n  \\\"g\\\" : 134,\\n  \\\"b\\\" : 143\\n  }\",\n  \"uiTextColor\" : \"{\\n  \\\"r\\\" : 20,\\n  \\\"g\\\" : 20,\\n  \\\"b\\\" : 20\\n  }\",\n  \"uiHeaderColor\" : \"\"\n  }")

print ex.getInstTitle( "secondFuncName" ) # Should also be renamed ?

# Create two functions with the same name in the same graph /subgraph
binding.addFunc("", "sameName", "dfgEntry {\n  // result = a + b;\n}\n", 560.0, 70.0)
binding.addFunc("", "sameName", "dfgEntry {\n  // result = a + b;\n}\n", 816.0, 110.0)

print ex.getNodeName(2) # The second should be renamed
