binding_exec = binding.binding.getExec()
print "Number of Port before adding : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  print "Port  " + str(binding_exec.getExecPortName(i))

binding.addPort("", "IAmAVeryVeryVeryVeryVeryVeryVeryLongName", "in", "Float32", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
print "Number of Port after adding : " + str(binding_exec.getExecPortCount())
for i in range(binding_exec.getExecPortCount()):
  print "Port  " + str(binding_exec.getExecPortName(i))
