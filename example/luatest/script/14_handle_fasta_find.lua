local data = tsg.data
local data_len = string.len(data)
local feature = "qGCTGA"

local result
local _index, _end = string.find(data, feature)
if not _index then
    result = "not find"
    return string.len(result), result
else
    result = "find"
    return string.len(result), result
end
