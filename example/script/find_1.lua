local data = tsg.data
local data_len = string.len(data)
local str = string.format("[lua]:data:%s len:%d", data, data_len)
print(str)
for i = 1, data_len do  
    if string.byte(data, i) == 1 then
        str = string.format("[lua]:return %d, %d", 8, i)
        print(str);
        return 8, i  
    end 
end 
return 1, true 

