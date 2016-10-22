binding.addGraph("", "graph", 72.0, 206.0)

print "Before removing graph"
binding_exec = binding.binding.getExec()
for i in range(binding_exec.getNodeCount()):
  print binding_exec.getNodeName(i)

binding.removeNodes("", "graph")

print "After removing graph"
for i in range(binding_exec.getNodeCount()):
  print binding_exec.getNodeName(i)