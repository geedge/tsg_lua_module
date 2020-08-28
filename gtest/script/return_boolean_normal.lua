local str = TSG.data    
local s_index, e_index = string.find(str, 'hello') 
if s_index then 
    return 1, true 
else return 1, false 
end 
