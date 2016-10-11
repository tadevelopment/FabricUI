binding.instPreset("", "Fabric.Core.Math.Mul", 150.0, 205.0)
# Result: Mul_1
binding.addPort("", "timeline", "in", "Float32", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: timeline
binding.connect("", "timeline", "Mul_1.lhs")
binding.instPreset("", "Fabric.Exts.Geometry.PolygonMesh.EmptyPolygonMesh", 367.0, 259.0)
# Result: EmptyPolygonMesh_1
binding.instPreset("", "Fabric.Exts.Geometry.PolygonMesh.AddTorus", 354.0, 259.0)
# Result: AddTorus_1
binding.moveNodes("", "AddTorus_1", "606.0", "161.0")
binding.moveNodes("", "EmptyPolygonMesh_1", "341.0", "93.0")
binding.connect("", "EmptyPolygonMesh_1.mesh", "AddTorus_1.this")
binding.instPreset("", "Fabric.Compounds.Blocks.Geometry.PolygonMesh.DeformInPlace", 834.0, 283.0)
# Result: DeformInPlace_1
binding.addPort("", "frequency", "in", "Scalar", "", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: frequency
binding.addInstBlockPort("", "DeformInPlace_1", "deform", "frequency", "Scalar", "frequency", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: frequency
binding.addInstBlockPort("", "DeformInPlace_1", "deform", "offset", "Float32", "Mul_1.result", "", "{\n  \"uiRange\" : \"\",\n  \"uiHardRange\" : \"\"\n  }")
# Result: offset
binding.setPortDefaultValue("", "Mul_1.rhs", "Float32", "0.300000011920929")
binding.connect("", "AddTorus_1.this", "DeformInPlace_1.mesh")
binding.instPreset("", "Fabric.Exts.InlineDrawing.DrawingHandle.EmptyDrawingHandle", 1021.59530013, 190.579759192)
# Result: EmptyDrawingHandle_1
binding.instPreset("", "Fabric.Exts.InlineDrawing.DrawingHandle.DrawPolygonMesh", 987.994817139, 188.543366284)
# Result: DrawPolygonMesh_1
binding.moveNodes("", "DrawPolygonMesh_1", "1279.19912267", "166.143058538")
binding.connect("", "EmptyDrawingHandle_1.handle", "DrawPolygonMesh_1.this")
binding.moveNodes("", "DrawPolygonMesh_1", "1303.63556767", "186.506980419")
binding.moveNodes("", "DrawPolygonMesh_1", "1303.63562012", "188.543366194")
binding.moveNodes("", "DrawPolygonMesh_1", "1317.89024854", "190.579758406")
binding.addPort("", "this", "out", "DrawingHandle", "DrawPolygonMesh_1.this", "", "")
# Result: this
binding.connect("", "DeformInPlace_1.mesh", "DrawPolygonMesh_1.mesh")
binding.instPreset("DeformInPlace_1.deform", "Fabric.Exts.Math.Func.MultiplyScalar", 318.0, 443.0)
# Result: MultiplyScalar_1
binding.instPreset("DeformInPlace_1.deform", "Fabric.Exts.Math.Vec3.DecomposeVec3", 449.0, 467.0)
# Result: DecomposeVec3_1
binding.moveNodes("DeformInPlace_1.deform", "DecomposeVec3_1", "549.0", "431.0")
binding.moveNodes("DeformInPlace_1.deform", "MultiplyScalar_1", "344.0", "407.0")
binding.connect("DeformInPlace_1.deform", "originalPosition", "MultiplyScalar_1.this")
binding.connect("DeformInPlace_1.deform", "MultiplyScalar_1.result", "DecomposeVec3_1.value")
binding.connect("DeformInPlace_1.deform", "frequency", "MultiplyScalar_1.other")
binding.instPreset("DeformInPlace_1.deform", "Fabric.Core.Math.Add", 657.0, 368.0)
# Result: Add_1
binding.moveNodes("DeformInPlace_1.deform", "Add_1", "719.0", "412.0")
binding.paste("DeformInPlace_1.deform", "{\n  \"nodes\" : [\n    {\n      \"objectType\" : \"Inst\",\n      \"metadata\" : {\n        \"uiGraphPos\" : \"{\\\"x\\\":719.0,\\\"y\\\":412.0}\"\n        },\n      \"name\" : \"Add_1\",\n      \"ports\" : [\n        {\n          \"objectType\" : \"InstPort\",\n          \"name\" : \"exec\",\n          \"nodePortType\" : \"IO\"\n          },\n        {\n          \"objectType\" : \"InstPort\",\n          \"name\" : \"lhs\",\n          \"nodePortType\" : \"In\"\n          },\n        {\n          \"objectType\" : \"InstPort\",\n          \"name\" : \"rhs\",\n          \"nodePortType\" : \"In\"\n          },\n        {\n          \"objectType\" : \"InstPort\",\n          \"name\" : \"result\",\n          \"nodePortType\" : \"Out\"\n          }\n        ],\n      \"executable\" : \"Fabric.Core.Math.Add\",\n      \"presetGUID\" : \"8146B3E77857E24CAE33F8B5284585E7\"\n      }\n    ],\n  \"blocks\" : [],\n  \"connections\" : [],\n  \"requiredPresets\" : {\n    \"Fabric.Core.Math.Add\" : {\n      \"objectType\" : \"Func\",\n      \"title\" : \"Add\",\n      \"ports\" : [\n        {\n          \"objectType\" : \"ExecPort\",\n          \"name\" : \"exec\",\n          \"nodePortType\" : \"IO\",\n          \"execPortType\" : \"IO\",\n          \"typeSpec\" : \"Execute\"\n          },\n        {\n          \"objectType\" : \"ExecPort\",\n          \"name\" : \"lhs\",\n          \"nodePortType\" : \"Out\",\n          \"execPortType\" : \"In\",\n          \"typeSpec\" : \"$TYPE$\"\n          },\n        {\n          \"objectType\" : \"ExecPort\",\n          \"name\" : \"rhs\",\n          \"nodePortType\" : \"Out\",\n          \"execPortType\" : \"In\",\n          \"typeSpec\" : \"$TYPE$\"\n          },\n        {\n          \"objectType\" : \"ExecPort\",\n          \"name\" : \"result\",\n          \"nodePortType\" : \"In\",\n          \"execPortType\" : \"Out\",\n          \"typeSpec\" : \"$TYPE$\"\n          }\n        ],\n      \"extDeps\" : {},\n      \"presetGUID\" : \"8146B3E77857E24CAE33F8B5284585E7\",\n      \"code\" : \"\ndfgEntry {\n\tresult = lhs + rhs;\n}\n\"\n      }\n    }\n  }", 786.0, 425.0)
# Result: [u'Add_2']
binding.moveNodes("DeformInPlace_1.deform", "Add_2", "782.0", "609.0")
binding.moveNodes("DeformInPlace_1.deform", "Add_1", "717.0", "488.0")
binding.connect("DeformInPlace_1.deform", "DecomposeVec3_1.x", "Add_1.lhs")
binding.connect("DeformInPlace_1.deform", "offset", "Add_1.rhs")
binding.connect("DeformInPlace_1.deform", "offset", "Add_2.rhs")
binding.connect("DeformInPlace_1.deform", "DecomposeVec3_1.y", "Add_2.lhs")
binding.moveNodes("DeformInPlace_1.deform", "Add_2", "710.0", "661.0")
binding.instPreset("DeformInPlace_1.deform", "Fabric.Core.Math.Mul", 733.0, 604.0)
# Result: Mul_1
binding.moveNodes("DeformInPlace_1.deform", "Mul_1", "877.0", "634.0")
binding.connect("DeformInPlace_1.deform", "Add_2.result", "Mul_1.lhs")
binding.instPreset("DeformInPlace_1.deform", "Fabric.Core.Trigonometry.Sin", 797.0, 541.0)
# Result: Sin_1
binding.moveNodes("DeformInPlace_1.deform", "Sin_1", "891.0", "505.0")
binding.connect("DeformInPlace_1.deform", "Add_1.result", "Sin_1.value")
binding.paste("DeformInPlace_1.deform", "{\n  \"nodes\" : [\n    {\n      \"objectType\" : \"Inst\",\n      \"metadata\" : {\n        \"uiGraphPos\" : \"{\\\"x\\\":891.0,\\\"y\\\":505.0}\"\n        },\n      \"name\" : \"Sin_1\",\n      \"ports\" : [\n        {\n          \"objectType\" : \"InstPort\",\n          \"name\" : \"exec\",\n          \"nodePortType\" : \"IO\"\n          },\n        {\n          \"objectType\" : \"InstPort\",\n          \"name\" : \"value\",\n          \"nodePortType\" : \"In\"\n          },\n        {\n          \"objectType\" : \"InstPort\",\n          \"name\" : \"result\",\n          \"nodePortType\" : \"Out\"\n          }\n        ],\n      \"executable\" : \"Fabric.Core.Trigonometry.Sin\",\n      \"presetGUID\" : \"770343876572443C9B4721A66A428C2D\"\n      }\n    ],\n  \"blocks\" : [],\n  \"connections\" : [],\n  \"requiredPresets\" : {\n    \"Fabric.Core.Trigonometry.Sin\" : {\n      \"objectType\" : \"Func\",\n      \"title\" : \"Sin\",\n      \"ports\" : [\n        {\n          \"objectType\" : \"ExecPort\",\n          \"name\" : \"exec\",\n          \"nodePortType\" : \"IO\",\n          \"execPortType\" : \"IO\",\n          \"typeSpec\" : \"Execute\"\n          },\n        {\n          \"objectType\" : \"ExecPort\",\n          \"name\" : \"value\",\n          \"nodePortType\" : \"Out\",\n          \"execPortType\" : \"In\",\n          \"typeSpec\" : \"$TYPE$\"\n          },\n        {\n          \"objectType\" : \"ExecPort\",\n          \"name\" : \"result\",\n          \"nodePortType\" : \"In\",\n          \"execPortType\" : \"Out\",\n          \"typeSpec\" : \"$TYPE$\"\n          }\n        ],\n      \"extDeps\" : {},\n      \"presetGUID\" : \"770343876572443C9B4721A66A428C2D\",\n      \"code\" : \"\ndfgEntry {\n\tresult = sin(value);\n}\n\"\n      }\n    }\n  }", 948.0, 537.0)
# Result: [u'Sin_2']
binding.connect("DeformInPlace_1.deform", "Mul_1.result", "Sin_2.value")
binding.moveNodes("DeformInPlace_1.deform", "Sin_2", "1006.0", "627.0")
binding.instPreset("DeformInPlace_1.deform", "Fabric.Exts.Math.Vec3.Vec3", 1008.0, 589.0)
# Result: Vec3_1
binding.moveNodes("DeformInPlace_1.deform", "Vec3_1", "1158.0", "531.0")
binding.connect("DeformInPlace_1.deform", "Sin_1.result", "Vec3_1.x")
binding.connect("DeformInPlace_1.deform", "Sin_2.result", "Vec3_1.y")
binding.instPreset("DeformInPlace_1.deform", "Fabric.Core.Math.Mul", 1226.0, 489.0)
# Result: Mul_2
binding.moveNodes("DeformInPlace_1.deform", "Mul_2", "1304.0", "479.0")
binding.connect("DeformInPlace_1.deform", "Vec3_1.result", "Mul_2.rhs")
binding.connect("DeformInPlace_1.deform", "originalNormal", "Mul_2.lhs")
binding.removeNodes("DeformInPlace_1.deform", "PassThrough")
binding.instPreset("DeformInPlace_1.deform", "Fabric.Core.Math.Add", 1418.0, 439.0)
# Result: Add_3
binding.moveNodes("DeformInPlace_1.deform", "Add_3", "1448.0", "393.0")
binding.connect("DeformInPlace_1.deform", "Mul_2.result", "Add_3.rhs")
binding.connect("DeformInPlace_1.deform", "originalPosition", "Add_3.lhs")
binding.connect("DeformInPlace_1.deform", "Add_3.result", "newPosition")
binding.setPortDefaultValue("", "AddTorus_1.innerRadius", "Scalar", "2.980000019073486")
binding.setPortDefaultValue("", "AddTorus_1.outerRadius", "Scalar", "9.380000114440918")
binding.setPortDefaultValue("", "AddTorus_1.detail", "Size", "60")
binding.setPortDefaultValue("", "AddTorus_1.setUVs", "Boolean", "false")
binding.setArgValue("frequency", "Scalar", "1.789999961853027")
binding.setPortDefaultValue("", "DrawPolygonMesh_1.color", "Color", "{\n  \"r\" : 0.3529411852359772,\n  \"g\" : 0.6000000238418579,\n  \"b\" : 1,\n  \"a\" : 1\n  }")
binding.implodeNodes("", "EmptyPolygonMesh_1|AddTorus_1", "torus")
# Result: torus 
timeLine.updateTime(20.0)
binding.setPortDefaultValue("", "Mul_1.rhs", "Float32", "0.699999988079071")
binding.setPortDefaultValue("torus", "AddTorus_1.innerRadius", "Scalar", "1.429999947547913")
binding.setPortDefaultValue("torus", "AddTorus_1.outerRadius", "Scalar", "12")
binding.setPortDefaultValue("torus", "AddTorus_1.detail", "Size", "33")
binding.setPortDefaultValue("torus", "AddTorus_1.detail", "Size", "11")
binding.explodeNode("", "torus")
# Result: [u'EmptyPolygonMesh_1', u'AddTorus_1']
binding.disconnect("", "Mul_1.result", "DeformInPlace_1.deform.offset")
binding.disconnect("", "frequency", "DeformInPlace_1.deform.frequency")
binding.addGraph("", "graph", 968.0, 80.0)
binding.undo()
binding.undo()
binding.undo()
binding.undo()
binding.undo()
binding.undo()
binding.undo()
binding.undo()
binding.undo()
binding.redo()
binding.redo()
binding.redo()
binding.redo()
binding.redo()
binding.redo()
binding.redo()
binding.redo()
binding.redo()
binding.redo()
binding.redo()
#Iterate through all the nodes
binding_exec = binding.binding.getExec()
for i in range(binding_exec.getNodeCount()):
  print binding_exec.getNodeName(i)