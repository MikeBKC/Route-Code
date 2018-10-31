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
.container{text-align:center;padding-right:30px;}
.EasyManagetitle{ text-align:center; padding-bottom:15px;}
.EasyManagetips2{ padding-bottom:20px; padding-top:10px;}
</style>
<script type="text/javascript" ContentType="text/xml">
function timeLeft() {
    var iTime = document.getElementById("timer").innerHTML;
    if (iTime - 1 <= 0)
    {
	clearInterval(timename);
	window.opener = null;    
	window.open('', '_self', ''); 
	window.close();    
    }
    document.getElementById("timer").innerHTML=iTime-1;
}
timename=setInterval('timeLeft();',1000);
$(function(){
	$(".easywrapper").corner("15px");
})
</script>
<style type="text/css">

</style>
<body id="EasySuccess">
<form action="" method="post" name="EasyManageWelcome_config">
<div class="easywrapper">
   <div class="container">
   		<div class="EasyManagetitle"><%write(eas_success);%></div>
   		<div class="EasyManagetips1"><%write(eas_exitTips1);%><span id="timer">3</span><%write(eas_exitTips2);%></div>
        <div class="EasyManagetips2"><%write(eas_exitTips4);%></div>
   </div>
</div>
</form>
</body>
</html>
