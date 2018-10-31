<!DOCTYPE HTML>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<%langTran("left");%>
<title><%write(left_1);%></title>
<script src="js/config.js" type="text/javascript"></script>
	<link rel="stylesheet" href="css/style.css">
	<!--[if lt IE 8]><!-->
	<link rel="stylesheet" href="css/ie7/ie7.css">
	<!--<![endif]-->
<style type="text/css">
*{margin:0px;padding:0px;font-family: "微软雅黑";}
	.col-1-div,.tabs{float:left;}
.col-1-div{width:420px;color:#fff;font-size: 12px!important;}
*{margin: 0;padding: 0;}
.p1{padding:2px 0px 0px 18px;color:#ffffff!important; }
.p2{padding:1px 0px 2px 18px; color:#ffffff!important;}
.tabs{padding-top:12px;}
.tabs > div {color: #fff;background: #6bb23a;padding-left:10px;padding-right: 10px;line-height: 26px;height: 26px;border-top-left-radius: 2px;border-top-right-radius: 2px;-webkit-border-top-left-radius: 2px;-webkit-border-top-right-radius: 2px;-moz-border-top-left-radius: 2px;-moz-border-top-right-radius: 2px;-o-border-top-left-radius: 2px;-o-border-top-right-radius: 2px;margin-right: 4px;font-size:14px;}
.active{background: #6bb23a!important;cursor: pointer;}
.col-2-div{float:right;}
.col-2-div a{color:#ffffff;display:inline-block;margin-top:9px;font-size:12px;margin-right:14px;}
</style>
</head>

<script language="javascript">
<% outputSystemVersion();%>	//var revisions="nv3520Gv1.0.0-110908";
<% outputProductID();%>	//var ProductID="3520G";
if (typeof(NotAC)=='undefined')
    var NotAC = 0;

/***显示在线ap***/
var onlineap = 0;
var offlineap = 0;
var onlineuser = 0;
var refresh = 5;
//var aj=ajax();
function showAPnum(){
var str ='<a>在线AP&nbsp;:&nbsp;<span id="online">' + onlineap + '</span></a>'+
	'<a>离线AP&nbsp;:&nbsp;<span id="offline">'+ offlineap +'</span></a>'+
	'<a>客户端数&nbsp;:&nbsp;<span id="onlineuser">'+ onlineuser +'</span></a>'; 
    return str;	
}

function getApInform(){
	$.ajax({
				cache : false,
				async:false,
				url : "/goform/formStatForTopDis",
				type : "post",
				dataType : "json",
				success : function(data) {	
					$("#online").html(data.onlineap);
					$("#offline").html(data.offlineap);
					$("#onlineuser").html(data.onlineuser);
				}
		}); 
}

function initization(){
	var str = showAPnum();
	$("#showAp").html(str);
//	$("#refreshInterval").val(refresh);
	apfresh = setInterval("getApInform()",refresh*1000);	
}

function init()                                                                 
{                                                                               
        document.getElementById("productId").innerHTML=ProductID;                 
        document.getElementById("revision").innerHTML=revisions;                 
        if(NotAC ==1){                                                          
                showAp.style.display = "none";                                  
        }
	else
	{	
	    initization();
	}
}                            
</script>
<body style="background:#3a3a3a;" onload="init();">
<div class="container">
	<div class="col-1-div">
		<div class="p1">型号：<span id="productId"></span></div>
		<div class="p2">版本：<span id="revision"></span></div>
	</div>
	<!--<div id="tabs" class="tabs">
		<div><a>快速配置</a></div>
	</div>-->
	<div class="col-2-div" id="showAp">
    
<!--	<a>在线AP&nbsp;:&nbsp;<span>12</span></a>
	<a>离线AP&nbsp;:&nbsp;<span>34</span></a>
	<a>客户端数&nbsp;:&nbsp;<span>456</span></a>
	-->
	</div>
	
</div>
<div style="clear:both;border-bottom:3px solid #8ac760;position:absolute;bottom:0px;width:100%;"></div>

<script type="text/javascript" src="js/jquery.min.js"></script>
<script type="text/javascript">
	function writeTabs(val){
		var s="";
	s = '<div><a>'+val+'</a></div>';
	
     $("#tabs").html(s);
	}

</script>

</body>
</html>
