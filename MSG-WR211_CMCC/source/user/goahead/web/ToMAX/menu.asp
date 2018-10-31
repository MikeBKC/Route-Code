<!DOCTYPE HTML>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<%langTran("left");%>
<title></title>

	<link rel="stylesheet" href="css/style.css">
	<link rel="stylesheet" href="css/menu.css">
	<!--[if lt IE 8]><!-->
	<link rel="stylesheet" href="css/ie7/ie7.css">
	<!--<![endif]-->
	<script type="text/javascript" src="js/config.js"></script>
</head>

<body style="background:#6d6362;">
<div class="nav">
		<ul>
		<script type="text/javascript"> 
			if(typeof RunStatus!= "undefined" && RunStatus!=0){
			document.write('<li class="firstMenu first_active runstatus">'+
			'<a><i class="icon-stats">&#xe99b;</i>运行状态</a>'+
			'<ul class="secondMenu" style="display:block;">');
				if(typeof SysInfo!="undefined" && SysInfo!=0){
				document.write('<li><a onclick="openPage(\'SysInfo.asp\',this)" id="sysinfo"><i class="icon-circle">&#xf111;</i>系统信息</a></li>');
				}
				if(typeof InterfaceFlow!="undefined" && InterfaceFlow!=0){
				document.write('<li><a onclick="openPage(\'InterfaceFlow.asp\',this)"><i class="icon-circle">&#xf111;</i>流量统计</a></li>');
				}
				if(typeof UserStats!="undefined" && UserStats!=0){
				document.write('<li><a onclick="openPage(\'UserLinkRate.asp\',this)"><i class="icon-circle">&#xf111;</i>用户统计</a></li>');
				}
				if(typeof firstPage!="undefined" && firstPage!=0){
				document.write('<li><a onclick="openPage(\'firstPage.asp\',this)"><i class="icon-circle">&#xf111;</i>AP与用户统计</a></li>');
				}
			document.write('</ul></li>');
		}
		if(typeof FastConfig!= "undefined" && FastConfig !=0){
		    document.write('<li id="fastconfig" class="firstMenu"><a onclick="openPage(\'FastConfig.asp\',this)"><i class="icon-wizard">&#xe102;</i>快速配置</a></li>');
		}
		if(typeof BasicConf!="undefined" && BasicConf!=0){
		document.write('<li class="firstMenu"><a><i class="icon-setting">&#xe602;</i>基本配置</a><ul class="secondMenu">');
				if(typeof acMode!="undefined" && acMode!=0){
				document.write('<li><a onclick="openPage(\'acMode.asp\',this)"><i class="icon-circle">&#xf111;</i>系统模式</a></li>');
				}
				if(typeof WANConfig!="undefined" && WANConfig!=0){
				document.write('<li><a onclick="openPage(\'WANConfig.asp\',this)"><i class="icon-circle">&#xf111;</i>WAN口配置</a></li>');
				}
				if(typeof DoubleRoad!="undefined" && DoubleRoad!=0){
				document.write('<li><a onclick="openPage(\'DoubleRoad.asp\',this)"><i class="icon-circle">&#xf111;</i>线路配置</a></li>')
				}
				if(typeof WANPort!="undefined" && WANPort!=0){
				document.write('<li><a onclick="openPage(\'portnumber.asp\',this)"><i class="icon-circle">&#xf111;</i>动态WAN口</a></li>');
				}
				if(typeof LANConfig!="undefined" && LANConfig!=0){
				document.write('<li><a onclick="openPage(\'LANConfig.asp\',this)"><i class="icon-circle">&#xf111;</i>LAN口配置</a></li>');
				}
				if(typeof vlanConfig!="undefined" && vlanConfig!=0){
				document.write('<li><a onclick="openPage(\'vlanConfig.asp\',this)"><i class="icon-circle">&#xf111;</i>VLAN接口配置</a></li>');
				}
				if(typeof DhcpServer!="undefined" && DhcpServer != 0 ){
				document.write('<li><a onclick="openPage(\'DhcpServer.asp\',this)"><i class="icon-circle">&#xf111;</i>DHCP服务器</a></li>');
				}
				if(typeof DhcpServerAC!="undefined" && DhcpServerAC != 0 ){
				document.write('<li><a onclick="openPage(\'DhcpPoolForAc.asp\',this)"><i class="icon-circle">&#xf111;</i>DHCP Server</a></li>');
				}
				if(typeof DDNS!="undefined" && DDNS != 0 ){
				document.write('<li><a onclick="openPage(\'DDNS.asp\',this)"><i class="icon-circle">&#xf111;</i>DDNS配置</a></li>');
				}
				if(typeof PortMirror8367M!="undefined" && PortMirror8367M!=0){
				document.write('<li><a onclick="openPage(\'PortMirror8367M.asp\',this)"><i class="icon-circle">&#xf111;</i>端口镜像</a></li>');
				}
				if(typeof PortVlanRdd!="undefined" && PortVlanRdd!=0){
				document.write('<li><a onclick="openPage(\'port_vlan_rdd.asp\',this)"><i class="icon-circle">&#xf111;</i>端口VLAN</a></li>');
				}
			document.write('</ul></li>');
		}
		if(typeof WlanMng!="undefined" && WlanMng!=0){
		document.write('<li class="firstMenu"><a><i class="icon-apmodel">&#xe601;</i>无线控制</a><ul class="secondMenu">');
				if(typeof ApList!="undefined" && ApList!=0){
				document.write('<li><a onclick="openPage(\'apManage.asp\',this)"><i class="icon-circle">&#xf111;</i>AP列表</a></li>');}
				if(typeof ApSZ!="undefined" && ApSZ!=0){
				document.write('<li><a onclick="openPage(\'serviceZone.asp\',this)"><i class="icon-circle">&#xf111;</i>网络列表</a></li>')};
				if(typeof UsersList!="undefined" && UsersList!=0){
				document.write('<li><a onclick="openPage(\'WlanApClientsStatistics.asp\',this)"><i class="icon-circle">&#xf111;</i>用户列表</a></li>')}
				if(typeof ApPosition!="undefined" && ApPosition!=0){
				document.write('<li><a onclick="openPage(\'DrawingPreview.asp\',this)"><i class="icon-circle">&#xf111;</i>AP位置</a></li>')};
				if(typeof ApLoadBalance!="undefined" && ApLoadBalance!=0){
				document.write('<li><a onclick="openPage(\'apLoadBalanceList.asp\',this)"><i class="icon-circle">&#xf111;</i>AP负载均衡</a></li>')};
				if(typeof FitApMacFilter!="undefined" && FitApMacFilter!=0){
				document.write('<li><a onclick="openPage(\'FitApMacFilter.asp\',this)"><i class="icon-circle">&#xf111;</i>无线MAC过滤</a></li>')};
				if(typeof ApTmp!="undefined" && ApTmp!=0){
				document.write('<li><a onclick="openPage(\'apTmp.asp\',this)"><i class="icon-circle">&#xf111;</i>AP配置模板</a></li>')};
				if(typeof ApSoftMng!="undefined" && ApSoftMng!=0){
				document.write('<li><a onclick="openPage(\'apSoftMng.asp\',this)"><i class="icon-circle">&#xf111;</i>AP软件管理</a></li>')};
				if(typeof ApUpdateConf!="undefined" && ApUpdateConf!=0){
				document.write('<li><a onclick="openPage(\'apUpdateConf.asp\',this)"><i class="icon-circle">&#xf111;</i>AP配置文件</a></li>')};
				if(typeof ApLog!="undefined" && ApLog!=0){
				document.write('<li><a onclick="openPage(\'apLog.asp\',this)"><i class="icon-circle">&#xf111;</i>AP日志管理</a></li>')};
			document.write('</ul></li>');
		}
		if(typeof WlsConf!="undefined" && WlsConf!=0){
		document.write('<li class="firstMenu"><a><i class="icon-wifi-24g">&#xe91d;</i>无线配置2.4G</a><ul class="secondMenu">');
				if(typeof WlanBase!="undefined" && WlanBase!=0){
				document.write('<li><a onclick="openPage(\'WlanBase.asp\',this)"><i class="icon-circle">&#xf111;</i>无线基本配置</a></li>')};
				if(typeof WlanSecurity!="undefined" && WlanSecurity!=0){
				document.write('<li><a onclick="openPage(\'WlanSecurity.asp\',this)"><i class="icon-circle">&#xf111;</i>无线安全配置</a></li>')};
				if(typeof WlanMacFilter!="undefined" && WlanMacFilter!=0){
				document.write('<li><a onclick="openPage(\'WlanMacFilter.asp\',this)"><i class="icon-circle">&#xf111;</i>无线MAC过滤</a></li>')};
				if(typeof WlanAdvanced!="undefined" && WlanAdvanced!=0){
				document.write('<li><a onclick="openPage(\'WlanAdvanced.asp\',this)"><i class="icon-circle">&#xf111;</i>无线高级配置</a></li>')};
				if(typeof WlanUserList!="undefined" && WlanUserList!=0){
				document.write('<li><a onclick="openPage(\'WlanHostInfo.asp\',this)"><i class="icon-circle">&#xf111;</i>无线用户列表</a></li>')};
			document.write('</ul></li>');
		}
		if(typeof WlsConf5g!="undefined" && WlsConf5g!=0){

		document.write('<li class="firstMenu"><a><i class="icon-wifi-5g">&#xe91c;</i>无线配置5G</a><ul class="secondMenu">');
				if(typeof WlanBase5g!="undefined" || WlanBase5g!=0){
				document.write('<li><a onclick="openPage(\'WlanBase_5g.asp\',this)"><i class="icon-circle">&#xf111;</i>无线基本配置</a></li>');}
				if(typeof WlanSecurity5g!="undefined" || WlanSecurity5g!=0){
				document.write('<li><a onclick="openPage(\'WlanSecurity_5g.asp\',this)"><i class="icon-circle">&#xf111;</i>无线安全配置</a></li>');}
				if(typeof WlanMacFilter5g!="undefined" || WlanMacFilter5g!=0){
				document.write('<li><a onclick="openPage(\'WlanMacFilter_5g.asp\',this)"><i class="icon-circle">&#xf111;</i>无线MAC过滤</a></li>');}
				if(typeof WlanAdvanced5g!="undefined" || WlanAdvanced5g!=0){
				document.write('<li><a onclick="openPage(\'WlanAdvanced_5g.asp\',this)"><i class="icon-circle">&#xf111;</i>无线高级配置</a></li>');}
				if(typeof WlanUserList5g!="undefined" || WlanUserList5g!=0){
				document.write('<li><a onclick="openPage(\'WlanHostInfo_5g.asp\',this)"><i class="icon-circle">&#xf111;</i>无线用户列表</a></li>');}
			document.write('</ul></li>');
		}
		if(typeof AdvConf!="undefined" && AdvConf!=0){
		document.write('<li class="firstMenu"><a><i class="icon-advanced">&#xea0c;</i>高级配置</a><ul class="secondMenu">');
				if(typeof NatStatic!="undefined" && NatStatic!=0){
				document.write('<li><a onclick="openPage(\'NatStatic.asp\',this)"><i class="icon-circle">&#xf111;</i>NAT配置</a></li>');}
				if(typeof Route!="undefined" && Route!=0){
				document.write('<li><a onclick="openPage(\'Route.asp\',this)"><i class="icon-circle">&#xf111;</i>路由配置</a></li>');}
				if(typeof PolicyRoute!="undefined" && PolicyRoute!=0){
				document.write('<li><a onclick="openPage(\'PolicyRoute.asp\',this)"><i class="icon-circle">&#xf111;</i>策略路由</a></li>');}
				if(typeof AntiNetSnifer!="undefined" && AntiNetSnifer!=0){
				document.write('<li><a onclick="openPage(\'AntiNetSnifer.asp\',this)"><i class="icon-circle">&#xf111;</i>网络尖兵防御</a></li>');}
				if(typeof Hotel!="undefined" && Hotel!=0){
				document.write('<li><a onclick="openPage(\'hotel.asp\',this)"><i class="icon-circle">&#xf111;</i>即插即用</a></li>');}
				if(typeof SyslogConf!="undefined" && SyslogConf!=0){
				document.write('<li><a onclick="openPage(\'Syslog.asp\',this)"><i class="icon-circle">&#xf111;</i>SYSLOG配置</a></li>');}
				if(typeof PortMirror8367M!="undefined" && PortMirror8367M!=0){
				document.write('<li><a onclick="openPage(\'PortMirror8367M.asp\',this)"><i class="icon-circle">&#xf111;</i>端口镜像</a></li>');}
				if(typeof port_vlan_rdd!="undefined" && port_vlan_rdd!=0){
				document.write('<li><a onclick="openPage(\'vlanConfig.asp\',this)"><i class="icon-circle">&#xf111;</i>端口Vlan</a></li>');}
				if(typeof hw_nat!="undefined" && hw_nat!=0){
				document.write('<li><a onclick="openPage(\'HardwareNAT.asp\',this)"><i class="icon-circle">&#xf111;</i>硬件NAT</a></li>');}
				if(typeof UPnP!="undefined" && UPnP!=0){
				document.write('<li><a onclick="openPage(\'UPnP.asp\',this)"><i class="icon-circle">&#xf111;</i>UPnP</a></li>');}
			document.write('</ul></li>');
		}
		if(typeof BehCont !="undefined" && BehCont!=0){
			document.write('<li class="firstMenu"><a><i class="icon-active">&#xe603;</i>行为管控</a><ul class="secondMenu">');
				if(typeof GlobalBase !="undefined" && GlobalBase!=0){
				document.write('<li><a onclick="openPage(\'GlobalBase.asp\',this)"><i class="icon-circle">&#xf111;</i>上网行为管理</a></li>');}
				if(typeof WhiteList !="undefined" && WhiteList!=0){
				document.write('<li><a onclick="openPage(\'QQFilter.asp\',this)"><i class="icon-circle">&#xf111;</i>白名单</a></li>');}
				if(typeof NetAction !="undefined" && NetAction!=0){
				document.write('<li><a onclick="openPage(\'NetActionAudit.asp\',this)"><i class="icon-circle">&#xf111;</i>行为审计</a></li>');}
				if(typeof Notice !="undefined" && Notice!=0){
				document.write('<li><a onclick="openPage(\'Notice.asp\',this)"><i class="icon-circle">&#xf111;</i>电子通告</a></li>');}
				if(typeof PdbUpdata !="undefined" && PdbUpdata!=0){
				document.write('<li><a onclick="openPage(\'PdbUpdata.asp\',this)"><i class="icon-circle">&#xf111;</i>策略库</a></li>');}
			document.write('</ul></li>');
		}
		if(typeof ObjCont!="undefined" && ObjCont!=0){
			document.write('<li class="firstMenu"><a><i class="icon-object">&#xe976;</i>对象管控</a><ul class="secondMenu">');
				if(typeof UserGroup!="undefined" && UserGroup!=0){
				document.write('<li><a onclick="openPage(\'UserGroup.asp\',this)"><i class="icon-circle">&#xf111;</i>用户组</a></li>');}	
				if(typeof TimeRange!="undefined" && TimeRange!=0){
				document.write('<li><a onclick="openPage(\'TimeRange.asp\',this)"><i class="icon-circle">&#xf111;</i>时间段</a></li>');}
				if(typeof FwService!="undefined" && FwService!=0){
				document.write('<li><a onclick="openPage(\'FwService.asp\',this)"><i class="icon-circle">&#xf111;</i>服务组</a></li>');}
		
			document.write('</ul></li>');
		}
		if(typeof UserAuth!="undefined" && UserAuth!=0){
		document.write('<li class="firstMenu"><a><i class="icon-auth">&#xe605;</i>用户认证</a><ul class="secondMenu">');
				if(typeof WebAuth!="undefined" && WebAuth!=0){
				document.write('<li><a onclick="openPage(\'WebAuthGlobal.asp\',this)"><i class="icon-circle">&#xf111;</i>网页认证</a></li>');}
				if(typeof PPPoEGlobal!="undefined" && PPPoEGlobal!=0){
				document.write('<li><a onclick="openPage(\'PPPoEGlobal.asp\',this)"><i class="icon-circle">&#xf111;</i>拨号认证</a></li>');}
				if(typeof IPMac!="undefined" && IPMac!=0){
				document.write('<li><a onclick="openPage(\'IPMac.asp\',this)"><i class="icon-circle">&#xf111;</i>IP/MAC绑定</a></li>');}
				if(typeof MacFilter!="undefined" && MacFilter!=0){
				document.write('<li><a onclick="openPage(\'MacFilter.asp\',this)"><i class="icon-circle">&#xf111;</i>MAC地址过滤</a></li>');}		
			document.write('</ul></li>');
		}
		if(typeof BM !="undefined" && BM!=0){
		document.write('<li class="firstMenu"><a><i class="icon-bandwidth">&#xe992;</i>带宽管控</a><ul class="secondMenu">');
				if(typeof Group!="undefined" && Group!=0){
				document.write('<li><a onclick="openPage(\'Group.asp\',this)"><i class="icon-circle">&#xf111;</i>精细化限速</a></li>');}
				if(typeof SmartQos!="undefined" && SmartQos!=0){
				document.write('<li><a onclick="openPage(\'SmartQos.asp\',this)"><i class="icon-circle">&#xf111;</i>智能弹性限速</a></li>');}
				if(typeof ConnectionLimit!="undefined" && ConnectionLimit!=0){
				document.write('<li><a onclick="openPage(\'ConnectionLimit.asp\',this)"><i class="icon-circle">&#xf111;</i>会话数控制</a></li>');}
			document.write('</ul></li>');
		}
		if(typeof Fw!="undefined" && Fw!=0){
		document.write('<li class="firstMenu"><a><i class="icon-security">&#xe998;</i>安全管控</a><ul class="secondMenu">');
				if(typeof FireWall!="undefined" && FireWall!=0){
				document.write('<li><a onclick="openPage(\'FireWall.asp\',this)"><i class="icon-circle">&#xf111;</i>防火墙</a></li>');}
				if(typeof FwBase!="undefined" && FwBase!=0){
				document.write('<li><a onclick="openPage(\'FwBase.asp\',this)"><i class="icon-circle">&#xf111;</i>防攻击</a></li>');}
				if(typeof DnsFilter!="undefined" && DnsFilter!=0){
				document.write('<li><a onclick="openPage(\'DnsFilter.asp\',this)"><i class="icon-circle">&#xf111;</i>域名过滤</a></li>');}
				document.write('</ul></li>');
		}
		if(typeof VPN!="undefined" && VPN!=0){
		document.write('<li class="firstMenu"><a><i class="icon-vpn">&#xe9ca;</i>VPN管理</a><ul class="secondMenu">');
			if(typeof PPTPClientList!="undefined" && PPTPClientList!=0){
				document.write('<li><a onclick="openPage(\'PPTPClientList.asp\',this)"><i class="icon-circle">&#xf111;</i>PPTP VPN</a></li>');}
				if(typeof L2TPList!="undefined" && L2TPList!=0){
				document.write('<li><a onclick="openPage(\'L2TPList.asp\',this)"><i class="icon-circle">&#xf111;</i>L2TP VPN</a></li>');}
				if(typeof IPSEC!="undefined" && IPSEC!=0){
				document.write('<li><a onclick="openPage(\'IPSec.asp\',this)"><i class="icon-circle">&#xf111;</i>IPSec VPN</a></li>');}				
			document.write('</ul></li>');
		}
		if(typeof NetShare !="undefined" && NetShare!=0){
		document.write('<li class="firstMenu"><a><i class="icon-share">&#xe600;</i>网络共享</a><ul class="secondMenu">');	
				if(typeof NetShareManage !="undefined" && NetShareManage!=0){
				document.write('<li><a onclick="openPage(\'NetShareManage.asp\',this)"><i class="icon-circle">&#xf111;</i>网络共享配置</a></li>');}
				if(typeof FtpServer !="undefined" && FtpServer!=0){
				document.write('<li><a onclick="openPage(\'FtpServer.asp\',this)"><i class="icon-circle">&#xf111;</i>FTP服务器</a></li>');}
				if(typeof ShareAccountList !="undefined" && ShareAccountList!=0){
				document.write('<li><a onclick="openPage(\'ShareAccountList.asp\',this)"><i class="icon-circle">&#xf111;</i>共享账户</a></li>');}		
			document.write('</ul></li>');
		}
		if(typeof SysMng!="undefined" && SysMng!=0){
		document.write('<li class="firstMenu"><a><i class="icon-system">&#xe996;</i>系统管理</a><ul class="secondMenu">');
				if(typeof SysLogInfo!="undefined" && SysLogInfo!=0){
				document.write('<li><a onclick="openPage(\'SysLogInfo.asp\',this)"><i class="icon-circle">&#xf111;</i>系统日志</a></li>');}
				if(typeof TaskScheduler!="undefined" && TaskScheduler!=0){
				document.write('<li><a onclick="openPage(\'TaskScheduler.asp\',this)"><i class="icon-circle">&#xf111;</i>计划任务</a></li>');}
				if(typeof User!="undefined" && User!=0){
				document.write('<li><a onclick="openPage(\'User.asp\',this)"><i class="icon-circle">&#xf111;</i>管理员管理</a></li>');}
				if(typeof SysClock!="undefined" && SysClock!=0){
				document.write('<li><a onclick="openPage(\'SysClock.asp\',this)"><i class="icon-circle">&#xf111;</i>时钟管理</a></li>');}
				if(typeof Remote!="undefined" && Remote!=0){
				document.write('<li><a onclick="openPage(\'Remote.asp\',this)"><i class="icon-circle">&#xf111;</i>远程管理</a></li>');}
				if(typeof DeviceConfig!="undefined" && DeviceConfig!=0){
				document.write('<li><a onclick="openPage(\'DeviceConfig.asp\',this)"><i class="icon-circle">&#xf111;</i>配置管理</a></li>');}
				if(typeof SoftwareUpdate!="undefined" && SoftwareUpdate!=0){
				document.write('<li><a onclick="openPage(\'SoftwareUpdate.asp\',this)"><i class="icon-circle">&#xf111;</i>系统升级</a></li>');}
				if(typeof ReBoot!="undefined" && ReBoot!=0){
				document.write('<li><a onclick="openPage(\'ReBoot.asp\',this)"><i class="icon-circle">&#xf111;</i>重启设备</a></li>');}
				if(typeof PingCheck!="undefined" && PingCheck!=0){
				document.write('<li><a onclick="openPage(\'diagnose.html\',this)"><i class="icon-circle">&#xf111;</i>ping诊断</a></li>');}
			document.write('</ul></li>');
		}
		</script>
	</ul>
</div>
<br><br><br><br>
<div style="width:100%;text-align:center;font-size:12px;color:#fff; ">
	深圳市兴宏旺科技有限公司
</div>
<script type="text/javascript" src="js/jquery.min.js"></script>
<script type="text/javascript">
	$(function(){
		$(".firstMenu > a").click(function(event) {
			var p = $(this).parent('li');
			$(".firstMenu").removeClass('first_active')
			p.addClass('first_active');
			$(this).siblings("ul").toggle();                                 
		    $(this).closest("li").siblings().children("ul").hide();      			
		});
	})
function openSysInfo(){
	$(".icon-circle").css("color","#6a605f");
	$(".secondMenu").hide();
	$(".secondMenu").removeClass('sh');
	var p = $(".runstatus");
	$(".firstMenu").removeClass('first_active')
	p.addClass('first_active');
	p.children('ul').show();
	parent.main.location ="SysInfo.asp";
	//$(".icon-circle").css("color","#6a605f");
//	parent.secondMenu.writeTabs("系统信息");
	 $("#sysinfo").children('i').css("color","#6bb23a");
        $("#sysinfo").css("color","#6bb23a");
       	
}
function openSysInfo1(){
	$(".icon-circle").css("color","#6a605f");
	$(".secondMenu").hide();
	$(".secondMenu").removeClass('sh');
	var p = $(".runstatus");
	$(".firstMenu").removeClass('first_active')
	p.addClass('first_active');
	p.children('ul').show();
//	parent.secondMenu.writeTabs("系统信息");
	 $("#sysinfo").children('i').css("color","#6bb23a");
$("#sysinfo").css("color","#6bb23a");
	
}

function openPage(url,obj){
	parent.main.location=url;
	$(".icon-circle").css("color","#6a605f");
	var t = $(obj).text();
	var i = $(obj).children("i").text();
	t = t.replace(i,"");
//	parent.secondMenu.writeTabs(t);
	$("a").css("color","#fff"); 
	if(t!="快速配置"){
		$(obj).children('i').css("color","#6bb23a");
		   $(obj).css("color","#6bb23a"); 	
}
}
function setFastConfigStyle()
{
    var p = $("#fastconfig");                                    
    $(".firstMenu").removeClass('first_active')                      
	p.addClass('first_active');              
//    parent.secondMenu.writeTabs("快速配置");
    $(".secondMenu").hide();


}
function setFastConfigStyle()
{
    var p = $("#fastconfig");                                    
    $(".firstMenu").removeClass('first_active')                      
	p.addClass('first_active');              
//    parent.secondMenu.writeTabs("快速配置");
    $(".secondMenu").hide();	
}
function hideMenu(){
	$(".nav").hide();
	$("#hide_div").hide();
	$("#show_div").show();
	parent.frameset1.attr("cols","20,*");
}
function showMenu(){
	$(".nav").show();
	$("#hide_div").show();
	$("#show_div").hide();
	parent.frameset1.attr("cols","200,*");
}
</script>
</body>
</html>
