<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>��̩�Ƽ�</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
<%aspOutputFlowNoticeData();%>
var NoticeType = new Array(); NoticeType[2] = "edithtml";
var errorstr="";

/*�ַ���ת����html����*/
function strToHtml(str)
{
    str = str.replace(/ /g ,"&nbsp;");
    str = str.replace(/\r/g,'');
    str = str.replace(/\n/g ,"<br>");
    str = str.replace(/%/g ,"%%");
    return str;
}

/*��麯��*/
function checkData()
{
	var tbl = PPPoEFlowNotice;
	if (tbl.noticeType.selectedIndex == 0)
	{
	    var htmlLen = 1200;
	    //if (!checkIP(tbl.FromAddr, "��ʼ����", 0)) 
	    //	return false;
	    //if (!checkIP(tbl.EndAddr, "��������", 0)) 
	    //	return false; 
	    //if (compareIP(tbl.FromAddr.value, tbl.EndAddr.value) == 1)
	    //{
	       	//alert("���η�Χ��д����");
		//return false;
	    //}
	    if(!checkNumber(tbl.txtrxflowidle, 0, 4294967295, "ͨ������ʣ������", 1))
	    {
		return false;
	    }
	    if(!checkNumber(tbl.txttxflowidle, 0, 4294967295, "ͨ������ʣ������", 1))
	    {
		return false;
	    }
	    if (!checkName(tbl.noticeTitle, 20, "ͨ�����", 1))
		return false;
	    if (tbl.bdy.value.lstrLen() > htmlLen)
	    {
		alert("ͨ�����ݳ��Ȳ��ܳ���"+htmlLen+"λ��");
		return false;
	    } 
	}
	else if (tbl.noticeType.selectedIndex == 1)
	{
		if (!checkName(tbl.importhtml, 63, "�����ҳ��", 0))
			return false;
	}
	return true;
}

/*���溯��*/
function save(){
    var tbl = PPPoEFlowNotice;

    if (!checkData())
    {
	return;
    }
    //getTimeValue(tbl);
    //if(tbl.day.value=="0000000"&&!tbl.everyday.checked){
	//alert("��ѡ�����ڣ�");
	//tbl.day.value="";
	//return;
    //}
    tbl.action = "/goform/formFlowNoticeConfig";
    tbl.submit();
}


/*����֪ͨ��ʽ���������ṩҳ�淽ʽ*/
function showInstInformation(instance)
{
    var tbl = PPPoEFlowNotice;
    tbl.enabled.checked = (lively[instance].toLowerCase() == "1") ? true : false;
    tbl.noticeTitle.value = htmlTitle[instIndex];
    if (NoticeType[2].toLowerCase() == "edithtml")
    {
	tbl.noticeType.selectedIndex = 0;
    }
    else if (NoticeType[2].toLowerCase() == "importhtml")
    {
	tbl.noticeType.selectedIndex = 1;
    }
    noticeTypeChg();

}

/*����֪ͨ��ʽ��ť*/
function timeTypeChanged()
{
	var tbl = PPPoEFlowNotice;
	
	if (tbl.timeType.selectedIndex == 0) 
	{
		enableNoticeTd.innerHTML = "�������ڵ�������";
		
		showTable(timeTR, 1);
		showTable(flowrxTR, 0);
		showTable(flowtxTR, 0);
		showInstInformation(1);
	} 
	else if (tbl.timeType.selectedIndex == 1)
	{
		enableNoticeTd.innerHTML = "����������������";
		
		showTable(timeTR, 0);
		showTable(flowrxTR, 1);
		showTable(flowtxTR, 1);
		showInstInformation(2);
	}
}

/*��ʼ����ť*/
function init()
{
	var tbl = PPPoEFlowEotice;
	tb1.enabled.checked=(lively.toLowerCase() == "1")?true : false;

	if(NoticeType[2].toLowerCase() == "edithtml")
	{
	    tb1.noticeType.selectedIndex = 0;
	}
	else if(NoticeType[2].toLowerCase() == "importhtml")
	{
	    tb1.noticeType.selectedIndex = 1;
	}
	noticeTypeChg();
	tbl.noticeTitle.value = htmlTitle;
	tbl.txtrxflowidle.value = rxflowidle;
	tbl.txttxflowidle.value = txflowidle;
}


/*Ԥ��ҳ��*/
function preview()
{
    var tbl = PPPoEFlowNotice;

    tbl.hidBdy.value = ""; 
    var str = tbl.bdy.value;
    tbl.hidBdy.value = strToHtml(str);
    openForm(PPPoEFlowNotice,"Notify.asp",750,470);
}


/*�ṩҳ�淽ʽ���ķ�ʽ*/
function noticeTypeChg() {
	var tbl = PPPoEFlowNotice;
	if (tbl.noticeType.selectedIndex == 0) {
		showTable(noticeHtml, 1);
		showTable(noticeHtml2, 1);
		previewTd.innerHTML = '<input type="button" name="previewB" value=" Ԥ��ҳ�� " onclick="preview()" />';
	} else {
		showTable(noticeHtml, 0);
		showTable(noticeHtml2, 0);
		showTable(noticeUrl, 1);
		previewTd.innerHTML = '&nbsp;';
	}
}

</script>
</head>

<body onLoad="" onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****���ϼ��****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****�м�����****-->
<tr>
<td width="15"></td>
<!--////����-->
<td><script language="javascript">showSecondTitle("ͨ��");</script>
<table width="100%" border="0" height="600" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr,80);</script>
<tr valign="top">
<td><form id="PPPoEFlowNotice" name="PPPoEFlowNotice" action="/goform/" method="post">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
					"80%",
						3,
						new Array("ͨ��",'Notice.asp',1,0),
						new Array("�˺ŵ���ͨ��",'PPPoENotice.asp',1,0),
						new Array("�˺���������ͨ��",'PPPoEFlowNotice.asp',1,1)
		);
		showThreeMenu();
	</script>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" id="all">
<tr>
<td align="center" class="three_td">
	<table border="0" cellpadding="0" cellspacing="0" width="600">
	
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
	
	<tr style="display:none">
	<td width="34%" height="30" align="right">ͨ�淽ʽ</td>
	<td width="4%">&nbsp;</td>
	<td width="52%"><select id="timeType" name="timeType" class="input" style="width:100 " onChange="timeTypeChanged()">
	<option value="time">���ڵ���ͨ��</option>	
	<option value="flow">��������ͨ��</option>
	</select>
	</td>
	</tr>	
	
	<tr>
	<td width="34%" height="30" align="right" id="enableNoticeTd">����</td>
	<td width="4%">&nbsp;</td>
	<td width="52%"><input type="checkbox" name="enabled" id="enabled"></td>
	</tr>
	
	<tr id = "flowrxTR" style="display: ">
	<td width="290" height="30" align="right">ͨ������ʣ������</td>
	<td width="23">&nbsp;</td>
	<td width="405"><input type="text" name="txtrxflowidle" id="txtflowrxidle" class="input">&nbsp;MBytes</td>
	</tr>
	
	<tr id = "flowtxTR" style="display: ">
	<td width="290" height="30" align="right">ͨ������ʣ������</td>
	<td width="23">&nbsp;</td>
	<td width="405"><input type="text" name="txttxflowidle" id="txtflowtxidle" class="input">&nbsp;MBytes</td>
	</tr>
	
	<tr style="display:none">
	<td align="right" height="30">�ṩҳ�淽ʽ</td>
	<td>&nbsp;</td>
	<td><select name="noticeType" id="noticeType" onChange="noticeTypeChg()" class="input" style="width:100 ">
	<option value="edithtml" >�༭ҳ��</option>
	<option value="importhtml" >����ҳ��</option>
	</select></td>
	</tr>
	
	</table>
	
	

	<table border="0" cellpadding="0" cellspacing="0" width="600" id="noticeHtml" style="display">

	<tr>
	<td width="34%" height="30" align="right">ͨ�����</td>
	<td width="4%" >&nbsp;</td>
	<td width="52%" ><input type="text" name="noticeTitle" id="noticeTitle" class="input2" maxlength="20"><input type="hidden" name="hidBdy" id="hidBdy" value=""></td>
	</tr>


	</table>


	<table border="0" cellpadding="0" cellspacing="0" width="600" id="noticeHtml2" style="display">
	
	<tr>
	<td height="30" colspan="2" align="center" valign="bottom">ͨ������</td>
	</tr>

	<tr>
	<td height="10">&nbsp;</td>
	</tr>

	<tr>
	<td>&nbsp;</td>
	<td align="center"><textarea name="bdy" id="bdy" style="font-size:13px; font-family:'������'; width:492px; height:250px"><% write(aspOutputNoticeContentData()); %></textarea></td>
	<td>&nbsp;</td>
	</tr>

	</table>

	<table width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr>
	<td height="40" width="15%" id="saveTd">&nbsp;</td>
	<td width="58%" id="saveTd">&nbsp;</td>
	<td width="6%" id="previewTd"><input type="button" id="preview" name="preview" value=" Ԥ��ҳ�� " onClick="preview()" /></td>
	<td width="4%"><input type="button" id="baocun" name="baocun" value=" �� �� " onClick="save()" /></td>
	<td width="17%" >&nbsp;</td>
	</tr>
	</table>
	

	

</td></tr></table>
</form></td>
</tr><script language="javascript">init();</script>
</table>
</td>
<td width="15"></td>
</tr>
</table>
</body>
</html>

