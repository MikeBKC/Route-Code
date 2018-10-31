<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("Remote");%>
<title><%write(Remote);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
</head>
<script type="text/javascript">
/*远程管理控制信息*/
<% aspOutRemoteControlInfo();%>

function init(){
    var f=Remote_Config;
    var switch_b = $(".on-off").children("button");
    var httpEnables = (HttpEnables =="1"?"on":"off");
    initEnabledButton(switch_b,httpEnables);
    f.OutPort.value=OutPorts;
    $("#HttpEnable").val(HttpEnables);	
    $("#error_msg").html(errorstr);
    showSuccessTips(rightstr);
}
function save(){
    var f=Remote_Config;
    if(!checkNumber_P(f.OutPort,1,65535,"<%write(OutPort);%>",0)) return; 
    /*
    var enabled = $("#HttpEnable").val();
    f.HttpEnable.value=(enabled=="on"?"1":"0"); 
    */
    Remote_Config.action ="/goform/formRemoteControl";
    Remote_Config.submit();
}

$(function(){
    init();
    $(".on-off").click(function(){
	    var switch_b = $(".on-off").children("button");
	    var status_now = switch_b.attr("class");
	    if(status_now=="on"){
		$("#HttpEnable").val("0");	
	    }else{
		$("#HttpEnable").val("1");	
	    }
	    swicth_act(status_now);
    });
});
</script>
<body>
<div style="width:530px; margin:0 auto; text-align:center;">

<br>
	<form action="" method="post" name="Remote_Config">
    
	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;" class="group" width="530px">
   
    	<tr class="tab_info">
        	<td width="190px"><%write(rou_open);%></td>
            <td width="360px" colspan="2"><span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="HttpEnable" id="HttpEnable" value="" /></span></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(rou_dealPort);%></td>
            <td colspan="2"><input type="text" size="12" style="width:60px;"class="txt-input" name="OutPort" onKeyPress="figureOnly()" maxlength="5"><input type="hidden" name="Profile" value="WAN1"> </td>
           
        </tr>
       
       
          <tr class="tab_savetd_tip"  valign="top"> 
        	<td colspan="3" align="left" style="padding-left:10px; padding-top:4px;">
            	<table cellpadding="0" cellspacing="0" border="0" width="510">
                	<tr valign="top" >
                    	<td colspan="3" align="left"><span id="tips"><%write(rou_warn);%></span></td>
                    </tr>
                	<tr height="45px">
                    	<td width="90px">&nbsp;</td>
                        <td align="left"><span id="error_msg"  style="color:red;"></span></td>
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
