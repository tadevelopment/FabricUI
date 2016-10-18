binding.addGraph("", "graph", 876.0, 53.0)

binding_exec = binding.binding.getExec().getSubExec("graph")
print "Number of Port before adding : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.addPort("graph", "firstPort", "in", "UInt8", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
binding.addPort("graph", "secondPort", "in", "UInt16", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
binding.addPort("graph", "thirdPort", "in", "UInt32", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")

print "Number of Port after adding : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.reorderPorts("graph", "", "0|1|3|2")
print "0|1|2|3 -> 0|1|3|2 Reordering : "
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.reorderPorts("graph", "", "0|2|3|1")
print "0|1|3|2 -> 0|2|3|1 Reordering : "
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.reorderPorts("graph", "", "0|2|1|3")
print "0|2|3|1 -> 0|2|1|3 Reordering : "
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

