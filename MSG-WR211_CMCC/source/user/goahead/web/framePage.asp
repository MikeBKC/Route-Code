<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<title><%write(com_utt);%></title>
<script language="JavaScript" type="text/javascript">
	if (top!= self)
	{ top.location.href = location.href; 
		}

//window.onbeforeunload = onbeforeunload_handler;   
function onbeforeunload_handler(){ 
 	if ((event.clientY <= -5 && event.clientY >= -20) || (event.clientX > document.body.clientWidth && event.clientY < 0 || event.altKey)) {
		//location.href="/goform/Logout";	
		//alert("您已经成功退出系统!");
	} else if (document.body.clientWidth - event.clientX < 60 && event.clientY < 0 || event.altKey) {
		//location.href="/goform/Logout";	
		//alert("您已经成功退出系统!");
	}
	 
} 
function init()
{
	main.location.href="/goform/FastConfFirstLoad";/*防止早于assist加载*/
}
</script>
</head>
<frameset cols="70,955,70" frameborder="0" scrolling="no"  id="mostOuterFrameSet" name="mostOuterFrameSet">
<!--<frameset cols="70,955,70" frameborder="0" scrolling="no" onUnload="onbeforeunload_handler()" >-->
<!--左侧框架(不使用)-->
<frame  scrolling="no" noresize></frame>
<!--<frame src="UntitledFrame-5" noresize></frame>-->
<noframes></noframes>
<!--中间居中的框架,中间自己要用的框架,可按需求在对此框架进行处理-->
<frameset rows="88,*,29" scrolling="no" frameborder="0" id="mainFrameSet" name="mainFrameSet">
<frame src="top.asp"   scrolling="no" noresize></frame>
<frameset cols="205,*" frameborder="0" id="middleFrameSet" name="middleFrameSet">
	<!--<frame src="" scrolling="no" noresize ></frame>-->
<frame src="left.asp"   scrolling="auto" noresize name="leftMenu"></frame>
<frameset rows="0,28,*" scrolling="no" frameborder="0" onload="init()">
<frame src="assist.asp" id=assist name=assist style="display:none"></frame> <!--隐藏框架 用于辅助-->
<frame src="secondTop.asp" scrolling="no" noresize name="secondTop"></frame>
<frame scrolling="auto" name="main" noresize  ></frame>
<!--<frame src="FastConfig.asp"   scrolling="auto" name="main" noresize  ></frame>-->
</frameset>
</frameset>
<frame src="bottom.asp" scrolling="no"  noresize></frame>
<frame  scrolling="no"  noresize></frame>
</frameset>

<!--右面框架(不使用)-->
<frame scrolling="no" noresize></frame> 
<!--<frame src="UntitledFrame-6" scrolling="no" noresize></frame> -->
</frameset><noframes></noframes>
</html>
