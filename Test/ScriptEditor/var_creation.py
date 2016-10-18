binding_exec = binding.binding.getExec()
# Should return False
print binding_exec.hasVar("test")
binding.addVar("", "test", "Float32", "", 623.0, 75.0)
# Should return True
print binding_exec.hasVar("test")

