local str = TEST.data

TEST.set_count()

local sum = 0
for i = 0, 1000 do
	for j = 0, 1000 do
		sum = sum + i + j
	end
end
return string.len(str), str
