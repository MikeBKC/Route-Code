<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<title>绑定</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
</head>
<style type="text/css">
	#bindInfo div{
		text-align:center;
	}
	#bindInfo .btn,#bindInfo,#tip{
		display: none;
	}
	#waiting{
		text-align: center;
		line-height: 100px;
		height: 100px;
	}
	#bindInfo div{
		line-height: 40px;
	}
	#tip{
		display: none;
	}
	.btns{
		position: absolute;
		bottom: 20px;
		margin: 0 auto;
		text-align: center;
		left: 0;
		right: 0;
	}
</style>

<body id="wireless_set" scroll="no">
<div>
	<div id="waiting">正在获取信息，请稍等....</div>
<div id="bindInfo">
	<div id="tips"><div id="bindTip">设备已绑定到微信，可以通过微信管理设备</div>
		<div id="tip">微信扫描首页二维码，绑定设备，实现微信管理设备</div>
	</div>
	<div class="btns">
	   <div class="btndiv"><a class="btn" id="bindBtn">绑定</a></div>
	   <div class="btndiv"><a class="btn" id="unbindBtn">解除绑定</a></div>
  	</div>
</div>
</div>
<form id="form">

</form>
<script type="text/javascript">
<% outputProductID();%>	//var ProductID="3520G";
var url = "http://api.greenwifi.com.cn/wx/deviceStatus?info="+info 
var time = 6; 
var srcEle = document.createElement('script'); 
srcEle.setAttribute('src', url); 
document.getElementsByTagName('head')[0].appendChild(srcEle); 
function poll_bind() { 
    if (typeof (deviceStatus) != "undefined") { 
	$("#waiting").hide(); 
	$("#bindInfo").show(); 
	if(deviceStatus == 1){//.......... 
	    $("#unbindBtn").show(); 
	    $("#bindBtn").hide(); 
	    $("#bindTip").show(); 
	}else{ 
	    $("#bindTip").hide(); 
	    $("#unbindBtn").hide(); 
	    $("#bindBtn").show(); 
	} 
	return true; 
    } 
    setTimeout(poll_bind, 100); 
    return false; 
} 
poll_bind(); 
function poll_unbind() { 
    if (typeof (unbindStatus) != "undefined") { 
	$("#form").submit(); 
	return true; 
    } 
    setTimeout(poll_unbind, 100); 
    return false; 
} 
var timeInerval = setInterval(setTime,1000); 
function setTime(){ 
    time--; 
    if(time <= 0){ 
	clearInterval(timeInerval); 
	$("#waiting").html("......................"); 
    } 
} 
$("#bindBtn").click(function(event) { 
	$("#tip").show(); 
	}); 
$("#unbindBtn").click(function(event) { 
	var url = "http://api.greenwifi.com.cn/wx/unbindDevice?info="+info 
	var srcEle = document.createElement('script'); 
	srcEle.setAttribute('src', url); 
	document.getElementsByTagName('head')[0].appendChild(srcEle); 
	poll_unbind(); 
	}); 
</script>
</body>
</html>
