<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("commonWords");%>
<%langTran("wlanUpdateUpload");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
<%langTran("apSoftMng");%>
//交互begin
var errorstr="";
<%
	aspOutPutApPositionDraw();
    aspOutPutUploadDrawing();
%>
// 交互end

function drawingUpload()
{
	if(totalrecs == max_totalrecs)
	{
		alert("图纸数已达上限，无法导入图纸");
		return false;
	}
	
	if(re_checkSpecialName(DrawingUpload.drawingName, '图纸名称'))
	{
		alert("图纸名称中含有非法字符");
		return;
	}
	

	if(checkDrawing()&&checkData())
	{
	  DrawingUpload.action = "/goform/formUploadDrawing";
	  DrawingUpload.submit();
	}
}



function checkData(){
	var f = DrawingUpload;
	if(!checkName(f.drawingName,11,"图纸名称",0)) return false;	//检查用户名：不能为空、不能全为0、不能超11个字符、应为除:< > , % \' \\ \" & ; 和空格之外的字符。
	return true;
} 


function checkDrawing()
{
	var f=DrawingUpload.uploadDrawing;
	var format=f.value.substr(f.value.lastIndexOf(".")).toLowerCase();
	if (f.value==""){
		 alert("请指定图片");
		 return false;
	}
	else if(format!=".jpg"){
	    alert("请上传.jpg格式的图片!");
	    return false;
	}
	
	var name=f.value.split("\\")[f.value.split("\\").length-1];

	/*
	var allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890._";
	for (i = 0; i < name.length; i++) {
	    if (allowChar.indexOf(name.substring(i, i + 1)) == -1) {
		alert("文件名只允许包含大小写字母、数字、下划线和'.'");
		return false;
	    }
	}
	*/
	var name_tmp = name.split(".");

	name = name_tmp[0]+"."+name_tmp[1].toLowerCase();
	var newname=document.getElementById("drawingName").value;
    var tmpNewName = newname+".jpg";
		//alert("tmpNewName :"+tmpNewName);
	for(i=0; i < totalrecs; i++){
		
	    if(DrawingNames[i] == tmpNewName){
		alert("文件名重复！");
		return false;
	    }
	}

	return true;
}




</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
 <td> <script language="javascript">showSecondTitle("AP分布图");//显示页面名称</script>
   <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						new Array("分布图",'DrawingPreview.asp',1,0),
						new Array("导入图纸",'ImportDrawing.asp',1,1)
						


		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form method="post" name="DrawingUpload" enctype="multipart/form-data" action="">
<br>
<table border="0" bordercolor="red" width="600" align='center'>
  <tr>
    <td width="210" align="right" id="choosDrawing">请选择图纸</td>
    <td width="5" >&nbsp;</td>
    <td width="372" height="20"><input name="uploadDrawing" type="file"></td>
  </tr>
  <tr>
  	<td width="210" align="right">图纸名称<img src="images/qt.gif" width="7" height="9"></td>
    <td width="5">&nbsp;</td>
    <td width="372" height="20"><input name="drawingName" id="drawingName" type="text">(*.jpg)</td>
  </tr>
  <tr>
    <td align="right">&nbsp;</td>
    <td  height="25" align="center">&nbsp;</td>
    <td id="update then reset4">&nbsp;</td>
  </tr>
  <tr><td>&nbsp;</td><td>&nbsp;</td><td   height="25" ><input type="button" name="upload" class="input-button"  value="导入图纸" onClick="drawingUpload();" id="upload">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(help);%>" class="input-button" onClick="window.open('rhelpinfo.htm#Distribution')" id="helpBotton"></td></tr>
</table>
</form>
<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
</td><td class="gapTd"></td></tr>
  </table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
  <td width="15">&nbsp;</td>
  </tr>
</table>
</body>
</html>
