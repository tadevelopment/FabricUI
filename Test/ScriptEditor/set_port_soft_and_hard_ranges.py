binding.addPort("", "port", "in", "Float32", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")

binding_exec = binding.binding.getExec()
print "Number of Port after adding : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  if(str(portName) == "port"):
  	portType = binding_exec.getPortType(portName)
  	uiRange = binding_exec.getExecPortMetadata("port", "uiRange")
  	uiHardRange = binding_exec.getExecPortMetadata("port", "uiHardRange")
  	print "Port " + str(portName) + " type " + str(portType) + " soft-range " + str(uiRange) + " hard-range " + str(uiHardRange)

binding.editPort("", "port", "port", "in", "Float32", "", "{\n  \"uiHidden\" : \"\",\n  \"uiOpaque\" : \"\",\n  \"uiPersistValue\" : \"\",\n  \"uiRange\" : \"(-4.5, 4.5)\",\n  \"uiHardRange\" : \"\",\n  \"uiCombo\" : \"\"\n  }")
print "After setting soft range (-4.5, 4.5)"
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  if(str(portName) == "port"):
  	portType = binding_exec.getPortType(portName)
  	uiRange = binding_exec.getExecPortMetadata("port", "uiRange")
  	uiHardRange = binding_exec.getExecPortMetadata("port", "uiHardRange")
  	print "Port " + str(portName) + " type " + str(portType) + " soft-range " + str(uiRange) + " hard-range " + str(uiHardRange)


binding.editPort("", "port", "port", "in", "Float32", "", "{\n  \"uiHidden\" : \"\",\n  \"uiOpaque\" : \"\",\n  \"uiPersistValue\" : \"\",\n  \"uiRange\" : \"(-4.5, 4.5)\",\n  \"uiHardRange\" : \"(-2.5, 2.5)\",\n  \"uiCombo\" : \"\"\n  }")
print "After setting hard range (-2.5, 2.5)"
for i in range(binding_exec.getExecPortCount()):
  portName = binding_exec.getExecPortName(i)
  if(str(portName) == "port"):
  	portType = binding_exec.getPortType(portName)
  	uiRange = binding_exec.getExecPortMetadata("port", "uiRange")
  	uiHardRange = binding_exec.getExecPortMetadata("port", "uiHardRange")
  	print "Port " + str(portName) + " type " + str(portType) + " soft-range " + str(uiRange) + " hard-range " + str(uiHardRange)

