<!DOCTYPE HTML>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />

<title>ToMAX</title>
<script language="JavaScript" type="text/javascript">

function init()
{
	main.location.href="/goform/FastConfFirstLoad";/*防止早于assist加载*/
}
</script>
</head>

<frameset rows="46px,40,*" border="0" frameborder="0" id="mainFrameSet" name="mainFrameSet" framespacing="0" marginwidth="0" marginheight="0">
	<frame name="header" src="header.asp" scrolling="no" />
	<frame name="secondMenu" id="secondMenu" src="secondMenu.asp" scrolling="no" />
		<frameset cols="208px,0,*" id="frameset1" name="frameset1" border="0" frameborder="0" framespacing="0" onload="init()" />
			<frame name="menu" src="menu.asp" scrolling="auto"/>
			 <frame src="assist.asp" id=assist name=assist style="display:none"></frame>
			<!-- <frame name="btn" src="btn.htm" scrolling="auto"/> -->
			<frame name="main" id="main" scrolling="auto"/>
		</frameset>
	<!-- <frame name="footer" src="footer.htm" scrolling="no"> -->
</frameset>
<noframes>
<body>

</body>
</html>
