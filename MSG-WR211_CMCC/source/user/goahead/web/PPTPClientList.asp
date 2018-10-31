<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("PPTP"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
var strSecondMenuFocus="PPTP";
<% aspPptpClientConfig(); %>
var cli_err = errorstr;
<% OutputPptpSrvListData(); %>
if (cli_err != "") {
    errorstr = cli_err;
}

/*
 * �б��У�������˺����ϣ��ͻ����˺�����,0 ~ srv_totalrecs�Ƿ�����˺ţ�srv_totalrecs ~ totalrecs + srv_totalrecs�ǿͻ����˺�
 */
/*
 * �����б�����ʾ����
 */
var setNamesw = new Array(); var userNamesw = new Array(); var enablesw = new Array(); var tunnelTypesw = new Array(); var peerTypesw = new Array(); var remoteGwIpsw = new Array(); var remoteLanIpsw = new Array(); var remoteLanNetmasksw = new Array(); var statussw = new Array(); var useOtimesw = new Array(); var outboundssw = new Array();  var inboundssw = new Array();

/*
 * ��������ݺͿͻ������ݺϲ�
 */
for (i = 0; i < (srv_totalrecs + totalrecs); i++) {
	if (i < srv_totalrecs) {	/* ������˺� */
		setNamesw[i] = srv_instNames[i];
		userNamesw[i] = srv_userNames[i];
		enablesw[i] = ((srvAcc_enables[i] == 1)?"yes":"no");
		tunnelTypesw[i] = "<% write(SevSide); %>";
		if (srv_peerTypes[i] == "lantolan") {
			peerTypesw[i] = "<% write(LanToLan); %>";
		} else {
			peerTypesw[i] = "<% write(MobileUser); %>";
		}
		remoteGwIpsw[i] = srv_remoteGwIps[i];
		remoteLanIpsw[i] = srv_remoteLanIps[i];
		remoteLanNetmasksw[i] = srv_remoteNetmasks[i];
		statussw[i] = (srv_statuss[i]=="1"?"<% write(Connected); %>":"<% write(Disconnected); %>");
		if(srv_useTimes[i] != "-")
		{
			var runday=parseInt((eval(srv_useTimes[i])/3600)/24);
			var runhour=parseInt((eval(srv_useTimes[i])/3600)%24);
			var runmiu=parseInt((eval(srv_useTimes[i])%3600)/60);
			var runsec=parseInt((eval(srv_useTimes[i])%3600)%60);
			useOtimesw[i] = runday+"<% write(com_day); %>"+runhour+"<% write(com_hour); %>"+runmiu+"<% write(com_minute); %>"+runsec+"<% write(com_second); %>";
			outboundssw[i] =  parseInt(eval(srv_outboundss[i])/8);
			inboundssw[i] =  parseInt(eval(srv_inboundss[i])/8);
 		}
		else
		{
			useOtimesw[i] = "-";
			outboundssw[i] = "-";
			inboundssw[i] = "-";
		}			
	} else {	/* �ͻ����˺� */
		setNamesw[i] = setNames[i - srv_totalrecs];
		userNamesw[i] = userNames[i - srv_totalrecs];
		enablesw[i]=(PPTPClientEnables[i - srv_totalrecs]=="on"?"yes":"no");
		tunnelTypesw[i] = "<% write(Client); %>";	
		peerTypesw[i] = "-";
		remoteGwIpsw[i] = serverGwIps[i - srv_totalrecs];
		remoteLanIpsw[i] = serverLanIps[i - srv_totalrecs];
		remoteLanNetmasksw[i] = serverLanNetMasks[i - srv_totalrecs];
		if (statuss[i - srv_totalrecs]=="1")
		        statussw[i] = "<% write(Connected); %>";
		else if (statuss[i - srv_totalrecs]=="0")
		        statussw[i] = "<% write(Disconnected); %>";
		else
		        statussw[i] = "<% write(Connecting); %>";
		if(usetimes[i - srv_totalrecs] != "-")
		{
			var runday=parseInt((eval(usetimes[i - srv_totalrecs])/3600)/24);
			var runhour=parseInt((eval(usetimes[i - srv_totalrecs])/3600)%24);
			var runmiu=parseInt((eval(usetimes[i - srv_totalrecs])%3600)/60);
			var runsec=parseInt((eval(usetimes[i - srv_totalrecs])%3600)%60);
			useOtimesw[i] = runday+"<% write(com_day); %>"+runhour+"<% write(com_hour); %>"+runmiu+"<% write(com_minute); %>"+runsec+"<% write(com_second); %>";
			outboundssw[i] =  parseInt(eval(outboundss[i - srv_totalrecs])/8);
			inboundssw[i] =  parseInt(eval(inboundss[i - srv_totalrecs])/8);
 		}
		else
		{
			useOtimesw[i] = "-";
			outboundssw[i] = "-";
			inboundssw[i] = "-";
		}	
	}
}

recsPerPage[0] = 10;
lineButton[0] =10;
currpage[0]=1;
recscount[0]=(totalrecs + srv_totalrecs);
maxrecs[0]=(max_totalrecs + srv_max_totalrecs);
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<% write(PPTPMesList); %>");
var titles1=new Array(11);
var index = 0;
titles1[index++]="<% write(TunnelName); %>";
titles1[index++]="<% write(com_user); %>";
titles1[index++]="<% write(StartUsing); %>";
titles1[index++]="<% write(Profession); %>";
titles1[index++]="<% write(Usertype); %>";
titles1[index++]="<% write(RemoteGwIPAddr); %>";
titles1[index++]="<% write(RemoteIPAddr); %>";
titles1[index++]="<% write(RemoteMaskAddr); %>";
titles1[index++]="<% write(SessionState); %>";
titles1[index++]="<% write(UsingTime); %>";
titles1[index++]="<% write(Tflow); %>";
titles1[index++]="<% write(Rflow); %>";

arrayData1=new Array(setNamesw, userNamesw, enablesw, tunnelTypesw, peerTypesw, remoteGwIpsw, remoteLanIpsw, remoteLanNetmasksw, statussw, useOtimesw, outboundssw, inboundssw);
sortable[0]=new Array(1,1,1,1,1,1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE,TYPE_STR, TYPE_STR, TYPE_STR,TYPE_STR,TYPE_CHINESE,TYPE_NUM,TYPE_NUM);
arrayCount[0]=12;


 
function dsplist(index,list)
{
	if(index==0) dspList(index,list,820,titles1,12,arrayData1,1,1);
}
function SaveUserAllow(index,i)
{
    if (enablesw[i].toLowerCase() == "no")��//��ѡ��ԭ����ֵ��"DENY"������ѡ�У���ô�������ñ�����ʱ״̬Ӧ����ѡ��
    {
	str="<%write(com_allow);%>";
	PPTPClientList.Allow.value= '1' ;
    }
    else
    {
	str="<%write(com_ban);%>";
	PPTPClientList.Allow.value= '0' ;
    }
    PPTPClientList.AllowID.value= userNamesw[i];
    if (confirm("<%write(com_really);%>"+str+"<%write(com_relNet);%>"))
    {
	if (tunnelTypesw[i] == "�����") {
	    PPTPClientList.action ="/goform/formPPTPSrvAccAllow";
	} else {
	    PPTPClientList.action ="/goform/formPPTPCliAccAllow";
	}
	PPTPClientList.submit();
    }
    else
    {
	dsplisthead(index,frame11);
	dsplist(index,frame12);
    }
}


function init(){

}

function showOpButton(){
		showbutton.innerHTML='<input type="button" value="<% write(AddClient); %>" class="input-button" onClick="EditConfig(0, 0, 0)">&nbsp;&nbsp;<input type="button" value="<% write(AddSev); %>" class="input-button" onClick="EditConfig(0, 0, 1)">&nbsp;&nbsp;<input type="submit" value="<% write(DelAll); %>" class="input-button" onClick="delAll()" id="delA">&nbsp;&nbsp;<input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="DeletePPTP()" id="del">';
	//	if (totalrecs != 0) {
			showbutton.innerHTML+='&nbsp;&nbsp;<input type="submit" value="<% write(Build); %>" onClick="return(genVpnDialData(0,frame12))" class="input-button" id = "dail">&nbsp;&nbsp;<input type="submit" value="<% write(Hangup); %>" onClick="return(genVpnHangData(0,frame12))" class="input-button" id="hang">';
	//	}
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
			if (eval(max_totalrecs-totalrecs)<1) {
				alert("<% write(PPTPClientMaxErrMsg); %>");
				return;		
			}
			PPTPClientList.action="PPTPClient.asp";
		} else if(type == 1) {
			if (srv_enables == 1) {
				PPTPClientList.action="PPTPSrvAccount.asp";
			} else {
				PPTPClientList.action="PPTPSrvGlobal.asp";
			}
		} else {
			;
		}
	} 
	/*�༭*/
	else if(val == 1) {
		PPTPClientList.id.value = setNamesw[i];
		/*
		 * �б��У�������˺������棬�ͻ����˺�������
		 */
		if (tunnelTypesw[i] == "<% write(SevSide); %>") {
			PPTPClientList.action="PPTPSrvAccount.asp";
		} else {
			PPTPClientList.action="PPTPClient.asp";
		}
	} else {
	;
	}
		

	PPTPClientList.submit();
}


function DeletePPTP(){
			return(genData(PPTPClientList,0,frame12,setNamesw,'<% write(PPTPUserMessage); %>'))
}

/*
 * delstrsrv��������ɾ��ʵ����
 * delstr����ͻ���ɾ��ʵ����
 */
function DeleteConfig(index){
			if(confirm("<% write(com_warning3); %>")){
				if (tunnelTypesw[index] == "<% write(SevSide); %>") {
					PPTPClientList.delstrsrv.value=setNamesw[index];
				} else {
					PPTPClientList.delstr.value=setNamesw[index];
				}
				PPTPClientList.action="/goform/pptpListDel";
				PPTPClientList.submit();
			}
}

function delAll(){
	if(confirm("<% write(com_warning4); %>")){
		PPTPClientList.action="/goform/pptpListDelAll";
		PPTPClientList.submit();
	}
}


function genVpnDialData(index,list)
{
  var recbegin;
  var recend;
  var t=0;
    var chks = list.document.getElementsByName("chklist");
  PPTPClientList.dialstr.value="";
  recbegin=(currpage[index]-1)*recsPerPage[index];
  recend=recbegin+recsPerPage[index];
  if(recend>=recscount[index]) recend=recscount[index];
  for(var i=recbegin;i<recend;i++) {
		if(chks.length) {
			if(chks[t].checked==1) {
				/*
				 * ������˺Ų��ܲ���
				 */
				if (tunnelTypesw[i] == "<% write(SevSide); %>") {
					alert("<% write(PleaseChooseClientUser); %>");
					return false;
				}
				if((statussw[i]!="<% write(Disconnected); %>")) {
					alert(setNamesw[i]+"<% write(BreakErrMes1); %>");
					return false;
				}
		
				else if((enablesw[i]!="yes")) {
					alert(setNamesw[i]+"<% write(BreakErrMes2); %>");
					return false;
				}else{
					PPTPClientList.dialstr.value = PPTPClientList.dialstr.value + setNamesw[i]+",";
				}
				
			}
		}
		else {
			if(chks.checked==1)  {
				/*
				 * ������˺Ų��ܲ���
				 */
				if (tunnelTypesw[i] == "<% write(SevSide); %>") {
					alert("<% write(PleaseChooseClientUser); %>");
					return false;
				}			
				if((statussw[i]!="<% write(Disconnected); %>")) {
					alert(setNamesw[i]+"<% write(BreakErrMes1); %>");
					return false;
				}
		
				else if((enablesw[i]!="yes")) {
					alert(setNamesw[i]+"<% write(BreakErrMes2); %>");
					return false;
				}else{
					PPTPClientList.dialstr.value = PPTPClientList.dialstr.value + setNamesw[i]+",";
				}
				
			}
		}
	t=t+1;
  }
  
  if(PPTPClientList.dialstr.value!="")
  {
  				//PPTPClientList.dialstr.value=setNames[index];
				PPTPClientList.action="/goform/formDia";
				//PPTPClientList.submit();
				return true;
  }
  else {
	alert("<% write(BreakErrMes3); %>");
	return false;
  }
}

function genVpnHangData(index,list)
{
  var recbegin;
  var recend;
  var t=0;
  var chks = list.document.getElementsByName("chklist");
  PPTPClientList.hangstr.value="";
  recbegin=(currpage[index]-1)*recsPerPage[index];
  recend=recbegin+recsPerPage[index];
  if(recend>=recscount[index]) recend=recscount[index];
  for(var i=recbegin;i<recend;i++) {
		if(chks.length) {
			if(chks[t].checked==1) {
				/*
				 * 
				 */
				if (tunnelTypesw[i] == "<% write(SevSide); %>") {
				//	alert("<% write(PleaseChooseClientUser); %>");
				//	return false;i
					if (chks[t].value != "on") {
						if((statuss[i] =="<% write(Disconnected); %>")) {
							alert(userNamesw[eval(chks[t].value)]+"<% write(OffMes); %>");
							return false;
						}else{
						PPTPClientList.hangstrsrv.value = PPTPClientList.hangstrsrv.value + userNamesw[i]+",";
					}
					
					} else {
						if((statussw[i] =="<% write(Disconnected); %>")) {
							alert(userNamesw[i]+"<% write(OffMes); %>");
							return false;
						}else{
						PPTPClientList.hangstrsrv.value = PPTPClientList.hangstrsrv.value + userNamesw[i]+",";
						}
					}

				}else{			
					if (chks[t].value != "on") {
						if((statuss[i] =="<% write(Disconnected); %>")) {
							alert(setNamesw[eval(chks[t].value)]+"<% write(OffMes); %>");
							return false;
						}else{
						PPTPClientList.hangstr.value = PPTPClientList.hangstr.value + setNamesw[i]+",";
					}
					
					} else {
						if((statussw[i] =="<% write(Disconnected); %>")) {
							alert(setNamesw[i]+"<% write(OffMes); %>");
							return false;
						}else{
						PPTPClientList.hangstr.value = PPTPClientList.hangstr.value + setNamesw[i]+",";
						}
					}
				}
			}
		}
		else {
			if(chks.checked==1) {
				/*
				 * 
				 */
				if (tunnelTypesw[i] == "<% write(SevSide); %>") {
				//	alert("<% write(PleaseChooseClientUser); %>");
				//	return false;
					if (chks.value != "on") {
						if((statussw[i] =="<% write(Disconnected); %>")) {
							alert(userNamesw[eval(chks.value)]+"<% write(OffMes); %>");
							return false;
						}
						else{
						PPTPClientList.hangstrsrv.value = PPTPClientList.hangstrsrv.value + userNamesw[i]+",";
					}
					} else {
						if((statussw[i] =="<% write(Disconnected); %>")) {
							alert(userNamesw[i]+"<% write(ExOffMes); %>");
							return false;
						}else{
						PPTPClientList.hangstrsrv.value = PPTPClientList.hangstrsrv.value + userNamesw[i]+",";
					}
	
					}
				}else{		
							
					if (chks.value != "on") {
						if((statussw[i] =="<% write(Disconnected); %>")) {
							alert(setNamesw[eval(chks.value)]+"<% write(OffMes); %>");
							return false;
						}
						else{
						PPTPClientList.hangstr.value = PPTPClientList.hangstr.value + setNamesw[i]+",";
					}
					} else {
						if((statussw[i] =="<% write(Disconnected); %>")) {
							alert(setNamesw[i]+"<% write(ExOffMes); %>");
							return false;
						}else{
						PPTPClientList.hangstr.value = PPTPClientList.hangstr.value + setNamesw[i]+",";
					}
	
					}
				}
			}
		}
	t=t+1;
  }
  if((PPTPClientList.hangstr.value!="")||(PPTPClientList.hangstrsrv.value!=""))
  {
				//PPTPClientList.hangstr.value=setNames[index];
				PPTPClientList.action="/goform/formhang";
				//PPTPClientList.submit();
  }
  else {
	alert("<% write(HangupErrMes); %>");
	return false;
  }
}

/*
 * �����ʵ��������delstrsrv��
 * �ͻ���ʵ��������delstr��
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
					if (tunnelTypesw[i] == "<% write(SevSide); %>") {//�����
						formName.delstrsrv.value=formName.delstrsrv.value+array[i]+",";
					} else {
						formName.delstr.value=formName.delstr.value+array[i]+",";
					}
				}
		}
		else {
			if(chks.checked==1)
				if (chks.value != "on") {
					formName.delstr.value=formName.delstr.value+array[eval(chks.value)]+",";
				} else {
					if (tunnelTypesw[i] == "<% write(SevSide); %>") {//�����
						formName.delstrsrv.value=formName.delstrsrv.value+array[i]+",";
					} else {
						formName.delstr.value=formName.delstr.value+array[i]+",";
					}
				}
		}
		
	t=t+1;
  }

  if((formName.delstr.value!="") || (formName.delstrsrv.value!=""))		return true;		//�ж��Ƿ�ѡ��checkbox
  else
  {
	alert("<% write(PleaseChooseFirst); %>"+tips+"<% write(Record); %>");
	return false;
  }  
}
-->
</script>
</head>
<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
  <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
  <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle("<% write(PPTPCListSecondTitle); %>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
   <table id=grouplist width="90%"  border="0" align="center" cellpadding="0" cellspacing="0" height="250" >
  <tr >
    <td align="center" class="style7" ><form name="PPTPClientList" method="post" action="/goform/pptpListDel"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
			<tr><td colspan="4" height="20">&nbsp;</td></tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="650" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe>
		       </td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="650" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe> <span class="style6">
		 <input type="hidden" name="dialstr" size=10 ><input type="hidden" name="hangstr" size=10 ><input type="hidden" name="hangstrsrv">
		        </span></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"	height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5" height="35"></td>
			  <td  class="style6"><input type="hidden" name="delstr" size=10 value=""><input type="hidden" name="delstrsrv" size=10 value=""><input type="hidden" name="id"><input type="hidden" name="AllowID"><input type="hidden" name="Allow">&nbsp;</td>
			  <td  class="style6"><div align="left">
				  <input type="checkbox" name="delchk" onClick="selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))">
				  <% write(com_select); %></div></td>
				  <td id=showbutton colspan="4" height="20" align="right"><script language="javascript">showOpButton();</script></td>

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
