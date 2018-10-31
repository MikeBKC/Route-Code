<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("learnConfig");%>
<title><%write(learnConfig);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="script/frame.js"></script>
<script type="text/javascript" src="js/common.js"></script> 
</head>
<script type="text/javascript">
var rightstr = "";
<%aspOutKnowifiConfig();%>
function init()
{
    var f = knowifi_config;
    var switch_b = $(".on-off").children("button");
    var knowifiEnables = (knowifiEn =="1"?"on":"off");
    initEnabledButton(switch_b, knowifiEnables); 
    $("#knowifiEnable").val(knowifiEnables);
	if(knowifiEn=="1"){
		$("#openLearn_tr").show();
	}else{
		$("#openLearn_tr").hide();
	}
    showSuccessTips(rightstr);
	
	
}

function save()
{
    var f=document.knowifi_config;
    var switch_b = $(".on-off").children("button");
    var status_now = switch_b.attr("class");
    f.action="/goform/formKnowifiConfig";
    f.submit();
}

$(document).ready(function(){
	 $("#openLearn").click(function(){
		//var w = window.open("transition.asp");
		var w = window.open("/goform/formOpenLearn");
		/* $.ajax({
			url:"/goform/formOpenLearn",
			type:"post",
			cache:false,
			dataType:'text',
			data:{
			    routerID:routerID
			},
			success:function(resp){
			    eval(resp);
				w.location="http://service.qingxueba.com/lm/studysetting?rtoken="+router.rtoken;
			}
			    
		    });
			*/
	 });
});

$(function(){
    var f = knowifi_config;
    init();
    $(".on-off").click(function(){
	    var switch_b = $(".on-off").children("button");
	    var status_now = switch_b.attr("class");
	    if(status_now=="on"){
			f.knowifiEnable.value = "off";
			$("#knowifiEn").val("off");
			//$("#openLearn_tr").hide();
			
	    }else{
			f.knowifiEnable.value = "on";
			$("#knowifiEn").val("on");
			//$("#openLearn_tr").show();
	    }
			swicth_act(status_now);
	    });

});
</script>
<body>

<div style="width:500px; margin:0 auto; text-align:center;">
<br>
<form action="" method="post" name="knowifi_config" id="knowifi_config">
<table border="0" cellpadding="0" cellspacing="1" id="knowifiConfig" style="background:#5e5e5e;"class="group" width="500px">
    <tr class="tab_info">
    <td width="140px"><%write(knowifiEnable);%></td>
    <td colspan="2" width="360px">&nbsp;<span class="on-off"><button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="knowifiEn" id="knowifiEn"><input type="hidden" name="knowifiEnable" id="knowifiEnable" value="on" /></span></td>
    </tr>
	<tr class="tab_info" id="openLearn_tr">
		<td><%write(knowconfig);%></td>
		<td><div id="openLearn" name="openLearn" style="cursor:pointer;width:100px"><image src="images_Sui/configure.png" class="tupian" style="vertical-align:bottom;width: 20px;" /><span>&nbsp;<%write(config);%></span></div></td>
	</tr>

<!--
    <tr class="tab_info">
    <td>设备名称</td>
    <td colspan="2">
	<input type="text" class="txt-input" id="devname" name="devname" value="unknown">
    </td>
    </tr>

    <tr class="tab_info">
    <td>IP</td>
    <td colspan="2">
	<input type="text" class="txt-input" id="devip" name="devip" value="0.0.0.0">
    </td>
    </tr>

    <tr class="tab_info">
    <td>MAC</td>
    <td colspan="2">
	<input type="text" class="txt-input" id="devmac" name="devmac" value="00:00:00:00:00:00">
    </td>
    </tr>

    <tr class="tab_info">
    <td>连接状态</td>
    <td colspan="2">
	<input type="text" class="txt-input" id="devstatus" name="devstatus" value="0">
    </td>
    </tr>

    <tr class="tab_info">
    <td>连接类型</td>
    <td colspan="2">
	<input type="text" class="txt-input" id="devconntype" name="devconntype" value="0">
    </td>
    </tr>

    <tr class="tab_info">
    <td>Domain</td>
    <td colspan="2">
	<input type="text" class="txt-input" id="devdomain" name="devdomain" value="unknown">
    </td>
    </tr>

    <tr class="tab_info">
    <td colspan="3">
	<input type="button" class="btn" id="addbtn" name="addbtn" onClick="add();" value="添加">
    </td>
    </tr>

    <tr class="tab_info">
    <td colspan="3">
	<input type="button" class="btn" id="delbtn" name="delbtn" onClick="del();" value="删除">
    </td>
    </tr>

    <tr class="tab_info">
    <td colspan="3">
	<input type="button" class="btn" id="delAllbtn" name="delAllbtn" onClick="delAll();" value="清空">
    </td>
    </tr>
-->

    <tr  height="60px" class="tab_savetd" valign="top"> 
    <td colspan="3" align="left" style="padding-left:10px; padding-top:20px;">
    <table cellpadding="0" cellspacing="0" border="0" width="450">
    <tr valign="top">
    <td colspan="3" align="left"><span id="tips"></span></td>
    </tr>
    <tr>
    <td colspan="2">&nbsp;<span id="error_msg"  style="color:red;">    </span></td>
    <td align="right"><input type="button" class="btn" name="back" id="savebtn" onClick="save();"value="<%write(com_save);%>"/></td>
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
