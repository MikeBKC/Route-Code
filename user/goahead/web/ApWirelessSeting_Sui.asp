<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<!-- <%langTran("Remote");%> -->
<title>无线设置</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<style type="text/css">
	.gj_hide{
		display: none;
	}
	.safe_hide{
		display: none;
	}
</style>
</head>
<script type="text/javascript">


function init(){
//  var f=Remote_Config;
//  var switch_b = $(".on-off").children("button");
//  var httpEnables = (HttpEnables =="1"?"on":"off");
//  initEnabledButton(switch_b,httpEnables);
//  f.OutPort.value=OutPorts;
//  $("#HttpEnable").val(HttpEnables);	
//  $("#error_msg").html(errorstr);

	function safemodelChange(){
		if($('#for_safemodel').val() == 0){
			$('.for_safemodel').addClass('safe_hide');
		}else{
			$('.for_safemodel').removeClass('safe_hide');
		}
	
	
	
	safemodelChange();
	$('#for_safemodel').change(function(){
		safemodelChange();
	});
	
	}
	$('#for_gjmodel').click(function(){
		if($('.for_gjmodel').eq(0).hasClass('gj_hide')){
			$('.for_gjmodel').removeClass('gj_hide');
		}else{
			$('.for_gjmodel').addClass('gj_hide');
		}
	});
	
	$('#showpswd').click(function(){
		if($(this).is(':checked')){
			$('#password').attr('type','text');
		}else{
			$('#password').attr('type','password');
		}
	})
	
	
}
function save(){
//  var f=Remote_Config;
//  if(!checkNumber_P(f.OutPort,1,65535,"<%write(OutPort);%>",0)) return; 
//  
//  var enabled = $("#HttpEnable").val();
//  f.HttpEnable.value=(enabled=="on"?"1":"0"); 
//  
//  Remote_Config.action ="/goform/formRemoteControl";
//  Remote_Config.submit();
}

$(function(){
    init();
    $("[data-control]").change(function(){
    	$(this).prevAll('[data-input]').val($(this).val())
    });
});
</script>
<body>
<div style="width:530px; margin:0 auto; text-align:center;">

<br>
	<form action="" method="post" name="">
    
	<table border="0" cellpadding="0" cellspacing="1" id="" style="background:#5e5e5e;" class="group" width="530px">
    	<tr class="tab_info">
        	<td width="190px">无线网络名称</td>
            <td width="360px" >
            	<input type="text" class="txt-input" />
            	<span>编码</span>
            	<select class="select" style="width: 80px;">
            		<option></option>
            		<option></option>
            	</select>
            </td>
        </tr>
        
        <tr class="tab_info">
        	<td width="190px">隐藏网络</td>
            <td width="360px" >
            	<input type="checkbox" />
            </td>
        </tr>
       
        <tr class="tab_info">
        	<td width="190px">无线客户端隔离</td>
            <td width="360px" >
            	<input type="checkbox" />
            </td>
        </tr>
        
         <tr class="tab_info">
        	<td width="190px">安全模式</td>
            <td width="360px" >
            	<select class="select" id="for_safemodel">
            		<option value="0">无</option>
            		<option value="">有</option>
            	</select>
            	<a id="for_gjmodel" class="for_safemodel">高级选项</a>
            </td>
        </tr>
        
         <tr class="tab_info for_gjmodel for_safemodel">
        	<td width="190px"></td>
            <td width="360px" >
            	<select class="select">
            		<option value=""></option>
            		<option value=""></option>
            	</select>
            </td>
        </tr>
        
        <tr class="tab_info for_gjmodel for_safemodel">
        	<td width="190px"></td>
            <td width="360px" >
            	<select class="select">
            		<option value=""></option>
            		<option value=""></option>
            	</select>
            </td>
        </tr>
        
         <tr class="tab_info for_safemodel">
        	<td width="190px">密码</td>
            <td width="360px" >
            	<input type="password" id="password" />
            	<input type="checkbox" id="showpswd"/>
            	<span>显示密码</span>
            </td>
        </tr>
        
         <tr class="tab_info">
        	<td width="190px">限速策略</td>
            <td width="360px" >
            	<select class="select">
            		<option></option>
            		<option></option>
            	</select>
            </td>
        </tr>
        
         <tr class="tab_info">
        	<td width="190px">上行宽带</td>
            <td width="360px" >
            	<input type="text" class="txt-input" data-input="downs" style="width: 60px;"/>
            	<span>kbit/s <== </span>
            	<select class="select" data-control="downs"  style="width: 80px;">
            		<option value="0">不限制</option>
            		<option value="64">64K</option>
            	</select>
            	<span>（0表示不限速）</span>
            </td>
        </tr>
        
        <tr class="tab_info">
        	<td width="190px">下行宽带</td>
            <td width="360px" >
            	<input type="text" class="txt-input" data-input="downx" style="width: 60px;"/>
            	<span>kbit/s <== </span>
            	<select class="select" data-control="downx" style="width: 80px;">
            		<option value="0">不限制</option>
            		<option value="64">64K</option>
            	</select>
            	<span>（0表示不限速）</span>
            </td>
        </tr>
        
        <tr class="tab_info">
        	<td colspan="2" height="60px" style="text-align: right;">
        		<button type="button" class="btn" style="margin: 20px 50px;">保存</button>
        	</td>
        </tr>
    </table>
    </form>
<br>
<br>
</div>
</body>
</html>
