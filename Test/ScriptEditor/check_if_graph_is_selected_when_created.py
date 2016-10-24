binding.addGraph("", "graph", 30.0, 383.0)

print "Add first graph"
# binding_exec = binding.binding.getExec()
# for i in range(binding_exec.getNodeCount()):
#   print "graph " + str(binding_exec.getNodeName(i))

print "Truc " + str(controller.selectedNodes())
for selectedNode in controller.selectedNodes():
  print "selected graph " + str(selectedNode)


binding.addGraph("", "graph2", 30.0, 383.0)
print controller.selectedNodes()

# print "Add second graph"
# for i in range(binding_exec.getNodeCount()):
#   print "graph " + str(binding_exec.getNodeName(i))

for selectedNode in controller.selectedNodes():
  print "selected graph " + str(selectedNode)
