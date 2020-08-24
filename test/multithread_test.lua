--[[
--慢，原因是string.match不能转化成bytecode不能被jit
local str = tsg.data
if (string.match(str, "GGGGGGGGGG")) then
    print("get GGGGGGGGGG")
    return 1, true
end
return 1, false
]]
--[[
local str = tsg.data
local i, j
local sub = "GGGGGGGGGG"
local sub_len = string.len(sub)
for i = 1, string.len(str) - sub_len do
    if (string.byte(str, i) == "G") then
        for j = 2, sub_len do
            if string.byte(str, i + j - 1) ~= string.byte(sub, j) then
                break;
            end
            if j == sub_len then
                return 1, true
            end
        end
    end
end
return 1, false
]]

--[[
local str = tsg.data
local i
local sub = "GGGGGGGGGG"
for i = 1, string.len(str) - 10  do
    if string.byte(str, i) == 'G' then
        if string.sub(str, i, i+9) == sub then
            return 1, true
        end
    end
end
return 1, false
]]
local str = tsg.data
if string.find(str, "GGGGGGGGG") then
    return 1,true
else
    return 1, false
end
