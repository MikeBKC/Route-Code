<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("SmartQos");%>
<%langTran("SmartQosSui");%>
<%langTran("left");%>
<title><% aspGetTitle(); %></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<link href="css/default.css" rel="stylesheet" type="text/css" />
<style>
.gameTable{
    border-style: solid;
    border-width: 1px;
    border-color: transparent;
}
</style>
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript" src="script/menu.js"></script>
<script type="text/javascript">

var GBoostEn="off";
var policy = 0;

var RateArr = new Array("64K", "128K", "256K", "512K", "1M", "1.5M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M", "11M", "12M", "13M", "14M", "15M", "16M", "17M", "18M", "19M", "20M", "25M", "30M", "35M", "40M", "45M", "50M", "90M", "100M");
var RateArrValue = new Array("64", "128", "256", "512", "1000", "1500", "2000", "3000", "4000", "5000", "6000", "7000", "8000", "9000", "10000", "11000", "12000", "13000", "14000", "15000", "16000", "17000", "18000", "19000", "20000", "25000", "30000", "35000", "40000", "40000", "50000", "90000", "100000");
<%
getWanIfCount();
aspSmartQos();
%>

function changeRate(ctl,ctl2){	
	var selValue = getListValue(ctl);
	selValue=(selValue=="zidingyi"?"":selValue);
	ctl2.value = selValue;	
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
function save()
{
    var f=QosGlobalConfig;
    var uploadObj = f.txBand1;
    var downloadObj = f.rxBand1;
    uploadObj.value = $.trim($("#txBand1").val());
    downloadObj.value = $.trim($("#rxBand1").val());

    if ($("#uMaxDownRatew").val() == "zidingyi") {
	if (uploadObj.value == "") {
	    $("#error_msg").html("<%write(sma_noNullUp);%>");
	    return false;
	}
    }

    if ($("#MaxDownRatew").val() == "zidingyi") {
	if (downloadObj.value == "") {
	    $("#error_msg").html("<%write(sma_noNullDown);%>");
	    return false;
	}
    }
    if (uploadObj.value != 0) {
	if (checkNumber_P(uploadObj,11,102400,"<%write(sma_upBand);%>",0) == false) {
	    uploadObj.focus();
	    return false;
	}
    }
    if (downloadObj.value != 0) {
	if (checkNumber_P(downloadObj,11,102400,"<%write(sma_downBand);%>",0) == false) {
	    downloadObj.focus();
	    return false;
	}
    }
    $("#gamePolicy").val("");
    $("#gameTypes input[type='checkbox']").each(function(index){
	    if ($(this).is(":checked")) {
		$("#gamePolicy").val($("#gamePolicy").val() + $(this).val() + ";");
	    }
	}
    );
    $("#gamePolicy").val($("#gamePolicy").val().slice(0,$("#gamePolicy").val().lastIndexOf(";")));

    f.submit();
}  

$(function(){
    var htmlStr = "<tr>";
    if (englishSwitch == 0) {
	if (LOL.toLowerCase()=="on") {
	    htmlStr += '<td align="left"><input type="checkbox" name="LOL" value="148"  checked/><%write(GB_lol);%></td>';
	} else {
	    htmlStr += '<td align="left"><input type="checkbox" name="LOL" value="148"/><%write(GB_lol);%></td>';
	}
	if (DNF.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="DNF" value="149" checked /><%write(GB_dnf);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="DNF" value="149" /><%write(GB_dnf);%></td>';
	}
	if (CF.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="CF" value="150" checked /><%write(GB_cf);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="CF" value="150" /><%write(GB_cf);%></td>';
	}
	if (WOW1.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="WOW1" value="151" checked /><%write(GB_wow);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="WOW1" value="151" /><%write(GB_wow);%></td>';
	}
	if (MHXY.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="MHXY" value="152" checked /><%write(GB_mhxy);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="MHXY" value="152" /><%write(GB_mhxy);%></td>';
	}
	htmlStr += '</tr>';

	htmlStr += '<tr>';
	if (JL.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="JL" value="153" checked /><%write(GB_jl);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="JL" value="153" /><%write(GB_jl);%></td>';
	}
	if (DOTA2.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="DOTA2" value="154" checked />DOTA2</td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="DOTA2" value="154" />DOTA2</td>';
	}

	if (TKSJ.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="TKSJ" value="155" checked /><%write(GB_tksj);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="TKSJ" value="155"/><%write(GB_tksj);%></td>';
	}
	if (LZG.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="LZG" value="156" checked /><%write(GB_lzg);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="LZG" value="156" /><%write(GB_lzg);%></td>';
	}
	if (YXSG.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="YXSG" value="157" checked /><%write(GB_yxsg);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="YXSG" value="157" /><%write(GB_yxsg);%></td>';
	}
	htmlStr += '</tr>';

	htmlStr += '<tr>';
	if (FKJYOL.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="FKJYOL" value="158" checked /><%write(GB_cs);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="FKJYOL" value="158" /><%write(GB_cs);%></td>';
	}
	if (QQXW.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="QQXW" value="159" checked /><%write(GB_xw);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="QQXW" value="159" /><%write(GB_xw);%></td>';
	}
	if (WMSJ.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="WMSJ" value="160" checked /><%write(GB_wmsj);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="WMSJ" value="160" /><%write(GB_wmsj);%></td>';
	}
	if (LSCS.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="LSCS" value="161" checked /><%write(GB_lscs);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="LSCS" value="161" /><%write(GB_lscs);%></td>';
	}
	if (QQFC.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="QQFC" value="162" checked /><%write(GB_fc);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="QQFC" value="162" /><%write(GB_fc);%></td>';
	}
    } else {
	if (DNF.toLowerCase()=="on") {
	    htmlStr += '<td align="left" width="100px"><input type="checkbox" name="DNF" value="149" checked><%write(GB_dnf);%></td>';
	} else {
	    htmlStr += '<td align="left" width="100px"><input type="checkbox" name="DNF" value="149"><%write(GB_dnf);%></td>';
	}
	if (CF.toLowerCase()=="on") {
	    htmlStr += '<td width="100px"><input type="checkbox" name="CF" value="150" checked><%write(GB_cf);%></td>';
	} else {
	    htmlStr += '<td width="100px"><input type="checkbox" name="CF" value="150"><%write(GB_cf);%></td>';
	}
	if (WOW1.toLowerCase()=="on") {
	    htmlStr += '<td width="100px"><input type="checkbox" name="WOW1" value="151" checked><%write(GB_wow);%></td>';
	} else {
	    htmlStr += '<td width="100px"><input type="checkbox" name="WOW1" value="151"><%write(GB_wow);%></td>';
	}
	if (DOTA2.toLowerCase()=="on") {
	    htmlStr += '<td width="100px"><input type="checkbox" name="DOTA2" value="154" checked>DOTA2</td>';
	} else {
	    htmlStr += '<td width="100px"><input type="checkbox" name="DOTA2" value="154">DOTA2</td>';
	}
	htmlStr += '</tr>';

	htmlStr += '<tr>';
	if (TKSJ.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="TKSJ" value="155" checked><%write(GB_tksj);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="TKSJ" value="155"><%write(GB_tksj);%></td>';
	}
	if (FKJYOL.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="FKJYOL" value="158" checked><%write(GB_cs);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="FKJYOL" value="158"><%write(GB_cs);%></td>';
	}
	if (LSCS.toLowerCase()=="on") {
	    htmlStr += '<td><input type="checkbox" name="LSCS" value="161" checked><%write(GB_lscs);%></td>';
	} else {
	    htmlStr += '<td><input type="checkbox" name="LSCS" value="161"><%write(GB_lscs);%></td>';
	}
	htmlStr += '<td></td>';
	htmlStr += '<td></td>';
	htmlStr += '<td></td>';
    }
    htmlStr += '</tr>';

    $(".gameTable").html(htmlStr);
	
});
function init()
{
    var f=QosGlobalConfig;
    var switch_b = $(".on-off").children("button");
    var en = "off";
    if (smartQosEn== 1)
    {
	en = "on";
    }
    initEnabledButton(switch_b,en);
    $("#smartQosEn").val(en);	
    f.rxBand1.value  = rxBands[1];
    changeSelect(eval('f.rxBand1'), eval('f.MaxDownRatew'));
    f.txBand1.value  = txBands[1];
    changeSelect(eval('f.txBand1'), eval('f.uMaxDownRatew'));

    /*游戏加速*/
	
	if(GameBoostShow == 0)/*GameBoostShow=0时，是英文版，所以隐藏游戏加速相关*/
	{
		$("#hiddenGame").hide();
		$("#broadbandTest").hide();
	}else
	{
		$("#hiddenGame").show();
		$("#broadbandTest").show();
	}
	
    {
	/*
    if (englishSwitch == 0) {
	f.MHXY.checked =(MHXY.toLowerCase()=="on"?true:false); 
        f.QQYY.checked =(QQYY.toLowerCase()=="on"?true:false);	
        f.CCTV.checked =(CCTV.toLowerCase()=="on"?true:false);
	f.SINA.checked =(SINA.toLowerCase()=="on"?true:false);
	f.QQZB.checked =(QQZB.toLowerCase()=="on"?true:false);
	f.WOW1.checked = (WOW1.toLowerCase()=="on"?true:false);
	f.CF.checked = (CF.toLowerCase()=="on"?true:false);
	f.DNF.checked = (DNF.toLowerCase()=="on"?true:false);
	f.LOL.checked = (LOL.toLowerCase()=="on"?true:false);
	f.JL.checked =(JL.toLowerCase()=="on"?true:false); 
	f.DOTA2.checked = (DOTA2.toLowerCase()=="on"?true:false);
	f.TKSJ.checked = (TKSJ.toLowerCase()=="on"?true:false);
	f.LZG.checked = (LZG.toLowerCase()=="on"?true:false);
	f.YXSG.checked = (YXSG.toLowerCase()=="on"?true:false);
	f.FKJYOL.checked =(FKJYOL.toLowerCase()=="on"?true:false); 
	f.QQXW.checked = (QQXW.toLowerCase()=="on"?true:false);
	f.WMSJ.checked = (WMSJ.toLowerCase()=="on"?true:false);
	f.LSCS.checked = (LSCS.toLowerCase()=="on"?true:false);
	f.QQFC.checked = (QQFC.toLowerCase()=="on"?true:false);
    } else {
	f.DNF.checked = (DNF.toLowerCase()=="on"?true:false);
	f.CF.checked = (CF.toLowerCase()=="on"?true:false);
	f.WOW1.checked = (WOW1.toLowerCase()=="on"?true:false);
	f.DOTA2.checked = (DOTA2.toLowerCase()=="on"?true:false);
	f.TKSJ.checked = (TKSJ.toLowerCase()=="on"?true:false);
	f.FKJYOL.checked =(FKJYOL.toLowerCase()=="on"?true:false); 
	f.LSCS.checked = (LSCS.toLowerCase()=="on"?true:false);
    }
    */
	$("#gameTypes input[type='checkbox']").each(function(){
		if ($(this).is(":checked")) {
		    GBoostEn = "on";
		}
	    }
	);
	if (GBoostEn == "on") {
	    $("#game_box").show();
	    $("#gameShow").val("0");
	} else {
	    $("#game_box").hide();
	    $("#gameShow").val("1");
	}
	parent.art.dialog.list["func_box"].size(680,"auto");	
    }
    gameChangeClick();
showSuccessTips(rightstr);
}

function gameBoostClick(){
    var gameshows = $("#gameShow").val();
	
    if(gameshows=="0"){
	    $("#game_box").hide();
	    $("#gameShow").val("1");	
	    
	    parent.art.dialog.list["func_box"].size(680,$(document).height()-70);	
    }else{
	    $("#game_box").show();
	    $("#gameShow").val("0");
	    parent.art.dialog.list["func_box"].size(680,$(document).height()+30);	
    }
}

function gameChangeClick() {
    var flag = true;
    $("#gameTypes input[type='checkbox']").each(function(){
	    if ($(this).is(":checked") == false) {
		flag = false;
	    }
	}
    );
    if (flag) {
	$("#allGame").prop("checked",true);
    } else {
	$("#allGame").prop("checked",false);
    }
}
$(function(){
    init();
    $("#smartQosEnBt").click(function(){
	var switch_b = $(".on-off").children("button");
	var status_now = switch_b.attr("class");
	if(status_now=="on"){
	    $("#smartQosEn").val("off");	
	}else{
	    $("#smartQosEn").val("on");	
	}
	swicth_act(status_now);
    });

    $("#gameAcceleration").click(function(){
	gameBoostClick();
    });	
    $("#allGame").click(function(){
	if ($(this).is(":checked")) {
	   $("#gameTypes input[type='checkbox']").prop("checked",true);
	} else {
	   $("#gameTypes input[type='checkbox']").prop("checked",false);
	}
    });  
    $("#gameTypes input[type='checkbox']").click(function(){
	gameChangeClick();
    });
});
</script>
</head>
<body  scroll="no">
<div class="warp">
<br>
	<form action="/goform/formConfigSmartQos" method="post" name="QosGlobalConfig">
	<table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
    	<tr class="tab_info">
	    <td width="120px"><%write(sma_open);%></td>
            <td width="480px" colspan="2"><span class="on-off">&nbsp;<button type="button" id="smartQosEnBt" class="on"><%write(com_switch);%></button><input type="hidden" name="smartQosEn" id="smartQosEn" value="" /></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="http://www.speedtest.cn" target="_blank" id="broadbandTest">带宽测试</a></td>
        </tr>
	<!--
	<tr class="tab_title">
	    <td colspan="3">WAN口</td>
	</tr>
	-->
        <tr class="tab_info">
	    <td><%write(sma_upBand);%></td>
            <td colspan="2"><input type="text" name="txBand1" value="0"  id="txBand1" style="width:70px;" onKeyPress="figureOnly_P(event)" onBlur="changeSelect(this,QosGlobalConfig.uMaxDownRatew)" size="8" maxlength="6" class="txt-input" /> &nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;
		<select name="uMaxDownRatew" id="uMaxDownRatew"  class="select" onChange="changeRate(this,QosGlobalConfig.txBand1)" style="width:90px;" >
		    <option value="0"><%write(com_noLimit);%></option>
		    <option value="zidingyi"><%write(com_defSelf);%></option>
			<script language="javascript">
			for(i=0;i<RateArr.length;i++)
				document.write('<option value="'+RateArrValue[i]+'">'+RateArr[i]+'</option>');
			</script>
		</select>&nbsp;<%write(com_limitTip);%>
	    </td>
        </tr>
        <tr class="tab_info">
	    <td><%write(sma_downBand);%></td>
            <td colspan="2"><input type="text" name="rxBand1" id="rxBand1" value="0" onchange="validateNumber(this,11,102400,'<%write(sma_downBand);%>',0)" onKeyPress="figureOnly_P(event)" onBlur="changeSelect(this,QosGlobalConfig.MaxDownRatew)"   style="width:70px;" size="8" maxlength="6" class="txt-input" /> &nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;
		<select name="MaxDownRatew" id="MaxDownRatew" onChange="changeRate(this,QosGlobalConfig.rxBand1)" class="select" style="width:90px;">
		    <option value="0"><%write(com_noLimit);%></option>
		    <option value="zidingyi"><%write(com_defSelf);%></option>
		    <script language="javascript">
		    for(i=0;i<RateArr.length;i++)
			    document.write('<option value="'+RateArrValue[i]+'">'+RateArr[i]+'</option>');
		    </script>
		</select>&nbsp;<%write(com_limitTip);%></td>
        </tr>
        <tr class="tab_info" id="hiddenGame">
        	<td><a id="gameAcceleration" style="cursor:pointer"><%write(sma_deepSpeed);%></a><input type="hidden" id="gameShow" value="1"/></td>
            <td colspan="2">
            </td>
           
        </tr>
         <tr class="tab_info" id="game_box" style="display:none;" height="70px">
        	<td><%write(sma_type);%></td>
            <td colspan="2">
            	<div >&nbsp;&nbsp;&nbsp;<input type="checkbox" name="allGame" id="allGame"><%write(checkall);%></div>
                <div id="gameTypes">
		    <table class="gameTable">
			<div id="gameTableContet">
			<!--
			</div>
			<tr>
			    <td align="left"><input type="checkbox" name="LOL" value="148">英雄联盟</td>
			    <td><input type="checkbox" name="DNF" value="149">地下城与勇士</td>
			    <td><input type="checkbox" name="CF" value="150">穿越火线</td>
			    <td><input type="checkbox" name="WOW1" value="151">魔兽世界</td>
			    <td><input type="checkbox" name="MHXY" value="152">梦幻西游</td>
			</tr>
			<tr>
			    <td><input type="checkbox" name="JL" value="153">剑灵</td>
			    <td><input type="checkbox" name="DOTA2" value="154">DOTA2</td>
			    <td><input type="checkbox" name="TKSJ" value="155">坦克世界</td>
			    <td><input type="checkbox" name="LZG" value="156">龙之谷</td>
			    <td><input type="checkbox" name="YXSG" value="157">英雄三国</td>
			</tr>
			<tr>
			    <td><input type="checkbox" name="FKJYOL" value="158">反恐精英</td>
			    <td><input type="checkbox" name="QQXW" value="159">QQ炫舞</td>
			    <td><input type="checkbox" name="WMSJ" value="160">完美世界</td>
			    <td><input type="checkbox" name="LSCS" value="161">炉石传说</td>
			    <td><input type="checkbox" name="QQFC" value="162">QQ飞车</td>
			</tr>
			<tr>
		            <td><input type="checkbox" name="QQYY" value="163">QQ语音</td>
                            <td><input type="checkbox" name="CCTV" value="164">CCTV直播</td>
                            <td><input type="checkbox" name="SINA" value="165">新浪直播</td>
                            <td><input type="checkbox" name="QQZB" value="166">QQ直播</td>
			    <td>&nbsp;</td>
			</tr>
			-->
		    </table>
		    <!--
                	<input type="checkbox" name="LOL" value="148">英雄联盟&nbsp;&nbsp;&nbsp;<input type="checkbox" name="DNF" value="149">地下城与勇士&nbsp;&nbsp;&nbsp;<input type="checkbox" name="CF" value="150">穿越火线&nbsp;&nbsp;&nbsp;<input type="checkbox" name="WOW1" value="151">魔兽世界&nbsp;&nbsp;&nbsp;<input type="checkbox" name="MHXY" value="152">梦幻西游<br>
                	<input type="checkbox" name="JL" value="153">剑灵&nbsp;&nbsp;&nbsp;<input type="checkbox" name="DOTA2" value="154">DOTA2&nbsp;&nbsp;&nbsp;<input type="checkbox" name="TKSJ" value="155">坦克世界&nbsp;&nbsp;&nbsp;<input type="checkbox" name="LZG" value="156">龙之谷&nbsp;&nbsp;&nbsp;<input type="checkbox" name="YXSG" value="157">英雄三国<br>
                	<input type="checkbox" name="FKJYOL" value="158">反恐精英OL&nbsp;&nbsp;&nbsp;<input type="checkbox" name="QQXW" value="159">QQ炫舞&nbsp;&nbsp;&nbsp;<input type="checkbox" name="WMSJ" value="160">完美世界&nbsp;&nbsp;&nbsp;<input type="checkbox" name="LSCS" value="161">炉石传说&nbsp;&nbsp;&nbsp;<input type="checkbox" name="QQFC" value="162">QQ飞车
			-->
			<input type="hidden" name="gamePolicy" id="gamePolicy">
                </div>
            </td>
           
        </tr>
       	 <tr  class="tab_savetd" valign="top"> 
        	<td colspan="3" align="left" style="padding-left:10px; padding-top:4px;">
            	<table cellpadding="0" cellspacing="0" border="0" width="550">
<tr valign="top" >
                    	<td colspan="3" align="left"><span id="tips"><%write(sma_warn);%></span></td>
                    </tr>
                	<tr height="45px">
                    	<!--<td width="70px">&nbsp;</td>-->
                        <td align="center" colspan="2" ><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right" width="200px"><input type="button" class="btn" name="back" onclick="save();" value="<%write(com_save);%>"/>&nbsp;&nbsp;</td>
                    </tr>
                </table>
           </td>
            
        </tr>

    </table>
    </form>
<br>
<br>
</div>
</body>
</html>
