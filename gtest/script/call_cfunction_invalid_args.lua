local data = TSG.data

local str = TSG.get.get_hello("test", "Test")

local ret = TSG.set.set_world(str, "world")
return string.len(ret), ret