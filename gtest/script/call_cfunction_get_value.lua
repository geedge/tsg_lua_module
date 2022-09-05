local data = TSG.data

local value = TSG.get_value()

local retvalue = nil
print(type(value))
for k, v in pairs(value) do
	if not retvalue then
		retvalue = string.format("%s:%s", tostring(k), tostring(v))
	else
		retvalue = string.format("%s,%s:%s", retvalue, tostring(k), tostring(v))
	end
end

return string.len(retvalue), retvalue