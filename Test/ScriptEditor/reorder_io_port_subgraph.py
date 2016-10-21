binding.addGraph("", "graph", 779.0, 79.0)

binding_exec = binding.binding.getExec().getSubExec("graph")
print "Number of Port before adding : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.addPort("graph", "ioPort", "io", "Float32", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
binding.addPort("graph", "firstOutPort", "out", "UInt8", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
binding.addPort("graph", "secondOutPort", "out", "UInt16", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
binding.addPort("graph", "firstInPort", "in", "UInt8", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
binding.addPort("graph", "secondInPort", "in", "UInt16", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
print "Number of Port after adding : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.reorderPorts("graph", "", "0|2|1|3|4|5")
print "0|1|2|3|4|5 -> 0|2|1|3|4|5 Reordering : "
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.reorderPorts("graph", "", "0|1|3|4|5|2")
print "0|2|1|3|4|5 -> 0|1|3|4|5|2 Reordering : "
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.reorderPorts("graph", "", "0|5|1|2|3|4")
print "0|1|3|4|5|2 -> 0|5|1|2|3|4 Reordering : "
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

