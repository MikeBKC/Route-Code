<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>

<script type="text/javascript">
<% outputhardwarenat(); %>
</script>
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
	
<div id="bindInfo">
	
	<div class="btns">
	   <div class="btndiv"><a class="btn" id="bindBtn">启动硬件NAT</a></div>
	   <div class="btndiv"><a class="btn" id="unbindBtn">关闭硬件NAT</a></div>
  	</div>
</div>
</div>
<form id="form">
  <input type="hidden" id="brc" name="natmark" value=""/>
</form>
<script type="text/javascript">

if(natmark == 0){
	showPage1();
}else{
	showPage2();
}

function showPage1(){
	$('#tip').hide();
	$('#bindBtn').show();
	$('#unbindBtn').hide();
}

function showPage2(){
	$('#tip').show();
	$('#bindBtn').hide();
	$('#unbindBtn').show();
}


$("#bindBtn").click(function(){
    form.action="/goform/Formhardwarenat";
    brc.value = '1';
    form.submit();
	showPage2();
});


$("#unbindBtn").click(function(){
	form.action="/goform/Formhardwarenat";
    brc.value = '0';
    form.submit();
    showPage1();
}); 
</script>
</body>
</html>
