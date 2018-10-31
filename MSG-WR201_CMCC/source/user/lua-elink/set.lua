function setWiFi(data)
    for i,wifidata in pairs(data) do
        local mode = wifidata.radio.mode
        local enable = "yes"
        local havessid = false
        local wifiswitch = fromSysGet('uttcli2 getbynm wireless wireless active')
        if(mode == "2.4G") then
            local file = io.open("./wireless2g.dat","w")
            for j,apdata in pairs(wifidata.ap) do
                if apdata.ssid then
                    os.execute("ifconfig ra0 down")
                    file:write("SSID"..j.."="..apdata.ssid.."\n")
                    havessid = true
                    if apdata.auth then                           
                        if(apdata.auth == "share") then
                            apdata.auth = "shared" 
                        end
                        file:write("AuthMode="..string.upper(apdata.auth)..";"..string.upper(apdata.auth).."\n")
                    end
                    if apdata.encrypt then                           
                        if(apdata.auth == "shared") then
                            apdata.encrypt = "wep"
                        end
                        file:write("EncrypType="..string.upper(apdata.encrypt)..";"..string.upper(apdata.encrypt).."\n")
                    end
                    if apdata.key then
                        if(apdata.encrypt == "wep") then
                            file:write("Key1Str"..j.."="..apdata.key.."\n")
                        else 
                            file:write("WPAPSK"..j.."="..apdata.key.."\n")
                        end
                    end
                    file:write("Channel="..wifidata.radio.channel.."\n")
                    if wifidata.radio.channel == 0 then
                        file:write("AutoChannelSelect=2")
                    else
                        file:write("AutoChannelSelect=0")
                    end

                    file:close()

                    if havessid then  
                        os.execute("cat /etc_ro/Wireless/default2g.dat >/etc/Wireless/RT2860/RT2860.dat")
                        os.execute("cat ./wireless2g.dat >>/etc/Wireless/RT2860/RT2860.dat")
                    end
                    if enable == "yes" and wifiswitch == "Yes" then
                        os.execute("ifconfig ra0 up &")
                    end

                    os.execute("uttwifiset.sh -s ".. apdata.ssid.. " -c "..wifidata.radio.channel.." -a ".. string.upper(apdata.auth).. " -p ".. apdata.key.. " -t ".. string.upper(apdata.encrypt) .."& ")
                end
                --file:write("Channel="..wifidata.radio.channel.."\n")
                --if wifidata.radio.channel == 0 then
                    --file:write("AutoChannelSelect=2")
                --else
                    --file:write("AutoChannelSelect=0")
                --end
                --file:close()

               -- if havessid then  
                 --   os.execute("cat ./default2g.dat >/etc/Wireless/RT2860/RT2860.dat")
                   -- os.execute("cat ./wireless2g.dat >>/etc/Wireless/RT2860/RT2860.dat")
                --end
                --if enable == "yes" and wifiswitch == "Yes" then
                  --  os.execute("ifconfig ra0 up &")
                --end
            end

        else if(mode == "5G") then
            local file=io.open("./wireless5g.dat","w")
            local havessid = false
            for j,apdata in pairs(wifidata.ap) do
                if apdata.ssid then
                    os.execute("ifconfig rai0 down")
                    file:write("SSID"..j.."="..apdata.ssid.."\n")
                    havessid= true
                    if apdata.auth then                
                        if(apdata.auth == "share") then
                             apdata.auth = "shared"
                        end
                        file:write("AuthMode="..string.upper(apdata.auth)..";"..string.upper(apdata.auth).."\n")
                    end
                    if apdata.encrypt then              
                        if(apdata.auth == "shared") then
                            apdata.encrypt = "wep"
                        end
                        file:write("EncrypType="..string.upper(apdata.encrypt)..";"..string.upper(apdata.encrypt).."\n")
                    end
                    if apdata.key then
                        if(apdata.encrypt == "wep") then
                            file:write("Key1Str"..j.."="..apdata.key.."\n")
                        else
                            file:write("WPAPSK"..j.."="..apdata.key.."\n")
                        end
                    end
                    file:write("Channel="..wifidata.radio.channel.."\n")
                    if wifidata.radio.channel == 0 then
                        file:write("AutoChannelSelect=2")
                    else
                        file:write("AutoChannelSelect=0")
                    end
                    file:close()
                    if havessid then  
                        os.execute("cat /etc_ro/Wireless/default5g.dat >/etc/Wireless/iNIC/iNIC_ap.dat")
                        os.execute("cat ./wireless5g.dat >>/etc/Wireless/iNIC/iNIC_ap.dat")
                    end
                    if enable == "yes" and wifiswitch == "Yes" then
                        os.execute("ifconfig rai0 up &")
                    end
                    os.execute("uttwifiset5g.sh -s ".. apdata.ssid.. " -c "..wifidata.radio.channel.." -a ".. string.upper(apdata.auth).. " -p ".. apdata.key.. " -t ".. string.upper(apdata.encrypt) .."& ")
                end
                --file:write("Channel="..wifidata.radio.channel.."\n")
                --if wifidata.radio.channel == 0 then
                    --file:write("AutoChannelSelect=2")
                --else
                    --file:write("AutoChannelSelect=0")

                --end

                --file:close()
                --if havessid then  
                    --os.execute("cat ./default5g.dat >/etc/Wireless/iNIC/iNIC_ap.dat")
                    --os.execute("cat ./wireless5g.dat >>/etc/Wireless/iNIC/iNIC_ap.dat")
                --end
                --if enable == "yes" and wifiswitch == "Yes" then
                    --os.execute("ifconfig rai0 up &")
                --end
            end
        end
    end
end
end
function setWiFiswitch(data)
    if data and data.status then
        if data.status == "ON" then
            os.execute("uttwifiset5g.sh -e ON ")
            os.execute("uttwifiset.sh -e ON ")
        else
	        os.execute("uttwifiset.sh -e OFF ")
            os.execute("uttwifiset5g.sh -e OFF")
        end
    end
end

function setledswitch(data)
    print("set ledswitch:"..data.status)
end

function setWpsswitch(data)
    print("set Wpsswitch:"..data.status)
end

function setUpGrade(data)
    print("set wget :"..data.downurl)
    print("set reboot:"..data.isreboot)
end

function setWiFitimer(data)
    for i,timedata in (data) do
        print("set weekday:".. timedata.weekday)
        print("set time:"..timedata.time)
        print("set timerEnable:"..timedata.enable)
    end
end


