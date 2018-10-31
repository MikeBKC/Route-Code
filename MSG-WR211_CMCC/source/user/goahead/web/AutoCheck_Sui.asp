<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("WANConfigSui");%>
<title><%write(com_notitle);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<link href="css/default.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js" ></script>
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>
<script type="text/javascript" src="script/menu.js" ></script>
<script type="text/javascript" src="js/common.js" ></script>
</head>
<style type="text/css"> 
.btn_nomal {
	width:200px;
		background: #0099D2;	
}
.btn_loading {
	width:200px;
		background: #aaaaaa;
}


	table.zone td.tor{
		text-align: right;
		width:80px;
	} 
</style>
<body>
<form action="" method="post" name="AutoCheckConfig"> 

<div class="pop_page">
<!--<div class="title">-->
	<!--<h2>上网设置<i>将自动检测您的上网方式 , 或手动设置</i></h2>-->
<!--</div>-->
<div style="text-align:left;padding-left: 50px;">
	<br><br>
	<b><%write(acsui_tip);%></b> <br>  <br><input type="button" value="<%write(acsui_autoset);%>" class="btn btn_nomal" id="autowantype" style="width:200px;">&nbsp;&nbsp;&nbsp;<span style="color:red;text-align:center;" id="error_box"></span>
	<br><br>
	<b><%write(com_or);%></b><br><br>  <input type="button" value="<%write(acsui_manuset);%>" class="btn btn_nomal" id="go_setup" onclick="window.parent.open_windows(null,'internet',3);" style="width:200px;">
	<br><br>
</div>
 
	<div style="text-align:left;padding-left: 50px;">
	<a href="#" id="next_step" onclick="window.parent.open_windows(null,'guide_wifi');"   ><%write(wansui_skipset);%>  >></a>
	</div>
 
<br>
 </div>
<script type="text/javascript"> 
var wantype = new Array();
 
wantype[0] = "PPPoE";
wantype[1] = "DHCP";
wantype[2] = "<%write(acsui_static);%>";
var connStatus=0;
var connType="DHCP";
 var autowantype =3;
var waitflag = 1;
var oldconnStatus=0;

function reCheck(){
    waitflag = 0;
    $("#autowantype").val("<%write(acsui_checking);%>");
    $("#autowantype").attr("disabled",true);
    $("#autowantype").addClass("btn_loading").removeClass("btn_nomal");
    $("#error_box").html('');
    $.ajax({
	type:"get",
	url:"/goform/formWanReCheck_Sui?date="+new Date().getTime()
    });
}

	function check(){
		$("#autowantype").val("<%write(acsui_checking);%>");
		$("#autowantype").attr("disabled",true);
		$("#autowantype").addClass("btn_loading").removeClass("btn_nomal");
		$("#error_box").html('');
		//network/get_auto_wan_type 
			    
				if(autowantype == 3){
				
				} else if(autowantype == 98){
					$("#autowantype").val("<%write(acsui_unknown);%>");
					$("#autowantype").addClass("btn_loading").removeClass("btn_nomal");
				} else if (autowantype == 99){
					$("#autowantype").attr("disabled",false);
					$("#autowantype").val("<%write(acsui_recheck);%>");
					$("#autowantype").addClass("btn_nomal").removeClass("btn_loading");
					$("#error_box").html('<%write(acsui_disconnect);%>');
					$('#autowantype').unbind("click"); 
			    		$("#autowantype").click(function(){
					    reCheck();
					});
				} else {
					$("#autowantype").attr("disabled",false);
					$("#autowantype").val("<%write(acsui_setas);%>"+wantype[autowantype]+"<%write(acsui_way);%>");
					$("#autowantype").addClass("btn_nomal").removeClass("btn_loading");
					$('#autowantype').unbind("click"); 
					$("#autowantype").click(function(){
						var auto_type = autowantype;
						window.parent.open_windows(null,'internet',auto_type);
					});
					$("#autowantype").click();
				}
		
	
	}


function getConnStatus(){
    $.ajax({
	type:"get",
	url:"/goform/formStatForWan_Sui?date="+new Date().getTime(),
	success:function(data, textStatus){
	    eval(data);
	    if ((oldconnStatus == 0) && (connStatus != 0))
	    {
		$("#autowantype").val("<%write(acsui_checking);%>");
		$("#autowantype").attr("disabled",true);
		$("#autowantype").addClass("btn_loading").removeClass("btn_nomal");
		$("#error_box").html('');
	    }
	    if (connStatus == 0)
	    {
		autowantype = 99;
	    }
	    else if (connStatus != 3)
	    {
		if (connType == "PPPOE")
		{
		    autowantype = 0;
		}
		else if (connType == "DHCP")
		{
		    autowantype = 1;
		}
		else
		{
		    autowantype = 2;
		}
	    }
	    if (waitflag <1)
	    {
		waitflag = waitflag + 1;
	    }
	    else
	    {
		check();
	    }
	    oldconnStatus = connStatus;
	}
    });
}
$(function(){
	reCheck();
	$('body').everyTime('1s','TimerG',getConnStatus,0);
	
	//引导模式下一步
	function guide_mode_next_step(){
		window.parent.guide_step2();
		window.parent.art.dialog.list['func_box'].close();
	}
	
	$("#next_step").click(function(){guide_mode_next_step();});
	
})
</script>
</form>
</body>
</html>
