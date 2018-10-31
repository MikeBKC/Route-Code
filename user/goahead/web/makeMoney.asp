<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("WirelessSui");%>
<title>赚钱wifi</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
</head>
<script type="text/javascript">
<% aspMakeMoney();%>
<% aspoutPhone();%>

/*
var dRate= "0";
var uRate= "0";
var MakeMoneyEnables = "0";
var saftyEnables = "0";
var errorstr="";var rightstr="";
*/
RateArr = new Array("64K", "128K", "256K", "512K", "1M", "1.5M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M", "11M", "12M", "13M", "14M", "15M", "16M", "17M", "18M", "19M", "20M", "25M", "30M", "35M", "40M", "45M", "50M", "90M", "100M");
RateArrValue = new Array("64", "128", "256", "512", "1000", "1500", "2000", "3000", "4000", "5000", "6000", "7000", "8000", "9000", "10000", "11000", "12000", "13000", "14000", "15000", "16000", "17000", "18000", "19000", "20000", "25000", "30000", "35000", "40000", "45000", "50000", "90000", "100000");
function init(){
    var f=Money_Config;
    var switch_b = $(".on-off").children("button");
    var makeMoneyEnables = (MakeMoneyEnables =="1"?"on":"off");
    if(!smartQosEn)
	document.getElementById("tips").innerHTML = "建议您启用'游戏加速'功能，以增强上网体验";
    else
	document.getElementById("tips").innerHTML = "";
    initEnabledButton(switch_b,makeMoneyEnables);
    f.saftyEnable.checked=(saftyEnables=="1"?true:false);
    f.MaxDownRatetxt.value=dRate.replace(/k/g,"");
    f.uMaxDownRatetxt.value=uRate.replace(/k/g,"");
    changeSelect(f.MaxDownRatetxt, f.MaxDownRatew);
    changeSelect(f.uMaxDownRatetxt, f.uMaxDownRatew);
//    f.Account.value=account;
    f.FeeScale.value=charge;
    f.FreeTime.value=FreeTime;
    f.ssid.value=ssid;
    f.ssid_5g.value=ssid_5g;

    f.ssid2.value=ssid2;
    f.ssid2_5g.value=ssid2_5g;
    f.encodeType.value=encodeType;
    f.encodeType2.value=encodeType2;
    f.encodeType_5g.value=encodeType_5g;
    f.encodeType2_5g.value=encodeType2_5g;


    var index1 = timestarts.indexOf(":");
    var index2 = timestops.indexOf(":");
    f.fromhour.value = timestarts.substring(0, index1);
    f.fromminute.value = timestarts.substring(index1 + 1);
    f.endhour.value = timestops.substring(0, index2);
    f.endminute.value = timestops.substring(index2 + 1);
    parent.art.dialog.list["func_box"].size(700,100);
//    f.OutPort.value=OutPorts;
    $("#makeMoneyEnable").val(MakeMoneyEnables);	
    $("#error_msg").html(errorstr);
    if(errorstr == "")
    {
	if((MakeMoneyEnables == "1")&&(wireEn == 0))
	{
	    $("#error_msg").html("当前2.4G无线功能已关闭，赚钱wifi将没有2.4G无线信号");
	}
	if((MakeMoneyEnables == "1")&&(wireEn_5g == 0))
	{
	    $("#error_msg").html("当前5G无线功能已关闭，赚钱wifi将没有5G无线信号");
	}
	if((MakeMoneyEnables == "1")&&(wireEn == 0)&&(wireEn_5g == 0))
	{
	    $("#error_msg").html("当前无线功能已关闭，赚钱wifi将没有无线信号");
	}
//	if((timestarts < weekdaytimestarts)||(timestarts < weekendtimestarts)||(timestops > weekdaytimestops)||(timestops > weekendtimestops))
	
	   if(is_tip_config(timestarts,timestops,weekdaytimestarts,weekdaytimestops,weekendtimestarts,weekendtimestops))
	{
	    $("#error_msg").html("当前赚钱wifi营业时间与无线定时时间冲突");
	    $("#error_msg2").html("工作日："+weekdaytimestarts+"-"+weekdaytimestops+" 双休日："+weekendtimestarts+"-"+weekendtimestops);
	}
    }
    showSuccessTips(rightstr);
}
function is_tip_config(timestarts,timestops,weekdaytimestarts,weekdaytimestops,weekendtimestarts,weekendtimestops){

	    if((weekdaytimestarts<=weekdaytimestops)&&(weekendtimestarts<=weekendtimestops))
	    {
		if(timestarts<=timestops)
		{
		    if((timestarts < weekdaytimestarts)||(timestarts < weekendtimestarts)||(timestops > weekdaytimestops)||(timestops > weekendtimestops))
		    {
			return 1;
		    }
		}
		else
		{
		    if((weekdaytimestarts>"00:00")||(weekendtimestarts>"00:00")||(weekdaytimestops<"23:59")||(weekendtimestops<"23:59"))
		    {
			return 1;
		    }

		}
	    }
	    else if((weekdaytimestarts>=weekdaytimestops)&&(weekendtimestarts>=weekendtimestops))//两个都大于
	    {
		if(timestarts<=timestops)
		{
		     if(!((timestarts>=weekdaytimestarts)||(timestarts>=weekendtimestarts)||(timestops<=weekdaytimestops)||(timestops<=weekendtimestops)))
		     {
			return 1;
		    } 
		}
		else
		{
		    if((timestops>weekdaytimestops)||(timestops>weekendtimestops)||(timestarts<weekdaytimestarts)||(timestarts<weekendtimestarts))
		    {
			return 1;
		    }
		}
	    }	
	    else if((weekdaytimestarts<weekdaytimestops)&&(weekendtimestarts>weekendtimestops))//第一个前小于后，第二个前大于后
	    {
		if(timestarts<=timestops)
		{
		    if((timestarts<weekdaytimestarts)||(timestops>weekdaytimestops)||(!((timestarts>=weekendtimestarts)||(timestops<=weekendtimestops))))
		    {
			return 1;
		    }
		}
		else
		{
		    if((weekdaytimestarts>"00:00")||(weekdaytimestops<"23:59")||(timestarts<weekendtimestarts)||(timestops>weekendtimestops))
		    {
			return 1;
		    }
		}
	    }
	    else if((weekdaytimestarts>weekdaytimestops)&&(weekendtimestarts<weekendtimestops))//第一个前大于后，第二个前小于后
	    {
		if(timestarts<=timestops)
		{
		    if((!((timestarts>=weekdaytimestarts)||(timestops<=weekdaytimestops)))||(timestarts<weekendtimestarts)||(timestops>weekendtimestops))
		    {
			return 1;
		    }
		}
		else
		{
		
		    if((weekendtimestarts>"00:00")||(weekendtimestops<"23:59")||(timestarts<weekdaytimestarts)||(timestops>weekdaytimestops))
		    {
			return 1;
		    }
		}
	    }
}//函数结尾

function changeRate(ctl,ctl2){	
    var selValue = getListValue(ctl);
    selValue=(selValue=="zidingyi"?"":selValue);
    ctl2.value = selValue;  
}
function printbind()
{
    document.getElementById("bind").href = "";
}
function printvisityun()
{
    document.getElementById("visityun").href = "";
}

function changeSelect(ctrl1, ctrl2)
{

    if (ctrl1.value == "")
    {
	var selectIndex = 0;
    } else {
	var selectIndex = getSelIndexByValue(ctrl2, ctrl1.value);
    }
    if (selectIndex != -1)
    {
	ctrl2.selectedIndex = selectIndex;
    } else {
	ctrl2.selectedIndex = 1;
    }
}
var SeniorShowEn=0;
function clickSenior(frm) {
    if(SeniorShowEn == 0) {
        SeniorShowEn = 1;
    } else {
        SeniorShowEn = 0;
    }
    showTable(time, SeniorShowEn);
    showTable(wire, SeniorShowEn);
    showTable(wire2, SeniorShowEn);
    showTable(wire5, SeniorShowEn);
    showTable(wire5_2, SeniorShowEn);
    showTable(safe, SeniorShowEn);
    showTable(upRate, SeniorShowEn);
    showTable(downRate, SeniorShowEn);
    parent.art.dialog.list["func_box"].size(700,$("#makeMoney").height()+100);
   // parent.art.dialog.list["func_box"].size(700,$("#wireless_set").height()+130);
}
function checkData(){
    var f = Money_Config;
    var uploadObj = f.uMaxDownRatetxt;
    var downloadObj = f.MaxDownRatetxt;
    if(f.uMaxDownRatetxt.value != 0)
	uploadObj.value = $.trim($("#uMaxDownRatetxt").val());
    if(f.MaxDownRatetxt.value != 0)
	downloadObj.value = $.trim($("#MaxDownRatetxt").val());

    if ($("#uMaxDownRatew").val() == "zidingyi") {
    if (uploadObj.value == "") {
        $("#error_msg").html("上传带宽不能为空");
        return false;
    }
    }

    if ($("#MaxDownRatew").val() == "zidingyi") {
    if (downloadObj.value == "") {
        $("#error_msg").html("下载带宽不能为空");
        return false;
    }
    }
    if (uploadObj.value != 0) {
    if (checkNumber_P(uploadObj,11,102400,"上传带宽",0) == false) {
        uploadObj.focus();
        return false;
    }
    }
    if (downloadObj.value != 0) {
    if (checkNumber_P(downloadObj,11,102400,"下载带宽",0) == false) {
        downloadObj.focus();
        return false;
    }
    }
    if((checkRates(f.FeeScale.value) == 0)||(f.FeeScale.value>10)){
	$("#error_msg").html("收费标准输入错误");
	f.FeeScale.focus(); 
	return false;
    }
    //    if(!checkName_P(f.Account,32,"收款账号",0)) return false;
    if((isBind == 0)&&(f.makeMoneyEnable.value == 1))
    {
	$("#error_msg").html("当前设备未绑定账号，请点击'收益'绑定账号后再启用赚钱wifi");	
	return false;
    }
    if(!checkNumber_P(f.FreeTime,0,20,"免费体验时长")) return false;
    if(!checkName_P(f.ssid,32,"无线名称1",0)) return false;
    if (f.ssid.value.length > 32) {
       if (confirm("无线名称1字符已超过32个，手机将搜索不到网络信号，是否继续保存？") == false) {
	   return false;
       }
    }

    if(!checkName_P(f.ssid2,32,"无线名称2",0)) return false;
    if (f.ssid2.value.length > 32) {
       if (confirm("无线名称2字符已超过22个，手机将搜索不到网络信号，是否继续保存？") == false) {
	   return false;
       }
    }

    if(!checkName_P(f.ssid_5g,32,"5G无线名称1",0)) return false;
    if (f.ssid_5g.value.length > 32) {
	if (confirm("5G无线名称1字符已超过22个，手机将搜索不到网络信号，是否继续保存？") == false) {
	    return false;
       }
    }

    if(!checkName_P(f.ssid2_5g,32,"5G无线名称2",0)) return false;
    if (f.ssid2_5g.value.length > 32) {
	if (confirm("5G无线名称2字符已超过32个，手机将搜索不到网络信号，是否继续保存？") == false) {
	    return false;
       }
    }
}
function checkRates(str){
    var re = /^(([1-9][0-9]*\.[0-9][0-9]*)|([0]\.[0-9][0-9]*)|([1-9][0-9]*)|([0]{1}))$/;   //判断字符串如果是整数不能以0开头后面加正整数，如果是浮点数整数部分不能为两个0：如00.00，如果是整数，
     var Sure;
     if (!re.test(str)){
         Sure =0;
     }else{
         Sure =1;
     }
     return Sure;
}

function save(){
    var f=Money_Config;
    if(checkData()!=false){
	f.timestart.value = f.fromhour.value + ":" +f.fromminute.value;
	f.timestop.value = f.endhour.value + ":" + f.endminute.value;
	Money_Config.action ="/goform/formMakeMoney";
	Money_Config.submit();
    }
}

$(function(){
    init();
    $(".on-off").click(function(){
	    var switch_b = $(".on-off").children("button");
	    var status_now = switch_b.attr("class");
	    if(status_now=="on"){
		$("#makeMoneyEnable").val("0");	
	    }else{
		$("#makeMoneyEnable").val("1");	
	    }
	    swicth_act(status_now);
    });
});
</script>
<style type="text/css">

ul li {  float:left; margin-left:2px; margin-top:10px; height:29px; padding-top:3px;  padding-left:10px; padding-right:10px; color:#fff; width:120px;cursor:pointer;}
#tabs{width:650px;}
#firstli{background:#fff;color:#0099D2;}
#secondli{background:#0099D2;color:#fff;}
</style>
<body id="wireless_set" scroll="no">
<div id="tabs" >
    <div style=" height:42px;background-color:#0099D2;" id="tabstitle">
	<ul>
		<li style="margin-left:15px; " id="firstli" onclick="window.parent.open_windows(null,'makeMoney',null)" >配置</li>
		<li id="secondli" onclick="window.parent.open_windows(null,'income',null)" >收益</li>
	    </ul>
    </div>

<div id="makeMoney" style="width:600px; margin:0 auto; text-align:center;">

<br>

	<form action="" method="post" name="Money_Config">
    
	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;" class="group" width="600px">
   
     <!--	<tr class="tab_info">                     
	  <td width="140px">收益</td>
          <td width="360px">截至2016-06-01总收益为500元</td>
      </tr> -->                                                                    
    	<tr class="tab_info">
        	<td width=120px>开启赚钱wifi</td>
            <td colspan="2" width=480px><span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="makeMoneyEnable" id="makeMoneyEnable" value="" /></span>&nbsp;&nbsp;&nbsp;<span id="tips"></span></td>
        </tr>
       <!-- <tr class="tab_info">
        	<td>收款账号（微信）</td>
            <td colspan="2"><input type="text" size="12" style="width:80px;"class="txt-input" name="Account" maxlength="11"></td>
           
        </tr>-->
               <tr class="tab_info">
        	<td>营业时间</td>
           <td><select name="fromhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="fromminute" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;到&nbsp;<select name="endhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="endminute" class="list"><script language="javascript">CreateHOp(60);</script></select><input type="hidden" name="timestart"><input type="hidden" name="timestop"></td>
        </tr>
     <tr class="tab_info">
        	<td>收费标准</td>
            <td colspan="2"><input type="text" size="12" style="width:60px;"class="txt-input" name="FeeScale" maxlength="4" onKeyPress="floatOnly()">&nbsp;元/30分钟（范围为0-10元）</td>
           
        </tr>
     <tr class="tab_info">
        	<td height="30" align="right" ><div style="position:relative; z-index:2;"><a href="javascript:clickSenior(Money_Config)" style="color:#000000;text-decoration:underline;"><strong>高级选项</strong></a></div></td>

            <td></td>
           
        </tr>

     <tr class="tab_info" id=time style="display:none">
        	<td>免费体验时长</td>
            <td colspan="2"><input type="text" size="12" style="width:60px;"class="txt-input" name="FreeTime" onKeyPress="figureOnly()" maxlength="2">&nbsp;分钟</td>
           
        </tr>
     <tr class="tab_info" id=wire style="display:none">
        	<td>无线名称1</td>
            <td colspan="2"><input type="text" size="12" style="width:160px;"class="txt-input" name="ssid" maxlength="32">&nbsp;&nbsp;&nbsp;&nbsp;
<select name="encodeType" id="encodeType" width=8px>
                    <option value="GBK" selected="selected"><%write(wire_priComp);%>(<%write(wire_simple);%>)</option>
                    <option value="BIG5"><%write(wire_priComp);%>(<%write(wire_complex);%>)</option>
                    <option value="UTF-8"><%write(wire_priPhone);%></option>
	</select>
</td>
           
        </tr>
     <tr class="tab_info" id=wire2 style="display:none">
        	<td>无线名称2</td>
            <td colspan="2"><input type="text" size="12" style="width:160px;"class="txt-input" name="ssid2" maxlength="32">&nbsp;&nbsp;&nbsp;&nbsp;
<select name="encodeType2" id="encodeType2" width=8px>
                    <option value="GBK" selected="selected"><%write(wire_priComp);%>(<%write(wire_simple);%>)</option>
                    <option value="BIG5"><%write(wire_priComp);%>(<%write(wire_complex);%>)</option>
                    <option value="UTF-8"><%write(wire_priPhone);%></option>
	</select>
</td>
           
        </tr>
     <tr class="tab_info" id=wire5 style="display:none">
        	<td>5G无线名称1</td>
            <td colspan="2"><input type="text" size="12" style="width:160px;"class="txt-input" name="ssid_5g" maxlength="32">&nbsp;&nbsp;&nbsp;&nbsp;
<select name="encodeType_5g" id="encodeType_5g" width=8px>
                    <option value="GBK" selected="selected"><%write(wire_priComp);%>(<%write(wire_simple);%>)</option>
                    <option value="BIG5"><%write(wire_priComp);%>(<%write(wire_complex);%>)</option>
                    <option value="UTF-8"><%write(wire_priPhone);%></option>
	</select>
</td>
           
        </tr>
     <tr class="tab_info" id=wire5_2 style="display:none">
        	<td>5G无线名称2</td>
            <td colspan="2"><input type="text" size="12" style="width:160px;"class="txt-input" name="ssid2_5g" maxlength="32">&nbsp;&nbsp;&nbsp;&nbsp;
<select name="encodeType2_5g" id="encodeType2_5g" width=8px>
                <option value="GBK" selected="selected"><%write(wire_priComp);%>(<%write(wire_simple);%>)</option>
                    <option value="BIG5" ><%write(wire_priComp);%>(<%write(wire_complex);%>)</option>
                    <option value="UTF-8"><%write(wire_priPhone);%></option>
	</select>
</td>
           
        </tr>
    <tr class="tab_info" id=safe style="display:none">
        	<td>安全防护</td>
            <td colspan="2"><input name="saftyEnable" id="saftyEnable"  type="checkbox" /></span>&nbsp;隔离赚钱wifi与家庭私有网络</td>
           
        </tr>
     <tr class="tab_info" id=upRate style="display:none">
        	<td>上传限速</td>
		<td><input type="text" name="uMaxDownRatetxt" id="uMaxDownRatetxt" onKeyPress="figureOnly()" onBlur="changeSelect(this,Money_Config.uMaxDownRatew)" size="8" style="width:60px;"class="txt-input" maxlength="7">&nbsp;kbit/s&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="uMaxDownRatew" id="uMaxDownRatew" class="list" onChange="changeRate(this,Money_Config.uMaxDownRatetxt)">
      <option value="0">不限速</option>
       <option value="zidingyi">自定义</option>
	<script language="javascript">
	for(i=0;i<RateArr.length;i++)
	    document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
	</script>
</select>&nbsp;(0表示不限速)<input type="hidden" name="page" value="limit"><input type="hidden" name="dRate"><input type="hidden" name="dCeil" value="0"><input type="hidden" name="uRate"><input type="hidden" name="uCeil" value="0"><input type="hidden" name="policy_name"></td> 
           
        </tr>
     <tr class="tab_info" id=downRate style="display:none">
        	<td>下载限速</td>
 <td><input type="text" name="MaxDownRatetxt" id="MaxDownRatetxt" onKeyPress="figureOnly()" onBlur="changeSelect(this,Money_Config.MaxDownRatew)" size="8" style="width:60px;"class="txt-input"maxlength="7">&nbsp;kbit/s&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="MaxDownRatew" id="MaxDownRatew" class="list" onChange="changeRate(this,Money_Config.MaxDownRatetxt)">
      <option value="0">不限速</option>
       <option value="zidingyi">自定义</option>
	<script language="javascript">
	for(i=0;i<RateArr.length;i++)
	    document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
	</script>
</select>&nbsp;(0表示不限速)<input type="hidden" name="page" value="limit"><input type="hidden" name="dRate"><input type="hidden" name="dCeil" value="0"><input type="hidden" name="uRate"><input type="hidden" name="uCeil" value="0"><input type="hidden" name="policy_name"></td>           
        </tr>

       
          <tr class="tab_savetd_tip"  valign="top"> 
        	<td colspan="3" align="left" style="padding-left:10px; padding-top:4px;">
            	<table cellpadding="0" cellspacing="0" border="0" width="510">
                	<tr height="30px">
                    	<td width="90px">&nbsp;</td>
                        <td align="left"><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right"><input type="button" class="btn" name="back" id="savebtn" onClick="save();"value="<%write(com_save);%>"/></td>
                    </tr>
		    <tr height="30px">
                    	<td width="90px">&nbsp;</td>
                        <td align="left"><span id="error_msg2"  style="color:red;"></span></td>
                    </tr>

                </table>
           </td>
            
        </tr>
    </table>
    </form>
<br>
<br>
</div>
<br>
</div>
<br>
</body>
</html>
