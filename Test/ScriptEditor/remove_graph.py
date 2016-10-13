binding.addGraph("", "graph", 72.0, 206.0)

binding_exec = binding.binding.getExec()
for i in range(binding_exec.getNodeCount()):
  print binding_exec.getNodeName(i)

binding.removeNodes("", "graph")
for i in range(binding_exec.getNodeCount()):
  print binding_exec.getNodeName(i)