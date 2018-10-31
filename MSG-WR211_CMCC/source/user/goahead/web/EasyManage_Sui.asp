<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("Menu");%>
<%langTran("EasyTask");%>
<title><%write(easyManage);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript" src="js/common.js"></script>
</head>
<script type="text/javascript">
var easyTaskEn = "off";
var easyTaskNoticeEn = "on";
<% aspoutEasyTask(); %>

function init(){
	var f = EasyManageConfig;
	var switch_b = $(".on-off").children("button");
	initEnabledButton(switch_b, easyTaskNoticeEn);	
	$("#noticeEnable").val(easyTaskNoticeEn);
showSuccessTips(rightstr);
}
function save(){
    var f=EasyManageConfig;
    var switch_b = $(".on-off").children("button");
    var status_now = switch_b.attr("class");
    f.action="/goform/formEasyTask";
    f.submit();
}
$(function(){
	init();
	$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		if(status_now=="on"){
		    $("#noticeEnable").val("off");	
		}else{
		    $("#noticeEnable").val("on");	
		}
		swicth_act(status_now);
	});
});
</script>
<body scroll="no">
<div style="width:500px; margin:0 auto; text-align:center;">

<br>
	<form action="" id="EasyManageConfig" method="post" name="EasyManageConfig">
    
	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;"class="group" width="500px">
   
    	<tr class="tab_info">
        	<td width="140px"><%write(eas_errWarn);%></td>
            <td colspan="2" width="360px">&nbsp;<span class="on-off"><button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="noticeEnable" id="noticeEnable"><input type="hidden" name="easyManageEn" id="easyManageEn" value="on" /></span></td>
	</tr>

          <tr  height="60px" class="tab_savetd" valign="top"> 
        	<td colspan="3" align="left" style="padding-left:10px; padding-top:4px;">
            	<table cellpadding="0" cellspacing="0" border="0" width="480">
       
<tr valign="top">
        	<td colspan="3" align="left"><span id="tips"><%write(eas_warn);%></span></td>
	</tr>
                	<tr>
                    	<td colspan="2">&nbsp;<span id="error_msg"  style="color:red;"></span></td>
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
