<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("GlobalBase_edit");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
var homeuser=0;
<%coefficient();%>
<%aspOutIpGrpList();%>
var AddGroupName=new Array();var userGroupNames=new Array();
		var QQ="";var WLMessenger="";var AliIM="";var Fetion="";var WebQQ=""; var yahootong="";var BitTorrent="";var Thunder="";var QQLive="";var PPStream=""; var KuGou="";
		var PPTV="";var Qvod="";var QQGame="";var bnb="";var zhengtu="";var xyq="";var wanmei="";  var au9you="";var HaoFang="";var WOW="";var AION=""; 
		var PopKart="";var GAMEURL="";var STOCKURL=""; var STOCKSW="";var Advertise="";var SHOPURL=""; var SOCIALURL="";var HttpProxy="";var Socks="";var FileType="";var WebPost=""; var YouKu="";	
		var TuDou="";var AiQi="";var SouHuVideo="";var KanKan="";var Ku6="";var Net56="";var SinaVideo="";var TencentVideo="";var CNTV="";
		var iFengVideo="";var BaoFeng="";var Joy="";var PPS="";var FunShion="";var QQCF="";var SDODN="";
		var THE9SXZ="";	var SDOMIR="";var TL3="";var CSOL="";var SDOMXD="";var QN163="";var WD="";var M3GUO="";var WOT="";var TaoBao="";var TMall="";
		var buy360="";var DangDang="";var Amazon="";var PaiPai="";var Alibaba="";var M18="";var hao123="";var Vancl="";var VipShop="";var Fclub="";var Xiu="";
		var SSQY="";var MoonBasa="";var YinTai="";var Vjia="";var MasaMaso="";var pb89="";var SuNing="";var Gome="";var buy51="";var Coo8="";var NewEgg="";
		var LuSen="";var okBuy="";var LeTao="";var YouGou="";var scn="";var mBaoBao="";var Lafaso="";var TianTian="";var Sephora="";var DHC="";var jxdyf="";
		var straw="";var hiTao="";var guopi="";var caomeipai="";var redBaby="";var muyingzhijia="";var yihaodian="";var womai="";var lashou="";var nuomi="";
		var meituan="";	var gaopeng="";var tuan55="";var QQZone="";var RenRen="";var PengYou="";var weiboSina="";var kaixin001="";var com51="";var douban="";
		var taomi="";var weiboTencent="";var jiayuan="";var jianghu="";var baisohu="";var zhenai="";var baihe="";var ipart="";var DuoWan="";var BaiduGame="";
		var Wan37="";var G265="";var eeyy="";var kunlun="";var Game4399="";var Gamepps="";var Game7k7k="";var Wan05="";var MailSina="";var Mail163="";var Mail126="";
		var MailQQ="";var Mop="";var TianYa="";var LiBa="";var Kds="";var TieXue="";var iFengbbs="";var XiCi="";var BaiduPlayer="";var facebook="";var youtobe="";var twitter="";
var errorstr="";
var dRate="nolimit";
var dCeil="nolimit";
var uRate="nolimit";  
var uCeil="nolimit";
<%
var edit_Name=""; 
edit_Name=Form("id");
if ( edit_Name!="") 
{    
  	aspOutOneGroupBaseConfig(edit_Name);
	
}
%>




var edit_Names = "<% write(Form("id"));%>";



/*

   var dRate="nolimit";
   var dCeil="nolimit";
   var uRate="nolimit";
   var uCeil="nolimit";
   var edit_Names="";
   var GroupNames="";
   var FromIPs="";
   var EndIPs="";
   var DenyQQs="";
   var DenyMSNs="";
   var DenyBTs="";
   var DenyThunders="";

   var group_days="1111111";
   var group_timestarts="ALL";
   var group_timestops="";
 */
<!--
var imflag=true;

/*function yahooshow()
{
    var f=Group_Config;
	f.yahootong.style.display=(yahoo_mark==1?"":"none");
	document.getElementById("yahootongname").style.display=(yahoo_mark==1?"":"none");
	f.facebook.style.display=(yahoo_mark==1?"":"none");
	document.getElementById("facebookname").style.display=(yahoo_mark==1?"":"none");
	f.youtobe.style.display=(yahoo_mark==1?"":"none");
	document.getElementById("youtobename").style.display=(yahoo_mark==1?"":"none");
	f.twitter.style.display=(yahoo_mark==1?"":"none");
	document.getElementById("twittername").style.display=(yahoo_mark==1?"":"none");
}
*/

function IMshow()
{
	if(imflag)
	{
		tableIM.style.display="";
		Group_Config.imButton.value="-";
	}
	else
	{
		tableIM.style.display="none";
		Group_Config.imButton.value="+";
	}
	imflag=!imflag;
}
var p2pflag=true;
function P2Pshow()
{
	if(p2pflag)
	{
		tableP2P.style.display="";
		Group_Config.p2pButton.value="-";
	}
	else
	{
		tableP2P.style.display="none";
		Group_Config.p2pButton.value="+";
	}
	p2pflag=!p2pflag;
}
var stockflag=true;
function STOCKshow()
{
	if(stockflag)
	{
		tableSTOCK.style.display="";
		Group_Config.stockButton.value="-";
	}
	else
	{
		tableSTOCK.style.display="none";
		Group_Config.stockButton.value="+";
	}
	stockflag=!stockflag;
}
var nvflag=true;
function NVshow()
{
	if(nvflag)
	{
		tableNV.style.display="";
		Group_Config.nvButton.value="-";
	}
	else
	{
		tableNV.style.display="none";
		Group_Config.nvButton.value="+";
	}
	nvflag=!nvflag;
}
var gameflag=true;
function Gameshow()
{
	if(gameflag)
	{
		tablegame.style.display="";
		Group_Config.GameButton.value="-";
	}
	else
	{
		tablegame.style.display="none";
		Group_Config.GameButton.value="+";
	}
	gameflag=!gameflag;
}
var nsflag=true;
function NSshow()
{
	if(nsflag)
	{
		tableNS.style.display="";
		Group_Config.nsButton.value="-";
	}
	else
	{
		tableNS.style.display="none";
		Group_Config.nsButton.value="+";
	}
	nsflag=!nsflag;
}
var snsflag=true;
function SNSshow()
{
	if(snsflag)
	{
		tableSNS.style.display="";
		Group_Config.snsButton.value="-";
	}
	else
	{
		tableSNS.style.display="none";
		Group_Config.snsButton.value="+";
	}
	snsflag=!snsflag;
}
var webgameflag=true;
function WebGameshow()
{
	if(webgameflag)
	{
		tableWebGame.style.display="";
		Group_Config.WebGameButton.value="-";
	}
	else
	{
		tableWebGame.style.display="none";
		Group_Config.WebGameButton.value="+";
	}
	webgameflag=!webgameflag;
}
var mailflag=true;
function Mailshow()
{
	if(mailflag)
	{
		tableMail.style.display="";
		Group_Config.mailButton.value="-";
	}
	else
	{
		tableMail.style.display="none";
		Group_Config.mailButton.value="+";
	}
	mailflag=!mailflag;
}
var bbsflag=true;
function BBSshow()
{
	if(bbsflag)
	{
		tableBBS.style.display="";
		Group_Config.bbsButton.value="-";
	}
	else
	{
		tableBBS.style.display="none";
		Group_Config.bbsButton.value="+";
	}
	bbsflag=!bbsflag;
}
var otherflag=true;
function othershow()
{
	if(otherflag)
	{
		tableother.style.display="";
		Group_Config.otherButton.value="-";
	}
	else
	{
		tableother.style.display="none";
		Group_Config.otherButton.value="+";
	}
	otherflag=!otherflag;
}

var ecflag=true;
function ECshow()
{
    if(ecflag)
    {
	EC.style.display="";
	Group_Config.ecButton.value="-";
    }
    else
    {
	EC.style.display="none";
	Group_Config.ecButton.value="+";
    }
    ecflag=!ecflag;
}
var tcflag=true;
function TCshow()
{
    if(tcflag)
    {
	TC.style.display="";
	Group_Config.tcButton.value="-";
    }
    else
    {
	TC.style.display="none";
	Group_Config.tcButton.value="+";
    }
    tcflag=!tcflag;
}
var daflag=true;
function DAshow()
{
    if(daflag)
    {
	DA.style.display="";
	Group_Config.daButton.value="-";
    }
    else
    {
	DA.style.display="none";
	Group_Config.daButton.value="+";
    }
    daflag=!daflag;
}
var sbflag=true;
function SBshow()
{
    if(sbflag)
    {
	SB.style.display="";
	Group_Config.sbButton.value="-";
    }
    else
    {
	SB.style.display="none";
	Group_Config.sbButton.value="+";
    }
    sbflag=!sbflag;
}
var bhflag=true;
function BHshow()
{
    if(bhflag)
    {
	BH.style.display="";
	Group_Config.bhButton.value="-";
    }
    else
    {
	BH.style.display="none";
	Group_Config.bhButton.value="+";
    }
    bhflag=!bhflag;
}
var btflag=true;
function BTshow()
{
    if(btflag)
    {
	BT.style.display="";
	Group_Config.btButton.value="-";
    }
    else
    {
	BT.style.display="none";
	Group_Config.btButton.value="+";
    }
    btflag=!btflag;
}
var duflag=true;
function DUshow()
{
    if(duflag)
    {
	DU.style.display="";
	Group_Config.duButton.value="-";
    }
    else
    {
	DU.style.display="none";
	Group_Config.duButton.value="+";
    }
    duflag=!duflag;
}
var gpflag=true;
function GPshow()
{
    if(gpflag)
    {
	GP.style.display="";
	Group_Config.gpButton.value="-";
    }
    else
    {
	GP.style.display="none";
	Group_Config.gpButton.value="+";
    }
    gpflag=!gpflag;
}

function init(){
    var f=Group_Config;
    seldate(f,true);
    seltime(f,"All");
    if (homeuser == 1)
    {
	document.getElementById("homeUser").style.display="none";
    }
    if(edit_Names!=""&&edit_Names!=null){
		group_days=(group_days==""?"1111111":group_days);
		group_timestarts=(group_timestarts==""?"All":group_timestarts); 
		f.GroupName.value=GroupNames;
		f.GroupNameold.value=GroupNames;
    if(sourceIPs == 0)
    {
	f.FromIP.value=FromIPs;
	f.EndIP.value=EndIPs;
	f.sourceIP[0].checked = true;
	selObject(f,"ipRange");
    }
    else
    {
	f.GroupNameType.value = ipGrpNames;
	f.sourceIP[1].checked = true;
	selObject(f,"groupSel");
    }
		//f.pdbPolicy.value = pdbPolicys;
	    f.QQ.checked=(QQ.toLowerCase()=="on"?true:false);
		f.WLMessenger.checked=(WLMessenger.toLowerCase()=="on"?true:false);
		f.AliIM.checked=(AliIM.toLowerCase()=="on"?true:false);
		f.Fetion.checked=(Fetion.toLowerCase()=="on"?true:false); 	 
		f.WebQQ.checked=(WebQQ.toLowerCase()=="on"?true:false); 
 	//	f.yahootong.checked=(yahootong.toLowerCase()=="on"?true:false);	 
		f.Fetion.checked=(Fetion.toLowerCase()=="on"?true:false);	 
		f.BitTorrent.checked=(BitTorrent.toLowerCase()=="on"?true:false);
		f.Thunder.checked=(Thunder.toLowerCase()=="on"?true:false);
		f.QQLive.checked=(QQLive.toLowerCase()=="on"?true:false);
		f.PPStream.checked=(PPStream.toLowerCase()=="on"?true:false);  	   
		f.KuGou.checked=(KuGou.toLowerCase()=="on"?true:false);
		f.PPTV.checked=(PPTV.toLowerCase()=="on"?true:false);
		f.Qvod.checked=(Qvod.toLowerCase()=="on"?true:false);
		f.BaiduPlayer.checked=(BaiduPlayer.toLowerCase()=="on"?true:false);
		f.STOCKURL.checked=(STOCKURL.toLowerCase()=="on"?true:false);
		f.STOCKSW.checked=(STOCKSW.toLowerCase()=="on"?true:false);
		f.YouKu.checked=(YouKu.toLowerCase()=="on"?true:false);
		
		f.TuDou.checked=(TuDou.toLowerCase()=="on"?true:false);
		f.AiQi.checked=(AiQi.toLowerCase()=="on"?true:false);
		f.SouHuVideo.checked=(SouHuVideo.toLowerCase()=="on"?true:false);
		f.KanKan.checked=(KanKan.toLowerCase()=="on"?true:false);
		f.Ku6.checked=(Ku6.toLowerCase()=="on"?true:false);
		f.Net56.checked=(Net56.toLowerCase()=="on"?true:false);
		f.SinaVideo.checked=(SinaVideo.toLowerCase()=="on"?true:false);
		f.TencentVideo.checked=(TencentVideo.toLowerCase()=="on"?true:false);
		f.CNTV.checked=(CNTV.toLowerCase()=="on"?true:false);
		f.iFengVideo.checked=(iFengVideo.toLowerCase()=="on"?true:false);
		f.BaoFeng.checked=(BaoFeng.toLowerCase()=="on"?true:false);
		f.Joy.checked=(Joy.toLowerCase()=="on"?true:false);
		f.PPS.checked=(PPS.toLowerCase()=="on"?true:false);
		f.FunShion.checked=(FunShion.toLowerCase()=="on"?true:false);
		
		f.GAMEURL.checked=(GAMEURL.toLowerCase()=="on"?true:false);
		f.QQGame.checked=(QQGame.toLowerCase()=="on"?true:false);  	   
		f.bnb.checked=(bnb.toLowerCase()=="on"?true:false);
		f.zhengtu.checked=(zhengtu.toLowerCase()=="on"?true:false);
		f.xyq.checked=(xyq.toLowerCase()=="on"?true:false);
		f.wanmei.checked=(wanmei.toLowerCase()=="on"?true:false);  	   
		f.au9you.checked=(au9you.toLowerCase()=="on"?true:false);
		f.HaoFang.checked=(HaoFang.toLowerCase()=="on"?true:false);
		f.WOW.checked=(WOW.toLowerCase()=="on"?true:false);
		f.AION.checked=(AION.toLowerCase()=="on"?true:false);  	   
		f.PopKart.checked=(PopKart.toLowerCase()=="on"?true:false);
		
		f.QQCF.checked=(QQCF.toLowerCase()=="on"?true:false);
		f.SDODN.checked=(SDODN.toLowerCase()=="on"?true:false);
		f.THE9SXZ.checked=(THE9SXZ.toLowerCase()=="on"?true:false);
		f.SDOMIR.checked=(SDOMIR.toLowerCase()=="on"?true:false);
		f.TL3.checked=(TL3.toLowerCase()=="on"?true:false);
		f.CSOL.checked=(CSOL.toLowerCase()=="on"?true:false);
		f.SDOMXD.checked=(SDOMXD.toLowerCase()=="on"?true:false);
		f.QN163.checked=(QN163.toLowerCase()=="on"?true:false);
		f.WD.checked=(WD.toLowerCase()=="on"?true:false);
		f.M3GUO.checked=(M3GUO.toLowerCase()=="on"?true:false);
		f.WOT.checked=(WOT.toLowerCase()=="on"?true:false);
		f.TaoBao.checked=(TaoBao.toLowerCase()=="on"?true:false);
		f.TMall.checked=(TMall.toLowerCase()=="on"?true:false);
		f.buy360.checked=(buy360.toLowerCase()=="on"?true:false);
		f.DangDang.checked=(DangDang.toLowerCase()=="on"?true:false);
		f.Amazon.checked=(Amazon.toLowerCase()=="on"?true:false);
		f.PaiPai.checked=(PaiPai.toLowerCase()=="on"?true:false);
		f.Alibaba.checked=(Alibaba.toLowerCase()=="on"?true:false);
		f.M18.checked=(M18.toLowerCase()=="on"?true:false);
		f.hao123.checked=(hao123.toLowerCase()=="on"?true:false);
		f.Vancl.checked=(Vancl.toLowerCase()=="on"?true:false);
		f.VipShop.checked=(VipShop.toLowerCase()=="on"?true:false);
		f.Fclub.checked=(Fclub.toLowerCase()=="on"?true:false);
		f.Xiu.checked=(Xiu.toLowerCase()=="on"?true:false);
		f.SSQY.checked=(SSQY.toLowerCase()=="on"?true:false);
		f.MoonBasa.checked=(MoonBasa.toLowerCase()=="on"?true:false);
		f.YinTai.checked=(YinTai.toLowerCase()=="on"?true:false);
		f.Vjia.checked=(Vjia.toLowerCase()=="on"?true:false);
		f.MasaMaso.checked=(MasaMaso.toLowerCase()=="on"?true:false);
		f.pb89.checked=(pb89.toLowerCase()=="on"?true:false);
		f.SuNing.checked=(SuNing.toLowerCase()=="on"?true:false);
		f.Gome.checked=(Gome.toLowerCase()=="on"?true:false);
		f.buy51.checked=(buy51.toLowerCase()=="on"?true:false);
		f.Coo8.checked=(Coo8.toLowerCase()=="on"?true:false);
		f.NewEgg.checked=(NewEgg.toLowerCase()=="on"?true:false);
		f.LuSen.checked=(LuSen.toLowerCase()=="on"?true:false);
		f.okBuy.checked=(okBuy.toLowerCase()=="on"?true:false);
		f.LeTao.checked=(LeTao.toLowerCase()=="on"?true:false);
		f.YouGou.checked=(YouGou.toLowerCase()=="on"?true:false);
		f.scn.checked=(scn.toLowerCase()=="on"?true:false);
		f.mBaoBao.checked=(mBaoBao.toLowerCase()=="on"?true:false);
		f.Lafaso.checked=(Lafaso.toLowerCase()=="on"?true:false);
		f.TianTian.checked=(TianTian.toLowerCase()=="on"?true:false);
		f.Sephora.checked=(Sephora.toLowerCase()=="on"?true:false);
		f.DHC.checked=(DHC.toLowerCase()=="on"?true:false);
		f.jxdyf.checked=(jxdyf.toLowerCase()=="on"?true:false);
		f.straw.checked=(straw.toLowerCase()=="on"?true:false);
		f.hiTao.checked=(hiTao.toLowerCase()=="on"?true:false);
		f.guopi.checked=(guopi.toLowerCase()=="on"?true:false);
		f.caomeipai.checked=(caomeipai.toLowerCase()=="on"?true:false);
		f.redBaby.checked=(redBaby.toLowerCase()=="on"?true:false);
		f.muyingzhijia.checked=(muyingzhijia.toLowerCase()=="on"?true:false);
		f.yihaodian.checked=(yihaodian.toLowerCase()=="on"?true:false);
		f.womai.checked=(womai.toLowerCase()=="on"?true:false);
		f.lashou.checked=(lashou.toLowerCase()=="on"?true:false);
		f.nuomi.checked=(nuomi.toLowerCase()=="on"?true:false);
		f.meituan.checked=(meituan.toLowerCase()=="on"?true:false);
		f.gaopeng.checked=(gaopeng.toLowerCase()=="on"?true:false);
		f.tuan55.checked=(tuan55.toLowerCase()=="on"?true:false);
 	//	f.facebook.checked=(facebook.toLowerCase()=="on"?true:false);
        //	f.youtobe.checked=(youtobe.toLowerCase()=="on"?true:false);
 	//	f.twitter.checked=(twitter.toLowerCase()=="on"?true:false);
		f.QQZone.checked=(QQZone.toLowerCase()=="on"?true:false);
		f.RenRen.checked=(RenRen.toLowerCase()=="on"?true:false);
		f.PengYou.checked=(PengYou.toLowerCase()=="on"?true:false);
		f.weiboSina.checked=(weiboSina.toLowerCase()=="on"?true:false);
		f.kaixin001.checked=(kaixin001.toLowerCase()=="on"?true:false);
		f.com51.checked=(com51.toLowerCase()=="on"?true:false);
		f.douban.checked=(douban.toLowerCase()=="on"?true:false);
		f.taomi.checked=(taomi.toLowerCase()=="on"?true:false);
		f.weiboTencent.checked=(weiboTencent.toLowerCase()=="on"?true:false);
		f.jiayuan.checked=(jiayuan.toLowerCase()=="on"?true:false);
		f.jianghu.checked=(jianghu.toLowerCase()=="on"?true:false);
		f.baisohu.checked=(baisohu.toLowerCase()=="on"?true:false);
		f.zhenai.checked=(zhenai.toLowerCase()=="on"?true:false);
		f.baihe.checked=(baihe.toLowerCase()=="on"?true:false);
		f.ipart.checked=(ipart.toLowerCase()=="on"?true:false);
		f.DuoWan.checked=(DuoWan.toLowerCase()=="on"?true:false);
		f.BaiduGame.checked=(BaiduGame.toLowerCase()=="on"?true:false);
		f.Wan37.checked=(Wan37.toLowerCase()=="on"?true:false);
		f.G265.checked=(G265.toLowerCase()=="on"?true:false);
		f.eeyy.checked=(eeyy.toLowerCase()=="on"?true:false);
		f.kunlun.checked=(kunlun.toLowerCase()=="on"?true:false);
		f.Game4399.checked=(Game4399.toLowerCase()=="on"?true:false);
		f.Gamepps.checked=(Gamepps.toLowerCase()=="on"?true:false);
		f.Game7k7k.checked=(Game7k7k.toLowerCase()=="on"?true:false);
		f.Wan05.checked=(Wan05.toLowerCase()=="on"?true:false);
		f.MailSina.checked=(MailSina.toLowerCase()=="on"?true:false);
		f.Mail126.checked=(Mail126.toLowerCase()=="on"?true:false);
		f.Mail163.checked=(Mail163.toLowerCase()=="on"?true:false);
		f.MailQQ.checked=(MailQQ.toLowerCase()=="on"?true:false);
		f.Mop.checked=(Mop.toLowerCase()=="on"?true:false);
		f.TianYa.checked=(TianYa.toLowerCase()=="on"?true:false);
		f.LiBa.checked=(LiBa.toLowerCase()=="on"?true:false);
		f.Kds.checked=(Kds.toLowerCase()=="on"?true:false);
		f.TieXue.checked=(TieXue.toLowerCase()=="on"?true:false);
		f.iFengbbs.checked=(iFengbbs.toLowerCase()=="on"?true:false);
		f.XiCi.checked=(XiCi.toLowerCase()=="on"?true:false);
		f.Advertise.checked=(Advertise.toLowerCase()=="on"?true:false);
		
		//f.SHOPURL.checked=(SHOPURL.toLowerCase()=="on"?true:false);  	   
		//f.SOCIALURL.checked=(SOCIALURL.toLowerCase()=="on"?true:false);
		f.HttpProxy.checked=(HttpProxy.toLowerCase()=="on"?true:false);
		f.Socks.checked=(Socks.toLowerCase()=="on"?true:false);
		f.FileType.checked=(FileType.toLowerCase()=="on"?true:false); 
		f.WebPost.checked=(WebPost.toLowerCase()=="on"?true:false); 
		f.Action.value="modify";
		setTimeValue(f,group_days,group_timestarts,group_timestops);//common.js中的公用函数
    }
	else{
		f.Action.value="add";
		selObject(f,"ipRange");
	}
    isSelIMAll();
    isSelP2PAll();
    isSelSTOCKAll();
    isSelNVAll();
    isSelGameAll();
    isSelECAll();
    isSelTCAll();
    isSelDAAll();
    isSelSBAll();
    isSelBHAll();
    isSelBTAll();
    isSelDUAll();
    isSelGPAll();
    isSelNetShopAll();
    isSelSNSAll();
    isSelWebGameAll();
    isSelMailAll();
    isSelBBSAll();
    isSelOtherAll();
//	yahooshow();
}
function Reset(){
    Group_Config.reset();
    init();
    for(i=1;i<=3;i++){
	document.all["yutd"+i].innerHTML="";
    }
}


function save(){
    var f=Group_Config;
    if(checkData()){
			if(f.Action.value=="add") {
        		f.GroupNameold.value=f.GroupName.value;
			}
	getTimeValue(f);
	if(f.day.value=="0000000"&&!f.everyday.checked){
	    alert("<%write(com_selectDate);%>");
	    f.day.value="";
	    return;
	}
	/*	if(f.MaxDownRatetxt.value=="0")
		f.dRate.value="nolimit";
		else
		f.dRate.value=f.MaxDownRatetxt.value+"k";

	//		if(f.MaxUpRatetxt.value=="0")
	//			f.dCeil.value="nolimit";
	//		else
	//			f.dCeil.value=f.MaxUpRatetxt.value+"k";

	if(f.uMaxDownRatetxt.value=="0")
	f.uRate.value="nolimit";
	else
	f.uRate.value=f.uMaxDownRatetxt.value+"k";

	//		if(f.uMaxUpRatetxt.value=="0")
	//			f.uCeil.value="nolimit";
	//		else
	//			f.uCeil.value=f.uMaxUpRatetxt.value+"k";*/

	/*	f.DenyQQ.value=(f.DenyQQw.checked?"1":"2");
		f.DenyMSN.value=(f.DenyMSNw.checked?"1":"2");
		f.DenyBT.value=(f.DenyBTw.checked?"1":"2");
		f.DenyThunder.value=(f.DenyThunderw.checked?"1":"2");*/
	//alert(f.DenyQQ.value+f.DenyMSN.value+f.DenyBT.value+f.DenyThunder.value);
	
	f.pdbPolicy.value = "";
	
	
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("table1"), "input");
    
	for(var i =0 ;i<chks.length;i++)
	{
	    if((chks[i].checked == true) && (chks[i].value != "all"))
		{
			f.pdbPolicy.value = f.pdbPolicy.value + chks[i].value + ";";	
		}
	}
	 
	 	f.pdbPolicy.value = f.pdbPolicy.value.slice(0, f.pdbPolicy.value.lastIndexOf(":"));
	if(f.pdbPolicy.value  == "")
	{
		alert("<%write(gbe_forbiden);%>");
		return false;
	}
	
	f.action="/goform/formGroupConfig";
	f.submit();
    }
}
function checkData(){
    var f = Group_Config;
    if(!checkName(f.GroupName,12,"<%write(com_group);%>",0)) return false;
	if(f.sourceIP[0].checked==true)
	{
	    if(!checkIP(f.FromIP,"<%write(com_startIP);%>",0)) return false;
	    if(!checkIP(f.EndIP,"<%write(com_endIP);%>",0)) return false;
	    if (compareIP(f.FromIP.value,f.EndIP.value)==1)
	    {
	    alert("<%write(com_cmpIPTip);%>");
	    return false;
	    }
	}
     
    return true;
}
 
function changeSelect(ctrl1, ctrl2)
{

    if (ctrl1.value == "")
    {
	var selectIndex = 0;
    } else {
	var selectIndex = getSelIndexByValue(ctrl2, ctrl1.value);
    }
    if (selectIndex != -1)
    {
	ctrl2.selectedIndex = selectIndex;
    } else {
	ctrl2.selectedIndex = 1;
    }
}

var tagUIL = 1;
var tagIM = 1;
var taggame = 1;
var tagP2P = 1;
var tagSTOCK = 1;
var tagNV = 1;
var tagNS = 1;
var tag_EC = 1;
var tag_TC = 1;
var tag_DA = 1;
var tag_SB = 1;
var tag_BH = 1;
var tag_BT = 1;
var tag_DU = 1;
var tag_GP = 1;
var tagSNS = 1;
var tagWebGame = 1;
var tagMail = 1;
var tagBBS = 1;
var tagother = 1 ;
var tag = 1;
function CheckAll(){
 	 
	     if (homeuser != 1)
	     {
		 var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("table1"), "input");
		     if(tag == 1){
			 for(var i =0 ;i<chks.length;i++)
			 {
			     chks[i].checked = true;
			 }
			 tag = 0;
			     tagother =0;
			     taggame =0;
			     tagIM =0;
			     tagUIL =0;
			     tagP2P =0;
			     tagSTOCK=0;
			     tagNV=0;
			     tagNS=0;
			     tagSNS=0;
			     tagWebGame=0;
			     tagMail=0;
			     tagBBS=0;
		     }else{
			 for(var i =0 ;i<chks.length;i++)
			 {
			     chks[i].checked = false;
			 }
			 tag = 1;
			     tagother =1;
			     taggame =1;
			     tagIM =1;
			     tagUIL =1;
			     tagP2P =1;
			     tagSTOCK = 1;
			     tagNV = 1;
			     tagNS = 1;
			     tagSNS = 1;
			     tagWebGame = 1;
			     tagMail = 1;
			     tagBBS = 1;
		     }
	     }
	    else
	    {
		if (tag == 1)
		{
		    tag = 0;
		    tagIM =1;
		    tagP2P=1;
		    CheckIM();
		    CheckP2P();
		    Group_Config.delchk02.checked=true;
		    Group_Config.delchk03.checked=true;
		}
		else
		{
		    tag = 1;
		    tagIM =0;
		    tagP2P=0;
		    CheckIM();
		    CheckP2P();
		    Group_Config.delchk02.checked=false;
		    Group_Config.delchk03.checked=false;
		}
	    }
}

function CheckBBS()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableBBS"), "input");
	if(tagBBS == 1)
	{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = true;
	}
	tagBBS = !tagBBS;
	}else{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = false;
	}
	tagBBS = !tagBBS;
	}
}
function isSelBBSAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableBBS"), "input");
    var SelAll_BBSflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_BBSflag=false;
	}
    }
    if(SelAll_BBSflag==true)
    {
	Group_Config.delchk19.checked=true;
	tagBBS = !tagBBS;
    }
}

function CheckMail()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableMail"), "input");
	if(tagMail == 1)
	{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = true;
	}
	tagMail = !tagMail;
	}else{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = false;
	}
	tagMail = !tagMail;
	}
}
function isSelMailAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableMail"), "input");
    var SelAll_Mailflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_Mailflag=false;
	}
    }
    if(SelAll_Mailflag==true)
    {
	Group_Config.delchk18.checked=true;
	tagMail = !tagMail;
    }
}

function CheckWebGame()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableWebGame"), "input");
	if(tagWebGame == 1)
	{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = true;
	}
	tagWebGame = !tagWebGame;
	}else{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = false;
	}
	tagWebGame = !tagWebGame;
	}
}
function isSelWebGameAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableWebGame"), "input");
    var SelAll_WebGameflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_WebGameflag=false;
	}
    }
    if(SelAll_WebGameflag==true)
    {
	Group_Config.delchk17.checked=true;
	tagWebGame = !tagWebGame;
    }
}

function CheckSNS()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableSNS"), "input");
	if(tagSNS == 1)
	{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = true;
	}
	tagSNS = !tagSNS;
	}else{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = false;
	}
	tagSNS = !tagSNS;
	}
}
function isSelSNSAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableSNS"), "input");
    var SelAll_SNSflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_SNSflag=false;
	}
    }
    if(SelAll_SNSflag==true)
    {
	Group_Config.delchk16.checked=true;
	tagSNS = !tagSNS;
    }
}

function CheckNetShop()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableNS"), "input");
	if(tagNS == 1)
	{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = true;
	}
	tagNS = !tagNS;
	tag_EC=0;
	tag_TC=0;
	tag_DA=0;
	tag_SB=0;
	tag_BH=0;
	tag_BT=0;
	tag_DU=0;
	tag_GP=0;
	}else{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = false;
	}
	tagNS = !tagNS;
	tag_EC = 1;
	tag_TC = 1;
	tag_DA = 1;
	tag_SB = 1;
	tag_BH = 1;
	tag_BT = 1;
	tag_DU = 1;
	tag_GP = 1;
	}

}
function isSelNetShopAll()
{
    //var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("EC"), "input");
    var SelAll_NSflag=true;
    var tb=Group_Config;
    if((tb.delchk08.checked==false)||(tb.delchk09.checked==false)||(tb.delchk10.checked==false)||(tb.delchk11.checked==false)||(tb.delchk12.checked==false)||(tb.delchk13.checked==false)||(tb.delchk14.checked==false)||(tb.delchk15.checked==false))
    {
	SelAll_NSflag=false;
    }
    if(SelAll_NSflag==true)
    {
	tb.delchk07.checked=true;
	tagNS = !tagNS;
    }
}

function CheckEC()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("EC"), "input");
	if(tag_EC ==1)
	{
		for(var i=0;i<chks.length;i++)
		{
		chks[i].checked = true;
		}
		tag_EC = !tag_EC;
	}else{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = false;
		}
		tag_EC = !tag_EC;
	}
}
function isSelECAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("EC"), "input");
    var SelAll_ECflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_ECflag=false;
	}
    }
    if(SelAll_ECflag==true)
    {
	Group_Config.delchk08.checked=true;
	tag_EC = !tag_EC;
    }
}

function CheckTC()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("TC"), "input");
	if(tag_TC ==1)
	{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = true;
		}
		tag_TC = !tag_TC;
	}else{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = false;
		}
		tag_TC = !tag_TC;
	}
}
function isSelTCAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("TC"), "input");
    var SelAll_TCflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_TCflag=false;
	}
    }
    if(SelAll_TCflag==true)
    {
	Group_Config.delchk09.checked=true;
	tag_TC = !tag_TC;
    }
}
function CheckDA()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("DA"), "input");
	if(tag_DA ==1)
	{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = true;
		}
		tag_DA = !tag_DA;
	}else{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = false;
		}
		tag_DA = !tag_DA;
	}
}
function isSelDAAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("DA"), "input");
    var SelAll_DAflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_DAflag=false;
	}
    }
    if(SelAll_DAflag==true)
    {
	Group_Config.delchk10.checked=true;
	tag_DA = !tag_DA;
    }
}
function CheckBH()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("BH"), "input");
	if(tag_BH ==1)
	{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = true;
		}
		tag_BH = !tag_BH;
	}else{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = false;
		}
		tag_BH = !tag_BH;
	}
}
function isSelBHAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("BH"), "input");
    var SelAll_BHflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_BHflag=false;
	}
    }
    if(SelAll_BHflag==true)
    {
	Group_Config.delchk12.checked=true;
	tag_BH = !tag_BH;
    }
}
function CheckBT()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("BT"), "input");
	if(tag_BT ==1)
	{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = true;
		}
		tag_BT = !tag_BT;
	}else{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = false;
		}
		tag_BT = !tag_BT;
	}
}
function isSelBTAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("BT"), "input");
    var SelAll_BTflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_BTflag=false;
	}
    }
    if(SelAll_BTflag==true)
    {
	Group_Config.delchk13.checked=true;
	tag_BT = !tag_BT;
    }
}
function CheckDU()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("DU"), "input");
	if(tag_DU ==1)
	{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = true;
		}
		tag_DU = !tag_DU;
	}else{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = false;
		}
		tag_DU = !tag_DU;
	}
}
function isSelDUAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("DU"), "input");
    var SelAll_DUflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_DUflag=false;
	}
    }
    if(SelAll_DUflag==true)
    {
	Group_Config.delchk14.checked=true;
	tag_DU = !tag_DU;
    }
}
function CheckGP()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("GP"), "input");
	if(tag_GP ==1)
	{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = true;
		}
		tag_GP = !tag_GP;
	}else{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = false;
		}
		tag_GP = !tag_GP;
	}
}
function isSelGPAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("GP"), "input");
    var SelAll_GPflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_GPflag=false;
	}
    }
    if(SelAll_GPflag==true)
    {
	Group_Config.delchk15.checked=true;
	tag_GP = !tag_GP;
    }
}
function CheckSB()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("SB"), "input");
	if(tag_SB ==1)
	{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = true;
		}
		tag_SB = !tag_SB;
	}else{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = false;
		}
		tag_SB = !tag_SB;
	}
}
function isSelSBAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("SB"), "input");
    var SelAll_SBflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_SBflag=false;
	}
    }
    if(SelAll_SBflag==true)
    {
	Group_Config.delchk11.checked=true;
	tag_SB = !tag_SB;
    }
}
function CheckNV()
{
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableNV"), "input");
	if(tagNV == 1)
	{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = true;
	}
	tagNV = !tagNV;
	}else{
	for(var i=0;i<chks.length;i++)
	{
		chks[i].checked = false;
	}
	tagNV = !tagNV;
	}
}
function isSelNVAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableNV"), "input");
    var SelAll_NVflag=true;
    for(var i=0;i<chks.length-1;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_NVflag=false;
	}
    }
    if(SelAll_NVflag==true)
    {
	Group_Config.delchk05.checked=true;
	tagNV = !tagNV;
    }
}

function CheckSTOCK()
{
	    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableSTOCK"), "input");
		if(tagSTOCK == 1)
		{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = true;
		}
		tagSTOCK = !tagSTOCK;
		}else{
		for(var i=0;i<chks.length;i++)
		{
			chks[i].checked = false;
		}
		tagSTOCK = !tagSTOCK;
		}
}
function isSelSTOCKAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableSTOCK"), "input");
    var SelAll_STOCKflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_STOCKflag=false;
	}
    }
    if(SelAll_STOCKflag==true)
    {
	Group_Config.delchk04.checked=true;
	tagSTOCK = !tagSTOCK;
    }
}

function CheckP2P(){
 	 
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableP2P"), "input");
    if(tagP2P == 1){
	for(var i =0 ;i<chks.length;i++)
	{
	    chks[i].checked = true;
	}
	tagP2P =! tagP2P;
    }else{
	for(var i =0 ;i<chks.length;i++)
	{
	    chks[i].checked = false;
	}
	tagP2P  =!  tagP2P;
    }
}

function isSelP2PAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableP2P"), "input");
    var SelAll_P2Pflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_P2Pflag=false;
	}
    }
    if(SelAll_P2Pflag==true)
    {
	Group_Config.delchk03.checked=true;
	tagP2P  =!  tagP2P;
    }
}

function CheckGame(){
 	 
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tablegame"), "input");
    if(taggame == 1){
	for(var i =0 ;i<chks.length;i++)
	{
	    chks[i].checked = true;
	}
	taggame = !taggame;
    }else{
	for(var i =0 ;i<chks.length;i++)
	{
	    chks[i].checked = false;
	}
	taggame = !taggame;
    }
}
function isSelGameAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tablegame"), "input");
    var SelAll_Gameflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_Gameflag=false;
	}
    }
    if(SelAll_Gameflag==true)
    {
	Group_Config.delchk06.checked=true;
	taggame = !taggame;
    }
}

function CheckIM(){
 	 
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableIM"), "input");
    if(tagIM == 1){
	for(var i =0 ;i<chks.length;i++)
	{
	    chks[i].checked = true;
	}
	tagIM = !tagIM;
    }else{
	for(var i =0 ;i<chks.length;i++)
	{
	    chks[i].checked = false;
	}
	tagIM = !tagIM;
    }
}

function isSelIMAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableIM"), "input");
    var SelAll_IMflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_IMflag=false;
	}
    }
    if(SelAll_IMflag==true)
    {
	Group_Config.delchk02.checked=true;
	tagIM = !tagIM;
    }
}

function CheckUIL(){
 	 
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableUIL"), "input");
    if(tagUIL == 1){
	for(var i =0 ;i<chks.length;i++)
	{
	    chks[i].checked = true;
	}
	tagUIL = !tagUIL;
    }else{
	for(var i =0 ;i<chks.length;i++)
	{
	    chks[i].checked = false;
	}
		tagUIL = !tagUIL;
    }
}

function CheckOther(){
 	 
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableother"), "input");
    if(tagother == 1){
	for(var i =0 ;i<chks.length;i++)
	{
	    chks[i].checked = true;
	}
	tagother = !tagother;
    }else{
	for(var i =0 ;i<chks.length;i++)
	{
	    chks[i].checked = false;
	}
	tagother = !tagother;
    }
}
function isSelOtherAll()
{
    var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("tableother"), "input");
    var SelAll_Otherflag=true;
    for(var i=0;i<chks.length;i++)
    {
	if(chks[i].checked==false)
	{
	    SelAll_Otherflag=false;
	}
    }
    if(SelAll_Otherflag==true)
    {
	Group_Config.delchk20.checked=true;
	tagother = !tagother;
    }
}
function selObject(f,val)
{
	if(val=="ipRange")
	{
		f.FromIP.disabled=false;
		f.EndIP.disabled=false;
		f.GroupNameType.disabled=true;
	}
	if(val=="groupSel")
	{
		f.FromIP.disabled=true;
		f.EndIP.disabled=true;
		f.GroupNameType.disabled=false;
	}
}
//-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" >
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle('<%write(gbe_config);%>');</script>
<table width="100%" border="0" height="600" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr);</script>
<tr valign="top">
<td><form action="/goform/"  name="Group_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center" id="all">
<p>&nbsp;</p>
<tr>
<td align="center" class="style7">
<table border="0" width="600" align='center' >
<tr><td colspan="3" height="20"><input type="hidden" name="Action" ><input type="hidden" name="pdbPolicy" value="pdbPolicy">&nbsp;</td></tr>
<tr>
<td>
<table >
<tr>
<td width="9">&nbsp;</td>
<td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_grpConf);%>：</strong></td>
<td width="10">&nbsp;</td>
</tr>
</table></td></tr>
<tr width="100%">
<td  width="100%">
<table width="593">
<tr>
<td width="3">&nbsp;</td>
<td width="139" height="25" align="right"><%write(com_group);%></td>
<td width="20" height="25" align="right"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
<td width="300">&nbsp;&nbsp;<input name="GroupName" type="text" maxlength="12" size="18" onChange="showfiledtip(re_checkName(this,'<%write(com_group);%>'),yutd1)"></td>
<td width="200" id="yutd1"></td>
</tr>
<input type="hidden" name="GroupNameold" >

<tr>
</tr>
  
<tr>
</tr>
</table>
</td>
</tr>
<tr><td>
<table width="593">
<tr>
	<td width="3">&nbsp;</td>
	<td width="139" height="25" align="right"><%write(gbe_selObj);%></td>
	<td width="20" height="25" align="right"><input type="radio" name="sourceIP" value="ipRange" onClick="selObject(this.form,this.value)" checked="checked"> </td>
	<td width="<%write(gbe_width2);%>" align="right"><%write(com_segment);%>&nbsp;</td>
	<td width="342"><input name="FromIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()" value="0.0.0.0">&nbsp;<%write(com_toIP);%>&nbsp;<input name="EndIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()" value="0.0.0.0"></td>
	<td width="10"  id="yutd2"></td>
</tr>
<tr>
	<td width="3">&nbsp;</td>
	<td width="139" height="25" align="left"></td>
	<td height="25" align="right"><input type="radio" name="sourceIP" value="groupSel" onClick="selObject(this.form,this.value)"></td>
	<td align="right"><%write(com_userGrp);%>&nbsp;</td>
	<td>
	<select name="GroupNameType" id="GroupNameType">
	   <option value=""><%write(com_userAll);%></option>
	   <script language="javascript">
	   for(var i=0;i<addGroupNames.length;i++)
	   {
	       document.write('<option value='+addGroupNames[i]+'>'+addGroupNames[i]+'</option>');
	   }
	   </script>
	</select>
	</td>
	<td id="yutd3"></td>
</tr>
<!--<tr>
	<td width="3">&nbsp;</td>
	<td width="139" height="25" align="left">&nbsp;</td>
	<td height="25" align="right"><input type="radio" name="sourceIP" value="ipRange" onClick=""></td><td align="right">用户组&nbsp;</td>
	<td>
	<select name="userGroupName" id="userGroupName">
	   <option value="AllUser">所有用户</option>
	   <script language="javascript">
	   for(var i=0;i<userGroupNames.length;i++)
	   {
	   		document.write('<option value='+userGroupNames[i]+'>'+userGroupNames[i]+'</option>');
	   }
	   </script>
	</select>
	</td>
	<td id="yutd2"></td>
</tr>-->
</table>
</td></tr>

<tr width="100%">
<td  width="100%">
<table  width="100%">

<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>
</table>

</td></tr>


<tr>
<td>
<table>
<tr>

<td width="60" align="right"><%write(gbe_selAll);%></td> 
<td width="10">&nbsp;</td>
<td width="79" height="25"><input type="checkbox" name="delchk01" onClick="CheckAll()" value="all"></td>


<td width="108">&nbsp;</td>
</tr>
</table></td>	  </tr>


<tr><td  align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0"  id="table1"><tr>
<td>
<table>
<tr>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_im);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk02" onClick="CheckIM()" value="all"></td>

<td width="148"><input type="button" value="+" onClick="IMshow()" name="imButton">&nbsp;</td>
</tr>
</table></td>	  </tr>

<tr width="100%">
<td width="100%">
<table  id="tableIM" style="display:none">
<tr>
<td width="74" height="25" align="right"><input type="checkbox" name="QQ" value="1"></td>
<td width="10">&nbsp;</td>
<td width="114"><%write(gbe_qq);%></td> <td width="8">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="WLMessenger" value="2"></td>
<td width="10">&nbsp;</td>
<td width="108"><%write(gbe_msn);%></td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="AliIM" value="3"></td>
<td width="10">&nbsp;</td>
<td width="161"><%write(gbe_ali);%></td>
</tr>

<tr>
<td align="right" height="25"><input type="checkbox" name="WebQQ" value="4"></td>
<td >&nbsp;</td>
<td><%write(gbe_webqq);%></td> 
<td width="8">&nbsp;</td>
<td align="right" height="25"><input type="checkbox" name="Fetion" value="5"></td>
<td >&nbsp;</td>
<td><%write(gbe_fetion);%></td>
<td width="11">&nbsp;</td>
<td align="right" height="25"><!--<input type="checkbox" name="yahootong" value="143" style="display:none">--></td>
<td >&nbsp;</td>
<td id="yahootongname" style="display:none"><%write(gbe_yahootong);%></td>
<td width="11">&nbsp;</td>
<td align="right" height="25">&nbsp;</td>
<td >&nbsp;</td>
<td>&nbsp;</td>
</tr>
</table>		   </td>
</tr>

<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>


<tr>
<td>
<table>
<tr>
<td width="10">&nbsp;</td><td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_p2p);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk03" onClick="CheckP2P()" value="all"></td>

<td width="150"><input type="button" value="+" onClick="P2Pshow()" name="p2pButton">&nbsp;</td>
</tr>
</table></td>	  </tr>

<tr width="100%">
<td width="100%">
<table  id="tableP2P" style="display:none">
<tr>
<td width="74" height="25" align="right"><input type="checkbox" name="BitTorrent" value="6"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_bitTorrent);%></td> 
<td width="8">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="Thunder" value="7"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_thunder);%></td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="QQLive" value="8"></td>
<td width="10">&nbsp;</td>
<td width="161"><%write(gbe_qqLive);%></td>
</tr>

<tr>
<td align="right" height="25"><input type="checkbox" name="PPStream" value="9"></td>
<td >&nbsp;</td>
<td><%write(gbe_ppsVideo);%></td> 
<td width="8">&nbsp;</td>
<td align="right" height="25"><input type="checkbox" name="KuGou" value="10"></td>
<td >&nbsp;</td>
<td><%write(gbe_kuGou);%></td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="PPTV" value="11"></td>
<td width="10">&nbsp;</td>
<td width="161"><%write(gbe_pptv);%></td>
</tr>
<tr>
<td align="right" height="25"><input type="checkbox" name="Qvod" value="12"></td>
<td >&nbsp;</td>
<td><%write(gbe_qvod);%></td> 
<td width="8">&nbsp;</td>
<td align="right" height="25"><input type="checkbox" name="BaiduPlayer" value="13"></td>
<td >&nbsp;</td>
<td><%write(gbe_baiduPlayer);%></td>
<td width="11">&nbsp;</td>
<td align="right" height="25">&nbsp;</td>
<td >&nbsp;</td>
<td>&nbsp;</td>
</tr>
</table>		   </td>
</tr>

<tr style="display:" id = "homeuser" name="homeuser">
<td colspan="6">
<!--<table id="homeUser">-->
<tr>
<td colspan="7" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>



<tr>
<td>
<table>
<tr>
<td width="10">&nbsp;</td><td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_stock);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk04" onClick="CheckSTOCK()" value="all"></td>

<td width="150"><input type="button" value="+" onClick="STOCKshow()" name="stockButton">&nbsp;</td>
</tr>
</table></td>	  </tr>

<tr width="100%">
<td width="100%">
<table  id="tableSTOCK" style="display:none">
<tr>
<td width="74" height="25" align="right"><input type="checkbox" name="STOCKURL" value="14"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_stockUrl);%>&nbsp;&nbsp;&nbsp;<a href="<% write(com_helppage); %>#Stock" target="_blank"><%write(gbe_look);%></a></td> 
<td width="8">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="STOCKSW" value="168"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>">禁止股票软件&nbsp;&nbsp;&nbsp;<a href="<% write(com_helppage); %>#StockSW" target="_blank"><%write(gbe_look);%></a></td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right"></td>
<td width="10">&nbsp;</td>
<td width="161">&nbsp;&nbsp;</td>
</tr>
<tr>
</tr>
</table></td>
</tr>

<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>



<tr>
<td>
<table>
<tr>
<td width="10">&nbsp;</td><td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_nv);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk05" onClick="CheckNV()" value="all"></td>

<td width="150"><input type="button" value="+" onClick="NVshow()" name="nvButton">&nbsp;</td>
</tr>
</table></td>	  </tr>

<tr width="100%">
<td width="100%">
<table  id="tableNV" style="display:none">
<tr>
<td width="74" height="25" align="right"><input type="checkbox" name="YouKu" value="15"></td>
<td width="10">&nbsp;</td>
<td width="114"><%write(gbe_youku);%>&nbsp;&nbsp;</td> 
<td width="8">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="TuDou" value="16"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_tudou);%>&nbsp;&nbsp;</td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="AiQi" value="17"></td>
<td width="10">&nbsp;</td>
<td width="161"><%write(gbe_aiqi);%>&nbsp;&nbsp;</td>
</tr>
<tr>
<td width="74" height="25" align="right"><input type="checkbox" name="SouHuVideo" value="18"></td>
<td width="10">&nbsp;</td>
<td width="114"><%write(gbe_sohuVideo);%>&nbsp;&nbsp;</td> 
<td width="8">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="KanKan" value="19"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_kankan);%>&nbsp;&nbsp;</td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="Ku6" value="20"></td>
<td width="10">&nbsp;</td>
<td width="161"><%write(gbe_ku6);%>&nbsp;&nbsp;</td>
</tr>
<tr>
<td width="74" height="25" align="right"><input type="checkbox" name="Net56" value="21"></td>
<td width="10">&nbsp;</td>
<td width="114"><%write(gbe_net56);%>&nbsp;&nbsp;</td> 
<td width="8">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="SinaVideo" value="22"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_sinaVideo);%>&nbsp;&nbsp;</td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="TencentVideo" value="23"></td>
<td width="10">&nbsp;</td>
<td width="161"><%write(gbe_qqVideo);%>&nbsp;&nbsp;</td>
</tr>
<tr>
<td width="74" height="25" align="right"><input type="checkbox" name="CNTV" value="24"></td>
<td width="10">&nbsp;</td>
<td width="114"><%write(gbe_cntv);%>&nbsp;&nbsp;</td> 
<td width="8">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="iFengVideo" value="25"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_ifengVideo);%>&nbsp;&nbsp;</td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="BaoFeng" value="26"></td>
<td width="10">&nbsp;</td>
<td width="161"><%write(gbe_baofeng);%>&nbsp;&nbsp;</td>
</tr>
<tr>
<td width="74" height="25" align="right"><input type="checkbox" name="Joy" value="27"></td>
<td width="10">&nbsp;</td>
<td width="114"><%write(gbe_joy);%>&nbsp;&nbsp;</td> 
<td width="8">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="PPS" value="28"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_pps);%>&nbsp;&nbsp;</td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="FunShion" value="29"></td>
<td width="10">&nbsp;</td>
<td width="161"><%write(gbe_funshion);%>&nbsp;&nbsp;</td>
</tr>
<tr id="advertise" style="display:none">
<td width="74" height="25" align="right"><input type="checkbox" name="Advertise" value="143"></td>
<td width="10">&nbsp;</td>
<td width="114"><%write(gbe_adverUrl);%>&nbsp;&nbsp;&nbsp;</td>
<td width="8">&nbsp;</td>
</tr>
<!--<tr>
<td width="74" height="25" align="right"><input type="checkbox" name="youtobe" value="145" style="display:none"></td>
<td width="10">&nbsp;</td>
<td width="114" id="youtobename" style="display:none"><%write(gbe_youtobe);%></td>
<td width="8">&nbsp;</td>
</tr>-->
</table></td>
</tr>

<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>



<tr>
<td>
<table>
<tr>	  <td width="10">&nbsp;</td>
<td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_game);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk06" onClick="CheckGame()" value="all"></td>
<td width="150"><input type="button" value="+" onClick="Gameshow()" name="GameButton">&nbsp;</td>
</tr>
</table></td></tr>

<tr width="100%">
<td width="100%">
<table  id="tablegame" style="display:none" border="0">
<tr>
<td width="<%write(gbe_width4);%>" height="25" align="right"><input type="checkbox" name="GAMEURL" value="30"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_gameUrl);%>&nbsp;&nbsp;&nbsp;<a href="<% write(com_helppage); %>#Game" target="_blank"><%write(gbe_look);%></a></td> 
<!--<td width="0"></td>-->
<td width="20" height="25" align="right"><input type="checkbox" name="QQGame" value="31"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_qqGame);%>&nbsp;&nbsp;&nbsp;<a href="<% write(com_helppage); %>#QQGame" target="_blank"><%write(gbe_look);%></a></td>
<!--<td width="0"></td>-->
<td width="20" height="25" align="right"><input type="checkbox" name="bnb" value="32"></td>
<td width="10">&nbsp;</td>
<td><%write(gbe_bnb);%></td>
</tr>

<tr>
<td align="right" height="25"><input type="checkbox" name="zhengtu" value="33"></td>
<td >&nbsp;</td>
<td><%write(gbe_zhengtu);%></td> 
<!--<td>&nbsp;</td>-->
<td align="right" height="25"><input type="checkbox" name="wanmei" value="34"></td>
<td >&nbsp;</td>
<td><%write(gbe_wanmei);%></td>
<!--<td>&nbsp;</td>-->
<td height="25" align="right"><input type="checkbox" name="xyq" value="35"></td>
<td>&nbsp;</td>
<td><%write(gbe_xyq);%></td>
</tr>

<tr>
<td align="right" height="25"><input type="checkbox" name="au9you" value="36"></td>
<td >&nbsp;</td>
<td><%write(gbe_au9you);%></td> 
<!--<td width="8">&nbsp;</td>-->
<td align="right" height="25"><input type="checkbox" name="HaoFang" value="37"></td>
<td >&nbsp;</td>
<td><%write(gbe_haofang);%></td>
<!--<td>&nbsp;</td>-->
<td height="25" align="right"><input type="checkbox" name="WOW" value="38"></td>
<td>&nbsp;</td>
<td><%write(gbe_wow);%></td>
</tr>

<tr>
<td align="right" height="25"><input type="checkbox" name="AION" value="39"></td>
<td >&nbsp;</td>
<td><%write(gbe_aion);%></td> 
<!--<td>&nbsp;</td>-->
<td align="right" height="25"><input type="checkbox" name="PopKart" value="40"></td>
<td >&nbsp;</td>
<td><%write(gbe_popkart);%></td>
<!--<td>&nbsp;</td>-->
<td height="25" align="right"><input type="checkbox" name="QQCF" value="41"></td>
<td>&nbsp;</td>
<td><%write(gbe_qqcf);%></td>
</tr>

<tr>
<td align="right" height="25"><input type="checkbox" name="SDODN" value="42"></td>
<td >&nbsp;</td>
<td><%write(gbe_sdodn);%></td>
<!--<td>&nbsp;</td>-->
<td height="25" align="right"><input type="checkbox" name="THE9SXZ" value="43"></td>
<td>&nbsp;</td>
<td><%write(gbe_the9sxz);%></td>
<!--<td>&nbsp;</td>-->
<td align="right" height="25"><input type="checkbox" name="SDOMIR" value="44"></td>
<td >&nbsp;</td>
<td><%write(gbe_sdomir);%></td> 
</tr>

<tr>

<td align="right" height="25"><input type="checkbox" name="TL3" value="45"></td>
<td >&nbsp;</td>
<td><%write(gbe_tl3);%></td>
<!--<td>&nbsp;</td>-->
<td height="25" align="right"><input type="checkbox" name="CSOL" value="46"></td>
<td>&nbsp;</td>
<td><%write(gbe_csol);%></td>
<!--td>&nbsp;</td>-->
<td align="right" height="25"><input type="checkbox" name="SDOMXD" value="47"></td>
<td >&nbsp;</td>
<td><%write(gbe_sdomxd);%></td>
</tr>

<tr>
<td height="25" align="right"><input type="checkbox" name="QN163" value="48"></td>
<td>&nbsp;</td>
<td><%write(gbe_qn163);%></td>
<!--<td>&nbsp;</td>-->
<td align="right" height="25"><input type="checkbox" name="WD" value="49"></td>
<td >&nbsp;</td>
<td><%write(gbe_wd);%></td> 
<!--<td>&nbsp;</td>-->
<td align="right" height="25"><input type="checkbox" name="M3GUO" value="50"></td>
<td >&nbsp;</td>
<td><%write(gbe_meng3guo);%></td>
</tr>

<tr>
<td width="" height="25" align="right"><input type="checkbox" name="WOT" value="51"></td>
<td width="">&nbsp;</td>
<td width="" colspan="7"><%write(gbe_wot);%></td>
</tr>
</table></td>
</tr>

<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>


<tr>
<td>
<table>
<tr>	  <td width="10">&nbsp;</td>
<td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_netshop);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk07" onClick="CheckNetShop()" value="all"></td>
<td width="150"><input type="button" value="+" onClick="NSshow()" name="nsButton">&nbsp;</td>
</tr>
</table></td></tr>

<tr width="100%">
<td width="100%"><table  id="tableNS" style="display:none" border="0">
	
    <tr><td colspan="10" height="25">
	<table>
	  <tr><td width="81">&nbsp;</td>
	  <td width="<%write(gbe_width5);%>"><%write(gbe_ec);%></td>
		<td width="<%write(gbe_width6);%>"><%write(gbe_select);%></td>
		<td width="30"><input type="checkbox" name="delchk08" onClick="CheckEC()" value="all"></td>
	    <td width="70" height="25"><input type="button" value="+" onClick="ECshow()" name="ecButton"></td>
	</tr></table>
    </td></tr>

    <tr><td colspan="10">
    <table id="EC" style="display:none" border="0">
    <tr>
        <input type="hidden" name="page2" value="GlobalBase">
        <input type="hidden" name="policy_name2">
        <td width="72" height="25" align="right"><input type="checkbox" name="TaoBao" value="52"></td>
        <td width="12">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_taobao);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="TMall" value="53"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_tmall);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="buy360" value="54"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_buy360);%></td>
    </tr>
	
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="DangDang" value="55"></td>
        <td width="12">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_dangdang);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="Amazon" value="56"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_amazon);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="PaiPai" value="57"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_paipai);%></td>
    </tr>
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="Alibaba" value="58"></td>
        <td width="12">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_alibaba);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="M18" value="59"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_m18);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="hao123" value="60"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_hao123);%></td>
    </tr>
    </table></td></tr>

    <tr><td colspan="10" height="25">
	<table><tr><td width="81">&nbsp;</td><td width="<%write(gbe_width5);%>"><%write(gbe_tc);%></td>
	<td width="<%write(gbe_width6);%>"><%write(gbe_select);%></td>
	<td width="30"><input type="checkbox" name="delchk09" onClick="CheckTC()" value="all"></td>
	<td width="70"><input type="button" value="+" onClick="TCshow()" name="tcButton"></td>
	</tr></table>
    </td></tr>
    
    <tr><td colspan="10">
    <table id="TC" style="display:none">
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="Vancl" value="61"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_vancl);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="VipShop" value="62"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_vipshop);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="Fclub" value="63"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_fclub);%></td>
    </tr>
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="Xiu" value="64"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_xiu);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="SSQY" value="65"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_ssqy);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="MoonBasa" value="66"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_moonbasa);%></td>
    </tr>
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="YinTai" value="67"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_yintai);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="Vjia" value="68"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_vjia);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="MasaMaso" value="69"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_masamaso);%></td>
    </tr>
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="pb89" value="70"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_pb89);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"></td>
        <td width="10">&nbsp;</td>
        <td width="108"></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"></td>
        <td width="10">&nbsp;</td>
        <td width="108"></td>
    </tr>
    </table></td></tr>
	
    <tr><td colspan="10" height="25">
	<table><tr><td width="81">&nbsp;</td><td width="<%write(gbe_width5);%>"><%write(gbe_da);%></td>
	<td width="<%write(gbe_width6);%>"><%write(gbe_select);%></td>
	<td width="30"><input type="checkbox" name="delchk10" onClick="CheckDA()" value="all"></td>
	<td width="70"><input type="button" value="+" onClick="DAshow()" name="daButton"></td>
	</tr></table>
    </td></tr>
	
    <tr><td colspan="10">
    <table id="DA" style="display:none">
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="SuNing" value="71"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_suning);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="Gome" value="72"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_gome);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="buy51" value="73"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_buy51);%></td>
    </tr>
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="Coo8" value="74"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_coo8);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="NewEgg" value="75"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_newegg);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="LuSen" value="76"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_lusen);%></td>
    </tr>
    </table></td></tr>
	
    <tr><td colspan="10" height="25">
	<table><tr><td width="81">&nbsp;</td><td width="<%write(gbe_width5);%>"><%write(gbe_sb);%></td>
	<td width="<%write(gbe_width6);%>"><%write(gbe_select);%></td>
	<td width="30"><input type="checkbox" name="delchk11" onClick="CheckSB()" value="all"></td>
	<td width="70"><input type="button" value="+" onClick="SBshow()" name="sbButton"></td>
	</tr></table>
    </td></tr>
	
    <tr><td colspan="10">
    <table id="SB" style="display:none">
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="okBuy" value="77"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_okbuy);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="LeTao" value="78"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_letao);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="YouGou" value="79"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_yougou);%></td>
    </tr>	
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="scn" value="80"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_scn);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="mBaoBao" value="81"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_mbaobao);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"></td>
        <td width="10">&nbsp;</td>
        <td width="108"></td>
    </tr>
    </table></td></tr>
	
    <tr><td colspan="10" height="25">
	<table><tr><td width="81">&nbsp;</td><td width="<%write(gbe_width5);%>"><%write(gbe_bh);%></td>
	<td width="<%write(gbe_width6);%>"><%write(gbe_select);%></td>
	<td width="30"><input type="checkbox" name="delchk12" onClick="CheckBH()" value="all"></td>
	<td width="70"><input type="button" value="+" onClick="BHshow()" name="bhButton"></td>
	</tr></table>
    </td></tr>
	
    <tr><td colspan="10">
    <table id="BH" style="display:none">
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="Lafaso" value="82"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_lafaso);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="TianTian" value="83"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_tiantian);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="Sephora" value="84"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_sephora);%></td>
    </tr>	
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="DHC" value="85"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_dhc);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="jxdyf" value="86"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_jxdyf);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="straw" value="87"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_straw);%></td>
    </tr>	
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="hiTao" value="88"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_hitao);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="guopi" value="89"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_guopi);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="caomeipai" value="90"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_caomei);%></td>
    </tr>
    </table></td></tr>
	
    <tr><td colspan="10" height="25">
	<table><tr><td width="81">&nbsp;</td><td width="<%write(gbe_width5);%>"><%write(gbe_bt);%></td>
	<td width="<%write(gbe_width6);%>"><%write(gbe_select);%></td>
	<td width="30"><input type="checkbox" name="delchk13" onClick="CheckBT()" value="all"></td>
	<td width="70"><input type="button" value="+" onClick="BTshow()" name="btButton"></td>
	</tr></table>
    </td></tr>
	
    <tr><td colspan="10">
    <table id="BT" style="display:none">
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="redBaby" value="91"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_redbaby);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="muyingzhijia" value="92"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_myzhijia);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"></td>
        <td width="10">&nbsp;</td>
        <td width="108"></td>
    </tr>
    </table></td><tr>
	
    <tr><td colspan="10" height="25">
	<table><tr><td width="81">&nbsp;</td><td width="<%write(gbe_width5);%>"><%write(gbe_du);%></td>
	<td width="<%write(gbe_width6);%>"><%write(gbe_select);%></td>
	<td width="30"><input type="checkbox" name="delchk14" onClick="CheckDU()" value="all"></td>
	<td width="70"><input type="button" value="+" onClick="DUshow()" name="duButton"></td>
	</tr></table>
    </td></tr>
	
    <tr><td colspan="10">
    <table id="DU" style="display:none">
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="yihaodian" value="93"></td>
        <td width="12">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_yihao);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="womai" value="94"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_womai);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"></td>
        <td width="10">&nbsp;</td>
        <td width="108"></td>
    </tr>
    </table></td></tr>
	
    <tr><td colspan="10" height="25">
	<table><tr><td width="81">&nbsp;</td><td width="<%write(gbe_width5);%>"><%write(gbe_gp);%></td>
	<td width="<%write(gbe_width6);%>"><%write(gbe_select);%></td>
	<td width="30"><input type="checkbox" name="delchk15" onClick="CheckGP()" value="all"></td>
	<td width="70"><input type="button" value="+" onClick="GPshow()" name="gpButton"></td>
	</tr></table>
    </td></tr>
	
    <tr><td colspan="10">
    <table id="GP" style="display:none">
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="lashou" value="95"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_lashou);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="nuomi" value="96"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_nuomi);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="meituan" value="97"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_meituan);%></td>
    </tr>		
    <tr>
	    <td width="72" height="25" align="right"><input type="checkbox" name="gaopeng" value="98"></td>
        <td width="12">&nbsp;</td>
        <td width="114"><%write(gbe_gaopeng);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="tuan55" value="99"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_tuan);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"></td>
        <td width="10">&nbsp;</td>
        <td width="108"></td>
    </tr>		
    </table></td></tr>

</table></td>
</tr>

<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>


<tr>
<td>
<table>
<tr>	  <td width="10">&nbsp;</td>
<td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_sns);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk16" onClick="CheckSNS()" value="all"></td>
<td width="150"><input type="button" value="+" onClick="SNSshow()" name="snsButton"></td>
</tr>
</table></td></tr>

<tr width="100%">
<td width="100%"><table  id="tableSNS" style="display:none">
    <tr>
        <input type="hidden" name="page2" value="GlobalBase">
        <input type="hidden" name="policy_name2">
        <td width="74" height="25" align="right"><input type="checkbox" name="QQZone" value="100"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_qqzone);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="RenRen" value="101"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_renren);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="PengYou" value="102"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_pengyou);%></td>
    </tr>
	
    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="weiboSina" value="103"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_weibosina);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="kaixin001" value="104"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_kaixin);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="com51" value="105"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_com);%></td>
    </tr>
	
    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="douban" value="106"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_douban);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="taomi" value="107"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_taomi);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="weiboTencent" value="108"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_weiboqq);%></td>
    </tr>
	
    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="jiayuan" value="109"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_jiayuan);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="jianghu" value="110"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_jianghu);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="baisohu" value="111"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_baisohu);%></td>
    </tr>
	
    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="zhenai" value="112"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_zhenai);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="baihe" value="113"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_baihe);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="ipart" value="114"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_ipart);%></td>
    </tr>
     <tr>
         <td width="74" height="25" align="right"><!--<input type="checkbox" name="facebook" value="144" style="display:none">--></td>
         <td width="10">&nbsp;</td>
         <td width="114" id="facebookname" style="display:none"><%write(gbe_facebook);%></td>
         <td width="8">&nbsp;</td>
         <td width="20" height="25" align="right"><!--<input type="checkbox" name="twitter" value="146" style="display:none">--></td>
         <td width="10">&nbsp;</td>
         <td width="<%write(gbe_width3);%>" colspan="5" id="twittername" style="display:none"><%write(gbe_twitter);%></td>
     </tr>
</table></td>
</tr>

<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>


<tr>
<td>
<table>
<tr>	  <td width="10">&nbsp;</td>
<td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_webgame);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk17" onClick="CheckWebGame()" value="all"></td>
<td width="150"><input type="button" value="+" onClick="WebGameshow()" name="WebGameButton">&nbsp;</td>
</tr>
</table></td></tr>

<tr width="100%">
<td width="100%"><table  id="tableWebGame" style="display:none">
    <tr>
        <input type="hidden" name="page2" value="GlobalBase">
        <input type="hidden" name="policy_name2">
        <td width="74" height="25" align="right"><input type="checkbox" name="DuoWan" value="115"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_duowan);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="BaiduGame" value="116"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_baidugame);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="Wan37" value="117"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_wan37);%></td>
    </tr>
	
    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="G265" value="118"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_jzhg);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="eeyy" value="119"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_eeyy);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="kunlun" value="120"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_kunlun);%></td>
    </tr>
	
    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="Game4399" value="121"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_jzhgame);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="Gamepps" value="122"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_gamepps);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="Game7k7k" value="123"></td>
        <td width="10">&nbsp;</td>
        <td width="<%write(gbe_width3);%>"><%write(gbe_game7k7k);%></td>
    </tr>
	
    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="Wan05" value="124"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_wan05);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"></td>
        <td width="10">&nbsp;</td>
        <td width="108"></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"></td>
        <td width="10">&nbsp;</td>
        <td width="108"></td>
    </tr>
</table></td>
</tr>

<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>


<tr>
<td>
<table>
<tr>	  <td width="10">&nbsp;</td>
<td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_youjian);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk18" onClick="CheckMail()" value="all"></td>
<td width="150"><input type="button" value="+" onClick="Mailshow()" name="mailButton">&nbsp;</td>
</tr>
</table></td></tr>

<tr width="100%">
<td width="100%"><table  id="tableMail" style="display:none">
    <tr>
        <input type="hidden" name="page2" value="GlobalBase">
        <input type="hidden" name="policy_name2">
        <td width="74" height="25" align="right"><input type="checkbox" name="MailSina" value="125"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_sinamail);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="Mail126" value="126"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_mail126);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="Mail163" value="127"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_wangyi);%></td>
    </tr>
    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="MailQQ" value="128"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_qqmail);%></td>
		<td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"></td>
        <td width="10">&nbsp;</td>
        <td width="108"></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"></td>
        <td width="10">&nbsp;</td>
        <td width="108"></td>
    </tr>
</table></td>
</tr>

<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>


<tr>
<td>
<table>
<tr>	  <td width="10">&nbsp;</td>
<td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_bbs);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk19" onClick="CheckBBS()" value="all"></td>
<td width="150"><input type="button" value="+" onClick="BBSshow()" name="bbsButton">&nbsp;</td>
</tr>
</table></td></tr>

<tr width="100%">
<td width="100%"><table  id="tableBBS" style="display:none">
    <tr>
        <input type="hidden" name="page2" value="GlobalBase">
        <input type="hidden" name="policy_name2">
        <td width="74" height="25" align="right"><input type="checkbox" name="Mop" value="129"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_mop);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="TianYa" value="130"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_tianya);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="LiBa" value="131"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_liba);%></td>
    </tr>
    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="Kds" value="132"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_kds);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="TieXue" value="133"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_tiexue);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="iFengbbs" value="134"></td>
        <td width="10">&nbsp;</td>
        <td width="108"><%write(gbe_ifbbs);%></td>
    </tr>
	    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="XiCi" value="135"></td>
        <td width="10">&nbsp;</td>
        <td width="114"><%write(gbe_xici);%></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right">&nbsp;</td>
        <td width="10">&nbsp;</td>
        <td width="108">&nbsp;</td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right">&nbsp;</td>
        <td width="10">&nbsp;</td>
        <td width="108">&nbsp;</td>
    </tr>
</table></td>
</tr>

<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>

<!--
<tr>
<td>
<table>
<tr>	  <td width="10">&nbsp;</td>
<td width="74" height="25" align="left"><strong>网站过滤：</strong></td>
<td width="38" align="right"> 全选</td> 
<td width="5">&nbsp;</td>
<td width="20" height="25" ><input type="checkbox" name="delchk" onClick="CheckUIL()" value="all"></td>
</tr>
</table></td></tr>

<tr width="100%">
<td width="100%"><table  id="tableUIL">
    <tr>
        <input type="hidden" name="page2" value="GlobalBase">
        <input type="hidden" name="policy_name2">
        <td width="74" height="25" align="right"><input type="checkbox" name="GAMEURL" value="GAMEURL"></td>
        <td width="10">&nbsp;</td>
        <td width="114">禁止游戏网站&nbsp;&nbsp;&nbsp;<a href="<% write(com_helppage); %>#Game" target="_blank">查看</a></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="STOCKURL" value="STOCKURL"></td>
        <td width="10">&nbsp;</td>
        <td width="108">禁止证券网站&nbsp;&nbsp;&nbsp;<a href="<% write(com_helppage); %>#Stock" target="_blank">查看</a></td>
        <td width="8">&nbsp;</td>
        <td width="20" height="25" align="right"><input type="checkbox" name="SOCIALURL" value="SOCIALURL"></td>
        <td width="10">&nbsp;</td>
        <td width="108">禁止社交网站&nbsp;&nbsp;&nbsp;<a href="<% write(com_helppage); %>#Social" target="_blank">查看</a></td>
    </tr>
    <tr>
        <td width="74" height="25" align="right"><input type="checkbox" name="SHOPURL" value="SHOPURL"></td>
        <td width="10">&nbsp;</td>
        <td width="114">禁止购物网站&nbsp;&nbsp;&nbsp;<a href="<% write(com_helppage); %>#Shop" target="_blank">查看</a></td>
    </tr>
</table></td>
</tr>


<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
</tr>
</table></td>
</tr>
-->
<tr>
<td>
<table>
<tr>	  <td width="10">&nbsp;</td>
<td width="<%write(gbe_width1);%>" height="25" align="left"><strong><%write(gbe_other);%>：</strong></td>
<td width="53" align="right"> <%write(gbe_select);%></td> 
<td width="10">&nbsp;</td>
<td width="33" height="25" ><input type="checkbox" name="delchk20" onClick="CheckOther()" value="all"></td>
<td width="150"><input type="button" value="+" onClick="othershow()" name="otherButton">&nbsp;</td>
</tr>
</table></td></tr>

<tr width="100%">
<td width="100%">
<table id="tableother" style="display:none">
<tr>
<td width="74" height="25" align="right"><input type="checkbox" name="HttpProxy" value="136"></td>
<td width="10">&nbsp;</td>
<td width="114"><%write(gbe_httpProxy);%></td> 
<td width="8">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="Socks" value="137"></td>
<td width="10">&nbsp;</td>
<td width="<%write(gbe_width3);%>"><%write(gbe_socket);%></td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right"><input type="checkbox" name="FileType" value="138"></td>
<td width="10">&nbsp;</td>
<td width="161"><%write(gbe_fileType);%>&nbsp;&nbsp;&nbsp;<a href="<% write(com_helppage); %>#File" target="_blank"><%write(gbe_look);%></a></td>
</tr>

<tr>
<td align="right" height="25"><input type="checkbox" name="WebPost" value="139"></td>
<td >&nbsp;</td>
<td><%write(gbe_tijiao);%></td> 
<td width="8">&nbsp;</td>
<td align="right" height="25">&nbsp;</td>
<td >&nbsp;</td>
<td>&nbsp;</td>
<td width="11">&nbsp;</td>
<td width="20" height="25" align="right">&nbsp;</td>
<td width="10">&nbsp;</td>
<td width="161">&nbsp;</td>
</tr>
</table>		   </td>
</tr>

</table></td></tr>
<tr>
<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table></td>
</tr>
</table>
</td>
</tr>

<tr><td ><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr><td width="20%" height="28" align="right"><b><%write(com_setTime);%>:</b></td>
<td width="2%">&nbsp;</td>
<td width="78%">&nbsp;</td>
</tr>
<script language="javascript">CreateEftTime();</script>
</table>
</td></tr>




<tr><td colspan="3" height="20">&nbsp;</td></tr>
<tr><td colspan="4">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
<td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>"  onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#globalBase')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('GlobalBase.asp')"></td>
</tr>

</table>
</td></tr>
</table></td>
</tr>
</table>
</form></td>
</tr><script language="javascript">init();</script>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>
