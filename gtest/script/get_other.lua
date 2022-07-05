local data = TSG.data
local result = "true"
if not TSG.get.get_result() then
	result = "false"
end

local flag = "not nil"
if not TSG.get.get_nil() then
	flag = "is nil"
end

local str = string.format("result:%s, time:%d, %s", result, TSG.get.get_time(), flag)

return string.len(str), str