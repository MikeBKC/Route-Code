<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("commonWords");%>
<%langTran("wlanPara");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
//交互begin
<% 
aspOutPutFitApLocalConfTemp();
aspOutPutApEffectConf();
//	write('var wireless = false;\n');
//	write('var ssid = "SSID_A";\n');
//	write('var rate = 0;\n');
//	write('var sgi = true;\n');
//	write('var bw = 0;\n');
//	write('var channel = 0;\n');
//	write('var preamble = true;\n');
//	write('var power = 1;\n');
//	write('var manPower = 1;\n');
//	write('var wmm = true;\n');
//	write('var mode = 3;\n');
//	write('var errorstr = "";\n');
%>
//var maxRate = 4;

// 交互end

function init()
{
	var f=confTemp;
	if (config == 0)	//下发
	{
		option=document.createElement("OPTION");
		option.value = "100";
		option.text = "";
		f.power.options.add(option);
		option=document.createElement("OPTION");
		option.value = "100";
		option.text = "";
		f.rate.options.add(option);
		option=document.createElement("OPTION");
		option.value = "100";
		option.text = "";
		f.BW.options.add(option);
		option=document.createElement("OPTION");
		option.value = "100";
		option.text = "";
		f.channel.options.add(option);
		option=document.createElement("OPTION");
		option.value = "100";
		option.text = "";
		f.manual.options.add(option);
		option=document.createElement("OPTION");
		option.value = "100";
		option.text = "";
		f.mode.options.add(option);

		f.en_wireless.checked=false;
		f.en_wireless.disabled=true;
		f.ssid.value="";
		f.ssid.disabled=true;
		f.rate.value="100";
		f.rate.disabled=true;
		f.SGI.checked=false;
		f.SGI.disabled=true;
		f.BW.value="100";
		f.BW.disabled=true;
//		f.isolate.checked=false;
//		f.isolate.disabled=true;
		f.channel.value="100";
		f.channel.disabled=true;
		f.preamble.checked=false;
		f.preamble.disabled=true;
		f.power.value="100";
		f.power.disabled=true;
		f.manual.value="100";
		f.manual.disabled=true;
		f.wmm.checked=false;
		f.wmm.disabled=true;
		f.mode.value="100";
		f.mode.disabled=true;
		document.getElementById('en_wireless').checked = false;
		document.getElementById('en_wireless').disabled = true;
		document.getElementById('SGI').checked = false;
		document.getElementById('SGI').disabled = true;
		document.getElementById('wmm').checked = false;
		document.getElementById('wmm').disabled = true;
		document.getElementById('preamble').checked = false;
		document.getElementById('preamble').disabled = true;
		document.getElementById('manual').style.display = "none";
		document.getElementById('uttsave').disabled = true;
		document.getElementById('uttreset').disabled = true;
	}
	else	//本地
	{
		f.en_wireless.checked=wireless;
		f.ssid.value=ssid;
		//f.rate.value=rate;
		f.SGI.checked=sgi;
		f.BW.value=bw;
//		f.isolate.checked=cltIsolate;
		f.channel.value=channel;
		f.preamble.checked=preamble;
		f.power.value=power;
		f.manual.value=manPower;
		f.wmm.checked=wmm;
		f.mode.value=mode;
		document.getElementById('en_wireless').checked = wireless;
		document.getElementById('SGI').checked = sgi;
		document.getElementById('wmm').checked = wmm;
        document.getElementById('preamble').checked = preamble;
        manual_sw();
	
	if(maxRate == 1)
	{
	    option=document.createElement("OPTION");
	    option.value = "0";
	    option.text = "自动";
	    confTemp.rate.options.add(option);

	    option=document.createElement("OPTION");
	    option.value = "11";
	    option.text = "11M";
	    confTemp.rate.options.add(option);
	}
	else if(maxRate == 2)
	{
	    option=document.createElement("OPTION");
	    option.value = "0";
	    option.text = "自动";
	    confTemp.rate.options.add(option);

	    option=document.createElement("OPTION");
	    option.value = "11";
	    option.text = "11M";
	    confTemp.rate.options.add(option);

	    option=document.createElement("OPTION");
	    option.value = "54";
	    option.text = "54M";
	    confTemp.rate.options.add(option);

	}
	else if(maxRate == 3)
	{
	    option=document.createElement("OPTION");
	    option.value = "0";
	    option.text = "自动";
	    confTemp.rate.options.add(option);

	    option=document.createElement("OPTION");
	    option.value = "11";
	    option.text = "11M";
	    confTemp.rate.options.add(option);

	    option=document.createElement("OPTION");
	    option.value = "54";
	    option.text = "54M";
	    confTemp.rate.options.add(option);

            option=document.createElement("OPTION");
	    option.value = "150";
	    option.text = "150M";
	    confTemp.rate.options.add(option);
	}
	else if(maxRate == 4)
	{
	    option=document.createElement("OPTION");
	    option.value = "0";
	    option.text = "自动";
	    confTemp.rate.options.add(option);

	    option=document.createElement("OPTION");
	    option.value = "11";
	    option.text = "11M";
	    confTemp.rate.options.add(option);

	    option=document.createElement("OPTION");
	    option.value = "54";
	    option.text = "54M";
	    confTemp.rate.options.add(option);

        option=document.createElement("OPTION");
	    option.value = "300";
	    option.text = "300M";
	    confTemp.rate.options.add(option);
	}
	else
	{
	    option=document.createElement("OPTION");
	    option.value = "0";
	    option.text = "自动";
	    confTemp.rate.options.add(option);
	}
	f.rate.value=rate;





       /* switch (apType)
        {
            case "WA1800N":
            option=document.createElement("OPTION");
            option.value = "300";
            option.text = "300M";
            confTemp.rate.remove(3);
            confTemp.rate.options.add(option);

			break;
		case "WA2000N":
			option=document.createElement("OPTION");
			option.value = "300";
			option.text = "300M";
			confTemp.rate.remove(3);
			confTemp.rate.options.add(option);

			break;
		case "WA1000N":
			option=document.createElement("OPTION");
			option.value = "150";
			option.text = "150M";
			confTemp.rate.remove(3);
			confTemp.rate.options.add(option);

			break;
		case "WA500N":
			option=document.createElement("OPTION");
			option.value = "150";
			option.text = "150M";
			confTemp.rate.remove(3);
			confTemp.rate.options.add(option);

			break;
		case "WA501N":
			option=document.createElement("OPTION");
			option.value = "150";
			option.text = "150M";
			confTemp.rate.remove(3);
			confTemp.rate.options.add(option);

			break;
		case "WA1500N":
			option=document.createElement("OPTION");
			option.value = "150";
			option.text = "150M";
			confTemp.rate.remove(3);
			confTemp.rate.options.add(option);

			break;
		case "WA2200N":
			option=document.createElement("OPTION");
			option.value = "300";
			option.text = "300M";
			confTemp.rate.remove(3);
			confTemp.rate.options.add(option);

			break;
		case "WA1800NV3":
            option=document.createElement("OPTION");
            option.value = "300";
            option.text = "300M";
            confTemp.rate.remove(3);
            confTemp.rate.options.add(option);

			break;

            case "WA1700N":
            option=document.createElement("OPTION");
            option.value = "300";
            option.text = "300M";
            confTemp.rate.remove(3);
            confTemp.rate.options.add(option);

			break;
		
		default:
			break;

        }*/


	}
}

function manual_sw()
{
	if (document.getElementById('power').value == 0)
	{
		document.getElementById('manual').style.display = "none";
	}
	else
	{
		document.getElementById('manual').style.display = "block";
	}
	
}

function save()
{
	var f = confTemp;
	if (!confirm(_("<%write(confirmTakeEffectTips);%>")))
	{
		return;
	}
    if (!checkName(f.ssid, 32, _("<%write(ssid);%>"), 0)) {
        return ;
    }
	f.en_wireless.value = (f.en_wireless.checked == true ? 1 : 0);
	f.SGI.value = (f.SGI.checked == true ? 1 : 0);
//	f.isolate.value = (f.isolate.checked == true ? 1 : 0);
	f.preamble.value = (f.preamble.checked == true ? 1 : 0);
	f.wmm.value = (f.wmm.checked == true ? 1 : 0);

    var tempmode = document.getElementById('mode').value;
    var temprate = document.getElementById('rate').value;

    if ( tempmode == 4 ){
        if( temprate != 0 && temprate != 54 ){
            alert(_("<%write(mode_rate_not_match);%>"));
            return false;
        }
    }else if ( tempmode == 6 ){
        if( temprate != 0 && temprate != 150 && temprate != 300){
            alert(_("<%write(mode_rate_not_match);%>"));
            return false;
        }
    }



//	alert(f.en_wireless.value);
//	alert(f.ssid.value);
//	alert(f.SGI.value);
//	alert(f.wmm.value);
//	alert(f.preamble.value);
//	alert(f.mode.value);
//	alert(f.rate.value);
//	alert(f.BW.value);
//	alert(f.channel.value);
//	alert(f.power.value);
//	alert(f.manual.value);
//	return;
	f.action="/goform/formFitApLocalConfTemp";
	f.submit();	
}

function Reset()
{
	confTemp.reset();
	init();
}

</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("<%write(wirePar);%>"));//显示页面名称</script>
  <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						2,
						new Array(_("<%write(modeList);%>"),'apTmp.asp',0,0),
						new Array(_("<%write(editWirePar);%>"),'editTemp.asp',1,1)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form name="confTemp" method="post" action="">
<br>
<table width="600" border="0" cellspacing="1" cellpadding="4" class="style22">
  <tr class="style18">
    <td width="140" id="enWireFC_td"><%write(startwire);%></td>
    <td width="140"><input name="en_wireless" type="checkbox" id="en_wireless" checked></td>
    <td width="141" id="wireRate_td"><%write(wirespeed);%></td>
    <td width="142"><select name="rate" id="rate" class="input" style="width:90px">
    <!--   <option value="0" selected id="auto"><%write(auto);%></option>
     <option value="11">11M</option>
      <option value="54">54M</option>-->
<!--
      <option value="150">150M</option>
-->
    </select></td>
  </tr>
  <tr>
    <td>SSID</td>
    <td><input type="text" class="input" name="ssid" id="ssid" size="16" maxlength="32" value="" onChange='checkName(this,32,_("<%write(ssid);%>"),0)'></td>
    <td id="channelWith_td"><%write(channelBandWidth);%></td>
    <td><select name="BW" id="BW" class="input" style="width:90px">
      <option value="0">20M</option>
      <option value="1">20M/40M</option>
    </select></td>
  </tr>
  <tr class="style18">
    <td id="enWMM_td"><%write(startWMM);%></td>
    <td>
      <input name="wmm" type="checkbox" id="wmm" checked></td>
    <td id="channel_td"><%write(channel);%></td>
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
    </td>
  </tr>
  <tr>
    <td>Short Preamble</td>
    <td><input name="preamble" type="checkbox" id="preamble" checked></td>
    <td id="wirePower_td"><%write(wireTranPower);%></td>
    <td>
    	<table width="100%" border="0" cellspacing="0" cellpadding="0" class="style22">
        	<tr>
            	<td nowrap>
                    <select name="power" id="power" class="input" style="width:90px" onChange="manual_sw();">
                      <option value="0" id="auto"><%write(auto);%></option>
                      <option value="1" id="manu"><%write(manu);%></option>
                    </select>
                </td>
                <td nowrap>
                    <select name="manual" id="manual" class="input" style="width:40px">
                        <option value="1" id="low"><%write(low);%></option>
                        <option value="2" id="mid"><%write(middle);%></option>
                        <option value="3" id="high"><%write(hight);%></option>
                    </select>
          		</td>
          	</tr>
      </table>
      </td>
    </tr>
  <tr class="style18">
    <td id="wireMode_td">Short Guard Interval</td>
    <td><input name="SGI" type="checkbox" id="SGI" checked></td>
    <td id="wireMode_td"><!--无线客户端隔离-->       <%write(wireMode);%></td>
    <td><!--<input type="checkbox" name="checkbox5" id="checkbox5">-->
      <select name="mode" id="mode" class="input" style="width:90px">
    <  <option value="4" id="only11G"><%write(only11g);%></option>
        <option value="6" id="only11N"><%write(only11n);%></option>
        <option value="9" id="mix11" selected><%write(bgnMix);%></option>
      </select></td>
  </tr>
<!--
  <tr class="style18">
    <td><%write(clIsolate);%></td>
    <td><input name="isolate" type="checkbox" id="isolate" ></td>
    <td> </td>
    <td> </td>
  </tr>
-->

  </table>
<!--
<br>
<table width="600" border="0" cellspacing="1" cellpadding="4" class="style22">
  <tr>
    <td width="112"><select name="conf_1" id="conf_1" class="input">
      <option value="1">开启</option>
      <option value="0">关闭</option>
    </select></td>
    <td width="105">SSID标识</td>
    <td width="160"><input type="text" name="ssid_1" id="ssid_1" size="15" maxlength="14" class="input"></td>
    <td width="87">VLAN</td>
    <td width="90"><select name="vlan_1" id="vlan_1" class="input">
      <option value="1">1</option>
      <option value="2">2</option>
      <option value="3">3</option>
      <option value="4">4</option>
    </select></td>
  </tr>
  <tr class="style18">
    <td><select name="conf_2" id="conf_2" class="input">
      <option value="1">开启</option>
      <option value="0">关闭</option>
    </select></td>
    <td>SSID标识</td>
    <td><input type="text" name="ssid_2" id="ssid_2" size="15" maxlength="14" class="input"></td>
    <td>VLAN</td>
    <td><select name="vlan_2" id="vlan_2" class="input">
      <option value="1">1</option>
      <option value="2">2</option>
      <option value="3">3</option>
      <option value="4">4</option>
    </select></td>
  </tr>
  <tr>
    <td><select name="conf_3" id="conf_3" class="input">
      <option value="1">开启</option>
      <option value="0">关闭</option>
    </select></td>
    <td>SSID标识</td>
    <td><input type="text" name="ssid_3" id="ssid_3" size="15" maxlength="14" class="input"></td>
    <td>VLAN</td>
    <td><select name="vlan_3" id="vlan_3" class="input">
      <option value="1">1</option>
      <option value="2">2</option>
      <option value="3">3</option>
      <option value="4">4</option>
    </select></td>
  </tr>
  <tr class="style18">
    <td><select name="conf_4" id="conf_4" class="input">
      <option value="1">开启</option>
      <option value="0">关闭</option>
    </select></td>
    <td>SSID标识</td>
    <td><input type="text" name="ssid_4" id="ssid_4" size="15" maxlength="14" class="input"></td>
    <td>VLAN</td>
    <td><select name="vlan_4" id="vlan_4" class="input">
      <option value="1">1</option>
      <option value="2">2</option>
      <option value="3">3</option>
      <option value="4">4</option>
    </select></td>
  </tr>
</table>
-->
<table align="center"  ><tr><td height="5"></td></tr><tr><td>
<input type="button"  onClick="save()"class="input-button" value="<%write(save);%>" name="" id="uttsave">&nbsp;
<!--<input type="button" class="input-button" value="刷新" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="<%write(refill);%>" name="" onClick="Reset();" id="uttreset">
<input type="button" class="input-button" value="<%write(help);%>" name="" onClick="window.open('FitApHelpInfo.htm#wlanPara')" id="utthelp">
</td></tr></table>
<!--
<table  border="0" width="600" align="center" cellpadding="0" cellspacing="0">
<tr>
<td colspan="5" height="" align="center" valign="bottom">
<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="600" height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe>	  </td>
</tr>
<tr>
<td colspan="5" align="center" valign="top">
<iframe name="frame12" scrolling="no" valign="top" align="MIDDLE" width="600" height="248" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
</tr>
</table>
<table width="600" align="center"  cellpadding="0" cellspacing="0">
<tr>
  <td height="21" valign="top" nowrap><span id="span" name="">AP型号：
    <select name="ApType" id="ApType" class="input">
      <option value="1">510W</option>
      <option value="2">nv1800N</option>
      </select>
  </span>
    </td>
  </tr>
</table>
-->
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
