---------------------------------------------------------------------------------------------
--                          脚本功能：在数据中查找符合特征的weixinnum                      --
--  输入：                                                                                 --
--  TSG.data                                                                               --
--  特征如下：                                                                             --
--  00 00 00 09 | 77 65 69 78 69 6e 6d 75 2d | 00 00 00 0a | 31 39 35 35 37 34 30 37 38 30 --
--  weixinnum   | w  e  i  x  i  n  n  u  m  | weixinnum   | 1  9  5  5  7  4  0  7  8  0  --
--  字符串长度  |    字符串                  | 数值长度    |        字符串                 --
---------------------------------------------------------------------------------------------

local data = TSG.data   --TSG.data 获取待处理数据 
local data_len = string.len(data)
local feature = "weixinnum" --待识别的特征
local max_weixinnum_len = 12
local offset = 4 -- 字符串长度所占位数
local locate = 0 -- 字符串中当前正在处理的位置

-- 查找字符串weixinnum位置
local _start, _end = string.find(data, feature)
if not _end then
    return false
end

-- 获取weixinnum数值字符串长度
-- weixinnum数值字符串长度不超过12，所以前三位必须为0
if ((string.byte(data, _end + 1) ~= 0) or (string.byte(data, _end + 2) ~=0 ) or (string.byte(data, _end + 3) ~= 0)) then
    return false
end
local weixinnum_len = string.byte(data, _end + offset)
if not weixinnum_len then
    return false
end
locate = _end + offset

-- 判断weixinnum数值字符串长度的合法性
if weixinnum_len > data_len - _end - offset  or weixinnum_len > max_weixinnum_len then
    return false
end

-- 获取weixinnum数值字符串
local weixinnum = string.sub(data, locate + 1, locate + weixinnum_len)
-- weixinnum数值字符串是否可以转换为数字
if tonumber(weixinnum) then
    return weixinnum_len, weixinnum
else
    return false
end
