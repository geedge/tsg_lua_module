--[[
local str = tsg.data
for i = 0, string.len(str) do
    if (string.byte(str, i) == 0) then
        print("find 0\n")
    end
end
return 1, true
]]

local str = tsg.data
if string.find(str, "weixinnum") then
    return 1, false
else
    return 1, true
end
