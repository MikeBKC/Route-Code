<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("PPTP"); %>
<% langTran("common"); %>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
//Butterlate.setTextDomain("internet");
var strSecondMenuFocus="L2TP";
<% OutputL2tpSrvListData(); %>
var errorstr1=errorstr;
<% OutputL2tpCliListData(); %>


var useOtimes = new Array();
var peerTypesw = new Array();
var setNamesw = new Array();
var userNamessw =new Array();
var enablesw = new Array();
var tunnelTypesw = new Array();
var remoteLanIpsw = new Array();
var remoteLanNetmasksw = new Array();
var statusssw = new Array();
var useOtimessw = new Array();
var outboundsssw = new Array();
var inboundsssw = new Array();
for(i=0;i<(totalrecs+cli_totalrecs);i++){
    if(i<totalrecs) { /*������˺�*/
	setNamesw[i]=instNames[i];
	userNamessw[i]=userNames[i];
	enablesw[i]="yes";
	tunnelTypesw[i]="�����";
	remoteLanIpsw[i]=remoteLanIps[i];
	remoteLanNetmasksw[i]=remoteNetmasks[i];
	
	statusssw[i]=(statuss[i]=="1"?"������":"�Ͽ�");
	if (peerTypes[i] == "lantolan") {
		peerTypesw[i] = "LAN��LAN";
	} else {
		peerTypesw[i] = "�ƶ��û�";
	}
	if(useTimes[i] != "-")
	{
		var runday=parseInt((eval(useTimes[i])/3600)/24);
		var runhour=parseInt((eval(useTimes[i])/3600)%24);
		var runmiu=parseInt((eval(useTimes[i])%3600)/60);
		var runsec=parseInt((eval(useTimes[i])%3600)%60);
		useOtimes[i] = runday+"��"+runhour+"Сʱ"+runmiu+"��"+runsec+"��";
		outboundss[i] =  parseInt(eval(outboundss[i])/8);
		inboundss[i] =  parseInt(eval(inboundss[i])/8);
		useOtimessw[i] = runday+"��"+runhour+"Сʱ"+runmiu+"��"+runsec+"��";
		outboundsssw[i] =  parseInt(eval(outboundss[i])/8);
		inboundsssw[i] =  parseInt(eval(inboundss[i])/8);
 	}
	else
	{
		useOtimes[i] = "-";
		outboundss[i] = "-";
		inboundss[i] = "-";
		useOtimessw[i] = "-";
		outboundsssw[i] = "-";
		inboundsssw[i] = "-";
	}
    } else { /*�ͻ����˺�*/
	setNamesw[i]=cli_setNames[i-totalrecs];
	userNamessw[i]=cli_userNames[i-totalrecs];
	enablesw[i]=((L2TPClientEnables[i-totalrecs]=="on")?"yes":"no");
	tunnelTypesw[i]="�ͻ���";
	remoteLanIpsw[i]=cli_serverLanIps[i-totalrecs];
	remoteLanNetmasksw[i]=cli_serverLanNetMasks[i-totalrecs];

	peerTypesw[i] = "-";
	statusssw[i]=(cli_statuss[i-totalrecs]=="1"?"������":"�Ͽ�");
	if(cli_usetimes[i-totalrecs] != "-")
	{
		var runday=parseInt((eval(cli_usetimes[i-totalrecs])/3600)/24);
		var runhour=parseInt((eval(cli_usetimes[i-totalrecs])/3600)%24);
		var runmiu=parseInt((eval(cli_usetimes[i-totalrecs])%3600)/60);
		var runsec=parseInt((eval(cli_usetimes[i-totalrecs])%3600)%60);
		useOtimessw[i] = runday+"��"+runhour+"Сʱ"+runmiu+"��"+runsec+"��";
		outboundsssw[i] =  parseInt(eval(cli_outboundss[i-totalrecs])/8);
		inboundsssw[i] =  parseInt(eval(cli_inboundss[i-totalrecs])/8);
	}
	else
	{
		useOtimessw[i] = "-";
		outboundsssw[i] = "-";
		inboundsssw[i] = "-";
	}

    }
}


recsPerPage[0] = 5;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs+cli_totalrecs;
maxrecs[0]=max_totalrecs+cli_max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("L2TP��Ϣ�б�");
var titles1=new Array(10);
var index = 0;
titles1[index++]="�������";
titles1[index++]="�û���";
titles1[index++]="ҵ��";
titles1[index++]="�û�����";
titles1[index++]="Զ������IP��ַ";
titles1[index++]="Զ��������������";
titles1[index++]="�Ự״̬";
titles1[index++]="ʹ��ʱ��";
titles1[index++]="������(Byte)";
titles1[index++]="������(Byte)";
//proTypes = new Array();
//arrayData1=new Array(instNames,userNames,peerTypesw,remoteLanIps,remoteNetmasks,statuss,useOtimes,outboundss,inboundss);
arrayData1=new Array(setNamesw,userNamessw,tunnelTypesw,peerTypesw,remoteLanIpsw,remoteLanNetmasksw,statusssw,useOtimessw,outboundsssw,inboundsssw);
sortable[0]=new Array(1,1,1,1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE,TYPE_STR, TYPE_STR,TYPE_CHINESE,TYPE_NUM,TYPE_NUM);
arrayCount[0]=10;


 
function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,10,arrayData1,1,1);
}
 

function init(){
}

function showOpButton(){	
//	showbutton.innerHTML='<input type="button" value="��ӿͻ���" class="input-button" onClick="EditConfig(0, 0, 0)">&nbsp;&nbsp;<input type="button" value="��ӷ�����" class="input-button" onClick="EditConfig(0, 0, 1)">';
	/* ����l2tp�ͻ��� */
	showbutton.innerHTML='<input type="button" value="��ӿͻ���" class="input-button" onClick="EditConfig(0, 0, 0)">&nbsp;&nbsp;<input type="button" value="��ӷ�����" class="input-button" onClick="EditConfig(0, 0, 1)">';
	showbutton.innerHTML+='&nbsp;&nbsp;<input type="submit" value="����" onClick="return(genVpnDialData(0,frame12))" class="input-button" id = "dail">&nbsp;&nbsp;<input type="submit" value="�Ҷ�" onClick="return(genVpnHangData(0,frame12))" class="input-button" id="hang">';
}

/*
 * val: 0��ӣ�1�༭
 * i:	��һ��
 * type: 0�ͻ��ˣ�1�����
 */
function EditConfig(val, i, type){
	
	/* ��� */
	if(val == 0){ 
		/*�ͻ���*/
		if (type == 0) {
			L2TPList.action="L2TPClient.asp";
		} else if(type == 1) {
			if (srv_enables == 1) {
				L2TPList.action="L2TPSrvAccount.asp";
			} else {
				L2TPList.action="L2TPSrvGlobal.asp";
			}
		} else {
			;
		}
	} 
	/*�༭*/
	else if(val == 1) {
		L2TPList.id.value = setNamesw[i];
		
		/*
		 * �б��У�������˺������棬�ͻ����˺�������
		 */
		if (i < totalrecs) {
			L2TPList.action="L2TPSrvAccount.asp";
		} else {
			L2TPList.action="L2TPClient.asp";
		}
	} else {
	;
	}
		

	L2TPList.submit();
}


function DeletePPTP(){
			return(genData(L2TPList,0,frame12,setNamesw,'L2TP��Ϣ�б�'))
}

function DeleteConfig(index){
			if(confirm("<% write(com_warning3); %>")){
			    if (index < totalrecs) {                                                                                  
				L2TPList.delstr.value=setNamesw[index];
			    } else {                                                                                                  
				L2TPList.delstrCli.value=setNamesw[index];                                                            
			    }       
				L2TPList.action="/goform/l2tpSrvListDel";
				L2TPList.submit();
			}
}

function delAll(){
	if(confirm("<% write(com_warning4); %>")){
		L2TPList.action="/goform/l2tpSrvListDelAll";
		L2TPList.submit();
	}
}



function genVpnDialData(index,list)
{
  var recbegin;
  var recend;
  var t=0;

  L2TPList.dialstr.value="";
  var chks = list.document.getElementsByName("chklist");
  recbegin=(currpage[index]-1)*recsPerPage[index];
  recend=recbegin+recsPerPage[index];
  if(recend>=recscount[index]) recend=recscount[index];
  for(var i=recbegin;i<recend;i++) {
		if(chks.length) {
			if(chks[t].checked==1) {
				/*
				 * ������˺Ų��ܲ���
				 */
				if (tunnelTypesw[i] == "�����") {
					alert("��ѡ��ͻ����˺�");
					return false;
				}
				if((statusssw[i]!="�Ͽ�")) {
					alert(setNamesw[i]+"�����ӻ��������ӣ����ȹҶ����ؽ������ӣ�");
					return false;
				}
		
				else if((enablesw[i]!="yes")) {
					alert(setNamesw[i]+"����δ���ã������ú����ؽ������ӣ�");
					return false;
				}else{
					L2TPList.dialstr.value = L2TPList.dialstr.value + setNamesw[i]+",";
				}
				
			}
		}
		else {
			if(chks.checked==1)  {
				/*
				 * ������˺Ų��ܲ���
				 */
				if (tunnelTypesw[i] == "�����") {
					alert("��ѡ��ͻ����˺�");
					return false;
				}			
			
				if((statusssw[i]!="�Ͽ�")) {
					alert(setNamesw[i]+"�����ӻ��������ӣ����ȹҶ����ؽ������ӣ�");
					return false;
				}
		
				else if((enablesw[i]!="yes")) {
					alert(setNamesw[i]+"����δ���ã������ú����ؽ������ӣ�");
					return false;
				}else{
					L2TPList.dialstr.value = L2TPList.dialstr.value + setNamesw[i]+",";
				}
				
			}
		}
	t=t+1;
  }
  
  if(L2TPList.dialstr.value!="")
  {
  				//L2TPList.dialstr.value=setNames[index];
				L2TPList.action="/goform/formL2tpDial";
				//L2TPList.submit();
				return true;
  }
  else {
	alert("����ѡ��L2TP��Ϣ��¼,�ٽ�������!");
	return false;
  }
}

function genVpnHangData(index,list)
{
  var recbegin;
  var recend;
  var t=0;
  L2TPList.hangstr.value="";
  var chks = list.document.getElementsByName("chklist");
  recbegin=(currpage[index]-1)*recsPerPage[index];
  recend=recbegin+recsPerPage[index];
  if(recend>=recscount[index]) recend=recscount[index];
  for(var i=recbegin;i<recend;i++) {
		if(chks.length) {
			if(chks[t].checked==1) {
			    if (tunnelTypesw[i] == "�����") {
				if (chks[t].value != "on") {
					if((statusssw[i] =="�Ͽ�")) {
						alert(userNamessw[eval(chks[t].value)]+"�ѹرգ�������ѡ��");
						return false;
					}else{
					    L2TPList.hangstrsrv.value = L2TPList.hangstrsrv.value + userNamessw[i]+",";
					}
					
				} else {
					if((statusssw[i] =="�Ͽ�")) {
						alert(userNamessw[i]+"�ѹرգ�������ѡ��");
						return false;
					}else{
					L2TPList.hangstrsrv.value = L2TPList.hangstrsrv.value + userNamessw[i]+",";
					}
				}
			    } else {
				if (chks[t].value != "on") {
					if((statusssw[i] =="�Ͽ�")) {
						alert(setNamesw[eval(chks[t].value)]+"�ѹرգ�������ѡ��");
						return false;
					}else{
					L2TPList.hangstr.value = L2TPList.hangstr.value + setNamesw[i]+",";
				}
					
				} else {
					if((statusssw[i] =="�Ͽ�")) {
						alert(setNamesw[i]+"�ѹرգ�������ѡ��");
						return false;
					}else{
					L2TPList.hangstr.value = L2TPList.hangstr.value + setNamesw[i]+",";
					}
				}
			    }
			}
		}
		else {
			if(chks.checked==1) {
			    if (tunnelTypesw[i] == "�����") {
				if (chks.value != "on") {
					if((statusssw[i] =="�Ͽ�")) {
						alert(userNamessw[eval(chks.value)]+"�ѹرգ�������ѡ��");
						return false;
					}
					else{
					    L2TPList.hangstrsrv.value = L2TPList.hangstrsrv.value + userNamessw[i]+",";
					}
				} else {
					if((statusssw[i] =="�Ͽ�")) {
						alert(userNamessw[i]+"ʵ���ѹرգ�������ѡ��");
						return false;
					}else{
					    L2TPList.hangstrsrv.value = L2TPList.hangstrsrv.value + userNamessw[i]+",";
					}
				}
			    } else {
				if (chks.value != "on") {
					if((statusssw[i] =="�Ͽ�")) {
						alert(setNamesw[eval(chks.value)]+"�ѹرգ�������ѡ��");
						return false;
					}else{
					L2TPList.hangstr.value = L2TPList.hangstr.value + setNamesw[i]+",";
				}
				
				} else {
					if((statusssw[i] =="�Ͽ�")) {
						alert(setNamesw[i]+"�ѹرգ�������ѡ��");
						return false;
					}else{
					L2TPList.hangstr.value = L2TPList.hangstr.value + setNamesw[i]+",";
					}
				}
			}
			}
		}
	t=t+1;
  }
  if((L2TPList.hangstr.value!="")||(L2TPList.hangstrsrv.value!=""))
	{
				//L2TPList.hangstr.value=setNames[index];
				L2TPList.action="/goform/formL2tpHang";
				//L2TPList.submit();
	}
  else {
	alert("����ѡ��L2TP��Ϣ��¼,�ٹҶϸ�����!");
	return false;
  }
}

/*
 * �ͻ���ʵ��������delstrCli��
 * �����ʵ��������delstr��
 */
function genData(formName,index,list,array,tips)//ɾ���б��е�Ԫ��
{
  var recbegin;
  var recend;
  var t=0;
  var chks = list.document.getElementsByName("chklist");
  formName.delstr.value="";
  recbegin=(currpage[index]-1)*recsPerPage[index];		//��ʾҳ��Ŀ�ʼ��¼�� ��0Ϊ��ʼ�±�
  recend=recbegin+recsPerPage[index];					//��ʾҳ��Ľ�����¼��
  
  if(recend>=recscount[index]) recend=recscount[index];		//��������ҳ�����ܼ�����ֵ��recend
  
  for(var i=recbegin;i<recend;i++) {					//��Ϊrecbegin����ʾ�Ŀ�ʼ��¼��Сһ������i< ���� <= recend
	  
		if(chks.length) {						//ѭ������checkbox
			if(chks[t].checked==1)
				if (chks[t].value != "on") {
					formName.delstr.value=formName.delstr.value+array[eval(chks[t].value)]+",";
				//�������ز���
				} else {
					if (tunnelTypesw[i] == "�ͻ���") {
						formName.delstrCli.value=formName.delstrCli.value+array[i]+",";
					} else {//�����
						formName.delstr.value=formName.delstr.value+array[i]+",";
					}
				}
		}
		else {
			if(chks.checked==1)
				if (chks.value != "on") {
					formName.delstr.value=formName.delstr.value+array[eval(chks.value)]+",";
				} else {
					if (tunnelTypesw[i] == "�ͻ���") {
						formName.delstrCli.value=formName.delstrCli.value+array[i]+",";
					} else {//�����
						formName.delstr.value=formName.delstr.value+array[i]+",";
					}
				}
		}
		
	t=t+1;
  }

  if((formName.delstr.value!="") || (formName.delstrCli.value!=""))		return true;		//�ж��Ƿ�ѡ��checkbox
  else
  {
	alert("<% write(PleaseChooseFirst); %>"+tips+"<% write(Record); %>");
	return false;
  }  
}
-->
</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
  <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
  <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle("L2TP");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr1,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
   <table id=grouplist class="style7"  width="95%"  border="0" align="center" cellpadding="0" cellspacing="0" height="250" >
   <tr><td colspan="4" height="20">&nbsp;</td></tr>
  <tr >
    <td align="center"  ><form name="L2TPList" method="post" action="/goform/l2tpSrvListDel"><table id=grouplist style="display" width="920"  border="0" align="center" cellpadding="0" cellspacing="0">

		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe>
		       </td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="920" height="150" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe> <span class="style6">
		 <input type="hidden" name="dialstr" size=10 ><input type="hidden" name="hangstr" size=10 ><input type="hidden" name="hangstrsrv">
		        </span></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"	height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5" height="35"></td>
			  <td  class="style6"><div align="left" style="padding-top:5px;">
			  <input type="checkbox" name="delchk" onClick="selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))">
			  ȫѡ / ȫ��ѡ</script></div>
			  <input type="hidden" name="delstr" size=10 ><input type="hidden" name="delstrCli" size=10 ><input type="hidden" name="id">&nbsp;</td>
				  <td id=showbutton colspan="4" height="20" align="right"><script language="javascript">showOpButton();</script>
&nbsp;&nbsp;<input type="button" value="ɾ��������Ŀ" class="input-button" onClick="delAll()" id="delall">&nbsp;&nbsp;<input type="submit" value="ɾ��" class="input-button" onClick="DeletePPTP()" id="del">
</td>
			  </tr>
		      </table></td>
		    </tr>
		  </table>
		  </form>
			  </td>
			  </tr>  
  </table>		  
		  </td>	  
		  
	
		  </tr>
		  
		  
		  </table>
</td><td width="15">&nbsp;</td>
  </tr><script language="javascript">init();</script>
</table>
</body>
</html>
