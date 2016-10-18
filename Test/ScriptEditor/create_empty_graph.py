binding.addGraph("", "graph", 30.0, 383.0)
# Result: graph
binding_exec = binding.binding.getExec()
for i in range(binding_exec.getNodeCount()):
  print binding_exec.getNodeName(i)