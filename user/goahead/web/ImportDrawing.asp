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
//����begin
var errorstr="";
<%
	aspOutPutApPositionDraw();
    aspOutPutUploadDrawing();
%>
// ����end

function drawingUpload()
{
	if(totalrecs == max_totalrecs)
	{
		alert("ͼֽ���Ѵ����ޣ��޷�����ͼֽ");
		return false;
	}
	
	if(re_checkSpecialName(DrawingUpload.drawingName, 'ͼֽ����'))
	{
		alert("ͼֽ�����к��зǷ��ַ�");
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
	if(!checkName(f.drawingName,11,"ͼֽ����",0)) return false;	//����û���������Ϊ�ա�����ȫΪ0�����ܳ�11���ַ���ӦΪ��:< > , % \' \\ \" & ; �Ϳո�֮����ַ���
	return true;
} 


function checkDrawing()
{
	var f=DrawingUpload.uploadDrawing;
	var format=f.value.substr(f.value.lastIndexOf(".")).toLowerCase();
	if (f.value==""){
		 alert("��ָ��ͼƬ");
		 return false;
	}
	else if(format!=".jpg"){
	    alert("���ϴ�.jpg��ʽ��ͼƬ!");
	    return false;
	}
	
	var name=f.value.split("\\")[f.value.split("\\").length-1];

	/*
	var allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890._";
	for (i = 0; i < name.length; i++) {
	    if (allowChar.indexOf(name.substring(i, i + 1)) == -1) {
		alert("�ļ���ֻ���������Сд��ĸ�����֡��»��ߺ�'.'");
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
		alert("�ļ����ظ���");
		return false;
	    }
	}

	return true;
}




</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
 <td> <script language="javascript">showSecondTitle("AP�ֲ�ͼ");//��ʾҳ������</script>
   <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,100);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr valign="top">
    <td>
	<!--//�����˵�-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						new Array("�ֲ�ͼ",'DrawingPreview.asp',1,0),
						new Array("����ͼֽ",'ImportDrawing.asp',1,1)
						


		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form method="post" name="DrawingUpload" enctype="multipart/form-data" action="">
<br>
<table border="0" bordercolor="red" width="600" align='center'>
  <tr>
    <td width="210" align="right" id="choosDrawing">��ѡ��ͼֽ</td>
    <td width="5" >&nbsp;</td>
    <td width="372" height="20"><input name="uploadDrawing" type="file"></td>
  </tr>
  <tr>
  	<td width="210" align="right">ͼֽ����<img src="images/qt.gif" width="7" height="9"></td>
    <td width="5">&nbsp;</td>
    <td width="372" height="20"><input name="drawingName" id="drawingName" type="text">(*.jpg)</td>
  </tr>
  <tr>
    <td align="right">&nbsp;</td>
    <td  height="25" align="center">&nbsp;</td>
    <td id="update then reset4">&nbsp;</td>
  </tr>
  <tr><td>&nbsp;</td><td>&nbsp;</td><td   height="25" ><input type="button" name="upload" class="input-button"  value="����ͼֽ" onClick="drawingUpload();" id="upload">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(help);%>" class="input-button" onClick="window.open('rhelpinfo.htm#Distribution')" id="helpBotton"></td></tr>
</table>
</form>
<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
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
