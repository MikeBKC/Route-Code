<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("EasyTask"); %>
<title></title>
</head>
<script type="text/javascript" src="js/commonSkipAuth.js"></script>
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
</script>
<style type="text/css">
.wrapper{
width:80%;
margin:0 auto;
}
/*通用样式*/
*{margin:0;padding:0; font-size:20px;}
.sharp{width:640px;margin:20px ; padding-top:50px;}
.content{height:220px; padding:20px; padding-left:30px;}
h3{height:35px;line-height:35px;}
a:link,a:visited{color:#000; text-decoration:none; }
a:hover{text-decoration:none; border-bottom:1px orange solid;color:orange;}
a:focus {
     outline:none;
     -moz-outline:none;
}
/*上圆角框通用设置样式，如果要运用多个不同颜色，以下6句不用重新变化--------------------------------*/
.content {border:7px solid #000000;overflow:hidden;}
/*颜色方案一,蓝色边框----------------------------------------*/
/*下面第一、二句决定边框颜色，第三句决定背景颜色*/
/*边框色*/
.color1 .b2,.color1 .b3,.color1 .b4,.color1 .b5,.color1 .b6,.color1 .b7,.color1 .content{border-color:#96C2F1;}
.color1 .b1,.color1 .b8{background:#96C2F1;}
/*背景色*/
.color1 .b2,.color1 .b3,.color1 .b4,.color1 .b5,.color1 .b6,.color1 .b7,.color1 .content{background:#F2F2F2;}
</style>
<body style="background:#008899">
<form action="" method="post" name="EasyManageWelcome_config">
<div class="wrapper" align="center">
    <!--风格一，只需要变换一个CLASS就可以换一种颜色-->
    <div class="sharp color1">
        <b class="b1"></b><b class="b2"></b><b class="b3"></b><b class="b4"></b> 
        <div class="content"> 
			  <div align="center" style="padding-left:20px;">
<br/><br/><br/><p style="text-align:center; font-weight:bold; padding-right:40px;font-size:17px;"><%write(eas_success);%></p>
<br/><p  style="text-align:center; font-weight:bold; padding-right:40px;font-size:17px;"><%write(eas_exitTips1);%><span id="timer">3</span><%write(eas_exitTips2);%></p>
<p  style="text-align:center; font-weight:bold; padding-right:40px;font-size:17px"><%write(eas_exitTips3);%></p>

              </div>
        </div>
        <b class="b5"></b><b class="b6"></b><b class="b7"></b><b class="b8"></b>    
    </div>
	</div>
	<div style="clear:both;"></div>
	</div>
</form>
</body>
</html>
