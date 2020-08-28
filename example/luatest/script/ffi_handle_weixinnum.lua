local data = TSG.data
local data_len = string.len(data)
local feature = "weixinnum"
local feature_len = string.len(feature)
local max_weixinnum_len = 12
local C = TSG.C
local ffi = TSG.ffi

-- 查找weixinnum字符串位置
local weixinnum_name = C.memmem(data, data_len, feature, feature_len)
if weixinnum_name == nil then
    return 1, false
end

-- 获取weixinnum数值字符串长度
local locate = weixinnum_name + feature_len
if (locate[0] ~= 0) or (locate[1] ~= 0) or (locate[2] ~= 0) then
    return 1, false
end
local weixinnum_len = locate[3]
if not weixinnum_len then
    return 1, false
end
if weixinnum_len > max_weixinnum_len then
    return 1, false
end
-- 获取weixinnum数值字符串
locate = locate + 4
local weixinnum = ffi.string(locate)
-- weixinnum数值字符串是否可以转换为数字
if tonumber(weixinnum) then
    return weixinnum_len, weixinnum
else
    return 1, false
end
