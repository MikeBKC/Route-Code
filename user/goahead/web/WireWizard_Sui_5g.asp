<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("WirelessSui");%>
<title><%write(com_notitle);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript" src="script/jquery.artDialog.js"></script> 
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>
<script type="text/javascript" src="script/menu.js"></script> 
<script type="text/javascript" src="js/common.js"></script> 
</head>
<style type="text/css">

	.group th { padding-right:30px;}
</style>
<script type="text/javascript">

var WrlessEnables_5g="";var ssids_5g="";var broadcastEnables_5g=0;var pskPsswds_5g="";
<% aspoutWlanWizard_Sui_5g(); %>
	//关闭
		//开关
$(function(){
	parent.art.dialog.list["func_box"].title("<%write(wwsui_5gset);%>");
	var enabledWifi =WrlessEnables_5g;
	var switch_b = $(".on-off").children("button");
	initEnabledButton(switch_b,enabledWifi);
	$("#wifiname").val(ssids_5g);
	$("#wifipwd").val(pskPsswds_5g);
	$("#wifipwdis").val(pskPsswds_5g);
	if (broadcastEnables_5g == 0)
	{	
	    $("input[name='hidenet']").attr("checked", true);
	}
	$(".on-off").click(function(){
	    var switch_b = $(".on-off").children("button");
	    var status_now = switch_b.attr("class");
	    swicth_act(status_now);
	});
	$("#showpwd").click(function(){
	    var temp=document.getElementById('wifipwdis').value;
	    if($("#showpwd").attr("checked") == "checked")
	    {
		$("#sp1").html("<input type='text' class='txt-input' id='wifipwdis' name='wifipwdis'/>");
	    }
	    else
	    {
		$("#sp1").html("<input type='password' class='txt-input' id='wifipwdis' name='wifipwdis'/>");
	    }
	    $("#wifipwd").val(temp);
	    $("#wifipwdis").val(temp);
	});
});
function checkData()
{
    var f = GuideWireless_Config;
    if (!checkName_P(f.wifiname,32,"<%write(wwsui_netname);%>",0))  return false; 
    f.wifipwd.value = f.wifipwdis.value;
    var len=f.wifipwd.value.length;
    if(len>0 && len<8){$("#error_msg").html("<%write(wwsui_passlen);%>");return false;}
    if(!checkPassword1(f.wifipwd,"63",1)) return false;
    var switch_b = $(".on-off").children("button");
    f.ebabledwifi.value = switch_b.attr("class");
    return true;
}

function formsave(){
    var f = GuideWireless_Config;
    var state = "off";
    if (f.hidenet.checked == true)
    {
	state = "on";
    }
    f.hidenet.value=state;
    f.action="/goform/formWlanWizard_Sui_5g";
    f.submit();
    /*
    $.ajax({
	type:"post",
	data:{"ebabledwifi":f.ebabledwifi.value, "wifiname":f.wifiname.value, "hidenet":state, "wifipwd":f.wifipwd.value},
	url:"/goform/formWlanWizard_Sui",
	success:function(data, textStatus){
	    window.parent.art.dialog.list['func_box'].close();
	}
    });
    */
}

function save()
{
    if (checkData())
    {
	$('body').everyTime('1s','TimerG',formsave,1);
    }
}
</script>
<body id="guide_wireless_body">
<div class="warp">
	<br>   <br>
    <div class="tab_content" style="width:600px; text-align:center; margin:0 auto;">
    <br>
    <form action="" method="post" name="GuideWireless_Config">
 
	<table cellpadding="0" cellspacing="0" class="group" style=" width:600px;" border="0">
     <div id="error_msg" style="color:red; height:15px;"></div>
    	<tr>
        	<th style=" width:200px;"><%write(wwsui_wlanEn);%></th>
            <td  style=" width:200px;" align="left">&nbsp;<span class="on-off"><button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="ebabledwifi" id="ebabledwifi" value="" /></span>
			</td>
            <td style=" width:200px;"><input type="hidden" name="wifipwd" id="wifipwd" value=""/></td>
        </tr>
        <tr>
        	<th style=" width:200px;"><%write(wwsui_netname);%></th>
            <td align="left"><input type="text"  class="txt-input" name="wifiname" id="wifiname" value="" /></td>
            <td align="left"><%write(wwsui_hidenet);%>&nbsp;&nbsp;<input type="checkbox" name="hidenet" id="hidenet"/></td>
        </tr>
        <tr>
        	<th style=" width:200px;"><%write(wwsui_netpwd);%></th>
            <td align="left"><span id="sp1"><input type="password"  class="txt-input" name="wifipwdis" id="wifipwdis" value=""/></span></td>
            <td align="left"><%write(com_dispass);%>&nbsp;&nbsp;<input name="showpwd" id="showpwd" type="checkbox"/></td>
        </tr>
    </table>
    <br>
     <div class="tabs_link"><span onClick="window.parent.art.dialog.list['func_box'].close();"><%write(com_exitset);%> >></span></div>
     <br>
     <div class="tabs_button"><input type="button" class="btn" name="back" onClick="window.parent.open_windows(null,'guide_wifi',null);" value="<%write(com_previous);%>"/>&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="btn" name="back" value="<%write(com_save);%>" onClick="save();"/></div>
     </form>
     <br>
    </div>
</div>
</body>
</html>
