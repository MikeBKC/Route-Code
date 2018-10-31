<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("EasyTask"); %>
<title></title>
</head>
<script type="text/javascript" src="js/commonSkipAuth.js"></script>
<style type="text/css">
.wrapper{
width:80%;
margin:0 auto;
}
/*通用样式*/
*{margin:0;padding:0; font-size:20px;}
.sharp{width:540px;margin:20px ; padding-top:50px;}
.content{height:280px; padding:20px; padding-left:30px;}
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
.option-link{
	background:url(images/link1.png) no-repeat;
	background-position:left center; 
	font-size:12px; cursor:pointer;
	 
}
</style>
<script type="text/javascript">
var enabled = 0;
var easyTaskOn = 0;
var ConnStatus = 0;
var connStatusSW ="";
<% aspoutEasyTaskNotice(); %>
<% aspOutHotel(); %>
var ajaxObj=ajax();
function save()
{
    if (ajaxObj)
    {
	ajaxObj.url="goform/formEasyTaskNoticeEn";
	if (document.getElementById("easyTaskNoticeEn").checked == true)
	{
	    var sendstr="easyTaskNoticeEn="+"off";
	}
	else
	{
	    var sendstr="easyTaskNoticeEn="+"on";
	}
	ajaxObj.sendString=sendstr;
	var fun=ajaxObj.post;
	fun();
    }
}
function checkConnectStatus(){
    switch(ConnStatus)
    {
	case 0:
	    connStatusSW ="<%write(eas_disconnect);%>";
	    break;
	case 6:
	    connStatusSW ="<%write(eas_userpasserror);%>";
	    break;
	default:
	    connStatusSW ="<%write(eas_connecterror);%>";
    }
    document.getElementById("status").innerHTML = connStatusSW;
}

function pageRedirect(){
    if (easyTaskOn == 1)
    {
	NoticEasyManaConfig.action="/EasyManageConfig.asp?wir=0";
    }
    else
    {
	NoticEasyManaConfig.action="/index.asp";
    }
    NoticEasyManaConfig.submit();
}
		
function init(){
	checkConnectStatus();
	if (hotelPnpEn=="on")
	{
	    moduleDisplay("tips", 1);
	}
	else
	{
	    document.getElementById("contentdiv").style.height="220px";
	}
	document.getElementById("easyTaskNoticeEn").checked = (enabled ? false:true);
}    
</script>
<body style="background:#008899" onLoad="init();">
<form action="" method="post" name="NoticEasyManaConfig">
<div class="wrapper" align="center">
    <!--风格一，只需要变换一个CLASS就可以换一种颜色-->
    <div class="sharp color1">
        <b class="b1"></b><b class="b2"></b><b class="b3"></b><b class="b4"></b> 
        <div id="contentdiv" class="content"> 
				 
              <h3 align="left" style="padding-left:20px;font-size:17px;"><%write(eas_info1);%></h3>
			  <div align="center" style="padding-left:20px;">
              <h3 align="left" style="text-indent:2em;font-size:17px;"><%write(eas_info2);%></h3>
              <h3 align="left" style="text-indent:2em;font-size:17px;"><%write(eas_info4);%><span id="status" style="font-size:17px;color:#FF0000;"></span><%write(eas_info5);%></h3>
              <h3 align="left"></h3>

<br/>
<div class="option-link" style="text-align:right; font-weight:bold; padding-right:20px;padding-top:10px; padding-left:20px; padding-bottom:10px;display: inline;float:right;"><a href="#" title="" onClick="pageRedirect();"><font style="font-size:17px;color:#008899;"><%write(eas_config);%></font></a></div>
              </div>
			  <div style="font-weight:bold;padding-left:20px;padding-top:10px;" align="left">
			  	<input type="checkbox" name="easyTaskNoticeEn" id="easyTaskNoticeEn" onClick="save();" value=""/>&nbsp;<font style="font-size:17px;"><%write(eas_notips);%></font>


	<br/><br/>
		<h4 id="tips" name="tips" align="left" style="padding-left:0px;font-weight:bold;font-size:14px;display:none"><span style="font-size:14px;color:#777777"><%write(eas_hotelTips0);%></span><span style="font-size:14px;color:#777777"><%write(eas_hotelTips1);%></span><%write(eas_hotelTips2);%><span style="font-size:14px;color:#777777"><%write(eas_hotelTips3);%></span></h4>


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
