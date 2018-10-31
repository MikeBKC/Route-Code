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
    if(i<totalrecs) { /*服务端账号*/
	setNamesw[i]=instNames[i];
	userNamessw[i]=userNames[i];
	enablesw[i]="yes";
	tunnelTypesw[i]="服务端";
	remoteLanIpsw[i]=remoteLanIps[i];
	remoteLanNetmasksw[i]=remoteNetmasks[i];
	
	statusssw[i]=(statuss[i]=="1"?"已连接":"断开");
	if (peerTypes[i] == "lantolan") {
		peerTypesw[i] = "LAN到LAN";
	} else {
		peerTypesw[i] = "移动用户";
	}
	if(useTimes[i] != "-")
	{
		var runday=parseInt((eval(useTimes[i])/3600)/24);
		var runhour=parseInt((eval(useTimes[i])/3600)%24);
		var runmiu=parseInt((eval(useTimes[i])%3600)/60);
		var runsec=parseInt((eval(useTimes[i])%3600)%60);
		useOtimes[i] = runday+"天"+runhour+"小时"+runmiu+"分"+runsec+"秒";
		outboundss[i] =  parseInt(eval(outboundss[i])/8);
		inboundss[i] =  parseInt(eval(inboundss[i])/8);
		useOtimessw[i] = runday+"天"+runhour+"小时"+runmiu+"分"+runsec+"秒";
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
    } else { /*客户端账号*/
	setNamesw[i]=cli_setNames[i-totalrecs];
	userNamessw[i]=cli_userNames[i-totalrecs];
	enablesw[i]=((L2TPClientEnables[i-totalrecs]=="on")?"yes":"no");
	tunnelTypesw[i]="客户端";
	remoteLanIpsw[i]=cli_serverLanIps[i-totalrecs];
	remoteLanNetmasksw[i]=cli_serverLanNetMasks[i-totalrecs];

	peerTypesw[i] = "-";
	statusssw[i]=(cli_statuss[i-totalrecs]=="1"?"已连接":"断开");
	if(cli_usetimes[i-totalrecs] != "-")
	{
		var runday=parseInt((eval(cli_usetimes[i-totalrecs])/3600)/24);
		var runhour=parseInt((eval(cli_usetimes[i-totalrecs])/3600)%24);
		var runmiu=parseInt((eval(cli_usetimes[i-totalrecs])%3600)/60);
		var runsec=parseInt((eval(cli_usetimes[i-totalrecs])%3600)%60);
		useOtimessw[i] = runday+"天"+runhour+"小时"+runmiu+"分"+runsec+"秒";
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
listheadcaption[0]=_("L2TP信息列表");
var titles1=new Array(10);
var index = 0;
titles1[index++]="隧道名称";
titles1[index++]="用户名";
titles1[index++]="业务";
titles1[index++]="用户类型";
titles1[index++]="远端内网IP地址";
titles1[index++]="远端内网子网掩码";
titles1[index++]="会话状态";
titles1[index++]="使用时间";
titles1[index++]="出流量(Byte)";
titles1[index++]="入流量(Byte)";
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
//	showbutton.innerHTML='<input type="button" value="添加客户端" class="input-button" onClick="EditConfig(0, 0, 0)">&nbsp;&nbsp;<input type="button" value="添加服务器" class="input-button" onClick="EditConfig(0, 0, 1)">';
	/* 隐藏l2tp客户端 */
	showbutton.innerHTML='<input type="button" value="添加客户端" class="input-button" onClick="EditConfig(0, 0, 0)">&nbsp;&nbsp;<input type="button" value="添加服务器" class="input-button" onClick="EditConfig(0, 0, 1)">';
	showbutton.innerHTML+='&nbsp;&nbsp;<input type="submit" value="建立" onClick="return(genVpnDialData(0,frame12))" class="input-button" id = "dail">&nbsp;&nbsp;<input type="submit" value="挂断" onClick="return(genVpnHangData(0,frame12))" class="input-button" id="hang">';
}

/*
 * val: 0添加，1编辑
 * i:	哪一条
 * type: 0客户端，1服务端
 */
function EditConfig(val, i, type){
	
	/* 添加 */
	if(val == 0){ 
		/*客户端*/
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
	/*编辑*/
	else if(val == 1) {
		L2TPList.id.value = setNamesw[i];
		
		/*
		 * 列表中，服务端账号在上面，客户端账号在下面
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
			return(genData(L2TPList,0,frame12,setNamesw,'L2TP信息列表'))
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
				 * 服务端账号不能拨号
				 */
				if (tunnelTypesw[i] == "服务端") {
					alert("请选择客户端账号");
					return false;
				}
				if((statusssw[i]!="断开")) {
					alert(setNamesw[i]+"已连接或正在连接，请先挂断再重建立连接！");
					return false;
				}
		
				else if((enablesw[i]!="yes")) {
					alert(setNamesw[i]+"配置未启用，请启用后再重建立连接！");
					return false;
				}else{
					L2TPList.dialstr.value = L2TPList.dialstr.value + setNamesw[i]+",";
				}
				
			}
		}
		else {
			if(chks.checked==1)  {
				/*
				 * 服务端账号不能拨号
				 */
				if (tunnelTypesw[i] == "服务端") {
					alert("请选择客户端账号");
					return false;
				}			
			
				if((statusssw[i]!="断开")) {
					alert(setNamesw[i]+"已连接或正在连接，请先挂断再重建立连接！");
					return false;
				}
		
				else if((enablesw[i]!="yes")) {
					alert(setNamesw[i]+"配置未启用，请启用后再重建立连接！");
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
	alert("请先选择L2TP信息记录,再建立连接!");
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
			    if (tunnelTypesw[i] == "服务端") {
				if (chks[t].value != "on") {
					if((statusssw[i] =="断开")) {
						alert(userNamessw[eval(chks[t].value)]+"已关闭，请重新选择！");
						return false;
					}else{
					    L2TPList.hangstrsrv.value = L2TPList.hangstrsrv.value + userNamessw[i]+",";
					}
					
				} else {
					if((statusssw[i] =="断开")) {
						alert(userNamessw[i]+"已关闭，请重新选择！");
						return false;
					}else{
					L2TPList.hangstrsrv.value = L2TPList.hangstrsrv.value + userNamessw[i]+",";
					}
				}
			    } else {
				if (chks[t].value != "on") {
					if((statusssw[i] =="断开")) {
						alert(setNamesw[eval(chks[t].value)]+"已关闭，请重新选择！");
						return false;
					}else{
					L2TPList.hangstr.value = L2TPList.hangstr.value + setNamesw[i]+",";
				}
					
				} else {
					if((statusssw[i] =="断开")) {
						alert(setNamesw[i]+"已关闭，请重新选择！");
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
			    if (tunnelTypesw[i] == "服务端") {
				if (chks.value != "on") {
					if((statusssw[i] =="断开")) {
						alert(userNamessw[eval(chks.value)]+"已关闭，请重新选择！");
						return false;
					}
					else{
					    L2TPList.hangstrsrv.value = L2TPList.hangstrsrv.value + userNamessw[i]+",";
					}
				} else {
					if((statusssw[i] =="断开")) {
						alert(userNamessw[i]+"实例已关闭，请重新选择！");
						return false;
					}else{
					    L2TPList.hangstrsrv.value = L2TPList.hangstrsrv.value + userNamessw[i]+",";
					}
				}
			    } else {
				if (chks.value != "on") {
					if((statusssw[i] =="断开")) {
						alert(setNamesw[eval(chks.value)]+"已关闭，请重新选择！");
						return false;
					}else{
					L2TPList.hangstr.value = L2TPList.hangstr.value + setNamesw[i]+",";
				}
				
				} else {
					if((statusssw[i] =="断开")) {
						alert(setNamesw[i]+"已关闭，请重新选择！");
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
	alert("请先选择L2TP信息记录,再挂断该连接!");
	return false;
  }
}

/*
 * 客户端实例保存在delstrCli中
 * 服务端实例保存在delstr中
 */
function genData(formName,index,list,array,tips)//删除列表中的元素
{
  var recbegin;
  var recend;
  var t=0;
  var chks = list.document.getElementsByName("chklist");
  formName.delstr.value="";
  recbegin=(currpage[index]-1)*recsPerPage[index];		//显示页面的开始记录数 以0为开始下标
  recend=recbegin+recsPerPage[index];					//显示页面的结束记录数
  
  if(recend>=recscount[index]) recend=recscount[index];		//如果是最后页　把总计量赋值给recend
  
  for(var i=recbegin;i<recend;i++) {					//因为recbegin比显示的开始记录数小一，所以i< 并非 <= recend
	  
		if(chks.length) {						//循环遍历checkbox
			if(chks[t].checked==1)
				if (chks[t].value != "on") {
					formName.delstr.value=formName.delstr.value+array[eval(chks[t].value)]+",";
				//生成隐藏参数
				} else {
					if (tunnelTypesw[i] == "客户端") {
						formName.delstrCli.value=formName.delstrCli.value+array[i]+",";
					} else {//服务端
						formName.delstr.value=formName.delstr.value+array[i]+",";
					}
				}
		}
		else {
			if(chks.checked==1)
				if (chks.value != "on") {
					formName.delstr.value=formName.delstr.value+array[eval(chks.value)]+",";
				} else {
					if (tunnelTypesw[i] == "客户端") {
						formName.delstrCli.value=formName.delstrCli.value+array[i]+",";
					} else {//服务端
						formName.delstr.value=formName.delstr.value+array[i]+",";
					}
				}
		}
		
	t=t+1;
  }

  if((formName.delstr.value!="") || (formName.delstrCli.value!=""))		return true;		//判断是否选择checkbox
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
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
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
			  全选 / 全不选</script></div>
			  <input type="hidden" name="delstr" size=10 ><input type="hidden" name="delstrCli" size=10 ><input type="hidden" name="id">&nbsp;</td>
				  <td id=showbutton colspan="4" height="20" align="right"><script language="javascript">showOpButton();</script>
&nbsp;&nbsp;<input type="button" value="删除所有条目" class="input-button" onClick="delAll()" id="delall">&nbsp;&nbsp;<input type="submit" value="删除" class="input-button" onClick="DeletePPTP()" id="del">
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
