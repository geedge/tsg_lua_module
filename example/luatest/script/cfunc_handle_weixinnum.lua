
local data = tsg.data
local data_len = string.len(tsg.data)
local feature = "weixinnum"
local feature_len = string.len(feature);
local max_weixinnum_len = 12
local offset = 4
local locate = 0

-- 查找字符串weixinnum位置
local _start, _end = tsg.memmem(data, data_len, feature, feature_len)
if not _end then
    return 1, false
end

-- 获取weixinnum数值字符串长度
if ((string.byte(data, _end + 1) ~= 0) or (string.byte(data, _end + 2) ~=0 ) or (string.byte(data, _end + 3) ~= 0)) then
    return 1, false
end
local weixinnum_len = string.byte(data, _end + offset)
if not weixinnum_len then
    return 1, false
end
locate = _end + offset

-- 判断weixinnum数值字符串长度的合法性
if weixinnum_len > data_len - _end - 4  or weixinnum_len > max_weixinnum_len then
    return 1, false
end

-- 获取weixinnum数值字符串
local weixinnum = string.sub(data, locate + 1, locate + weixinnum_len)
-- weixinnum数值字符串是否可以转换为数字
if tonumber(weixinnum) then
    return weixinnum_len, weixinnum
else
    return 1, false
end
--[[
--print(_start, _end)
print(string.byte(data, _end))
return 1, true
]]
