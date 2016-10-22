binding.addGraph("", "graph", 72.0, 206.0)
binding.addGraph("", "graph", 1036.0, 66.0)
binding.addGraph("", "graph", 1164.0, 43.0)

print "Before removing graphs"
binding_exec = binding.binding.getExec()
for i in range(binding_exec.getNodeCount()):
  print binding_exec.getNodeName(i)

binding.removeNodes("", "graph|graph_2|graph_3")

print "After removing graphs"
for i in range(binding_exec.getNodeCount()):
  print binding_exec.getNodeName(i)

