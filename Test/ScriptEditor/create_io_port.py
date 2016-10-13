binding_exec = binding.binding.getExec()
print "Number of Port before adding : " + str(binding_exec.getExecPortCount())
binding.addPort("", "ioPort", "io", "Float32", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
print "Number of Port after adding : " + str(binding_exec.getExecPortCount())
