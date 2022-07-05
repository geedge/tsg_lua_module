local data = TEST.data
local context = TEST.context

if not context.count then
	context.count = 1
else
	context.count = context.count + 1
end

if not context.message then
	context.message = "This is first called"
else
	context.message = "This is not first called"
end

local ud = TEST.get.get_userdata()
local str = string.format("userdata:%s, context.count:%d, context.message:%s", ud, context.count, context.message)

return string.len( str ), str
