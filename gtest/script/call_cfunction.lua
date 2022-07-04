local data = TSG.data

local str = TSG.get.get_hello()

local ret = TSG.set.set_world(str)
return string.len(ret), ret