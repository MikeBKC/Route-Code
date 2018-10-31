<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<%langTran("left");%>
<title><%write(left_1);%></title>
<base target="main" />
<link rel=stylesheet type="text/css" href="css/style.css">
<script src="js/common.js" type="text/javascript"></script>
<script src="js/config.js" type="text/javascript"></script>
<script src="js/dyn_config.js" type="text/javascript"></script>

<style type="text/css">
<!--
A:link { text-decoration: none; color: #000000; }
A:active { text-decoration: none; color: #ffffff; }
A:visited { text-decoration: none; color: #000000; }  
A:hover { text-decoration: underline; color: #ffffff; }
-->
</style>
<script type="text/javascript" src="lang/b28n.js" ContentType="text/xml"></script>
<script>
<% langTran("left"); %>
<% langTran("common"); %>
<% getSysLang(); %>
<%getWanIfCount();%> 
<%getLicenseNeedFlag();%>
if(wanIfCount > 1)
{
    DoubleRoad=1;
}
if ((typeof(WanTagVlan) == 'undefined') || (WanTagVlan == 0)) {
    WanTagVlanEn = 0;
} else {
    if (wanIfCount > 1) {
	WanTagVlanEn = 0;
    } else {
	WanTagVlanEn = 1;
    }
}

var i = 0;
var j = 0;
var SmartName = "";
if (typeof(SysLogInfo)=='undefined')
	var SysLogInfo = 0;
if (typeof(CacheConfig)=='undefined')
	var CacheConfig=0;
if (typeof(NetMode)=='undefined')
	var NetMode=0;
if (typeof(Cache)=='undefined')
	var Cache=0;
if (typeof(MacFilter)=='undefined')
	var MacFilter=0;
if (typeof(EasyTask)=='undefined')
	var EasyTask=0;
if (typeof(P2PLimit)=='undefined')
	var P2PLimit=0;
if (typeof(acMode)=='undefined')
	var acMode=0;
if (typeof(TagVlanList)=='undefined')
	var TagVlanList=0;
if (typeof(advideo)=='undefined')
	var advideo=0;
if (typeof(GameBoost)=='undefined')
	var GameBoost=0;
if (typeof(NetSniper)=='NetSniper')
	var NetSniper=0;
if (typeof(hw_nat)=='undefined')
	var hw_nat=0;
if (typeof(PortMirror7620E)=='undefined')
	var PortMirror7620E=0;
if (typeof(snmp)=='undefined')
	var snmp=0;
if (typeof(apUpdateConf)=='undefined')
	var apUpdateConf=0;
if (typeof(vlanConfig)=='undefined')
	var vlanConfig=0;
if (typeof(apDrawing)=='undefined')
	var apDrawing=0;
if (typeof(SysStatus)=='undefined')
	var SysStatus=0;
if (typeof(FwService)=='undefined')
	var FwService=0;
if (typeof(PingCheck)=='undefined')
	var PingCheck=0;
if (typeof(Ipv6Lan)=='undefined')
        var Ipv6Lan=0;
if (typeof(Ipv6Wan)=='undefined')
        var Ipv6Wan=0;
if (typeof(radvd)=='undefined')
        var radvd=0;
if (typeof(DHCPv6)=='undefined')
        var DHCPv6=0;
if (typeof(Ipv6Route)=='undefined')
        var Ipv6Route=0;
if (typeof(Ipv6PrefixList)=='undefined')
        var Ipv6PrefixList=0;
if (typeof(Ipv6Config)=='undefined')                                        
        var Ipv6Config=0;          
if (typeof(tunnel)=='undefined')                                        
        var tunnel=0;          
if (typeof(isatap)=='undefined')                                        
        var isatap=0;       
if (typeof(DDNSAdvance)=='undefined')                                        
        var DDNSAdvance=0;       

if (SmartQos == 1)
{
    SmartName = "<% write(leftMenud45); %>";
}else if(GameBoost == 1)
{
    SmartName = "<% write(leftMenud87); %>";
}

	/**********************用数组来开始菜单******************************/
	var startMenu = new Array(
 					//菜单名字；菜单的第一个子菜单；菜单是否显示；
		7,//一级菜单个数
		new Array("<% write(leftMenud0); %>",'FastConfig.asp',FastConfig),
		new Array("<% write(leftMenud1); %>",'RunStatus.asp',RunStatus),
		new Array("<% write(leftMenud81); %>",'EasyManage.asp',EasyTask),
		new Array("<% write(leftMenud2); %>",'InterfaceFlow.asp',InterfaceFlow),
		new Array("<% write(workMode); %>",'fatFitSwitch.asp',fatFitSwitch),
		new Array("<% write(SysStatus); %>",'firstPage.asp',SysStatus),
		new Array("<% write(leftMenud3); %>",'ReBoot.asp',ReBoot)
	); 
/**********************用数组来画菜单*******************************/
	//一级菜单数组
	var arrayMainMenu =  new Array();
	arrayMainMenu[j++] = (total+1);
	arrayMainMenu[j++] = new Array("<% write(leftTag1); %>",'WANConfig.asp',0,NetPara);
        arrayMainMenu[j++] = new Array("<% write(leftTag17); %>",'ipv6LANConfig.asp',0,Ipv6Config);
	arrayMainMenu[j++] = new Array("<% write(leftTag2); %>",'WlanBase.asp',0,WlsConf);
	arrayMainMenu[j++] = new Array("<% write(leftTag15); %>",'WlanBase.asp',0,WlsConf_5g);
	arrayMainMenu[j++] = new Array("<% write(leftTag14); %>",'fitApList',0,wlanMng);
if (typeof(tag_vlan) == 'defined') {	
	arrayMainMenu[j++] = new Array("<% write(tagVlan); %>",'fitApList',0,tag_vlan);
}
	arrayMainMenu[j++] = new Array("<% write(leftTag3); %>",'NatStatic.asp',0,AdvConf);
	arrayMainMenu[j++] = new Array("<% write(leftTag16); %>",'NetShareManage.asp',0,NetShareManage);
	arrayMainMenu[j++] = new Array("<% write(leftTag4); %>",'switch/ports.asp',0,SwFun);
	arrayMainMenu[j++] = new Array("<% write(leftTag5); %>",'UserLinkRate.asp',0,UserMng);
	arrayMainMenu[j++] = new Array("<% write(leftTag6); %>", 'TimeRange.asp',0,PM);
	arrayMainMenu[j++] = new Array("<% write(leftTag7); %>",'Group.asp',0,BM);
	arrayMainMenu[j++] = new Array("<% write(leftTag8); %>",'FireWall.asp',0,Fw);
	arrayMainMenu[j++] = new Array("<% write(leftTag9); %>",'PPTPClientList.asp',0,VPN);
	arrayMainMenu[j++] = new Array("<% write(leftTag10); %>",'switch/discover.asp',0,Urcp);
	arrayMainMenu[j++] = new Array("<% write(leftTag11); %>",'SysClock.asp',0,SysMng);
	arrayMainMenu[j++] = new Array("<% write(leftTag12); %>",'WireStatus.asp',0,SysSts);
	arrayMainMenu[j++] = new Array("<% write(leftTag13); %>",'SaleService.asp',0,CusSrv);
	arrayMainMenu[0] = j;
	
	//二级菜单数组
	var arraySecondMenu = new Array();
	arraySecondMenu[i++] = new Array(//'网络设置'的二级菜单
				13,//二级菜单个数
				new Array("<% write(workMode); %>",'acMode.asp',acMode),
				new Array("<% write(leftMenud4); %>",'WANConfig.asp',WANConfig),
				new Array("<% write(fitApIfConf); %>",'fitApIfConf.asp',fitApIfConf),
				new Array("<% write(leftMenud5); %>",'DoubleRoad.asp',DoubleRoad),
				new Array("<% write(leftMenud6); %>",'portnumber.asp',portnumber),
				new Array("<% write(leftMenud7); %>",'LANConfig.asp',LANConfig),
				new Array("<% write(vlanConfig); %>",'vlanConfig.asp',vlanConfig),
				new Array("<% write(leftMenud8); %>",'DhcpServer.asp',DhcpServer),
				new Array("<% write(leftMenud8); %>",'DhcpPoolForAc.asp',DhcpServerForAc),
				new Array("<% write(leftMenud9); %>",'DDNS.asp',DDNS),
				new Array("<% write(leftMenud9); %>",'DDNSAdvance.asp',DDNSAdvance),
				new Array("<% write(leftMenud10); %>",'UPnP.asp',UPnP),
				new Array('<% write(leftMenud11); %>','WANportnumber.asp',WANPort)
			);
	arraySecondMenu[i++] = new Array(//ipv6的二级菜单
		7,
                                new Array("<% write(leftMenud7); %>",'ipv6LANConfig.asp',Ipv6Lan),
                                new Array("<% write(leftMenud4); %>",'ipv6WANConfig.asp',Ipv6Wan),
                                new Array("<% write(leftMenud94); %>",'radvd.asp',radvd),
                                new Array("<% write(leftMenud95); %>",'DHCPv6.asp',DHCPv6),
                                new Array('<% write(leftMenud21); %>','ipv6Route.asp',Ipv6Route),
                                new Array('<% write(leftMenud96); %>','tunnel.asp',tunnel),
                                new Array('<% write(leftMenud97); %>','isatap.asp',isatap)
                );
	if(platform=="nv1800N") arraySecondMenu[i-1][1][0]="<% write(leftMenud12); %>";
	arraySecondMenu[i++] = new Array(//'无线设置'的二级菜单
				9,//二级菜单个数
				new Array("<% write(workMode); %>",'fatFitSwitch.asp',fatFitSwitch),
				new Array("<% write(wirePar); %>",'wlanPara.asp',wlanPara),
				new Array("<% write(chooseConf); %>",'apParaSwitch.asp',apParaSwitch),
				new Array("<% write(leftMenud13); %>",'WlanBase.asp',WlanBase),
				new Array("<% write(leftMenud14); %>",'WlanSecurity.asp',WlanSecurity),
				new Array("<% write(leftMenud15); %>",'WlanMacFilter.asp',WlanMacFilter),
				new Array("<% write(leftMenud16); %>",'WlanAdvanced.asp',WlanAdvanced),//有三级菜单
				new Array('<% write(leftMenud17); %>','WlanMultiSsid.asp',WlanMultiSsid),
				new Array("<% write(leftMenud18); %>",'WlanHostInfo.asp',WlanHostInfo)
			);
	if(platform=="nv1800N") arraySecondMenu[i-1][1][0]="<% write(leftMenud12); %>";
	arraySecondMenu[i++] = new Array(//'无线设置'的二级菜单
				5,//二级菜单个数
				new Array("<% write(leftMenud13); %>",'WlanBase_5g.asp',WlanBase_5g),
				new Array("<% write(leftMenud14); %>",'WlanSecurity_5g.asp',WlanSecurity_5g),
				new Array("<% write(leftMenud15); %>",'WlanMacFilter_5g.asp',WlanMacFilter_5g),
				new Array("<% write(leftMenud16); %>",'WlanAdvanced_5g.asp',WlanAdvanced_5g),//有三级菜单
				new Array("<% write(leftMenud18); %>",'WlanHostInfo_5g.asp',WlanHostInfo_5g)
			);
	arraySecondMenu[i++] = new Array(//'ap集中管理'的二级菜单
				10,//二级菜单个数
				new Array("<% write(apManage); %>",'apManage.asp',apManage),
				new Array("<% write(apDrawing); %>",'DrawingPreview.asp',apDrawing),
				new Array("<% write(WirelessStatis); %>",'WlanApClientsStatistics.asp',wlanCltList),
				new Array("<% write(serverZone); %>",'serviceZone.asp',serviceZone),
				new Array("<% write(leftMenud15); %>",'FitApMacFilter.asp',fitApMacFilter),
				new Array("<% write(apConfigTmp); %>",'apTmp.asp',apTmp),
				new Array("<% write(apSoftManage); %>",'apSoftMng.asp',apSoftMng),
				new Array("<% write(APCongfile); %>",'apUpdateConf.asp',apUpdateConf),
				new Array("<% write(loadBalance); %>",'apLoadBalanceList.asp',apLoadBalance),
				new Array("<% write(apLog); %>",'apLog.asp',apLog)
			);
	if (typeof(tag_vlan) == 'defined') {
	arraySecondMenu[i++] = new Array(//'ap集中管理'的二级菜单
				2,//二级菜单个数
				new Array("<% write(lanTagVlan); %>",'TagVlanList.asp',TagVlanList),
				new Array("<% write(wanTagVlan); %>",'WanTagVlan.asp',WanTagVlanEn)
			);
	}
	arraySecondMenu[i++] = new Array(//'高级配置'的二级菜单
				16,
				new Array("<% write(leftMenud19); %>",'NatStatic.asp',NatStatic),
				new Array("<% write(leftMenud21); %>",'Route.asp',Route),
				new Array("<% write(com_policyroute); %>",'PolicyRoute.asp',PolicyRoute),
				new Array("<% write(leftMenud22); %>",'AntiNetSnifer.asp',AntiNetSnifer),
				new Array("<% write(leftMenud88); %>",'NetSniper.asp',NetSniper),
				new Array('<% write(leftMenud23); %>','hotel.asp',Hotel),
				new Array("<% write(leftMenud24); %>",'PortMirror.asp',PortMirror),
				new Array("<% write(leftMenud25); %>",'PortMirror7620e.asp',PortMirror7620E),
				new Array("<% write(leftMenud24); %>",'PortMirror840E.asp',PortMirror840E),
				new Array("<% write(leftMenud25); %>",'PortMirror6530G.asp',PortMirror6530G),
				new Array("<% write(leftMenud25); %>",'PortMirror8367M.asp',PortMirror8367M),
				new Array("<% write(leftMenud65); %>",'port_vlan_rdd.asp',port_vlan_rdd),
				new Array("<% write(leftMenud93); %>",'Snmp.asp',snmp),
				new Array("<% write(leftMenud68); %>",'Syslog.asp',Syslog),
				new Array("<% write(leftMenud83); %>",'snmpConf.asp',snmpConf),
				new Array("<% write(leftMenud73); %>",'HardwareNAT.asp',hw_nat),//隐藏硬件NAT页面
				new Array("<% write(leftMenud82); %>",'CacheServerConfig.asp',CacheConfig)
			);
	arraySecondMenu[i++] = new Array(//'网络共享'的二级菜单
				3,//二级菜单个数
				new Array("<% write(leftMenud89); %>",'NetShareManage.asp',NetShareManage),
				new Array("<% write(leftMenud90); %>",'FtpServer.asp',FtpServer),
				new Array("<% write(leftMenud91); %>",'ShareAccountList.asp',ShareAccountList)
			);
	arraySecondMenu[i++] = new Array(//'交换功能'的二级菜单
				5,//二级菜单个数
				new Array("<% write(leftMenud26); %>",'switch/ports.asp',ports),
				new Array("<% write(leftMenud27); %>",'switch/mirror.asp',mirror),
				new Array("<% write(leftMenud28); %>",'switch/port_vlan.asp',port_vlan),
				new Array('<% write(leftMenud29); %>','switch/aggregation.asp',aggregation),
				new Array('<% write(leftMenud30); %>','switch/aggregation_100M.asp',aggregation_100M)
			);
	arraySecondMenu[i++] = new Array(//'用户管理'的二级菜单
				6,
				new Array("<% write(leftMenud31); %>",'UserLinkRate.asp',UserLinkRate),
				new Array("<% write(leftMenud32); %>",'IPMac.asp',IPMac),
				new Array("<% write(leftMenud33); %>",'PPPoEGlobal.asp',PPPoEGlobal),
				new Array("<% write(leftMenud34); %>",'WebAuthGlobal.asp',WebAuth),
				new Array('<% write(leftMenud35); %>','UserGroup.asp',UserGroup),
				new Array('<% write(leftMenud92); %>','FwService.asp',FwService)
			);
	arraySecondMenu[i++] = new Array( //'行为管理'的二级菜单 
				8,
				new Array('<% write(leftMenud36); %>','TimeRange.asp',TimeRange),
				new Array("<% write(leftMenud37); %>",'GlobalBase.asp',GlobalBase),
				new Array('<% write(leftMenud38); %>','QQFilter.asp',QQExcept),
				new Array('<% write(leftMenud39); %>','MSNFilter.asp',MSNExcept),
				new Array('<% write(com_Aliwang); %>','AlitalkFilter.asp',AliExcept),
				new Array('<% write(leftMenud40); %>','Notice.asp',Notice),
				new Array('<% write(leftMenud41); %>','NetActionAudit.asp',NetAction),
				new Array("<% write(leftMenud42); %>",'PdbUpdata.asp',PdbUpdata)
				//new Array('<% write(leftMenud43); %>','FwAddress.asp',FwAddress),
			);
	arraySecondMenu[i++] = new Array( //'带宽管理'的二级菜单
				4,
				new Array("<% write(leftMenud44); %>",'Group.asp',Group),
				new Array(SmartName,'SmartQos.asp',SmartQos || GameBoost),
				new Array("<% write(leftMenud78); %>",'P2PLimit.asp',P2PLimit),
				new Array("<% write(leftMenud49); %>",'ConnectionLimit.asp',ConnectionLimit)
			);
	arraySecondMenu[i++] = new Array(//'防火墙'的二级菜单
				6,
				new Array("<% write(leftMenud46); %>",'FwBase.asp',FwBase),
				new Array("<% write(leftMenud47); %>",'FireWall.asp',FireWall),
				new Array("<% write(leftMenud80); %>",'ParentsControl.asp',ParentsControl),
				new Array("<% write(leftMenud48); %>",'DnsFilter.asp',DnsFilter),
				new Array("<% write(com_adver)  ; %>",'advideo.asp',advideo),
				new Array("<% write(leftMenud79); %>",'MacFilter.asp',MacFilter)
			);
	arraySecondMenu[i++] = new Array(//'VPN'的二级菜单		
				3,
				new Array("<% write(leftMenud50); %>",'PPTPClientList.asp',PPTPClientList),
				new Array("<% write(leftMenud51); %>",'L2TPList.asp',L2TPList),
				new Array("<% write(leftMenud66); %>",'IPSec.asp',IPSEC)
			);
	arraySecondMenu[i++] = new Array(//'联动管理'的二级菜单
				3,//二级菜单个数
				new Array("<% write(leftMenud52); %>",'switch/discover.asp',discover),
				new Array('<% write(leftMenud53); %>','switch/linkageConf.asp',linkageConf),
				new Array("<% write(leftMenud54); %>",'switch/topoy.asp',topoy)
			);
	arraySecondMenu[i++] = new Array(//'系统管理'的二级菜单
				10,
				new Array("<% write(leftMenud55); %>",'User.asp',User),
				new Array("<% write(leftMenud56); %>",'langChange.asp',langChange),
				new Array("<% write(leftMenud57); %>",'SysClock.asp',SysClock),
				new Array("<% write(leftMenud58); %>",'DeviceConfig.asp',DeviceConfig),
				new Array("<% write(leftMenud59); %>",'SoftwareUpdate.asp',SoftwareUpdate),
				new Array("<% write(leftMenud60); %>",'Remote.asp',Remote),
				new Array("<% write(leftMenud61); %>",'TaskScheduler.asp',TaskScheduler),
				new Array("<% write(leftMenud69); %>",'DeviceLoginControl.asp',DeviceLoginControl),
				new Array("<% write(leftMenud71);%>",'License.asp',License),
				new Array("<% write(pingCheck); %>",'diagnose.html',PingCheck)
			);
	arraySecondMenu[i++] = new Array(//'系统状态'的二级菜单
				6,
				new Array("<% write(leftMenud62); %>",'RunStatus.asp',RunStatus),
				new Array("<% write(leftMenud63); %>",'FluxStat.asp',FluxStat),
				new Array("<% write(leftMenud64); %>",'SysInfo.asp',SysInfo),
				new Array("<% write(leftMenud77); %>",'SysLogInfo.asp',SysLogInfo),
				new Array("<% write(leftMenud72); %>",'fitApClts.asp',fitApClts),
				new Array("<% write(leftMenud70); %>",'fitApSysInfo.asp',fitApSysInfo)
			);
	arraySecondMenu[i++] = new Array(//'售后服务'的二级菜单
				1
			);
	
	
function mOvr(src) {
    src.bgColor = "#008893";
    src.style.color = '#ffffff'; 
}
function mOut(src) {
    src.style.cursor = 'default'; 
    src.bgColor = "";
    src.style.color = '#000000';
}
function markActive(i,src){
    var tmp = document.all["m"+i].style.display;
    for(var j=1;j<=arrayMainMenu[0];j++)
    {
    	if(arraySecondMenu[i-1].length<=1){
	    parent.main.location=arrayMainMenu[i][1];
    	}
    	else{
            if(i==j){
            	document.all["m"+i].style.display = tmp=="none"?"":"none";
            	document.all["tag"+i].src= tmp=="none"?"<% write(com_imgtag1); %>":"<% write(com_imgtag); %>";
            }
    	}	
    }
    for(k=0;k<mm;k++){
    	document.all["menud"+k].style.color ="#000000";
    }
}
function showstart(){
    var disflag=document.all["starttbl1"].style.display;
    document.all["starttbl1"].style.display = disflag=="none"?"":"none";
}
function openMenu(i,j,l){
    parent.main.location=arraySecondMenu[i-1][j][1];
}
function openStartMenu(k,l){
    parent.main.location=startMenu[k][1];
}
</script>
</head>
<body bgcolor="#FFFFFF" link="#">
<div style="background:url(<% write(com_imgleft); %>); background-repeat:no-repeat">
<table  width="175" align="center">
<tr><td valign="top" width="175">
<table height="620" align="center" width="100%"><tr valign="bottom"><td height="25" width="175" align="center" valign="bottom"><img src="<% write(com_imgstart); %>" onClick="showstart()" id="top_gif"></td></tr>
    <tr valign="top">
      <td bgcolor="#84c6ce"><table width="100%">
	  <tr><td>
			<table id="starttbl1" style="display:" width="100%" bgcolor="#B1D5D1" align="center">
			<script language="javascript">
			var mm=0;
				for(k=1;k<=startMenu[0];k++){
					if(startMenu[k][2]){
					document.write('<tr><td width=175 id=menud'+mm+' height="15" onMouseOver="mOvr(this);"onMouseOut="mOut(this)" style="color:#000000"><img src="<% write(com_imgspace); %>" width="40" height="15" ><a style="cursor:pointer" onclick="openStartMenu('+k+','+mm+')">'+startMenu[k][0]+'</a></td></tr>');
						mm++;			
					}
				}
			</script>
              </table></td></tr>
          <tr>
            <td  valign="top">
			
			<script language="javascript">
			for(i=1;i<arrayMainMenu[0];i++){
				if(arrayMainMenu[i][3]){
					if(arraySecondMenu[i-1][0]>1){
						document.write(' <table><tr><td ><img src="<% write(com_imgspace); %>" width="20" height="20"><SPAN class=navi_title_span_click onMouseOver=this.style.color="#0060BF"  onMouseOut=this.style.color="#ffffff" onclick="markActive('+i+',this)"><IMG height=10 src='+(arrayMainMenu[i][2]==1?"<% write(com_imgtag1); %>":"<% write(com_imgtag); %>")+'  border=0 id=tag'+i+' width="10">&nbsp;'+arrayMainMenu[i][0]+'</SPAN></td></tr></table>');
					}
					else{
						document.write(' <table><tr><td ><img src="<% write(com_imgspace); %>" width="20" height="20"><SPAN class=navi_title_span_click onMouseOver=this.style.color="#0060BF"  onMouseOut=this.style.color="#ffffff" onclick="markActive('+i+')"><IMG height=10 src='+(arrayMainMenu[i][2]==1?"<% write(com_imgspace); %>":"<% write(com_imgspace); %>")+'  border=0 id=tag'+i+' width="10">&nbsp;'+arrayMainMenu[i][0]+'</SPAN></td></tr></table>');
					}
					document.write('<table id=m'+i+' style=display:'+(arrayMainMenu[i][2]==1?"":"none")+'>');	
					for(j=0;j<=arraySecondMenu[i-1][0];j++){
					    if((needlicense != 1)&&(arraySecondMenu[i-1][j][1] == "License.asp")){
						continue;
					    }
						if(arraySecondMenu[i-1][j][2]){
							document.write('<tr>');
							document.write('<td height="15" id=menud'+mm+' width=175 onMouseOver="mOvr(this)" onMouseOut="mOut(this)"   style="color:#000000"><img src="<% write(com_imgspace); %>" width="30" height="15">&nbsp;&nbsp;&nbsp;<a  style="cursor:pointer" onclick="openMenu('+i+','+j+','+mm+')">'+arraySecondMenu[i-1][j][0]+'</td>');
							document.write('</tr>');
							mm++;
						}		
					}
					document.write('</table>');		
				}		
			}
			
			</script>
              <TABLE>
                <TBODY>
                  
                </TBODY>
              </TABLE></td>
          </tr>
        </table></td>
    </tr>
  </table>
</td></tr>
</table>
  
</div>
</body>
</html>
