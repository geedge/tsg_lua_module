local data = TSG.data;
local ret = TSG.set_repeat()

if type(ret) == "string" then
	return string.len(ret), ret
else
	return ret
end