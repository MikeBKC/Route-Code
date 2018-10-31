<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("FastConfig_OpMode"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>

<% OutputFastOpMode();%>
<% OutputConnNum();%>

var errorstr="";
var objName;
function dopre(){
		  var f=FastConfig_OpMode;
							if(f.opModeWAN1.checked)
							f.opMode1.value = 1;
							else
							f.opMode1.value ="";
							
						 
							if(f.opModeWAN2.checked)
							f.opMode2.value = 2;
							else
							f.opMode2.value ="";
						 
							if(f.opModeG3.checked)
							f.opMode4.value = 3;
							else{
							f.opMode4.value ="";
							f.connectionType3.value="";
							}
						 
							if(f.opModeWLAN.checked)
							f.opMode3.value = 4;
							else{
							f.opMode3.value ="";
							f.connectionType4.value="";
							}
			 
	FastConfig_OpMode.connectionType_OpMode.value = "on";
	FastConfig_OpMode.action="FastConfig.asp";
	FastConfig_OpMode.submit();
}
function goaway(){
	parent.main.location="SysInfo.asp";
}
function donext(){
		var f=FastConfig_OpMode;
//	    f.oldopMode.value=OpModes;
							if(f.opModeWAN1.checked)
							f.opMode1.value = 1;
							else{
								f.opMode1.value ="";
								f.connectionType.value="";
							}
							
							if(f.opModeWAN2.checked)
							f.opMode2.value = 2;
							else{
								f.opMode2.value ="";
								f.connectionType2.value="";
							}
					 
							if(f.opModeG3.checked)
							f.opMode4.value = 3;
							else
							f.opMode4.value ="";
						 
//							if(f.opModeWLAN.checked)
//							f.opMode3.value = 4;
//							else
							f.opMode3.value ="";
						 
		if(!FastConfig_OpMode.opMode1.value == ""){
				FastConfig_OpMode.opMode.value = FastConfig_OpMode.opMode1.value;
				FastConfig_OpMode.action="FastConfig_Net.asp";
		
		
		}else{
				if(!FastConfig_OpMode.opMode2.value == "")
				{
							FastConfig_OpMode.opMode.value = FastConfig_OpMode.opMode2.value;
							FastConfig_OpMode.action="FastConfig_Net2.asp";
				}
				else
				{
					 if(!FastConfig_OpMode.opMode4.value == "")
					 {
							 FastConfig_OpMode.opMode.value = FastConfig_OpMode.opMode4.value;
							 FastConfig_OpMode.action="FastConfig_Net_3G.asp";
					 }else{
//							 if(!FastConfig_OpMode.opMode3.value == ""){
//									 FastConfig_OpMode.opMode.value = FastConfig_OpMode.opMode3.value;
//									 FastConfig_OpMode.action="FastConfig_Net_WLAN.asp";
//							 }else{
										  FastConfig_OpMode.opMode.value = FastConfig_OpMode.opMode1.value;
										  FastConfig_OpMode.action="FastConfig_Wlan.asp";
//							  }
					 } 
				}
			
		}
		f.connectionType_OpMode.value = "on";
		FastConfig_OpMode.submit();
}


function init(){
    var f=FastConfig_OpMode;
    objName= document.getElementById("FastConfig_OpMode")   
	var options=0; 
    if(f.connectionType_OpMode.value != ""){
	if(f.opMode1.value != "" ||f.opMode2.value != "" || f.opMode3.value != ""|| f.opMode4.value != "")
			 {		
			 		if(f.opMode1.value != "")
						{
							options++;
							f.opModeWAN1.checked = 1;
							 f.opMode1.value = "<% write(Form("opMode1"));%>";
						 }
					 if(f.opMode2.value != "")
						{
							options++;
							f.opModeWAN2.checked = 1;
							 f.opMode2.value = "<% write(Form("opMode2"));%>";
						 }
					if(f.opMode4.value != ""){
							options++;
							f.opModeG3.checked = 1;
							f.opMode4.value = "<% write(Form("opMode4"));%>";
						}
//					 if(f.opMode3.value != ""){
//							f.opModeWLAN.checked = 1;
//							f.opMode3.value = "<% write(Form("opMode3"));%>";
//						 
//						}	
	}
    }
    else
    {
	if(ConnTypes[0] != ""){
		options++;
	    f.opModeWAN1.checked = 1;
	    f.opMode1.value = ConnTypes[0];
	}	
	/*
	if(Wan2ConnModes != ""){
	    f.opModeWAN2.checked = 1;
	    f.opMode2.value = Wan2ConnModes;
	}
	*/
        if(ConnTypes[slot3G - 1] != ""){
		options++;
	    f.opModeG3.checked = 1;
	    f.opMode4.value = ConnTypes[slot3G - 1];
	}
//	if(ConnTypes[slotAPCli - 1] != ""){
//	    f.opModeWLAN.checked = 1;
//	    f.opMode3.value = ConnTypes[slotAPCli - 1];
//	}
    }
}

function Reset()
{
	FastConfig_OpMode.reset();
	FastConfig_OpMode.connectionType_OpMode.value = "";
	init();
	/* objName= document.getElementById("FastConfig_OpMode");
	 objName[0].checked = true;
	 objName[1].checked = 0;
	 objName[2].checked = 0;
	 objName[3].checked = 0;
	 FastConfig_OpMode.opMode1.value = "1";
	 FastConfig_OpMode.opMode2.value = "";
	 FastConfig_OpMode.opMode3.value = "";
	 FastConfig_OpMode.opMode4.value = "";*/
}
//-->
</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
    <!--*****顶上间隔****-->
    <tr>
        <td colspan="3">&nbsp;</td>
    </tr>
    <!--****中间内容****-->
    <tr>
        <td width="15"></td>
        <!--////内容-->
        <td><script language="javascript">showSecondTitle("<% write(SecondTitle); %>");</script>
            <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
                <script language="javascript">showerror(errorstr);</script>
                <tr valign="top">
                    <td><form  name="FastConfig_OpMode" method="post">
                            <p>&nbsp;</p>
                            <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
                                <tr>
                                    <td align="center" class="style7"><table border="0" width="600" align='center'>
                                            <tr>
                                                <td colspan="3" height="20">&nbsp;</td>
                                            </tr>
                                            <tr>
                                                <td align="right" width="39%" height="20"><% write(SelectOpMode); %></td>
                                                <td>&nbsp;</td>
                                            </tr>
                                            <tr>
                                                <td width="39%">&nbsp;</td>
                                                <td align="right" height="20" width="6%"><input type="checkbox" name="opMode"  id="opModeWAN1" value="1" >
                                                    &nbsp;&nbsp;</td>
                                                <td width="55%"><% write(WAN1Conf); %></td>
                                            </tr>
                                            <tr style="display:none">
                                                <td width="39%">&nbsp;</td>
                                                <td align="right" height="20" width="6%"><input type="checkbox" name="opMode"  id="opModeWAN2" value="2" >
                                                    &nbsp;&nbsp;</td>
                                                <td><% write(WAN2Conf); %></td>
                                            </tr>
                                            <tr>
                                                <td>&nbsp;</td>
                                                <td align="right" height="20"><input type="checkbox" name="opMode"  id="opModeG3" value="4" >
                                                    &nbsp;&nbsp;</td>
                                                <td><% write(G3ClientConf); %></td>
                                            </tr>
                                            <tr>
                                                <td colspan="3" height="20">&nbsp;</td>
                                            </tr>
                                            <tr>
                                                <td colspan="3"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
                                                        <tr>
                                                            <td  height="30" align="center"><input name="button" type="button" class="input-button" onClick="dopre()" value="<% write(com_previous); %>" id="last">
                                                                &nbsp;&nbsp;&nbsp;
                                                                <input name="button" type="button" class="input-button" onClick="Reset()" value="<% write(com_reset); %>" id="uttreset">
                                                                &nbsp;&nbsp;&nbsp;
                                                                <input name="button" type="button" class="input-button" onClick="goaway()" value="<% write(LeavePage); %>" id="leave">
                                                                &nbsp;&nbsp;&nbsp;
                                                                <input name="button" type="button" class="input-button" onClick="donext()" value="<% write(com_next); %>" id="next">
                                                                <input type="hidden" name="connectionType_OpMode"  value="<% write(Form("connectionType_OpMode"));%>" ></td>
                                                            <input type="hidden" name="notPopUp" value="<% write(Form("notPopUp"));%>" >
                                                            <input type="hidden" name="oldopMode">
                                                            <input type="hidden" name="options" value="1">
                                                            <input type="hidden" name="opMode1"  value="<% write(Form("opMode1"));%>" >
                                                            <input type="hidden" name="opMode2"  value="<% write(Form("opMode2"));%>">
                                                            <input type="hidden" name="opMode3"  value="<% write(Form("opMode3"));%>">
                                                            <input type="hidden" name="opMode4"  value="<% write(Form("opMode4"));%>">
                                                            <input type="hidden" name="connectionType" value="<% write(Form("connectionType")); %>">
                                                            <input type="hidden" name="staticIp" value="<% write(Form("staticIp")); %>">
                                                            <input type="hidden" name="staticNetmask" value="<% write(Form("staticNetmask")); %>">
                                                            <input type="hidden" name="staticGateway" value="<% write(Form("staticGateway")); %>">
                                                            <input type="hidden" name="staticPriDns" value="<% write(Form("staticPriDns")); %>">
                                                            <input type="hidden" name="staticSecDns" value="<% write(Form("staticSecDns")); %>">
                                                            <input type="hidden" name="pppoeUser" value="<% write(Form("pppoeUser")); %>">
                                                            <input type="hidden" name="pppoePass" value="<% write(Form("pppoePass")); %>">
                                                            <input type="hidden" name="connectionType2" value="<% write(Form("connectionType2")); %>">
                                                            <input type="hidden" name="staticIp2" value="<% write(Form("staticIp2")); %>">
                                                            <input type="hidden" name="staticNetmask2" value="<% write(Form("staticNetmask2")); %>">
                                                            <input type="hidden" name="staticGateway2" value="<% write(Form("staticGateway2")); %>">
                                                            <input type="hidden" name="staticPriDns2" value="<% write(Form("staticPriDns2")); %>">
                                                            <input type="hidden" name="staticSecDns2" value="<% write(Form("staticSecDns2")); %>">
                                                            <input type="hidden" name="pppoeUser2" value="<% write(Form("pppoeUser2")); %>">
                                                            <input type="hidden" name="pppoePass2" value="<% write(Form("pppoePass2")); %>">
                                                            <input type="hidden" name="connectionType3" value="<% write(Form("connectionType3"));%>" >
                                                            <input type="hidden" name="Dev3G" value="<% write(Form("Dev3G"));%>" >
                                                            <input type="hidden" name="ISP3G" value="<% write(Form("ISP3G"));%>" >
                                                            <input type="hidden" name="AuthType3G" value="<% write(Form("AuthType3G"));%>" >
                                                            <input type="hidden" name="PinCode" value="<% write(Form("PinCode"));%>" >
                                                            <input type="hidden" name="AuthName" value="<% write(Form("AuthName"));%>" >
                                                            <input type="hidden" name="AuthPasswd" value="<% write(Form("AuthPasswd"));%>" >
                                                            <input type="hidden" name="ConnName" value="<% write(Form("ConnName"));%>" >
                                                            <input type="hidden" name="PPPPasswd" value="<% write(Form("PPPPasswd"));%>" >
                                                            <input type="hidden" name="DialNum" value="<% write(Form("DialNum"));%>" >
                                                            <input type="hidden" name="PPPName" value="<% write(Form("PPPName"));%>" >
                                                            <input type="hidden" name="connectionType4" value="<% write(Form("connectionType4"));%>" >
                                                            <input type="hidden" name="APssid" value="<% write(Form("APssid"));%>" >
                                                            <input type="hidden" name="wds_list" value="<% write(Form("wds_list"));%>" >
                                                            <input type="hidden" name="apcli_mode" value="<% write(Form("apcli_mode"));%>" >
                                                            <input type="hidden" name="apcli_enc" value="<% write(Form("apcli_enc"));%>" >
                                                            <input type="hidden" name="apclikeyFormat" value="<% write(Form("apclikeyFormat"));%>" >
                                                            <input type="hidden" name="apclikeynum" value="<% write(Form("apclikeynum"));%>" >
                                                            <input type="hidden" name="apcliwepkey1" value="<% write(Form("apcliwepkey1"));%>" >
                                                            <input type="hidden" name="apcliwepkey2" value="<% write(Form("apcliwepkey2"));%>" >
                                                            <input type="hidden" name="apcliwepkey3" value="<% write(Form("apcliwepkey3"));%>" >
                                                            <input type="hidden" name="apcliwepkey4" value="<% write(Form("apcliwepkey4"));%>" >
                                                            <input type="hidden" name="apclikeyLength1" value="<% write(Form("apclikeyLength1"));%>" >
                                                            <input type="hidden" name="apclikeyLength2" value="<% write(Form("apclikeyLength2"));%>" >
                                                            <input type="hidden" name="apclikeyLength3" value="<% write(Form("apclikeyLength3"));%>" >
                                                            <input type="hidden" name="apclikeyLength4" value="<% write(Form("apclikeyLength4"));%>" >
                                                            <input type="hidden" name="apclipskPsswd" value="<% write(Form("apclipskPsswd"));%>" >
                                                            <input type="hidden" name="connectionType5" value="<% write(Form("connectionType5"));%>" >
                                                            <input type="hidden" name="ssid" value="<% write(Form("ssid"));%>" >
                                                            <input type="hidden" name="wrlessMode" value="<% write(Form("wrlessMode"));%>" >
                                                            <input type="hidden" name="channel" value="<% write(Form("channel"));%>" >
                                                            <input type="hidden" name="chanWidth" value="<% write(Form("chanWidth"));%>" >
                                                            <input type="hidden" name="wrlesMode_chanWid" value="<% write(Form("wrlesMode_chanWid"));%>" >
                                                        </tr>
                                                    </table></td>
                                            </tr>
                                            <script language="javascript">init();</script>
                                            <tr>
                                                <td colspan="3" height="20">&nbsp;</td>
                                            </tr>
                                        </table></td>
                                </tr>
                            </table>
                        </form></td>
                </tr>
            </table></td>
        <td width="15"></td>
    </tr>
</table>
</body>
</html>
