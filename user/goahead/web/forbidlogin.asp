<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("AccessControl");%>
<title><% aspGetTitle(); %></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js" ></script>
<script type="text/javascript" src="script/jquery.corner.js" ></script>
</head>
<style type="text/css">
.container{text-align:center;padding-right:30px;}
.errorinfo{text-align:center;}
.tryclass{text-align:center;padding-top:30px;}
.tryclass a:link,a:visited{color:#000; text-decoration:none; }
.tryclass a:hover{text-decoration:none; border-bottom:1px orange solid;color:orange;}
.tryclass a:focus {
     outline:none;
     -moz-outline:none;
}
</style>
<script type="text/javascript">
<%aspForbidLessTime();%>
var lesstime = lessTimes;
function tryAgain()
{
    window.location.href="home.asp";
}
function count_time()
{
    if(lesstime < 0)
    {
	document.getElementById('showtime').style.display = "none";
	document.getElementById('trybtn').style.display = "";
    }
    else
    {
	document.getElementById("timenum").innerHTML = lesstime;
	document.getElementById('showtime').style.display = "";
	document.getElementById('trybtn').style.display = "none";
	lesstime--;
    }
}
$(function(){
    $(".easywrapper").corner("15px");
    count_time();
    setInterval(count_time,1000);
})
</script>
<body id="EasySuccess">
<form action="" method="post" name="ForbidLogin">
    <div class="easywrapper">
    	<div class="container">
            <div class="errorinfo"><%write(forbid_message);%></div>
            <div class="tryclass" ID="showtime"><%write(less_time);%>&nbsp;<span id="timenum">30 &nbsp;</span>s</div>
            <div class="tryclass" ID="trybtn"><a onClick="tryAgain();" ><span><%write(try_again);%></span></a></div>
        </div>
    </div>
</form>
</body>
</html>
