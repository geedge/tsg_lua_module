local ss = TSG.data
print("[lua]:the parameter of lua function is '"..ss.."'")
print("[lua]:start find 'hello'...")
local index_s, index_e = string.find(ss, 'hello')   
local str = 'find!'
print("[lua]:result, start at", index_s, ", end at",index_e)                           
print("[lua]: return ", string.len(str), str) 
return string.len(str), str 
