local str = TSG.data
local len = string.len(str)
local thread_id = string.byte(str, len)
local str = string.format("[lua][thread_id:%d][count_1.lua]:str:%s, len:%d", thread_id, str, len - 1)
print(str)
local cnt = 0
local i = 0
for i = 0, len - 1 do
    --print(i, string.byte(str, i), cnt)
    if string.byte(str, i) == 1 then
        cnt = cnt + 1
    end
end
str = string.format("[lua][thread_id:%d][count_1.lua]:return %d, %s", thread_id, 8, cnt)
print(str)
return 8, cnt 
