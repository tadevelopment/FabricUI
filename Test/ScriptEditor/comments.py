ex = binding.binding.getExec()

# Add a comment to a Node
binding.addFunc("", "func", "dfgEntry {\n  // result = a + b;\n}\n", 218, 156)
binding.setNodeComment("", "func", "This is a comment on a Node")

print ex.getNodeMetadata("func", "uiComment")

# Add an empty comment to a Node
binding.instPreset("", "Fabric.Core.Constants.Float32", 220, 216)
binding.setNodeComment("", "Float32_1", "")

print "Empty comment added : " + str("uiComment" in binding.binding.getExec().getNodeDesc("Float32_1"))

# Add a new comment to a node. Make it empty. Undo.
binding.setNodeComment("", "func", "")
binding.undo()

print ex.getNodeMetadata("func", "uiComment")

# Add a comment to a backdrop
binding.addBackDrop("", "aBackdrop", 192.0, 91.0)
binding.setNodeComment("", "backDrop", "Some backdrop comment")

print ex.getNodeMetadata("backDrop", "uiComment")

# Add an empty comment to a node with an existing comment
binding.setNodeComment("", "func", "")

print ex.getNodeMetadata("func", "uiComment")

# Add a big comment to a node with an existing comment
bigComment = "\n".join([ "a v" + "".join([ "e" for _ in range(i*i) ]) + "ry long comment" for i in range(1,10) ])
binding.setNodeComment("", "backDrop", bigComment)

print ex.getNodeMetadata("backDrop", "uiComment")