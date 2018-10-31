<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("Diag");%>
<title><%write(diag_testing);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>
</head>
<script type="text/javascript">
function IsURL(str_url){
    var strRegex = "^((https|http|ftp|rtsp|mms)?://)"
    + "?(([0-9a-z_!~*'().&=+$%-]+: )?[0-9a-z_!~*'().&=+$%-]+@)?" //ftp��user@
    + "(([0-9]{1,3}\.){3}[0-9]{1,3}" // IP��ʽ��URL- 199.194.52.184
    + "|" // ����IP��DOMAIN��������
    + "([0-9a-z_!~*'()-]+\.)*" // ����- www.
    + "([0-9a-z][0-9a-z-]{0,61})?[0-9a-z]\." // ��������
    + "[a-z]{2,6})" // first level domain- .com or .museum
    + "(:[0-9]{1,4})?" // �˿�- :80
    + "((/?)|" // a slash isn't required if there is no file name
    + "(/[0-9a-z_!~*'().;?:@&=+$,%#-]+)+/?)$";
    var re=new RegExp(strRegex);
    if (re.test(str_url)){
	$("#error_msg").html("");
	return (true);
    }else{
	$("#error_msg").html("<%write(diag_warn1);%>");
	return (false);
    }
}

$(function(){
    $("#diagnoseBtn").click(function(){
	$("#result").val("");
	var $hostObj = $("#address");/*��ȡaddress�ı���jquery����*/	
	var host = $.trim($hostObj.val());
	if (host == "") {
	    $("#error_msg").html("<%write(diag_warn2);%>");
	    return false;
	} else {
	    $("#error_msg").html("");
	}
	if(!IsURL(host)){
	    return false;	
	}
	$(this).prop("disabled", true);
	$.post("/goform/formDiagnose",{host:host},function(){});
	$("#result").val("");/*����ı���*/
	$('body').everyTime('1s','Timer1',function(){
	    $.post("/goform/formDiagnoseResult",{host:host},function(data){
		if (data != "") {
		    $("#result").val(data);
		}
	    });
	},5,true);
	$('body').oneTime('5s','Timer1',function(){
	    $("#diagnoseBtn").prop("disabled", false);
	});

    });
    $("#address").get(0).focus();

    $('body').keydown(function(event){
	if (event.which == 13) {
	    event.preventDefault();
	    if ($("#diagnoseBtn").prop("disabled") == false) {
		$("#diagnoseBtn").click();
	    }
	}
    });
});
</script>
<body>
<div class="warp">

<br>
<!--	<form action="" method="post" name="Diagnostic_Config">-->
    
	<table border="0" cellpadding="0" cellspacing="1"  style="background:#5e5e5e;" class="group" >
   
    	<tr class="tab_info" style="height:40px;">
        	<td width="120px"><%write(diag_IPHost);%></td>
            <td colspan="2"><input type="text"  class="txt-input" name="address"  style="width:190px;"id="address" value="" />&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="btn" name="back" id="diagnoseBtn" value="<%write(diag_ping);%>"/>&nbsp;&nbsp;&nbsp;<span id="error_msg" style="color:red;"></span></td>
        </tr>
        <tr  style="height:200px;background:#fff;text-align:left;">
        	<td style="padding-left:10px;"><%write(diag_result);%></td>
            <td colspan="2" style="vertical-align:top;" id="" style="padding-left:0px;">
	    
<textarea id="result" value="" style="font-size:15px; font-family:'������'; width:480px; height:250px;padding-left:10px;" readonly="readonly"></textarea>
	    </td>
           
        </tr>
        
    </table>
<!--    </form>-->
<br>
<br>
</div>
</body>
</html>
