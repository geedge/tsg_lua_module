local str = string.format ('%s', tsg.data) 
local table = {}
setmetatable(table, {__index = _G)
return string.len(str), str  
