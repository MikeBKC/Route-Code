#!/usr/bin/lua 

local json = require('cjson')

function fromSysGet(cmd)
    local file = io.popen(cmd)
    local data=file:read()
    file:close()
    return data
end

function getAllChildDev()
    local req={}
    for i=1,2,1 do
        req[i]={}
        --req[i]=getOneChildDev()
        req[i].mac="112233445566"
        req[i].vmac="665544332211"
        req[i].connecttype=1
    end
    return req
end

function getWiFi()
    local wifi={}

    --g5json=fromSysGet('uttelink.sh get wireless5ginfo')
    g2json=fromSysGet('uttelink.sh get wirelessinfo')
    --g5jsondata=json.decode(g5json)
    g2jsondata=json.decode(g2json)

    wifi[1]={}
    wifi[1].radio={}
    wifi[1].radio.mode="2.4G"
    wifi[1].radio.channel=tonumber(g2jsondata.channel)
    wifi[1].ap={}
    wifi[1].ap[1]={}
    wifi[1].ap[1].apidx=0
    wifi[1].ap[1].enable="yes"
    wifi[1].ap[1].ssid=g2jsondata.ssid
    wifi[1].ap[1].key=g2jsondata.password
    wifi[1].ap[1].auth=string.lower(g2jsondata.authmode)
    wifi[1].ap[1].encrypt=string.lower(g2jsondata.encryptmethod)
    --[[
    wifi[2]={}
    wifi[2].radio={}
    wifi[2].radio.mode="5G"
    wifi[2].radio.channel=tonumber(g5jsondata.channel)
    wifi[2].ap={}
    wifi[2].ap[1]={}
    wifi[2].ap[1].apidx=0
    wifi[2].ap[1].enable="yes"
    wifi[2].ap[1].ssid=g5jsondata.ssid
    wifi[2].ap[1].key=g5jsondata.password
    wifi[2].ap[1].auth=string.lower(g5jsondata.authmode)
    wifi[2].ap[1].encrypt=string.lower(g5jsondata.encryptmethod)
    --]]
    --[[
    for i=1,2 do
    wifi[i]={}
    wifi[i].radio={}
    wifi[i].radio.mode="2.4G"
    wifi[i].radio.channel=12
    wifi[i].ap={}
    for j=1,2 do
    wifi[i].ap[j]={}
    wifi[i].ap[j].apidx=i-1
    wifi[i].ap[j].enable="yes"
    wifi[i].ap[j].ssid="ssid"
    wifi[i].ap[j].key="WiFikey"
    wifi[i].ap[j].auth="open"
    wifi[i].ap[j].encrypt="none"

    end
    end
    --]]
    return wifi

end

function getWiFiswitch()
    local s = fromSysGet('uttelink.sh get wirelessactive')
    if s == "Yes" then
        s='{"wirelessactive":"ON"}'
    else
        s='{"wirelessactive":"OFF"}'
    end
    local sdata=json.decode(s)
    local req = {}
    req.status=sdata.wirelessactive
    return req
end
function getledswitch()
    local s = fromSysGet('uttelink.sh get ledswith')
    if s and s=="" then
        s='{"ledswith":"OFF"}'
    else
        s='{"ledswith":"OFF"}'
    end
    local sdata=json.decode(s)
    local req={}
    req.status=sdata.ledswith
    return req
end
function getWiFitimer()
    local req={}
    req[1]={}
    req[1].weekday="1"
    req[1].time="21:56"
    req[1].enable="0"
    return req
end
function getOneChildDev()
    local req={}
    return req
end
function getChildStatus()
end

function getmodel()
    local modelCmd = "uname  -a |awk -F '[ ]' '{print $5}'"
    return fromSysGet(modelCmd)
end

function getswversion()
    local versionCmd = "uname  -a |awk -F '[ -]' '{print $6}'"
    return fromSysGet(versionCmd)
end

function gethdversion()
    local hwCmd = "uname  -v |awk -F '[ - ]' '{print $1}'"
    local hwTmp = fromSysGet(hwCmd)
    if(string.lower(string.sub(hwTmp,string.len(hwTmp)-2,string.len(hwTmp)-1)) == "v") then
        return string.lower(string.sub(hwTmp,string.len(hwTmp)-2,string.len(hwTmp)))..".0"
    else
        return "v1.0"
    end
end

function getGW()
    local GWCmd="route -n | awk '{print $2}'|grep '\\<[^0a-zA-Z]'"
    local GW = fromSysGet(GWCmd)
    return GW
end

function getmac(eth)
    local macCmd="ifconfig "..eth.." |awk '/HWaddr/{print $5}'"
    local macData=fromSysGet(macCmd)
    return macData
end


local function getOneChildDev()
    local req={}
    return req
end
