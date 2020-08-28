local str = TSG.data
local len = string.len(str)
local thread_id = string.byte(str, len)
local str = string.format("[lua][thread_id:%d][match_12345.lua]:str:%s, len:%d", thread_id, str, len - 1)
print(str)
local i = 0
for i = 1, string.len(str) do
    if string.byte(str, i) == 1 then
        if string.byte(str, i + 1) == 2 then
            if string.byte(str, i + 2) == 3 then
                if string.byte(str, i + 3) == 4 then
                    if string.byte(str, i + 4) == 5 then
                        local str = string.format("[lua][thread_id:%d][match_12345.lua]:return %d, %s", thread_id, 1, "true")
                        print(str)
                        return 1, true
                    end
                end
            end
        end
    end
end
local str = string.format("[lua][thread_id:%d][match_12345.lua]:return %d, %s", thread_id, 1, "false")
print(str)
return 1, false
