<!DOCTYPE html>
<html>
<head>
	<meta charset="GBK">
	<%langTran("SoftwareUpdate");%>
	<title>Document</title>
	<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script>
	<style type="text/css">
	.progress{width:50%;margin:0 auto;text-align:center;}
	.bgBar{width:100%;border:1px solid #bdbdbd;}
	.bar{height:30px;text-align:left;width:0;background: -webkit-gradient(linear, left top, left bottom, from(#3F8FD2), color-stop(0.5, #f2f2f2), to(#3F8FD2));
	background: -moz-linear-gradient(top,#3F8FD2,#f2f2f2,#3F8FD2);
	filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#3F8FD2', endColorstr='#f2f2f2', GradientType='0');
	}

</style>
</head>

<body>
	<div class="progress">
		<div class="bgBar" id="bgBar">
			<div class="bar" >&nbsp;</div>
		</div>
		
		<div class="message"></div>
	</div>


<script type="text/javascript">
    function setProgress(val){
	if(val){
	    $(".bar").css("width",val+"%");
	    $(".message").html(val+"%");
	}
    }
    var i = 0;
    function doProgress() { 
	if (i >= 100) { 
	    $.ajax({
		url:"/goform/formAutoUpdateFirmware",
		cache:false,
		success: function(rsp){
		    if (rsp) {
			location.href = rsp;
		    }
		},
		error:function(data){
		}
	    });
	    return; 
	} 
	if (i < 100) { 
	    setTimeout("doProgress()", 1000); 
	    $.ajax({
		url:"/goform/formSoftDownload",
		cache:false,
		async:false,
		dataType:"json",
		success: function(rsp){
		    if(rsp){
			if (rsp.isError == 1) {
			    $(".message").html("<%write(UploadWarn3);%>");//加载提示 
			} else if (rsp.isError == 2) {
			    $(".message").html("<%write(UploadWarn2);%>");//加载错误提示 
			} else {
			    setProgress(rsp.ratio); 
			    i = rsp.ratio; 
			    if (i == 100) {
				$(".message").html("<%write(UploadWarn1);%>");//加载完毕提示 
			    }
			}
		    }
		},
		error:function(data){
		
		}
	    });
	 } 
    } 
    $(function(){
	doProgress();
    });
</script>
</body>
</html>
