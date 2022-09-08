local str = TEST.data

local sum = 0

local function test()
	for i = 0, 10000 do
		for j = 0, 10000 do
			sum = sum + i + j
		end
	end
end

test()

return string.len(str), str
