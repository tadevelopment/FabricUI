binding.addGraph("", "graph", 67.0, 441.0)
binding.addGraph("", "graph", 144.0, 503.0)
binding.addGraph("", "graph", 191.0, 565.0)

binding_exec = binding.binding.getExec()
print "Before moving"
for i in range(binding_exec.getNodeCount()):
  nodeName = binding_exec.getNodeName(i)
  print str(nodeName) + " pos " + str(binding_exec.getNodeMetadata(nodeName, 'uiGraphPos'))

binding.moveNodes("", "graph|graph_2|graph_3", "46.0|123.0|170.0", "327.0|389.0|451.0")

print "After moving"
for i in range(binding_exec.getNodeCount()):
  nodeName = binding_exec.getNodeName(i)
  print str(nodeName) + " pos " + str(binding_exec.getNodeMetadata(nodeName, 'uiGraphPos'))
