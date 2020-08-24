local str = tsg.data;
local len = string.len(str)
local thread_id = string.byte(str, -1)
local info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:str:%s, str_len:%d", thread_id, str, len - 1)
print(info)
local ret = "unrecognition."
local index, _ = string.find(str, "tcp")
if index then
    ret = "tcp"
    info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
    print(info)
    return string.len(ret), ret
end
index, _ = string.find(str, "udp")
if index then
    ret = "udp"
    info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
    print(info)
    return string.len(ret), ret
end
index, _ = string.find(str, "ssh")
if index then
    ret = "ssh"
    info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
    print(info)
    return string.len(ret), ret
end
index, _ = string.find(str, "vnc")
if index then
    ret = "vnc"
    info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
    print(info)
    return string.len(ret), ret
end
index, _ = string.find(str, "sock")
if index then
    ret = "sock"
    info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
    print(info)
    return string.len(ret), ret
end
index, _ = string.find(str, "quic")
if index then
    ret = "quic"
    info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
    print(info)
    return string.len(ret), ret
end
index, _ = string.find(str, "pop3")
if index then
    ret = "pop3"
    info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
    print(info)
    return string.len(ret), ret
end
index, _ = string.find(str, "smpt")
if index then
    ret = "smpt"
    info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
    print(info)
    return string.len(ret), ret
end
index, _ = string.find(str, "imap")
if index then
    ret = "imap"
    info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
    print(info)
    return string.len(ret), ret
end
index, _ = string.find(str, "http")
if index then
    ret = "http"
    info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
    print(info)
    return string.len(ret), ret
end
info = string.format("[lua][thread_id:%d][protocol_recognition.lua]:return str_len:%d, str:%s", thread_id, string.len(ret), ret)
print(info)
return string.len(ret), ret
