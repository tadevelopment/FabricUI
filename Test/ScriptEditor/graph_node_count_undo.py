binding.addGraph("", "graph", 867.0, 218.0)
binding.addGraph("", "graph1", 1100.0, 218.0)
binding_exec = binding.binding.getExec()
# It should print 2
print binding_exec.getNodeCount()
binding.undo()
# It should print 1
print binding_exec.getNodeCount()


