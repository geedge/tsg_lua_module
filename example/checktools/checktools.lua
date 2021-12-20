#!/usr/local/bin/luajit
local flag = 0      -- 0: init;1: script;2: data 
local script_num = 0
local script_list = {}
local data_num = 0
local data_list = {}
TSG = {}
local conf_file = "./checktools.conf"

if arg[1] then
    if arg[1] == "-h" then
        print("usage: checktools.lua [conf] default conf is './checktools.conf'")
    else 
        conf_file = arg[1]
    end
end

local function file_is_exit(path)
    local file = io.open(path)
    if not file then
        return false
    end
    io.close(file)
    return true
end

if not file_is_exit(conf_file) then
    print("error: conf file: ".. conf_file .." not exits.")
    return false
end

for line in io.lines(conf_file) do
    if string.byte(line, 1) == '#' then
        --do nothing
    elseif string.len(line) == 0 then
        --do nothing
    elseif line == "[script]" then
        flag = 1
    elseif line == "[data]" then
        flag = 2
    else
        if flag == 1 then
           script_num = script_num + 1
           script_list[script_num] = line 
       elseif flag == 2 then
           data_num = data_num + 1
           local file = io.open(line, "r")
           if not file then
               print("error: ".. file .." not exits")
           end
           io.input(file)
           data_list[data_num] = io.read("*a")
           io.close(file)
       end
    end
end

for _, script in ipairs(script_list) do
    if not file_is_exit(script) then
        print("error: script: ".. script .." not exits.")
        return false
    end
    for _, data in ipairs(data_list) do
        TSG.data = data
        dofile(script)
    end
end

