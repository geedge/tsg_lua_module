----------------------------------------------------------------------------------------------
--                          脚本功能：在数据中查找符合特征的qqnum                           --
--  输入：                                                                                  --
--  TSG.data                                                                                --
--  特征如下：                                                                              --
--  特征一：                                                                                --
--  00 00 0b db 00 00 00 0b 01 00 00                                                        --
--  第1、2、5、6、7、10、11位必须为0x00，第9为必须为0x01                                    --
--  特征二：                                                                                --
--  00 00 00 00 0d                                                                          --
--  连续4个0x00后面紧跟一个字节的qq num 字符长度，                                          --
--  该长度实际值为当前值减4，并且实际值不大于12                                             --
----------------------------------------------------------------------------------------------
local data = TSG.data   --TSG.data 获取待处理数据 
local data_len = string.len(data)
local max_qqnum_len = 12
local locate = 1 -- 字符串中当前正在处理的位置

-- 判断特征一
if (string.byte(data, 1) ~= 0) or (string.byte(data, 2) ~= 0) or (string.byte(data, 5) ~= 0) or
    (string.byte(data, 6) ~= 0) or (string.byte(data, 7) ~= 0) or (string.byte(data, 9) ~= 1) or
    (string.byte(data, 10) ~= 0) or (string.byte(data, 11) ~= 0) then
    return 1, false
end

-- 判断特征二
-- 命中特征"\0\0\0\0"
local _, _end = string.find(data, "\0\0\0\0")
if not _end then
    return false
end
-- 获取qqnum长度
local qqnum_len = string.byte(data, _end + 1)
if not qqnum_len then
    return false
end
locate = _end + 1
qqnum_len = qqnum_len - 4
-- 判断qqnum长度的合法性
if qqnum_len > 12 or qqnum_len > (data_len - locate) then
    return false
end
locate = locate + 1
-- 获取qqnum
local qqnum = string.sub(data, locate, locate +  qqnum_len - 1)
if not qqnum then
    return false
end
-- 验证qqnum由全数字组成
if not tonumber(qqnum) then
    return false
end

return qqnum_len, qqnum

