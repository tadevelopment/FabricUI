binding.addGraph("", "graph", 867.0, 218.0)

binding_exec = binding.binding.getExec().getSubExec("graph")
print "Number of Port before adding : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.addPort("graph", "firstPort", "in", "UInt8", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
binding.addPort("graph", "secondPort", "in", "Float32", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")

print "Number of Port after adding : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.removePort("graph", "firstPort")

print "Number of Port after removing the first one : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

binding.undo()

print "Number of Port after undoing : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  print "Port  " + str(portName) + " type " + str(binding_exec.getPortType(portName))

