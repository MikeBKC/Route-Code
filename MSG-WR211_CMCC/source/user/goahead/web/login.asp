<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>login</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js" ></script>
<script type="text/javascript" src="script/curvycorners.js" ></script>
<script type="text/javascript">
if(window.parent != window.self) window.parent.location = window.self.location;
<% outputSystemVersion();%>	//var revisions="nv3520Gv1.0.0-110908";
<% outputProductID();%>	//var ProductID="3520G";
<% isDefaultPassword();%>
    $(document).ready(function(){
	$("#submit_btn").mousedown(function(){
	    login(); 
	});
	$("#input_password1").get(0).focus();
	$("#software").html(revisions);
	$("#hardware").html(hardwareID);
	if (isDefaultPasswd != 1) {
	    $("#isDefaultPassword").hide();
	}
    });
    function login(){ //函数 login();
	var username = $("#username").val();//取框中的用户名
	var password = $("#input_password1").val();//取框中的密码
	if(password==""){
		$("#error_msg").html("密码不能为空，请重新输入!");
		return ;
	}


	$.ajax({ //一个Ajax过程
		type: "post",  
		url : "/index_login.asp?timestamp="+(new Date()).getTime(), 
		dataType:'html',
		cache:false,
		username:username,
		password:password,
		statusCode: {
		401: function() {		
			$("#error_msg").html("密码错误，请重新输入!");
			$("#input_password1").val("");
			$("#input_password1").get(0).focus();
		}
		},
		success: function(data, textStatus){	
		window.location.href="home.asp";
		}
	});

    }

    $(function(){
	$('body').keydown(function(event){
	    if (event.which == 13) {
		event.preventDefault();
		login();
	    }
	});
    });

</script>

</head>

<body id="login">

<h6 style="display:none;">admin</h6>
<div id="loginbox">
	<form id="loginform" name="loginform" method="post" action="" onsubmit=""> 
    	<input autocomplete="off"  type="hidden" id="username" name="username" value="admin" />
		<label>密码</label><input autocomplete="off" class="txt" type="password" name="password" id="input_password1" />
		<input type="button" id="submit_btn" value="登录" class="btn" />
        <br>       
		<span id="error_msg" style="color:red;"></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
		<br>
       <span id="isDefaultPassword"><img src="images_Sui/alert-icon.png" height="15" />您当前的密码是默认密码 admin</span>

    </form>
</div>
<div id="footer">
	<div class="wrap">
		<p class="system_info">软件版本 : <span class="pipe" id="software"></span>硬件版本 : <span id="hardware"></span><span class="pipe"></span>服务热线  : <span id="iphonenum">4006-120-780</span></p>
		<p class="bottom_menu">
			Copyright Right &copy 2011 有限公司
			<span class="pipe">|</span><a href="http://www/" target="_blank">官方网站</a>

		</p>
	</div>
</div>
</body>
</html>
