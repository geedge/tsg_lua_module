local str = string.format ('%s', TSG.data) 
local table = {}
setmetatable(table, {__index = _G)
return string.len(str), str  
