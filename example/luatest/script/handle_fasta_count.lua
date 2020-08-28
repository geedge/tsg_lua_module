local data = TSG.data
local data_len = string.len(data)
local feature = "hAATCGCTT"

local _index
local _end = 1
local cnt  = 0
while true do
    _index, _end = string.find(data, feature, _end, -1)
    if not _end then
        break;
    end
    cnt = cnt + 1
end

local result = string.format("count:%d", cnt)
return string.len(result), result
