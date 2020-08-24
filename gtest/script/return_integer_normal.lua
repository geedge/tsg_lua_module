local str = tsg.data    
local s_index, e_index = string.find(str, 'world') 
if s_index then 
    return 8, s_index 
else 
    return 1, false 
end 
