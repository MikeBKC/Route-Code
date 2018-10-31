<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">

<% langTran("EasyTask"); %>
<title><% aspGetTitle(); %></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js" ></script>
<script type="text/javascript" src="script/jquery.corner.js" ></script>
</head>
<style type="text/css">

/**EasyManage**/

.EasyManhello{text-indent: 2em; padding-top:10px;}
.EasyManinfo1{text-indent: 2em; padding-top:10px;}
.EasyManinfo2{padding-top:10px;}
.EasyManyes{ <%write(EasyManageClass);%>; padding-top:20px;}
.EasyManno{ <%write(EasyManageClass);%>;padding-bottom:20px; padding-top:10px;}
.EasyManyes a:link,a:visited,.EasyManno a:link,a:visited{color:#000; text-decoration:none; }
.EasyManyes a:hover,.EasyManno a:hover{text-decoration:none; border-bottom:1px orange solid;color:orange;}
.EasyManyes a:focus,.EasyManno a:focus {
     outline:none;
     -moz-outline:none;
}


</style>
<script type="text/javascript">
$(function(){
$("#easywelcome").corner("15px");	
})
function closeEasyTask()
{
    EasyManageWelcome_config.action="/goform/formCloseDefConf";
    EasyManageWelcome_config.submit();
}
</script>
<body id="EasyWel">
<form action="" method="post" name="EasyManageWelcome_config">
	<div class="easywrapper" id="easywelcome">
    	<div class="container">
        	<div class="EasyMantit"><%write(eas_info1);%></div>
            <div class="EasyManhello"><%write(eas_info2);%></div>
            <div class="EasyManinfo1"><%write(eas_info6);%></div>
            <div class="EasyManinfo2"><%write(eas_info7);%></div>
            <div class="EasyManyes"><a onClick="location='home.asp'"><span><%write(eas_yes);%></span>&nbsp;&nbsp;<%write(eas_configNow);%></a></div>
            <div class="EasyManno"><a onClick="closeEasyTask();" ><span><%write(eas_no);%></span>&nbsp;&nbsp;<%write(eas_noConfig);%></a></div>
            <div class="tipinfo"></div>
        </div>
    </div>
</form>
</body>
</html>
