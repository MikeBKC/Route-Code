<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("WANConfig");%>
<%langTran("WANConfigSui");%>
<title><%write(com_notitle);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<link type="text/css" rel="stylesheet" href="css/jquery-ui-1.8.12.custom.css"/> 


<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script>  
<script type="text/javascript" src="script/jquery-ui-1.8.12.custom.min.js"></script> 
<script type="text/javascript" src="script/jquery.corner.js" ></script> 
<script type="text/javascript" src="script/frame.js" ></script>
<script type="text/javascript" src="js/common.js"></script>

</head>
  <script type="text/javascript">  
var guide = 0;//设置向导的页面 guide为1，向导页面  如果未定义 就是上网设置页面
var autotype = getQueryString("autotype");
var usermac = "";
var ConnTypes="PPPOE";var IPs="";var Masks="";var GateWays="";var MainDnss="";var SecDnss="";var mac="";var UserNames="";var PassWds="";var ppp_pppoeOPModes="KEEPALIVE";var ppp_pppoeDailModes="NORMAL";var ppp_MTUs="1480";var errorstr="";var rightstr="";var ppp_AuthTypess="EITHER";
<%aspWanConfig_Sui(); %>
<%aspoutDynConfig(); %>
<%outputChangemodel();%>
    $(document).ready(function(){ 
    if (autotype != null)
    {
	guide = 1;
    }
    if (autotype == 0)
    {
	var showpage=0;
	ConnTypes="PPPOE";
    }
    else if (autotype == 1)
    {
	var showpage=1;
	ConnTypes="DHCP";
    }
    else if(autotype == 2)
    {
	var showpage=1;
	ConnTypes="STATIC";
    }
    else
    {
	if (ConnTypes == "PPPOE")
	{
	    var showpage=0;
	}
	else
	{
	    var showpage=1;
	}
    }
	//var value=art.dialog.data('key'); 
	//if(value==2 || value==1){
	//	showpage=1;	
	//}

	 	
        $("#tabs").tabs({  
            collapsible:false,  
            selected:showpage,//设置默认展开的面板  
            event:"click",  
            fx:{  
                opacity:1,//设置透明度0--1    1代表完全不透明    0代表完全透明  
                height:"slideDown"//展开效果    既按哪种效果展开   toggle    fadeIn    fadeOut等  
            }//,  
            //disabled:[1,2],//1和2不可用  
        });  
        //修改选项卡  
      //  $('#tabs').tabs("url",2,"./tabs4.jsp");  
        //添加选项卡  
      //  $('#tabs').tabs("add","./tabs5.jsp","fifth"); 
	   $("li").corner("10px top");
	 	//guide=art.dialog.data('guide'); 
		//alert("guide:"+guide);
		if(guide=="1"){
			$(".tabs_link").show();	
			$("#pppoeOPMode_box").hide();
			$("#mtu_box").hide();
			$("#mac_box").hide();	
			$("#btn_box").hide();	
			$("#dhcp_mac").hide();
			$(".dhcp_mac_box").hide();
			$("#pppoeDailMode_box").hide();
			$("#advoption").hide();
 			$("#pdmethod").hide();
			
		}else{
			$(".tabs_link").hide();	
			$("#pppoeOPMode_box").show();	
			$("#mtu_box").show();	
			$("#mac_box").show();
			$("#btn_box").show();
			$("#dhcp_mac").show();	
			$(".dhcp_mac_box").show();
			$("#pppoeDailMode_box").show();
			$("#advoption").show();
 			$("#pdmethod").show();
		}
	   
	   
	   var radionIP = $('input[name="ipset"]:checked').val();
	  // alert("radionIP:"+radionIP);
	   if(radionIP=="dhcp"){
			$(".staticip_set").hide();  
	   }else{
			 $(".staticip_set").show(); 
	   }
	   //alert("test");
	   var arr_aplist = [ {  "ssid": "CAF597", "channel": 4, "rssi": 50, "bssid": "00:22:AA:CC:C9:60", "bgn_mode": "11n", "auth": "none" }, { "cwm": "HT40", "ssid": "ADMIN", "channel": 3, "rssi": 49, "bssid": "00:22:AA:CC:80:1C", "bgn_mode": "11n", "auth": "mixed-psk" }, { "ssid": "MT7620_AP", "channel": 1, "rssi": 47, "bssid": "00:0C:43:76:20:68", "bgn_mode": "11n", "auth": "none" }, { "cwm": "HT40", "ssid": "ssid_CK", "channel": 1, "rssi": 46, "bssid": "00:22:AA:CC:99:7C", "bgn_mode": "11n",
       "auth": "none" }, {"ssid": "HIPER_test12345", "channel": 9, "rssi": 44, "bssid": "00:22:AA:02:27:08", "bgn_mode": "11n", "auth": "none" }, {  "ssid": "YANFA_520W", "channel": 4, "rssi": 44, "bssid": "00:22:AA:E5:35:88", "bgn_mode": "11n", "auth": "mixed-psk" }, {"ssid": "HIPER_FFFFFFFF", "channel": 10, "rssi": 44, "bssid": "00:22:AA:02:27:08", "bgn_mode": "11n", "auth": "none" }, {"ssid": "ADMIN", "channel": 3, "rssi": 42, "bssid": "00:22:AA:CF:51:3C", "bgn_mode": "11n", "auth": "mixed-psk" }, { "ssid": "HIPER_C7EC15", "channel": 2, "rssi": 40, "bssid": "00:22:AA:C9:73:D4", "bgn_mode": "11n", "auth": "none" }, {"ssid": "mingcheng", "channel": 10, "rssi": 35, "bssid": "00:22:AA:CC:C9:8E", "bgn_mode": "11n", "auth": "none" }, {"ssid": "SHOUWANG", "channel": 11, "rssi": 30, "bssid": "00:22:AA:CC:C9:BA", "bgn_mode": "11n", "auth": "none" } ];
	  // alert("arr_aplist.length:"+arr_aplist.length);
		for (var i=0;i<arr_aplist.length;i++){
			//alert("11111");
				var	inp_v = arr_aplist[i]['ssid'];
				var	inp_rssi = arr_aplist[i]['rssi'];
				var	inp_channel = arr_aplist[i]['channel'];
				var	inp_encryption = arr_aplist[i]['auth'];
				var	inp_bssid = arr_aplist[i]['bssid'];
				var	inp_key = arr_aplist[i]['key'];
			//	alert("2222");
					if (!inp_key){
						inp_key = ""
					}
				//	alert("3333");
					var inp_show = inp_v;
					var sing_img = get_signal_url(inp_rssi);
					if (inp_v == "" || inp_encryption=="wep") {
						continue;
					}
			
					var tm_html = '';
					var class_html ='';
					var have_defult=true;
				//	if (rsp_bridge.ssid == inp_v) {
						//tm_html = tm_html+' selected="selected"';
					//	have_defult = true
					//}
			
					var class_html='';
					if (i == 0){
						class_html = ' frist_op';
					}
			
					var inp_html = '<option class="res_op'+class_html+'" value="'+inp_v+'" title="'+sing_img+'" id="'+inp_encryption+'|'+inp_channel+'|'+inp_bssid+'|'+inp_key+'"'+tm_html+'>'+inp_show+'</option>';
			try {
					$(".dd").click(function(){
						$(".dd .ddChild").css("overflow","auto");
					})
				} catch(e) {
					alert("Error: "+e.message);
				}
		
		}
function get_signal_url(signal){
	var d_signal_level;
	if(signal>40){
		d_signal_level=4;
	} else {
		d_signal_level = Math.floor(signal/10);
	}
	return 'images_Sui/signal'+d_signal_level+'.png';
}

 changeiptype("dhcp");
  showtables(0);
  $("#advoption").click(function(){
	  var showvalue = $("#advoptionshow").val();
	  showtables(showvalue);
	  
	  
	  })

    ADSL_Config.page_type.value = guide;
    WiringConnect_Config.page_type.value = guide;
    ADSL_Config.adsl_account.value = UserNames;
    ADSL_Config.adsl_pwd.value = PassWds;
    ADSL_Config.adsl_mac.value = mac;
    ADSL_Config.pppoeDailMode.value = ppp_pppoeDailModes;
    ADSL_Config.pppoeAuthType.value = ppp_AuthTypess;
    ADSL_Config.pppoeOPMode.value = ppp_pppoeOPModes;
    if (ppp_MTUs == "0")
    {
	ppp_MTUs = "1480";
    }
    ADSL_Config.mtu.value = ppp_MTUs;
    $("#adsl_error_msg").html(errorstr);
    WiringConnect_Config.dhcpmac.value = mac;
    WiringConnect_Config.ip.value = IPs;
    WiringConnect_Config.mask.value = Masks;
    WiringConnect_Config.gw.value = GateWays;
    WiringConnect_Config.priDns.value = MainDnss;
    WiringConnect_Config.seCDns.value = SecDnss;
    WiringConnect_Config.staticmac.value = mac;
    $("#con_error_msg").html(errorstr);
    if (ConnTypes == "STATIC")
    {
	changeiptype("static");
	$("input[name='ipset']").attr("checked","static");
    }
    showSuccessTips(rightstr);
    });
	function showtables(value){
		if(value==0){
			$("#pppoeDailMode_box").hide();	
			$("#pdmethod").hide();	
			$("#pppoeOPMode_box").hide();	
			$("#mtu_box").hide();
			$("#advoptionshow").val(1);
		}else{
			$("#pdmethod").show();	
			$("#pppoeDailMode_box").show();	
			$("#pppoeOPMode_box").show();	
			$("#mtu_box").show();
			$("#advoptionshow").val(0);	
		}
		
		}
	function changeiptype(value){
			//var rname=document.getElementsByName("ipset").value;
		//alert("rname:"+rname);
		if(value=="dhcp"){
			$(".staticip_set").hide(); 
			$("#dhcp_mac").show(); 
	   }else{
			 $(".staticip_set").show(); 
			 $("#dhcp_mac").hide();
			// $("#con_error_msg").html("输入错误！");
	   }
		
	} 
function checkData(value)
{
    if (value == 1)/*pppoe*/
    {
	if (!checkEnglishName_allowZero(ADSL_Config.adsl_account,31,"<%write(wansui_poename);%>",0)) return false;
	if (!checkPassword1(ADSL_Config.adsl_pwd,31,0,0)) return false;
	if (!checkMac(ADSL_Config.adsl_mac,"<%write(com_MAC);%>",0)) return false;
	if (!checkNumber(ADSL_Config.mtu,1,1492,"MTU",0)) return false;
    }
    else if (value == 2)
    {
	var radiovalue = $("input[name='ipset']:checked").val();
	if (radiovalue == "static")
	{
	    if (WiringConnect_Config.ip.value=="0.0.0.0"){ alert("<%write(com_checkIPAlert);%>");return false};
	    if (!checkIP(WiringConnect_Config.ip,"<%write(com_IP);%>",0)) return false;
	    if (!checkMask(WiringConnect_Config.mask,"<%write(com_NetMask);%>",0)) return false;
	    if (!checkIP(WiringConnect_Config.gw,"<%write(com_GateWayAddr);%>",0)) return false;
	    if (!checkSameSubnet(WiringConnect_Config.ip.value,WiringConnect_Config.gw.value,WiringConnect_Config.mask.value))
	    {
		alert("<%write(com_subnetAlert);%>");
		return false;
	    }
	    if (!checkMainDns(WiringConnect_Config.priDns,"<%write(wansui_MDNS);%>",0)) return false;
	    if (!checkIP(WiringConnect_Config.seCDns,"<%write(wansui_SDNS);%>",1)) return false;
	    if (!checkMac(WiringConnect_Config.staticmac,"<%write(com_MAC);%>",0)) return false;
	}
	else/*dhcp*/
	{
	    if (!checkMac(WiringConnect_Config.dhcpmac,"<%write(com_MAC);%>",0)) return false;
	}
    }   
    return true;
}
function send_msg()
{
    showWaitTips(1,2);
    window.parent.art.dialog.list['func_box'].close();
}
function save(value)
{
    if (checkData(value))
    {
	if(value == 1)
	{
	    $("#adsl_error_msg").html("<%write(wansui_wait);%>");
	    ADSL_Config.action="/goform/formWanIfConfig_Sui";
	    ADSL_Config.submit();
	}
	else if(value == 2)
	{
	    $("#con_error_msg").html("<%write(wansui_wait);%>");
	    WiringConnect_Config.action="/goform/formWanIfConfig_Sui";
	    WiringConnect_Config.submit();	
        if(ChangeModel == 1)
        {
            setTimeout("send_msg()",5000)
        }
	}
    }
}
function clone(value)
{
    if (usermac == "")
    {
	alert("<%write(wansui_usermac);%>");
    }
    else if (value == 1)
    {
	ADSL_Config.adsl_mac.value = usermac;
    }
    else if (value == 2)
    {
	WiringConnect_Config.staticmac.value = usermac;
    }
    else
    {
	WiringConnect_Config.dhcpmac.value = usermac;
    }
}
function jump()
{
    window.parent.open_windows(null,"guide_wifi");
}

function doDial(){
	ADSL_Config.action="/goform/formReConnect";
	ADSL_Config.submit();
}
function doDisconnect(i){
	ADSL_Config.action="/goform/formReleaseConnect";
	ADSL_Config.submit();
}
  </script>  

<body id="guide" style="background:#ffffff;">
<br><br>
<div id="tabs" class="group">
			<ul>
                <script>
                if(ChangeModel == 0)
                {
				    document.write('<li><a href="#tabs-1" style="outline:none;"><%write(wansui_pppoe);%></a></li>');

                }else{
				   document.write(' <li style="display:none"><a href="#tabs-1" style="outline:none;"><%write(wansui_pppoe);%></a></li>');
                }
                </script>
				<li><a href="#tabs-2" style="outline:none;"><%write(wansui_staordh);%></a></li>
			</ul>
                <!-- ADSL  START  -->
                <script>
                if(ChangeModel == 0)
                {
			        document.write('<div id="tabs-1">');
                }else{
			        document.write('<div id="tabs-1" style="display:none">');
                }
                </script>
                <div id="error_msg" style="color:red; height:15px;"></div>
            	<form action="" name="ADSL_Config" method="post">
                
            	<!-- asdl error message   -->
                <div id="adsl_error_msg" style="color:red; height:15px;"></div>
                <table border="0" cellspacing="0" cellpadding="0" class="adsl_tab">
			  <tr>
			    <th><%write(wansui_poename);%></th>
			    <td align="left">
					<input type="text"  class="txt-input" name="adsl_account"/>
			    </td>
			    <td>
				</td>
			  </tr>
              <tr>
			    <th><%write(wansui_poepass);%></th>
			    <td align="left">
			   		<input type="password"  class="txt-input" name="adsl_pwd"  />
					
			    </td>
			    <td>
			    </td>
		
               <tr id="mac_box">
			    <th><%write(com_MAC);%></th>
			    <td align="left">
			   		<input type="text"  class="txt-input" name="adsl_mac" onKeyPress="macOnly()" />
					
			    </td>
			    <td align="left">
                <input type="button" name=""  class="clone_btn" value="<%write(wansui_clone);%>" onClick="clone(1);"/>
			    </td>
			  </tr>
		<tr id="btn_box">
			    <td colspan="5" >
                <input type="button" name=""  class="clone_btn" value="<%write(wan_dial);%>" onClick="doDial(1);"/>&nbsp;&nbsp;
                <input type="button" name=""  class="clone_btn" value="<%write(wansui_hangup);%>" onClick="doDisconnect(1);"/>
			    </td>
			  </tr>
              <tr>
              	<th><span id="advoption" style="cursor:pointer;" ><strong><%write(wan_advance);%></strong></span></th>
                <input type="hidden" id="advoptionshow" value="0"/>
                <input type="hidden" id="Isp_Name" name="Isp_Name" value="1"/>
                <td>
			    </td>
                <td>
			    </td>
              </tr>
              
               <tr id="pdmethod"  style="display:none;">
			    <th><%write(com_checkPasswd);%></th>
			    <td align="left">
			   		<select name="pppoeAuthType" class="select">
                    <option value="NONE" >NONE</option>
                    <option value="PAP">PAP</option>
                    <option value="CHAP">CHAP</option>
                    <option value="EITHER" selected="selected">EITHER</option>
                </select>
					
			    	
			    </td>
			    <td>
			    </td>
			  </tr>

              	  <tr id="pppoeDailMode_box"  style="display:none;">
			    <th><%write(wan_dialMode);%></th>
			    <td align="left">
			   		<select  class="select" name="pppoeDailMode">
                	<option value="NORMAL" id=""><%write(wan_normal);%></option>
                    <option value="SP1" id=""><%write(wan_sp1);%></option>
                    <option value="SP2" id=""><%write(wan_sp2);%></option>
                    <option value="SP3" id=""><%write(wan_sp3);%></option>
                    </select>
					
			    	
			    </td>
			    <td>
			    </td>
			  </tr>
			   <tr id="pppoeOPMode_box"  style="display:none;">
			    <th><%write(wan_dialType);%></th>
			    <td align="left">
			   		<select  class="select" name="pppoeOPMode">
                	<option value="KEEPALIVE" id="auto"><%write(wan_auto);%></option>
                    <option value="DEMAND" id="demand"><%write(wan_demand);%></option>
                    <option value="MANUAL" id="menual"><%write(wan_menual);%></option>

                    </select>
					
			    	
			    </td>
			    <td>
			    </td>
			  </tr>
              <tr id="mtu_box" style="display:none;">
			    <th><%write(wansui_mtu);%></th>
			    <td align="left">
			   		<input type="text"  class="txt-input" name="mtu"  onKeyPress="figureOnly_P(event)"  />
					
			    </td>
			    <td>
			    </td>
			  </tr>
			</table>
		<input type="hidden" id="page_type" name="page_type" value="0"/>
		<input type="hidden" id="conn_type" name="conn_type" value="pppoe"/>
             <br>
            <div class="tabs_link"><span onClick="jump();"><%write(wansui_skipset);%> >></span>&nbsp;&nbsp;&nbsp;&nbsp;<span onClick="window.parent.art.dialog.list['func_box'].close();"><%write(com_exitset);%> >></span></div>
            <br>
            <div class="tabs_button"><input type="button" class="btn" name="back" value="<%write(com_save);%>" onClick="save(1);"/></div>
               </form>
            </div>
            <!-- ADSL  END-->
            
            <!-- WiringConnect  START  -->
			<div id="tabs-2">
            	<form action="" name="WiringConnect_Config" method="post">
                	<br>

                <div class="tabs_content"><input type="radio" name="ipset" checked value="dhcp" onClick="changeiptype(this.value);"/><%write(wansui_dhcp);%>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="radio" name="ipset" value="static" onClick="changeiptype(this.value);" /><%write(wansui_static);%></div>
                <div class="staticip_set">
                
               
                <div id="con_error_msg" style="color:red; height:15px;"></div><!--ERROR MESSAGE -->
                <table border="0" cellspacing="0" cellpadding="0" class="trunk" >
			  <tr>
			    <th><%write(com_IP);%></th>
			    <td>
					<input type="text"  class="txt-input" name="ip" onKeyPress="ipOnly_P(event)" maxlength="15"  />
			    </td>
			    <td>
				</td>
			  </tr>
              <tr>
			    <th><%write(com_NetMask);%></th>
			    <td align="left">
			   		<input type="text"  class="txt-input" name="mask" onKeyPress="ipOnly_P(event)" maxlength="15"  />
					
			    </td>
			    <td>
			    </td>
			  </tr>
			  <tr id="ssid_password_box"  style="display:;">
			    <th><%write(com_GateWayAddr);%></th>
			    <td align="left">
			   		<input type="text"  class="txt-input" name="gw" maxlength="15" onKeyPress="ipOnly_P(event)" />
					
			    	
			    </td>
			    <td>
			    </td>
			  </tr>
              <tr>
			    <th><%write(wansui_MDNS);%></th>
			    <td align="left">
			   		<input type="text"  class="txt-input" name="priDns" onKeyPress="ipOnly_P(event)" maxlength="15" />
					
			    	
			    </td>
			    <td>
			    </td>
			  </tr>
			  <tr>
			    <th><%write(wansui_SDNS);%></th>
			    <td align="left">
			   		<input type="text"  class="txt-input" name="seCDns" onKeyPress="ipOnly_P(event)"  maxlength="15" />	
			    </td>
			    <td>
			    </td>
			  </tr>
               <tr class="dhcp_mac_box"  style="display:;">
			    <th><%write(com_MAC);%></th>
			    <td align="left">
			   		<input type="text"  class="txt-input" name="staticmac"  onKeyPress="macOnly()" />
			    </td>
			    <td align="left">
                 <input type="button" name=""  class="clone_btn" value="<%write(wansui_clone);%>" onClick="clone(2);"/>
			    </td>
			  </tr>
			</table>
            	<br>
              </div> 
            
              <div id="mac_error_msg" style="color:red;height:15px;"></div>
               <table border="0" cellspacing="0" cellpadding="0" class="trunk" id="dhcp_mac" name="dhcp_mac" style="padding-left:80px;" >
			  <tr class="dhcp_mac_box">
			    <th><%write(com_MAC);%></th>
			    <td>
					<input type="text"  class="txt-input" name="dhcpmac"  onKeyPress="macOnly()" />
			    </td>
			    <td align="left">
                <input type="button" name=""  class="clone_btn" value="<%write(wansui_clone);%>" onClick="clone(3);"/>
				</td>
			  </tr>
              </table>
		<input type="hidden" id="page_type" name="page_type" value="0"/>
		<input type="hidden" id="conn_type" name="conn_type" value="static"/>
            	<br>
             <div class="tabs_link"><span onClick="jump();"><%write(wansui_skipset);%> >></span>&nbsp;&nbsp;&nbsp;&nbsp;<span onClick="window.parent.art.dialog.list['func_box'].close();"><%write(com_exitset);%> >></span></div>
                <br>
                <div class="tabs_button"><input type="button" class="btn" name="back" onClick="save(2);" value="<%write(com_save);%>"/></div> 
                </form>
            </div>
             <!-- WiringConnect  END  -->
		</div>
 
</body>
</html>
