print "Add new graph, create a output port and connected it to the exec"
binding.addGraph("", "graph", 965.0, 86.0)
binding.addInstPort("", "graph", "myPort", "out", "UInt32", "", "out", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
binding.addPort("", "myPort", "out", "UInt32", "graph.myPort", "", "")

print "Before removing graph"
binding_exec = binding.binding.getExec()
for i in range(binding_exec.getNodeCount()):
  print "Graph " + str(binding_exec.getNodeName(i))
for i in range(binding_exec.getExecPortCount()):
  print "Port  " + str(binding_exec.getExecPortName(i))

binding.removeNodes("", "graph")

print "After removing graph"
for i in range(binding_exec.getNodeCount()):
  print "Graph " + str(binding_exec.getNodeName(i))
for i in range(binding_exec.getExecPortCount()):
  print "Port  " + str(binding_exec.getExecPortName(i))
