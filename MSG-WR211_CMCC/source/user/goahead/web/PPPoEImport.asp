<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("PPPoEImport");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
//Butterlate.setTextDomain("firewall");

//function initTranslation()
//{
//
//}
<%aspGetPppoeUser();%> 
 
function getBytesCount2(str) 
{ 
	if (str == null) 
	{ 
		return 0; 
	} 
	else 
	{ 
		return (str.length + str.replace(/[\u0000-\u00ff]/g, "").length); 
	} 
}
//���ܣ�����Ƿ�Ϸ�Ӣ���û�����������ʾ��Ϣ
function re_checkPppoeEnglishName(name, tips) {
    var retips = "";
    var allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.<>/?[]{}-_+=|`~!@$^&*()";
    for (i = 0; i < name.length; i++) {
        if (allowChar.indexOf(name.substring(i, i + 1)) == -1) retips = tips +"<%write(ppi_nameTip);%>";
    }
    return retips;;
}
function re_checkPppoePassword1(pas1, maxlen, allownull) {
    var retips = "";
    maxlen = toNumber(maxlen, 0, maxlen.length);
    if (pas1.length > maxlen)
	{
		retips = "<%write(ppi_pwdTip);%>" + maxlen + "<%write(ppi_pwdApp);%>";
    }
	else if (pas1 == "")
	{
		if (!eval(allownull))
		{
			retips = "<%write(ppi_pwdNull);%>";
		}
		else
		{
			retips = "";
		}
    }
    if (pas1.indexOf(",") != -1 || pas1.indexOf(";") != -1 || pas1.indexOf("��") != -1 || pas1.indexOf("%") != -1 || pas1.indexOf("\"") != -1 || pas1.indexOf("\'") != -1 || pas1.indexOf("\\") != -1 || ��pas1.indexOf(" ") != -1)
	{
        retips = "<%write(ppi_pwdExcep);%>";
    }
    return retips;
}
var userAgent = navigator.userAgent.toLowerCase();
var is_opera = userAgent.indexOf('opera') != -1 && opera.version();
var is_moz = (navigator.product == 'Gecko') && userAgent.substr(userAgent.indexOf('firefox') + 8, 3);
var is_ie = (userAgent.indexOf('msie') != -1 && !is_opera) && userAgent.substr(userAgent.indexOf('msie') + 5, 3);
function saveall()
{
	var retips;
	var tbl = User_Config;
	tbl.data.value = "";
	var arr1 = new Array();
	var arr2 = new Array();
	var k=0;
	var tempName;
	var tempPasswd;
    var tempBindMode;
	var tempBindmac;
	var tempBindmac2;
	var tempBindmac3;
	var tempBindmac4;
	var tempAccMaxConv;
	var tempBindIp;
	var tempChargeType;
	var tempTmStart;
	var tempTmStop;
	var tempURateLimit;
	var tempDRateLimit;
	var tempRemark;
	var circle = new Array();
	var flag = 0;
    var MACStr = /[A-Fa-f0-9]{2}[A-Fa-f0-9]{2}[A-Fa-f0-9]{2}[A-Fa-f0-9]{2}[A-Fa-f0-9]{2}[A-Fa-f0-9]{2}/;
    var PPPOE_REPLACE_MARK ="*"; 

   	arr1 = tbl.textarea.value.split("\n");
	for(var i = 0;i<arr1.length;i++)
	{
	    if((is_ie == false) || (i == arr1.length - 1))
	    {
			arr1[i] = arr1[i].slice(0,arr1[i].length);		/*��IE�ں��������ȥ��ÿ�к���Ļس�*/
	    }
	    else
	    {
			arr1[i] = arr1[i].slice(0,arr1[i].length-1);		/*IE�ں������ȥ��ÿ�к���Ļس�*/
	    }
	}

	for(var i = 0;i<arr1.length;i++)
	{
		arr1[i] = arr1[i].replace(/\t/g," ");//ȥ���Ʊ��
		if(arr1[i] == "" || arr1[i].replace(/(^\s*)|(\s*$)/g, "") == "")  //ȥ�����л���ֻ���ո����
		{
			continue;
		}
		var temp;
		k++;
		flag =0;
		var n;
		arr2 = arr1[i].split(" ");	

		for(n = 0;n<arr2.length;n++)
		{			
			if(arr2[n].indexOf(" ") != -1 || arr2[n] =="" || arr2[n] == " ") 
			{
				continue;
			}	
			
			if(flag == 0)
			{
				tempName = arr2[n];
				flag = 1;
				continue;
			}
			if(flag == 1)
			{
				tempPasswd = arr2[n];
				flag = 2;
				continue;
			}
            if(flag == 2)
            {
               tempBindMode = arr2[n];
               flag = 3;
               continue;
            }
			if(flag == 3)
			{
				tempBindmac = arr2[n];
				flag = 4;
				continue;
			}
			if(flag == 4)
			{
				tempBindmac2 = arr2[n];
				flag = 5;
				continue;
			}
			if(flag == 5)
			{
				tempBindmac3 = arr2[n];
				flag = 6;
				continue;
			}
			if(flag == 6)
			{
				tempBindmac4 = arr2[n];
				flag = 7;
				continue;
			}
			if(flag == 7)				
			{
				tempAccMaxConv = arr2[n];
				flag = 8;
				continue;
			}
			if(flag == 8)
			{
				tempBindIp = arr2[n];
				flag = 9;
				continue;
			}
			if(flag == 9)
			{
				tempChargeType = arr2[n];
				flag = 10;
				continue;
			}
			if(flag == 10)
			{
				tempTmStart = arr2[n];
				flag = 11;
				continue;
			}
			if(flag == 11)
			{
				tempTmStop = arr2[n];
				flag = 12;
				continue;
			}
			if(flag == 12)
			{
				tempURateLimit = arr2[n];
				flag = 13;
				continue;
			}
			if(flag == 13)
			{
				tempDRateLimit = arr2[n];
				flag = 14;
				continue;
			}
			if(flag > 14)
			{
				alert("��" + k + "����������������������롣");
				return ;
			}
			if(flag ==14)
			{
				tempRemark = arr2[n];
				flag = 15;
				continue;
			}		
		}
		retips = re_checkPppoeEnglishName(tempName,"�û���");
		if(retips != "")
		{
			alert("��"+k+"��"+retips);
			return;
		}
		if(tempPasswd == undefined)
		{
		    alert("��"+k+"������Ϊ�գ����������롣");
		    return;
		}
        retips = re_checkPppoeEnglishName(tempPasswd,"<%write(com_passwd);%>");
		if(retips != "")
		{
			alert("��"+k+"��"+retips);
			return;
		}
		if(tempPasswd == null)
		{
			alert("��"+k+"����������Ϊ�գ� ����������");
			return ;
		}
		if(tempPasswd == "")
		{
			alert("��"+k+"������Ϊ�գ�����������");
			return ;
		}
		circle[k] = tempName;
		for(var j = 1;j<circle.length;j++)
		{
			if(j == k)
				continue;
			if(circle[j] == tempName)
			{
				alert("��"+j+"���û������"+k+"���û����ظ�����");
				return ;
			}
		}
		if(tempName.length > 31)
		{
			alert("��"+k+"���û������ȴ���31������������");
			return ;
		}
		if(tempPasswd.length > 31)
		{
			alert("��"+k+"�����볤�ȴ���31������������");
			return ;
		}
		for(var m = 0;m<totalrecs;m++)
		{
			if(tempName == PPPoENames[m] )
			{
				alert("�û���"+tempName+"�Ѿ��������˺��б����������룡");
				return ;
			}
		}
       
        if(getBytesCount2(tempRemark) > 31)
		{
			alert("��"+k+"���������ȴ���31���ֽڣ����������룡");
			return ;
		}
        
        if(tempBindMode != "handbind" && tempBindMode != "autobind" &&tempBindMode != "nobind")
        {
            alert("mac�󶨷�ʽ����!");
            return;
        }
        if(tempBindMode == "handbind" && tempAccMaxConv > 4)
        {
            alert("�ֹ���MAC��ַʱ���˺����Ự�����Ϊ4");
            return ;
        }
        if(tempBindMode == "autobind" && tempAccMaxConv > 1)
        {
            alert("�Զ���MAC��ַʱ���˺����Ự������Ϊ1");
            return;
        }
        if (tempBindmac != PPPOE_REPLACE_MARK && !MACStr.test(tempBindmac)) 
        {
            alert("�󶨵�Mac��ַ��ʽ����");
            return;
        }
        if (tempBindmac2 != PPPOE_REPLACE_MARK && !MACStr.test(tempBindmac2)) 
        {
            alert("�󶨵�Mac��ַ2��ʽ����");
            return;
        }
        if (tempBindmac3 != PPPOE_REPLACE_MARK && !MACStr.test(tempBindmac3)) 
        {
            alert("�󶨵�Mac��ַ3��ʽ����");
            return;
        }
        if (tempBindmac4 != PPPOE_REPLACE_MARK && !MACStr.test(tempBindmac4)) 
        {
            alert("�󶨵�Mac1��ַ4��ʽ����");
            return;
        }
        if(parseInt(tempAccMaxConv) > parseInt(smaxconvs) || parseInt(tempAccMaxConv) < 1){
            alert("���Ự������");
            return;
        } 
        circle[k] = tempBindIp;
		for(var j = 1;j<circle.length;j++)
		{
			if(j == k)
				continue;
            if(tempBindIp == "*")
                continue;
			if(circle[j] == tempBindIp)
			{
				alert("��"+j+"�й̶�IP���"+k+"�й̶�IP�ظ�����");
				return ;
			}
		}
        for(var m = 0;m<totalrecs;m++)
		{
            if(tempBindIp == aspips[m]	)
			{
				alert("�̶�IP "+tempBindIp+" �Ѿ��������б����������룡");
				return ;
			}
		}
        if(tempBindIp == "0.0.0.0")
        {
           alert("�̶�ip����Ϊ0.0.0.0");
           return;
        }
        if(tempBindIp != PPPOE_REPLACE_MARK && tempAccMaxConv > 1) {
            alert("����̶�IPʱ���˺����Ự������Ϊ1");
            return false
        }
        
        if(tempChargeType != PPPOE_REPLACE_MARK && tempChargeType != "1") {
            alert("�Ʒ�ģʽ����");
            return;
        }
        if(tempChargeType == "1") 
        {
            if(tempTmStop < tempTmStart) {
                alert("��ʼ���ڲ��ܴ��ڽ�������");
                return;
            }
            var tmStart = tempTmStart.split("-");
            var tmStop = tempTmStop.split("-");
            if(tmStart < 3 || tmStop < 3 ) {
                alert("���ڸ�ʽ�������");
                return;
            }
            //�Ƚ��꣬�£���
            if (parseInt(tmStart[0]) < 1970 || parseInt(tmStart[0]) > 2038 || parseInt(tmStop[0]) < 1970 || parseInt(tmStop[0]) > 2038 
                || parseInt(tmStart[1], 10) < 1 || parseInt(tmStart[1], 10) > 12 || parseInt(tmStart[1], 10) < 1 || parseInt(tmStart[1], 10) > 12
                || parseInt(tmStart[2], 10) < 1 || parseInt(tmStart[2], 10) > 31 || parseInt(tmStop[2], 10) < 1 || parseInt(tmStop[2], 10) > 31)
            {
                alert("�����������ںϷ���");
                return;
            }
        }
        if(tempURateLimit < 0 || tempURateLimit > 102400 || tempDRateLimit < 0 || tempDRateLimit > 102400) {
            alert("�������ƴ���");
        }

        if(tempRemark == null) {
			tempRemark = "";
		}
        if(!checkRemark(tempRemark, 31, "��ע", 1)) {
            return;
        }

		if(parseInt(totalrecs+ k) > max_totalrecs) {
			alert("û�п���ʵ���������ӵ�"+k+"���û�");
			return ;
		}
	
	tbl.data.value += tempName;
	tbl.data.value += " ";
	tbl.data.value += tempPasswd;
	tbl.data.value += " ";
    tbl.data.value += tempBindMode;
	tbl.data.value += " ";
	tbl.data.value += tempBindmac;
	tbl.data.value += " ";
	tbl.data.value += tempBindmac2;
	tbl.data.value += " ";
	tbl.data.value += tempBindmac3;
	tbl.data.value += " ";
	tbl.data.value += tempBindmac4;
	tbl.data.value += " ";
	tbl.data.value += tempAccMaxConv;
	tbl.data.value += " ";
	tbl.data.value += tempBindIp;
	tbl.data.value += " ";
	tbl.data.value += tempChargeType;
	tbl.data.value += " ";
	tbl.data.value += tempTmStart;
	tbl.data.value += " ";
	tbl.data.value += tempTmStop;
	tbl.data.value += " ";
	tbl.data.value += tempURateLimit;
	tbl.data.value += " ";
	tbl.data.value += tempDRateLimit;
	tbl.data.value += " ";
	tbl.data.value += tempRemark;
	tbl.data.value += ";";
	
	tempName = "";
	tempPasswd = "";
    tempBindMode = "";
	tempBindmac = "";
	tempBindmac2 = "";
	tempBindmac3 = "";
	tempBindmac4 = "";
	tempAccMaxConv = "";
	tempBindIp = "";
	tempChargeType = "";
	tempTmStart = "";
	tempTmStop = "";
	tempURateLimit = "";
	tempDRateLimit = "";
	tempRemark = "";
	}
	if(!tbl.data.value)
	{
		alert("<%write(ppi_null);%>");
		return ;
	}
	tbl.action="/goform/formPppoeImport";
	tbl.submit();
}
function init()
{
	 
	var tbl = User_Config;
	User_Config.reset();
}

</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
  <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle(_("<%write(com_pppSer);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"class="table_bg">
    <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form name="User_Config" method="post" action="">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						5,
						new Array(_("<%write(com_pppGlo);%>"),'PPPoEGlobal.asp',1,0),
						new Array(_("<%write(com_pppAcc);%>"),'PPPoEServer.asp',1,0),
						new Array(_("<%write(com_pppSta);%>"),'PPPoEConSta.asp',1,0),
						new Array(_("<%write(com_pppExp);%>"),'PPPoEExport.asp',1,0),
						new Array(_("<%write(com_pppImp);%>"),'PPPoEImport.asp',1,1)
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" height="400">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="700"  border="0" align="center" cellpadding="0" cellspacing="0">
			  <tr>
     <table border="0" width="600" align='center' >
  <tr><td colspan="3" height="30">&nbsp;</td></tr>
		<tr>
		<td>&nbsp;</td>
		<td align="center"><textarea id="textarea" name="textarea" style="font-size:15px; font-family:'������'; width:700px; height:200px"></textarea></td>
		<td>&nbsp;</td>
		</tr>		
		<tr>
		<td width="10%" height="40">&nbsp;</td>
		<td width="77%">
		<table width="100%"  style="width:500px;" align="center">
		<tr>
		

		<td  ><input type="hidden" name="PageId"/>
			<input type="hidden" name="data" />
			<input type="hidden" name="UserID" /></td>
		</tr>
		<tr>
    <td	 height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="saveall()" id="uttsave"></td>
  </tr>
		</table>		</td>
		<td width="13%">&nbsp;</td>
		</tr>

	</table>
	 
		<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" height="35">
		 <tr><td width="13%"></td>
         <td width="87%" colspan="4" height="20" class="helpfont" > <%write(ppi_HELP1);%></td></tr>
         </tr><tr><td width="13%"></td>
         <td width="87%" colspan="4" height="20" class="helpfont" > <%write(ppi_HELP2);%></td></tr>
         </tr><tr><td width="13%"></td>
         <td width="87%" colspan="4" height="20" class="helpfont" > <%write(ppi_HELP3);%></td></tr>
         </tr><tr><td width="13%"></td>
         <td width="87%" colspan="4" height="20" class="helpfont" > <%write(ppi_HELP4);%></td></tr>
        </tr><tr><td width="13%"></td>
         <td width="87%" colspan="4" height="20" class="helpfont" > <%write(ppi_HELP5);%></td></tr>

         </table>
 
			  </tr>
		  </table></td></tr>
<script language="javascript">init();</script></table>
  </form></td>
  </tr>
</table></td> 
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
