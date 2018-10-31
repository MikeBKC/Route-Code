<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("editTemp");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
 //交互begin
<% 
	aspOutPutApConfTempList();
	var edit_Name="";
	edit_Name=Form("oldTmpName");
	if (edit_Name != "") getOneApConfTempEntry(edit_Name);
%>
var edit_Names = "<% write(Form("oldTmpName"));%>";
// 交互end
/*var tmpname = new Array("134");
var wlFres = new Array("2.4GHz");
var wlModes = new Array("11b/g/n");
var wlRates = new Array("自动");
var totalrecs = 1;var errorstr="";var tempName = "134";
var wireless = true;
var sgi = true;
var preamble = true;
var wmm = true;
var rate = 0;
var bw = 0;
var wlFre = 0;
var channel = 10;
var power = 1;
var manPower = 2;
var mode = 3;
var errorstr="";

var beacon =111;*/
//var edit_Names = "134";
function init()
{
	var f=confTemp;
	var oChannel = document.getElementById("channel");
	var oMode = document.getElementById("mode");
    var oVHT = document.getElementById("VHT");
    var oVHT2 = document.getElementById("VHT2");
	
	if(edit_Names!=""&&edit_Names!=null)
	{   
		f.tempName.value=tempName;
		f.oldEntry.value=edit_Names;
		document.getElementById('tempName').onkeydown = function (e) { if(e ? e.which : event.keyCode==8) return false;}
		
		document.getElementById('tempName').readOnly = true;
		f.en_wireless.checked=wireless;
		f.rate.value=rate;
		f.SGI.checked=sgi;
		f.BW.value=bw;
		f.VHTBW.value=VHTBW;
		f.wlFre.value=wlFre;
		f.channel.value=channel;
		f.preamble.checked=preamble;
		f.power.value=power;
		f.manual.value=manPower;
		f.wmm.checked=wmm;
		f.mode.value=mode;
		f.BeaconPeriod.value =beaconPeriod;
		document.getElementById('en_wireless').checked = wireless;
		document.getElementById('SGI').checked = sgi;
		document.getElementById('wmm').checked = wmm;
		document.getElementById('preamble').checked = preamble;
		
		if(wlFre==0){
			document.getElementById("wireRate_td").style.display="block";
			document.getElementById("wireRate1_td").style.display="block";
			document.getElementById("VHT_TD").style.display="none";
			document.getElementById("VHT2_TD").style.display="none";
			oChannel.options.length = 0;
			oChannel.options.add(new Option("自动","0")); 
			for(i=1;i<12;i++){
				oChannel.options.add(new Option(i,i)); 
			}
			
			oMode.options.length = 0;
			oMode.options.add(new Option("11b/g/n混合","3"));
			oMode.options.add(new Option("仅11g","1"));
			oMode.options.add(new Option("仅11n","2"));

		}else if(wlFre==1){
			document.getElementById("wireRate_td").style.display="none";
			document.getElementById("wireRate1_td").style.display="none";
			document.getElementById("VHT_TD").style.display="block";
			document.getElementById("VHT2_TD").style.display="block";
			oChannel.options.length = 0;
			oChannel.options.add(new Option("自动","0")); 
			oChannel.options.add(new Option("36","36")); 
			oChannel.options.add(new Option("40","40")); 
			oChannel.options.add(new Option("44","44")); 
			oChannel.options.add(new Option("48","48")); 
			oChannel.options.add(new Option("149","149")); 
			oChannel.options.add(new Option("153","153")); 
			oChannel.options.add(new Option("157","157")); 
			oChannel.options.add(new Option("161","161")); 
			oChannel.options.add(new Option("165","165")); 
			oMode.options.length = 0;
			oMode.options.add(new Option("仅11a","4"));
			oMode.options.add(new Option("11a/n混合","5"));
			oMode.options.add(new Option("11vht AC/AN/A","6"));
			oMode.options.add(new Option("11vht AC/AN","7"));
		}
		
		if((mode == 6)||(mode == 7)){
			oVHT.style.display="";
			oVHT2.style.display="";
		}
		else{
			oVHT.style.display="none";
			oVHT2.style.display="none";
		}
		document.getElementById("mode").value = mode; 
		document.getElementById("channel").value = channel;
		document.getElementById("BW").value = bw;
		
	}else{
			oChannel.options.length = 0;
			oChannel.options.add(new Option("自动","0")); 
			for(i=1;i<12;i++){
				oChannel.options.add(new Option(i,i)); 
			}
			oMode.options.length = 0;
			oMode.options.add(new Option("11b/g/n混合","3"));
			oMode.options.add(new Option("仅11g","1"));
			oMode.options.add(new Option("仅11n","2"));
			oVHT.style.display="none";
			oVHT2.style.display="none";
			
	}
	
	manual_sw();
}

function manual_sw()
{
	if (document.getElementById('power').value == 1)
	{
		document.getElementById('manual').style.display = "block";
	}
	else
	{
		document.getElementById('manual').style.display = "none";
	}
	
}

function checkData()
{
	var f=confTemp;
	if(!checkName(f.tempName,11,"<%write(modeName);%>",0)) return false;	//检查用户名：不能为空、不能全为0、不能超11个字符、应为除:< > , % \' \\ \" & ; 和空格之外的字符。

    var tempmode = document.getElementById('mode').value;
    var tempbeacon = document.getElementById('BeaconPeriod').value;

	var mode = document.getElementById("wlFre").value;//无线频段 2.4
	if(mode==0){
    var temprate = document.getElementById('rate').value;

    if ( tempmode == 1 ){
        if( temprate != 0 && temprate != 54 ){
            alert(_("<%write(mode_rate_not_match);%>"));
            return false;
        }
    }else if ( tempmode == 2 ){
        if( temprate != 0 && temprate != 150 && temprate != 300){
            alert(_("<%write(mode_rate_not_match);%>"));
            return false;
        }
    }else if ( tempmode == 3 ){
        if( temprate != 0 ){
            alert(_("<%write(mode_rate_not_match2);%>"));
            return false;
        }
    }
	}
    /*
    if(tempbeacon < 20 || tempbeacon > 999){
	alert(_("<%write(beacon_tips);%>"));
	return false;
    }*/
	
    if (!checkNumber(f.BeaconPeriod,20,999,_("<%write(beacon_tips);%>"),0)) 
    {
	return false;
    }
    return true;
}

function save()
{
	var f = confTemp;
	var i;
	if (edit_Names == "")
	{
		for (i=0; i < tmpname.length; i++)
		{
			if (tmpname[i] == confTemp.tempName.value)
			{
				alert(_("<%write(warning14);%>"));
				return;
			}
		}
	}
	if (checkData())
	{
		f.en_wireless.value = (f.en_wireless.checked == true ? 1 : 0);
		f.SGI.value = (f.SGI.checked == true ? 1 : 0);
		f.preamble.value = (f.preamble.checked == true ? 1 : 0);
		f.wmm.value = (f.wmm.checked == true ? 1 : 0);
		f.action="/goform/formConfigApConfTemp";
		f.submit();	
	}
}

function Reset()
{
	confTemp.reset();
	init();
}

function changeWlFre(val){
	var oChannel = document.getElementById("channel");
	var oMode = document.getElementById("mode");
	if(val.selectedIndex == 0){//2.5G
		document.getElementById("wireRate_td").style.display="block";
		document.getElementById("wireRate1_td").style.display="block";
		document.getElementById("VHT_TD").style.display="none";
		document.getElementById("VHT2_TD").style.display="none";
		oChannel.options.length = 0;
		oChannel.options.add(new Option("自动","0")); 
		for(i=1;i<12;i++){
			oChannel.options.add(new Option(i,i)); 
			}
		oMode.options.length = 0;
		oMode.options.add(new Option("11b/g/n混合","3"));
		oMode.options.add(new Option("仅11g","1"));
		oMode.options.add(new Option("仅11n","2"));
		
	}
	else if(val.selectedIndex == 1){//5G

			document.getElementById("wireRate_td").style.display="none";
		document.getElementById("wireRate1_td").style.display="none";
		document.getElementById("VHT_TD").style.display="block";
		document.getElementById("VHT2_TD").style.display="block";
			oChannel.options.length = 0;
			oChannel.options.add(new Option("自动","0")); 
			oChannel.options.add(new Option("36","36")); 
			oChannel.options.add(new Option("40","40")); 
			oChannel.options.add(new Option("44","44")); 
			oChannel.options.add(new Option("48","48")); 
			oChannel.options.add(new Option("149","149")); 
			oChannel.options.add(new Option("153","153")); 
			oChannel.options.add(new Option("157","157")); 
			oChannel.options.add(new Option("161","161")); 
			oChannel.options.add(new Option("165","165")); 
			oMode.options.length = 0;
			oMode.options.add(new Option("11a/n混合","5"));
			oMode.options.add(new Option("仅11a","4"));
			//oMode.options.add(new Option("11a/n混合","5"));
			oMode.options.add(new Option("11vht AC/AN/A","6"));
			oMode.options.add(new Option("11vht AC/AN","7"));
	}
	
}
function changeMode(){
	var oMode = document.getElementById("mode");
    var oVHT = document.getElementById("VHT");
    var oVHT2 = document.getElementById("VHT2");
	
	
	if((oMode.value == 6)||(oMode.value == 7)){
			oVHT.style.display="";
			oVHT2.style.display="";
	}else{
			oVHT.style.display="none";
			oVHT2.style.display="none";
	}
}	
</script>
</head>
<body onLoad="init()" onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("<%write(APSettingMode);%>"));//显示页面名称</script>
  <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">//showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						2,
						new Array(_("<%write(modeList);%>"),'apTmp.asp',1,0),
						new Array(_("<%write(editMode);%>"),'editTemp.asp',1,1)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<br>
<form name="confTemp" method="post" action="">
<input type="hidden" name="oldEntry">
<table width="400" border="0" cellspacing="1" cellpadding="4" class="style22">
  <tr>
    <td width="13%" id="modeName_td"><%write(modeName);%></td>
    <td width="16%"><input type="text" name="tempName" id="tempName" size="10" class="input"></td>
    <td width="13%" id="wlFre_td"><%write(WlFre);%></td>
    <td width="16%"><select name="wlFre" id="wlFre" class="input" style="width:90px" onChange="changeWlFre(this);">
      <option value="0" selected>2.4G Hz</option>
      <option value="1">5G Hz</option>
    </select></td>
  </tr>
</table>
<br>
<table width="600" border="0" cellspacing="1" cellpadding="4" class="style22">
  <tr class="style18">
    <td width="140" id="wireFC_td"><%write(startwire);%></td>
    <td width="140"><input name="en_wireless" type="checkbox" id="en_wireless" checked></td>

    <td width="100" id="wireMode_td">
      <%write(wireMode);%></td>
    <td>
      <select name="mode" id="mode" class="input" style="width:90px" onChange="changeMode()">
        
      </select>
     </td>
	 
	 
  </tr>
  <tr> 
    <td>Short Guard Interval</td>
    <td><input name="SGI" type="checkbox" id="SGI" checked></td>
    <!--<td id="channelBW_td"><%write(channelBandWidth);%></td>
    <td><select name="BW" id="BW" class="input" style="width:90px">
      <option value="1" id="auto"><%write(auto);%></option>
      <option value="0">20M</option>
    </select></td>-->
     <td id="channel_td"><%write(channel);%></td>
    <td>
      <select name="channel" id="channel" class="input" style="width:90px">
       <!-- <option value="0" id="auto"><%write(auto);%></option>-->
       <!-- <option value="1">1</option>
        <option value="2">2</option>
        <option value="3">3</option>
        <option value="4">4</option>
        <option value="5">5</option>
        <option value="6">6</option>
        <option value="7">7</option>
        <option value="8">8</option>
        <option value="9">9</option>
        <option value="10">10</option>
        <option value="11">11</option>-->
      </select>
    </td>
  </tr>
  <tr class="style18">
    <td id="openWMM_td"><%write(startWMM);%></td>
    <td>
      <input name="wmm" type="checkbox" id="wmm" checked></td>
    <!--<td id="channel_td"><%write(channel);%></td>
    <td>
      <select name="channel" id="channel" class="input" style="width:90px">
        <option value="0" id="auto"><%write(auto);%></option>
        <option value="1">1</option>
        <option value="2">2</option>
        <option value="3">3</option>
        <option value="4">4</option>
        <option value="5">5</option>
        <option value="6">6</option>
        <option value="7">7</option>
        <option value="8">8</option>
        <option value="9">9</option>
        <option value="10">10</option>
        <option value="11">11</option>
      </select>
    </td>-->
    <td id="channelBW_td"><%write(channelBandWidth);%></td>
    <td><select name="BW" id="BW" class="input" style="width:90px">
      <option value="1" id="auto"><%write(auto);%></option>
      <option value="0">20M</option>
    </select></td>
  </tr>
  <tr style="background-color:white;">
    <td>Short Preamble</td>
    <td><input name="preamble" type="checkbox" id="preamble" checked></td>
	<td id="wireRate_td"><%write(wirespeed);%></td>
    <td id="wireRate1_td"><select name="rate" id="rate" class="input" style="width:90px">
      <option value="0" selected id="auto"><%write(auto);%></option>
      <option value="11">11M</option>
      <option value="54">54M</option>
      <option value="150">150M</option>
      <option value="300">300M</option>
    </select></td>
     <td id="VHT_TD" style="display:none;"><div id="VHT">VHT频道带宽</div></td>
     <td id="VHT2_TD" style="display:none;"><div id="VHT2"><select name="VHTBW" id="VHTBW" class="input" style="width:90px">
      <option value="0" id="auto">20/40</option>
      <option value="1">80M</option>
    </select></div></td>
    </tr>
  <tr class="style18">
    <td>
      Beacon 间隔</td>
    <td><input type="text" name="BeaconPeriod" id="BeaconPeriod" size="10" class="input" value=100></td>
	  <td id="wirePower_td"><%write(wireTranPower);%></td>
    <td>
    	<table width="100%" border="0" cellspacing="0" cellpadding="0" class="style22">
        	<tr>
            	<td nowrap>
                    <select name="power" id="power" class="input" style="width:90px" onChange="manual_sw();">
                      <option value="0" id="auto" selected><%write(auto);%></option>
                      <option value="1" id="manu"><%write(manu);%></option>
                    </select>
                </td>
                <td nowrap>
                    <select name="manual" id="manual" class="input" style="width:40px">
                        <option value="1" id="low"><%write(low);%></option>
                        <option value="2" id="mid"><%write(middle);%></option>
                        <option value="3" id="hight"><%write(hight);%></option>
                    </select>
          		</td>
          	</tr>
      </table>
      </td>
	
  </tr>
</table>
<table align="center"  ><tr><td height="5"></td></tr><tr><td>
<input type="button"  onClick="save()"class="input-button" value="<%write(save);%>" name="" id="savebutton">&nbsp;
<input type="button" class="input-button" value="<%write(refill);%>" name="" onClick="Reset();" id="resetbutton">
<input type="button" class="input-button" value="<%write(help);%>" name="" onClick="window.open('<%write(com_helppage);%>#apTmp')" id="helpbutton">
</td></tr></table>
</form>
<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
</td><td class="gapTd"></td></tr>
  </table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
  <td width="15">&nbsp;</td>
  </tr>
</table>
</body>
</html>
