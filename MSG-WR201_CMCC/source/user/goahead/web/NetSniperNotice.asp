<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("NetSniper"); %>
<% langTran("common"); %>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
</head>
<script type="text/javascript">
/*
var errorstr="";
var NoticeTitile=<%write("Notice");%>;
var NoticeContent="�𾴵��û���\n�����粻����˽�Թ����������������������쳣��������������ϵ�������Ա��";
*/
<%aspNetSniperNotice();%>
function init(){
	/*
		if(!NoticeTitle)
			NoticeTitle="<%write(Notice);%>";
		if(!NoticeContent)
			NoticeContent="<%write(content);%>";
	*/
		NetSniperNotice.noticeTitle.value=NoticeTitle;
}

function preview(){
	 var tbl = NetSniperNotice;
    var htmlLen = 898;
 
    tbl.hidBdy.value = ""; 
    var str = tbl.bdy.value;
    tbl.hidBdy.value = strToHtml(str);


    if (!checkName(tbl.noticeTitle, 20, "<%write(NoticeTitle);%>", 1))
	return false;
    if (tbl.hidBdy.value.lstrLen() > htmlLen)
    {
	//alert("Notification content has to be at  most "+htmlLen+" characters long");
	return false;
    } 
    openForm(NetSniperNotice,"Notify.asp",750,470);
	
	
}
/*�ַ���ת����html����*/
function strToHtml(str)
{
    str = str.replace(/ /g ,"&nbsp;");
    str = str.replace(/\r/g,'');
    str = str.replace(/\n/g ,"<br>");
    str = str.replace(/%/g ,"%%");/*Ԥ��ʱ%����ʧ����2��%����*/
    return str;
}

function strToHtml1(str)
{
    str = str.replace(/ /g ,"&nbsp;");
    str = str.replace(/\r/g,'');
    str = str.replace(/\n/g ,"<br>");
    return str;
}

/*��麯��*/
function checkData()
{
	var tbl = NetSniperNotice;
	var htmlLen = 898;
	if (!checkName(tbl.noticeTitle, 20, "<% write(NoticeTitle); %>", 1))
		return false;
	if (tbl.hidBdy.value.lstrLen() > htmlLen)
	{
		alert("<% write(NoticeLenErr); %>");
		return false;
	} 
	return true;
}

function save(){
	var tbl = NetSniperNotice;
	tbl.hidBdy.value = "";
	var str = tbl.bdy.value;
	tbl.hidBdy.value = strToHtml1(str);
	if(!checkData()){
		return;
	}
	tbl.action = "/goform/formSniperNotice";
	tbl.submit();
}

</script>
<body background="images/body_main.png" onLoad="init();">  
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****���ϼ��****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****�м�����****-->
<tr>
<td width="15"></td>
<!--////����-->
<td>
	<script language="javascript">showSecondTitle("<%write(NetSniper);%>");</script>
	<table width="100%" border="0" height="600" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
		<script language="javascript">showerror(errorstr,80);</script>
		<tr valign="top">
			<td><form id="NetSniperNotice" name="NetSniperNotice" action="" method="post">
			<p>&nbsp;</p>
			<script language="javascript"> 
								var arrayThreeMenu=new Array(
                                                "80%",
                                                3,
                                                new Array("<%write(NetSniperList);%>",'NetSniperList.asp',1,0),
                                                new Array("<%write(NetSniperConf);%>",'NetSniperConf.asp',1,0),
                                                new Array("<%write(NetSniperNotice);%>",'NetSniperNotice.asp',1,1)
                                );
                                showThreeMenu();
			</script>
				<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" id="all">
					<tr>
						<td align="center" class="three_td">
                        	<table border="0" cellpadding="0" cellspacing="0">							
                            <tr><td colspan="3" height="40">&nbsp;</td></tr>
                            	<tr>
                                    <td width="40%" height="30" align="right"><%write(NoticeTitle);%></td>
                                    <td width="2%" >&nbsp;</td>
                                    <td align="left"><input type="text" name="noticeTitle" value="" id="noticeTitle" class="input2" maxlength="20"><input type="hidden" name="hidBdy" id="hidBdy" value=""></td>
                                </tr>
								<!--<tr>
                                    <td height="30" align="right">����Աǩ��</td>
                                    <td>&nbsp;</td>
                                    <td align="left"><input type="text" name="manager" id="manager" class="input2" maxlength="20"><input type="hidden" name="hidmanager" id="hidmanager" value=""></td>
                                </tr> -->
                                
                              
                                     
                                 <tr>
                                    <td height="10" colspan="3">&nbsp;</td>
                                 </tr>
                                 <tr>
                                 	<td colspan="3" align="center">
                                    	<table border="0" cellpadding="0" cellspacing="0">
 
<tr>
<td height="30" colspan="2" align="center" valign="bottom"><%write(NoticeContent);%></td>
</tr>
 
<tr>
<td height="10">&nbsp;</td>
</tr>
 
<tr>
<td>&nbsp;</td>
<td align="center"><textarea name="bdy" id="bdy" style="font-size:13px; font-family:'������'; width:492px; height:250px"><%write(aspNetSniperNoticeContent());%></textarea></td>
<td>&nbsp;</td>
</tr>
 
</table>

                                    </td>
                                 </tr>
                                 
                                 <tr>
                                 	<td colspan="3" align="center">
                                    	<table  border="0" align="center" cellpadding="0" cellspacing="0">
                                        <tr>
                                             <td height="40" width="9%" id="saveTd">&nbsp;</td>
                                                                                            <td width="45%" id="saveTd">&nbsp;</td>
                                                                                            <td width="14%" id="previewTd"><input type="button" id="previewPage"  value="<%write(PreviewPage);%>" onClick="preview();" /></td>
                                                                                            <td width="11%"><input type="button" id="baocun" name="baocun" value="<%write(com_save);%>" onClick="save()" /></td>
                                                                                            <td width="10%" >&nbsp;</td>

                                        </tr>
                                        <tr><td colspan="6">&nbsp;</td></tr>
										</table>
                                    </td>
                                 </tr>  
                                 
							</table>
						</td>
                    </tr>
                 </table>
            </form>
           </td>
         </tr>
      </table>
</td>
<td width="15"></td>
</tr>
</table>
</body>
</html>
