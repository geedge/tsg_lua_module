local str = TSG.data    
local s_index, e_index = string.find(str, 'hello') 
if s_index then 
    return true 
else return false 
end 
