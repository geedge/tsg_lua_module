local data = TSG.data

local req_header = nil
local header = TSG.req.get_header()
for k, v in pairs(header) do
	if not req_header then
		req_header = string.format( "%s:%s", k, v)
	else
		req_header = string.format( "%s,%s:%s",req_header, k, v)
	end
end

local rsp_header = nil
header = TSG.rsp.get_header()
for k, v in pairs(header) do
	if not rsp_header then
		rsp_header = string.format("%s:%s", k, v)
	else
		rsp_header = string.format("%s,%s:%s", rsp_header, k, v)
	end
end

local ret = string.format( "req_header:%s; rsp_header:%s", req_header, rsp_header)
-- print(ret)
return string.len(ret), ret