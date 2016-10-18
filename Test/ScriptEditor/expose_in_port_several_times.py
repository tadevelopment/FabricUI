print "Add \"graph\" node"
binding.addGraph("", "graph", 612.0, 35.0)
binding.addPort("graph", "inPort", "in", "UInt8", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")

binding_exec = binding.binding.getExec().getSubExec("graph")
print "Ports in \"graph\" node: " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding_exec = binding.binding.getExec()
print "Expose \"graph\" inPort 5 times"
binding.addPort("", "inPort", "in", "UInt8", "graph.inPort", "", "")
binding.addPort("", "inPort", "in", "UInt8", "graph.inPort", "", "")
binding.addPort("", "inPort", "in", "UInt8", "graph.inPort", "", "")
binding.addPort("", "inPort", "in", "UInt8", "graph.inPort", "", "")
binding.addPort("", "inPort", "in", "UInt8", "graph.inPort", "", "")
print "Ports in main node: " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

