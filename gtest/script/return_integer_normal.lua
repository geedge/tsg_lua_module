local str = TSG.data    
local s_index, e_index = string.find(str, 'world') 
if s_index then 
    return s_index 
else 
    return false 
end 
