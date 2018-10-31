<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("WebAuth");%>
<% langTran("SysInfo");%>
<%langTran("DnsFilter");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<!--<script type="text/javascript" src="js/sms_send.js"></script>-->
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var strSecondMenuFocus = "<%write(com_webAuth);%>";
var rightstr="";
var staleTime=0;
<%aspoutErrorMsg();%>
<%aspOutputAddrexIPGrpList();%>
<%aspOutputWebAuthGlobal();%>
<% aspOutputWebAuthMessage(); %> /*获取IP*/
<%aspOutputTips();%>/*获取图片名字*/
<% outputSystemSn();%>
<% outputActiveCode();%>
var i;

function strToHtml(str)
{
    str = str.replace(/ /g ,"&nbsp;");
    str = str.replace(/\r/g,'');
    str = str.replace(/\n/g ,"<br>");
    //str = str.replace(/%/g ,"%%");/*预览时%会消失，用2个%即可*/
    return str;
}

//function initTranslation()
//{
/*
   var e;
   e.value= _("save");
   e= document.getElementById("WebAuth_Switch");
   e.innerHTML= "启用网页认证";
   e = document.getElementById("add");
   e.value= _("add");
   e = document.getElementById("delall");
   e.value= _("delall");
   e= document.getElementById("del");
   e.value= _("del");
 */
//}
function init()
{
    var tbl = WebAuthGlobal;
    tbl.enabled.checked = (lively.toLowerCase() == "1") ? true : false;
    switch_en_picture(authtype);
    if(authtype == "remoteAuth") {
	AuthT.style.display="";
	tbl.authType[1].checked = true;
	if(staleTime>0) {
	tbl.staletime.value = staleTime;
	} else {
	showTable(Stale, 0);
	}
	wifidogShowEn(0);
    } else {
	if(authtype == "localAuth") {
	    AuthT.style.display="";
	}
	tbl.authType[0].checked = true;
    
    if(selfEnable=="null")
    {
	showTable(SelfHelp, 0);
    }
    else
    {
	tbl.selfenabled.checked = (selfEnable.toLowerCase() == "on")? true:false;
    }
    if(staleTime>0) {
	tbl.staletime.value = staleTime;
    } else {
	showTable(Stale, 0);
    }
    tbl.ActivePic.checked = (activePic == 1)? true:false;
    tbl.exceptIpGroup.value = exceptIpGroups;

    WebAuthGlobal.tipstitle.value = tipstitles;
    WebAuthGlobal.tipsinfo.value = tipsinfos;

    WebAuthpicture.importpictureurl.value = pictureUrl;
    if(WebAuth_Advance.toLowerCase()=="off")
    {
	WebAuthpicture.selectpic[0].checked = true;
	WebAuthpicture.selectpic[1].checked = false;
	webConfig1.style.display="none";
	webConfig2.style.display="none";
	webConfig3.style.display="none";
	advance.style.display="none";
	selfpicture.style.display="none";                                       
	selfpicinfo.style.display="none";                                       
	netpicture.style.display="";   
    }

    if("undefined" != typeof idcardEnableds)
    {
        if((idcardEnableds=="1") && (webAuthIdcode == "1")){
            document.getElementById("idcardEnabled").checked=true;
            document.getElementById("idcardbox_td").style.display="";
        }else{
            document.getElementById("idcardEnabled").checked=false;
            document.getElementById("idcardbox_td").style.display="";
        }
    }

    if ((webAuthAutoFunc == "1")) {
	/***************************/

	/*
	   var idcardEnableds="0";//启用身份证认证
	   var smsEnableds = "1";//启用短信认证
	   var wechatEnableds = "0";//启用微信认证
	   var idcardTimeTotals="20";//传过来的参数是小时
	   var idcardMaxSessionss = "1";//身份认证最大并发数，默认为1
	   var smsTimeTotals="24";//短信有效期，传过来的参数是小时
	   var smsMaxSessionss = "1";//短信最大并发数，默认为1
	   var smsContentss = "您的验证码为##，有效期为**";

	   var wechatTimeTotals="24";//短信有效期，传过来的参数是小时
	   var wechatMaxSessionss = "1";//短信最大并发数，默认为1
	   var wechatContentss = "您的验证码为##，有效期为**";
	   var wechatTokens="dsdsdsd";
	 */
	var idcartime="";
	if(idcardTimeTotals%24==0){
	    idcartime= idcardTimeTotals/24 ;
	    document.getElementById("idcardday").value="day";
	}else{
	    idcartime= idcardTimeTotals;
	    document.getElementById("idcardday").value="hour"; 
	}
	document.getElementById("idcardTimeTotal").value=idcartime;
	document.getElementById("idcardMaxSessions").value=idcardMaxSessionss;
    if (webAuthAutoFunc == "1"){
	WebAuthJumpUrl.jumpurl.value = jumpUrl;
	var smstime="";
	if(smsTimeTotals%24==0){
	    smstime= smsTimeTotals/24	;
	    document.getElementById("smsday").value="day";
	}else{
	    smstime= smsTimeTotals;
	    document.getElementById("smsday").value="hour";    
	}
	document.getElementById("smsTimeTotal").value=smstime;
	document.getElementById("smsMaxSessions").value=smsMaxSessionss;
	document.getElementById("smsTimes").value=smsTimess;
	document.getElementById("smsContents").disabled=true;
	document.getElementById("smsContents").value=smsContentss;
	document.getElementById("smsServer").value=smsServers;

	var wechattime="";
	if(wechatTimeTotals%24==0){
	    wechattime= wechatTimeTotals/24	;
	    document.getElementById("day").value="day";
	}else{
	    wechattime= wechatTimeTotals;
	    document.getElementById("day").value="hour";	
	}
	document.getElementById("wechatTimeTotal").value=wechattime;
	document.getElementById("wechatMaxSessions").value=wechatMaxSessionss;
	document.getElementById("wechatTimes").value=wechatTimess;
	// document.getElementById("wechatContents").value=wechatContentss;
	document.getElementById("wechatName").value=wechatNames;
	document.getElementById("wechatToken").value=wechatTokens;
	document.getElementById("wechatKeyContents").value=wechatKeyContentss;
    }

	if(idcardEnableds=="1"){
	    document.getElementById("autoEnabled").checked=true;
	    document.getElementById("idcardEnabled").checked=true;

	}
    if (webAuthAutoFunc == "1"){
	if(smsEnableds=="1"){
	    document.getElementById("autoEnabled").checked=true;
	    document.getElementById("smsEnabled").checked=true;

	}
	if(wechatEnableds=="1"){
	    document.getElementById("autoEnabled").checked=true;
	    document.getElementById("wechatEnabled").checked=true;

	}
    }
	autoAuthAccount();
    }
    }

    AuthEnabled();
    whiteList_Show();
    sms_Show();
}
function switch_en_picture(val)
{
    switch(en_picture){
	case 0:
	    WebAuthGlobal.enpicture[0].checked = true;
	    WebAuthGlobal.enpicture[1].checked = false;
	    WebAuthpicture.selectpic[0].checked = true;
	    WebAuthpicture.selectpic[1].checked = false;
	    if(val == "remoteAuth") 
	        break;
	    simple.style.display="";
	    advance.style.display="none";
	    selfpicture.style.display="none";
	    selfpicinfo.style.display="none";
	    netpicture.style.display="";

	    break;
	case 1:
	    WebAuthGlobal.enpicture[0].checked = true;
	    WebAuthGlobal.enpicture[1].checked = false;
	    WebAuthpicture.selectpic[0].checked = false;
	    WebAuthpicture.selectpic[1].checked = true;
	    if(val == "remoteAuth") 
	        break;
	    simple.style.display="";
	    advance.style.display="none";
	    selfpicture.style.display="";
	    selfpicinfo.style.display="";
	    netpicture.style.display="none";

	    break;
	case 2:
	    WebAuthGlobal.enpicture[0].checked = false;
	    WebAuthGlobal.enpicture[1].checked = true;
	    WebAuthpicture.selectpic[0].checked = true;
	    WebAuthpicture.selectpic[1].checked = false;
	    if(val == "remoteAuth") 
	        break;
	    simple.style.display="none";
	    advance.style.display="";
	    selfpicture.style.display="none";
	    selfpicinfo.style.display="none";
	    netpicture.style.display="";

	    break;
	case 3:
	    WebAuthGlobal.enpicture[0].checked = false;
	    WebAuthGlobal.enpicture[1].checked = true;
	    WebAuthpicture.selectpic[0].checked = false;
	    WebAuthpicture.selectpic[1].checked = true;
	    if(val == "remoteAuth") 
	        break;
	    simple.style.display="none";
	    advance.style.display="";
	    selfpicture.style.display="";
	    selfpicinfo.style.display="";
	    netpicture.style.display="none";

	    break;
	default:
	    break;
    }

}

function checkTokenName(cntrl) {
    var name = cntrl.value;

    allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_";
    for (i = 0; i < name.length; i++) {
	if (allowChar.indexOf(name.substring(i, i + 1)) == -1) {
	    alert("微信Token码只能是字母数字或下划线。");
	    return false;
	}
    }
    if (name.length<3) {
	alert("微信Token码长度不能小于3位。");
	return false;
    }
    if (name.length>32) {
	alert("微信Token码长度不能大于32位。");
	return false;
    }

    return true;
}

function checkData()
{
    var tbl = WebAuthGlobal;

    if (!checkName(tbl.tipstitle, 32, "<%write(web_tipstitle);%>", 1))
    {
	return false;
    }
    if (!checkName(tbl.tipsinfo, 64, "<%write(web_tipsinfo);%>", 1))
    {
	return false;
    }
    if(! checkNumber(tbl.staletime,5,1440,"<%write(web_staletime);%>",0)) return false;
    tbl.hidcontact.value = "";
    var str = tbl.textarea.value;
    tbl.hidcontact.value = strToHtml(str);
    if (tbl.hidcontact.value.lstrLen() > 255)
    {
	alert("<% write(web_lenErr); %>");
	return false;
    }
    if(document.getElementById("autoEnabled").checked){
	if(document.getElementById("idcardEnabled").checked){
	    if(!checkNumber(tbl.idcardTimeTotal,1,99,"身份证认证有效期",0)) return false;
	    if(!checkNumber(tbl.idcardMaxSessions,1,5,"身份证认证最大并发数",0)) return false;	
	}
	if(document.getElementById("smsEnabled").checked){
	    if(!checkNumber(tbl.smsTimeTotal,1,99,"短信认证有效期",0)) return false;
	    if(!checkNumber(tbl.smsMaxSessions,1,5,"短信认证最大并发数",0)) return false;
	    if(!checkNumber(tbl.smsTimes,0,9,"每日可认证次数",0)) return false;
	    var smscontent = document.getElementById("smsContents").value;
	    if(smscontent.indexOf("##")==-1 ||smscontent.indexOf("**")==-1 || smscontent.indexOf("$$")==-1){
		alert("短信回复的内容格式不正确，必须包含##、**和$$（##代表验证码，**代表有效期，$$代表认证链接）");
		return false;    
	    }

	}
	if(document.getElementById("wechatEnabled").checked){
	    if(!checkNumber(tbl.wechatTimeTotal,1,99,"微信认证有效期",0)) return false;
	    if(!checkNumber(tbl.wechatMaxSessions,1,5,"微信认证最大并发数",0)) return false;   
	    if(!checkNumber(tbl.wechatTimes,0,9,"每日可认证次数",0)) return false;
	    if (!checkName(tbl.wechatName, 64, "微信公共号", 0)) return false;
	    if (!checkTokenName(tbl.wechatToken))
		return false;
	    if (tbl.wechatToken.value.length<3) {
		alert("微信Token码长度不能小于3位。");
		return false;
	    }
	    if (!checkName(tbl.wechatKeyContents, 20, "回复验证码关键字", 0)) 
		return false;
	    if (!checkName(tbl.wechatContents, 80, "微信回复内容", 0)) 
		return false;
	    var content = tbl.wechatContents.value;
	    if(content.indexOf("##") == -1 || content.indexOf("**") == -1 || content.indexOf("$$") == -1){
		alert("微信回复的内容格式不正确，必须包含##、**和$$（##代表验证码，**代表有效期、$$代表认证链接）"); 
		return false; 
	    }
	}
    }

    return true;
}
function save()
{
    var tbl = WebAuthGlobal;
    if(checkData()){
	if (WebAuthGlobal.enpicture[0].checked){
	    if (WebAuthpicture.selectpic[0].checked){
		WebAuthGlobal.hidenpicture.value = "0";
	    }else{
		WebAuthGlobal.hidenpicture.value = "1";
	    }
	}else{
	    if (WebAuthpicture.selectpic[0].checked){
		WebAuthGlobal.hidenpicture.value = "2";
	    }else{
		WebAuthGlobal.hidenpicture.value = "3";
	    }
	}	


	tbl.action = "/goform/formWebAuthGlobalConfig";
	tbl.submit();
    }
}

/*
   function hidEnOrBan()
   {
   var tbl = WebAuthGlobal;
   if(tbl.enabled.checked)
   {
   tbl.exceptIpGroup.disabled=false;
   tbl.enpicture.disabled=false;
   }   
   else{
   tbl.exceptIpGroup.disabled=true;
   tbl.enpicture.disabled=true;
   }
   }
 */
function hidEnOrBan(ctl)
{
    if (ctl.id=="simplesel")
    {
	simple.style.display="";
	advance.style.display="none";
    }   
    else
    {
	simple.style.display="none";
	advance.style.display="";
    }
}
function checkFileName(cntrl)
{
    var name=cntrl.value.split("\\")[cntrl.value.split("\\").length-1];
    var	allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890._";
    for (i = 0; i < name.length; i++) {
	if (allowChar.indexOf(name.substring(i, i + 1)) == -1) {
	    alert("<% write(web_allowcharacter); %>");
	    return false;
	}    
    }   
    return true;
}

function saveImport(){

    if (WebAuthpicture.importpicture.value=="")
	alert("<%write(web_depicture);%>");
    else {
	var str=WebAuthpicture.importpicture.value.split("\\")[WebAuthpicture.importpicture.value.split("\\").length-1];
	var re=/[\u0391-\uFFE5]+/g
	    if(str.match(re)!=null){
		alert("<%write(web_note1);%>");
	    }else if (checkFileName(WebAuthpicture.importpicture) == true)
	    {

		if(confirm("<%write(web_note2);%>")){ 
		    WebAuthpicture.action = "/goform/formPictureLoad";
		    WebAuthpicture.submit();
		}
	    }
    }
}

function saveWechatImport(){	    
    if (WebAuthWechatPic.wechatPicFile.value=="") {
	alert("请选择二维码图片");return false;
    } else {
	var pictype = WebAuthWechatPic.wechatPicFile.value.substr(WebAuthWechatPic.wechatPicFile.value.lastIndexOf(".")).toLowerCase();
	if(!pictype.match(/.jpg|.gif|.png|.bmp/i)){
	    alert('图片类型必须是: .jpg, .gif, .bmp or .png !');
	    return false;
	}

	var str=WebAuthWechatPic.wechatPicFile.value.split("\\")[WebAuthWechatPic.wechatPicFile.value.split("\\").length-1];
	var re=/[\u0391-\uFFE5]+/g
	    if(str.match(re)!=null){
		alert("文件名不能包含汉字！");
	    }else if (checkFileName(WebAuthWechatPic.wechatPicFile) == true)
	    {
		WebAuthWechatPic.action = "/goform/formWechatPicLoad";
		WebAuthWechatPic.submit();
	    }
    }
}

function checkUrl(str){
    var str1 = str.match(/http(s)?:\/\/.+/);
    if(str1 == null)
    {
	return false;
    }else{
	if (str.substring(0,4) == "http"){
	    var chk=str.split("//");
	    var chk2=chk[1].split("/");
	    if ((chk2[1] == null) ||(chk2[1] == ""))
	    {
		return false;
	    }else{
		return true;
	    }
	}else{
	    return false;
	}
    }
}
function saveImportUrl()
{
    if (WebAuthpicture.importpictureurl.value=="")
    {
	alert("<%write(web_inputUrl);%>");	
    }   
    else if (WebAuthpicture.importpictureurl.value.lstrLen() > 510)
    {
	alert("<%write(web_lenErr);%>");
    }
    else if ((WebAuthpicture.importpictureurl.value.indexOf("&") != -1)||(WebAuthpicture.importpictureurl.value.indexOf("+") != -1))
    {
	alert("<%write(web_illcharacter);%>");
    }
    else
    {	if (checkUrl(WebAuthpicture.importpictureurl.value)){
								WebAuthpicture.action = "/goform/formPictureUrl";
								WebAuthpicture.submit();
							    }
    else 
    {
	alert("<%write(web_urlError);%>");
    }
    }

}
function saveJumpUrl()
{
    if (WebAuthJumpUrl.jumpurl.value == "")
    {
	WebAuthJumpUrl.action = "/goform/formJumpUrl";
	WebAuthJumpUrl.submit();
    }
    else if (WebAuthJumpUrl.jumpurl.value.lstrLen() > 510)
    {
	alert("<%write(web_jumpurlmaxlen);%>");
    }
    else if ((WebAuthJumpUrl.jumpurl.value.indexOf("&") != -1)||(WebAuthJumpUrl.jumpurl.value.indexOf("+") != -1))
    {
	alert("<%write(web_illcharacter);%>");
    }
    else if (!checkName(WebAuthJumpUrl.jumpurl, 511, "<%write(web_jumpurl);%>", 1))
    {
	alert("<%write(web_urlError);%>");
    }
    else
    {
	if (searchStr("http://", WebAuthJumpUrl.jumpurl.value, true) == 0)
	{
	    WebAuthJumpUrl.jumpurl.value = WebAuthJumpUrl.jumpurl.value.substring(7);
	}
	WebAuthJumpUrl.action = "/goform/formJumpUrl";
	WebAuthJumpUrl.submit();
    }
}
function clearJumpUrl()
{
    WebAuthJumpUrl.jumpurl.value="";
}
function preWindow(){
    var preview_web = "http://" + ip + "/WebAuth_auth.asp"
	//	window.open(preview_web, '_blank', 'toolbar=no, menubar=no, scrollbars=no, resizable=no, location=no, status=no, top=0, left=0'); 
	openForm(WebAuthpicture,"WebAuth_auth.asp",750,470);
}
function activeDefPage()
{
    WebAuthAdvPage.action = "/goform/formActiveDefPage";
    WebAuthAdvPage.submit();
}
function downlaodPage()
{
    WebAuthAdvPage.action = "/goform/formDownloadPage";
    WebAuthAdvPage.submit();
}
function saveImportPage()
{
    if (WebAuthAdvPage.importpage.value=="")
    {
	alert("<%write(web_inputPage);%>");
    }else if(checkFileName(WebAuthAdvPage.importpage) == true) {
	WebAuthAdvPage.action = "/goform/formImportPage";
	WebAuthAdvPage.submit();
    }
}
function showImportOrNet(ctl)
{
    if(ctl.id=="selfpicturesel")
    {
	selfpicture.style.display="";
	selfpicinfo.style.display="";
	netpicture.style.display="none";
    }
    else
    {
	selfpicture.style.display="none";
	selfpicinfo.style.display="none";
	netpicture.style.display="";
    }
}
/***********认证方式****************/
function autoAuthAccount(){
    if(document.getElementById("autoEnabled").checked){
	document.getElementById("idcardbox_td").style.display="";
    if(webAuthAutoFunc == "1"){
        document.getElementById("smsbox_td").style.display="";
        document.getElementById("wechatbox_td").style.display="";
        document.getElementById("jumpurl_tr").style.display="";
    }
    }else{
    if(webAuthAutoFunc == "1")
    {
	document.getElementById("idcardbox_td").style.display="none";
	document.getElementById("idcardEnabled").checked =false;
    }
    document.getElementById("smsbox_td").style.display="none";
    document.getElementById("wechatbox_td") .style.display="none";
    document.getElementById("smsEnabled").checked=false;
    document.getElementById("wechatEnabled").checked=false;
    document.getElementById("jumpurl_tr").style.display="none";
    }
    if (webAuthAutoFunc == "1"){
    smsAuth();
    wechatAuth();
    }
}
function AuthEnabled(){
    if(document.getElementById("enabled").checked && (webAuthAutoFunc == "1") ){
	document.getElementById("webAuthAuto").style.display="";
    }else{
	document.getElementById("webAuthAuto").style.display="none";
	document.getElementById("autoEnabled").checked = false;
    if(webAuthAutoFunc == "1")
        document.getElementById("idcardEnabled").checked=false;
	document.getElementById("smsEnabled").checked=false;
	document.getElementById("wechatEnabled").checked=false;
	autoAuthAccount();
    }
}
function idcartAuth(){
    var f = WebAuthGlobal;
    var idcard = document.getElementById("idcardEnabled"),checked;
    if (f.idcardEnabled.checked == 1 && !confirm("此功能需要配合艾泰身份证认证客户端使用。\n您是否确认开启此功能？")) {
        f.idcardEnabled.checked = false;
        return false;
    }
}
function smsAuth(){

    if(document.getElementById("smsEnabled").checked){
	document.getElementById("smsTime_td").style.display="";
	document.getElementById("smsMaxSessions_td").style.display="";
	document.getElementById("smsTimes_td").style.display="";
	document.getElementById("smsContents_td").style.display="none";
	document.getElementById("smsServer_td").style.display="";
    }else{
	document.getElementById("smsTime_td").style.display="none";
	document.getElementById("smsMaxSessions_td").style.display="none";
	document.getElementById("smsTimes_td").style.display="none";
	document.getElementById("smsContents_td").style.display="none";
	document.getElementById("smsServer_td").style.display="none";
    }

}
function wechatAuth(){

    if(document.getElementById("wechatEnabled").checked){
	if (wechatEnableds=="0")
	    alert("启用微信认证，需同步启用系统管理->远程管理，配置详见帮助");
	document.getElementById("wechatTime_td").style.display="";
	document.getElementById("wechatMaxSessions_td").style.display="";
	document.getElementById("wechatTimes_td").style.display="";
	document.getElementById("wechatName_td").style.display="";
	document.getElementById("wechatPic_td").style.display="";
	document.getElementById("wechatToken_td").style.display="";
	document.getElementById("wechatKeyContents_td").style.display="";
	document.getElementById("wechatContent_td").style.display="";
    }else{
	document.getElementById("wechatTime_td").style.display="none";
	document.getElementById("wechatMaxSessions_td").style.display="none";
	document.getElementById("wechatTimes_td").style.display="none";
	document.getElementById("wechatName_td").style.display="none";
	document.getElementById("wechatPic_td").style.display="none";
	document.getElementById("wechatToken_td").style.display="none";
	document.getElementById("wechatKeyContents_td").style.display="none";
	document.getElementById("wechatContent_td").style.display="none";
    }

}

function wifidogShowEn(flag)
{
if(flag == 0) {
	AvtivePic.style.display="none";
	SelfHelp.style.display="none";
	webConfig1.style.display="none";
	webConfig2.style.display="none";
	simple.style.display="none";
	advance.style.display="none";
	BackPic.style.display="none";
	wifidogTip.style.display="";
	ActTab.style.display="";
	WebAuthGlobal.exceptIpGroup.value = exceptIpGroups;
} else {
	AvtivePic.style.display="";
	SelfHelp.style.display="";
	webConfig1.style.display="";
	webConfig2.style.display="";
	simple.style.display="";
	advance.style.display="";
	BackPic.style.display="";
	wifidogTip.style.display="none";
	ActTab.style.display="none";
}

}
function changeSelTab(form, val) {
    var tbl = WebAuthGlobal;
    switch_en_picture(val);
    if(val == "remoteAuth") {
	tbl.authType[1].checked = true;
	wifidogShowEn(0);

    if(webAuthIdcode == "1"){
	document.getElementById("idcardbox_td").style.display="none";
    }
    } else {

    if(webAuthIdcode == "1"){
	document.getElementById("idcardbox_td").style.display="";
    }
	tbl.authType[0].checked = true;
	wifidogShowEn(1);
    
    if(selfEnable=="null")
    {
	showTable(SelfHelp, 0);
    }
    else
    {
	tbl.selfenabled.checked = (selfEnable.toLowerCase() == "on")? true:false;
    }
    if(staleTime>0) {
	tbl.staletime.value = staleTime;
    } else {
	showTable(Stale, 0);
    }
	tbl.ActivePic.checked = (activePic == 1)? true:false;
	tbl.exceptIpGroup.value = exceptIpGroups;
	WebAuthGlobal.tipstitle.value = tipstitles;
	WebAuthGlobal.tipsinfo.value = tipsinfos;
	WebAuthpicture.importpictureurl.value = pictureUrl;
	if(WebAuth_Advance.toLowerCase()=="off")
	{
	WebAuthpicture.selectpic[0].checked = true;
	WebAuthpicture.selectpic[1].checked = false;
	webConfig1.style.display="none";
	webConfig2.style.display="none";
	webConfig3.style.display="none";
	advance.style.display="none";
	selfpicture.style.display="none";                                       
	selfpicinfo.style.display="none";                                       
	netpicture.style.display="";   
	}
    }
    whiteList_Show();
    sms_Show();
    return true;
}
var SeniorShowEn=0;
var linestr="-------------------------------------------------------";
function clickSenior(adv) 
{
    if(SeniorShowEn == 0) {
	creatList();
        SeniorShowEn = 1;
    } else {
        SeniorShowEn = 0;
    }
    showTable(adv, SeniorShowEn);
}
function creatList()
{
    var tbl=WebAuthGlobal;
    tbl.DnsList.options.length = 0;
    if(DnsLists.length>0){
      for (i=0;i<DnsLists.length;i++)
      {
	var option1=document.createElement("OPTION");
	option1.text=DnsLists[i];
	tbl.DnsList.options.add(option1);
	
      }
      option1=document.createElement("OPTION");
      option1.text=linestr;
      tbl.DnsList.options.add(option1);
    }
}
function sure()
{
  var tbl=WebAuthGlobal;
  if(max_totalrecs-DnsLists.length<1){
      alert("<%write(dns_addTip);%>");
	  return ;
  }
  for(i=0;i<DnsLists.length;i++){
   if(tbl.addHostFilter.value.toLowerCase()==DnsLists[i].toLowerCase()){
	     alert("<%write(dns_existTip);%>");
		 return ;
	 }
  }
  if (!checkName(tbl.addHostFilter,49,"<%write(dns_domain);%>",0)) return ;
  if (!checFilterName(tbl.addHostFilter,49,"<%write(dns_domain);%>",0)) return ;
  if (!checkData()) return;
  WebAuthGlobal.action="/goform/websWhiteList";
  WebAuthGlobal.submit();
}
function checFilterName(cntrl,maxlen,tips,allownull)
{
    var name=cntrl.value;
    if(name=="")
    {
	if(!eval(allownull))
	{
	  alert(tips+_("<%write(com_null);%>"));
	 cntrl.focus();   
	  return false;
	}
	else
	    return true;
    }
  else if(name==0){
	alert(tips+_("<%write(com_zero);%>"));
	return false;
    }
    else if (name.length > maxlen)
    {alert(name.length);
	alert(tips+_("<%write(com_chkLenTip);%>")+maxlen+_("<%write(com_lenAppend);%>"));
	cntrl.focus();
	return false;
    }
    else
    {
	allowChar="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.-";
	allowChar1="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.-*";
	for (i=0;i<name.length;i++)
	{
	    if(wildchar == 1) {
		if (allowChar1.indexOf(name.substring(i,i+1))==-1)
		{
		    alert(tips+_("域名只能由字母，数字，-，.和*组成！"));
		    return false;
		}
	    } else {
	    if (allowChar.indexOf(name.substring(i,i+1))==-1)
	    {
		alert(tips+_("域名只能由字母，数字，-和.组成！"));
		return false;
	    }
	    }
	}
    }
    return true;
}
function del(){
    var tbl=WebAuthGlobal;
    tbl.delstr.value=""; 
    if(tbl.DnsList.options.length == 0){
	alert("列表为空！");
	return;
    }

    if (tbl.DnsList.selectedIndex == -1){
	alert("请选择要删除的域名！"); 
	return;
     }
     for (var i=0;i<tbl.DnsList.options.length;i++) {
	if (tbl.DnsList.options[i].selected) {
	    if(tbl.DnsList.options[i].text!=linestr)
	    {
	      tbl.delstr.value=tbl.delstr.value+DnsLists[i]+",";
	    } 
	    else
	    {
	       alert("<%write(dns_delTip);%>");
	       return;
	    }
	 }   
      }
      WebAuthGlobal.action="/goform/formWhiteListDel";
      WebAuthGlobal.submit();
}
function DelAll(){
    var tbl=WebAuthGlobal;
    if(tbl.DnsList.options.length == 0){
	alert("白名单为空！");
	return;
    }
    if(confirm(_("<%write(dns_delete);%>"))){
	WebAuthGlobal.action="/goform/formWhiteListDelAll";
        WebAuthGlobal.submit();
    }
}
function whiteList_Show()
{   var tbl = WebAuthGlobal;
    if(whiteListShow == "yes"){
       if(tbl.authType[1].checked) {
           document.getElementById("advanceTable").style.display="";
	   if(totalrecs != old_totalrecs){
		clickSenior(adv1); 
	    }
       } else {
               document.getElementById("advanceTable").style.display="none";
     }
    }
}
function sms_Show()
{
    var tbl = WebAuthGlobal;
    if(smsShow == "yes"){
	if(tbl.authType[1].checked) {
           document.getElementById("smsShow").style.display="";
	   tbl.smsEnable.checked = (smsEnable.toLowerCase() == "1") ? true : false;
//	   read();
//	   setSMS()
       } else {
               document.getElementById("smsShow").style.display="none";
     }

    }
}
</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle(_("<%write(web_auth);%>"));</script>
<table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" background="images/table_main.png" style="background-repeat:repeat-x;">
<script language="javascript">showerror(errorstr);</script>
<script language="javascript">showright(rightstr);</script>
<tr valign="top">
<td><p>&nbsp;</p>
<script language="javascript">
var arrayThreeMenu=new Array(
	"90%",
	3,
	new Array(_("<%write(web_global);%>"),'WebAuthGlobal.asp',1,1),
	new Array(_("<%write(web_server);%>"),'WebAuthServer.asp',1,0),
	new Array(_("<%write(web_status);%>"),'WebAuthStatus.asp',1,0)
	);
showThreeMenu();
</script>
<table width="90%" border="0" cellspacing="0" cellpadding="0" align="center" height="250">
<tr>
<td align="center" class="three_td" valign="top">
<table border="0" width="600" align='center'>
<form action=""  name="WebAuthGlobal" method="post" >

<tr>
<td colspan="4" height="30">&nbsp;
</td>
</tr> 

<tr>
<td height="20" width="45%" align="right"><%write(web_startAuth);%>
</td>
<td width="2%">&nbsp;
</td>
<td align="left" width="53%"><input type="checkbox" name="enabled" id="enabled" onClick="AuthEnabled();">
</td>
</tr>
<tr id="webAuthAuto" style="display:none">
<td height="20" width="45%" align="right"><%write(web_auto);%>
</td>
<td width="2%">&nbsp;
</td>
<td align="left" width="53%"><input type="checkbox" name="authEnabled" id="autoEnabled"  onClick="autoAuthAccount();">
</td>
</tr>
<!--
<tr  id="idcardbox_td" style="display:none;">
<td height="20" width="45%" align="right"><%write(web_idcard);%>
</td>
<td width="2%">&nbsp;
</td>
<td align="left" width="53%"><input type="checkbox" name="idcardEnabled" id="idcardEnabled" onClick="idcartAuth();" >&nbsp;此功能需配合艾泰身份证认证客户端使用.
</td>
</tr>
-->
<tr id="idcardTime_td" style="display:none;">
<td height="20" width="45%" align="right"><%write(web_validTime);%>
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="idcardTimeTotal" id="idcardTimeTotal" style="width:40px;" maxlength="3" >
<select id="idcardday"  name="idcardday"><option value="day">天</option><option value="hour">小时</option></select>
</td>
</tr>
<tr id="idcardMaxSession_td" style="display:none;">
<td height="20" width="45%" align="right"><%write(web_maxSession);%>
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="idcardMaxSessions" id="idcardMaxSessions"  >
</td>
</tr>
<tr id="smsbox_td" style="display:none;">
<td height="20" width="45%" align="right" ><%write(web_sms);%>
</td>
<td width="2%">
</td>
<td align="left" width="53%"><input type="checkbox" name="smsEnabled" id="smsEnabled" onClick="smsAuth();" >
</td>
</tr>
<tr id="smsTime_td" style="display:none;">
<td height="20" width="45%" align="right" ><%write(web_validTime);%>
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="smsTimeTotal" id="smsTimeTotal" style="width:40px;" maxlength="3" >
<select id="smsday" name="smsday"><option value="day">天</option><option value="hour">小时</option></select>
</td>
</tr>
<tr id="smsMaxSessions_td" style="display:none;">
<td height="20" width="45%" align="right"><%write(web_maxSession);%>
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="smsMaxSessions" id="smsMaxSessions" >
</td>
</tr>
<tr id="smsTimes_td" style="display:none;">
<td height="20" width="45%" align="right">每日可认证次数
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="smsTimes" id="smsTimes" >
</td>
</tr>
<tr id="smsContents_td" style="display:none;">
<td height="20" width="45%" align="right"><%write(web_smsContent);%>
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><textarea  name="smsContents" id="smsContents" ><%aspOutputSmsContentData();%></textarea>
</td>
</tr>
<tr id="smsServer_td" style="display:none;">
<td height="20" width="45%" align="right">短信服务器地址
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input  type="text" name="smsServer" id="smsServer" >
</td>
</tr>
<tr id="wechatbox_td" style="display:none;">
<td height="20" width="45%" align="right" ><%write(web_wechat);%>
</td>
<td width="2%">
</td>
<td align="left" width="53%">
<input type="checkbox" name="wechatEnabled" id="wechatEnabled" onClick="wechatAuth();" >
</td>
</tr>
<tr id="wechatTime_td" style="display:none;">
<td height="20" width="45%" align="right" ><%write(web_validTime);%>
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="wechatTimeTotal" id="wechatTimeTotal" style="width:40px;" maxlength="3" >
<select id="day" name="day"><option value="day">天</option><option value="hour">小时</option></select>
</td>
</tr>
<tr id="wechatMaxSessions_td" style="display:none;">
<td height="20" width="45%" align="right"><%write(web_maxSession);%>
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="wechatMaxSessions" id="wechatMaxSessions" >
</td>
</tr>
<tr id="wechatTimes_td" style="display:none;">
<td height="20" width="45%" align="right">每日可认证次数
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="wechatTimes" id="wechatTimes" >
</td>
</tr>
<tr id="wechatName_td" style="display:none;">
<td height="20" width="45%" align="right"><%write(web_wechatName);%>
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="wechatName" id="wechatName" >
</td>
</tr>
<tr id="wechatToken_td" style="display:none;">
<td height="20" width="45%" align="right"><%write(web_wechatToken);%>
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="wechatToken" id="wechatToken" >
</td>
</tr>
<tr id="wechatKeyContents_td" style="display:none;">
<td height="20" width="45%" align="right">回复验证码关键字
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><input type="text" name="wechatKeyContents" id="wechatKeyContents" >
</td>
</tr>
<tr id="wechatContent_td" style="display:none;">
<td height="20" width="45%" align="right"><%write(web_wechatContent);%>
</td>
<td width="2%"><img src="images/qt.gif" width="7" height="9" />
</td>
<td align="left" width="53%"><textarea name="wechatContents" id="wechatContents" ><%aspOutputWechatContentData();%></textarea>
</td>
</tr>
    <tr>
	<td colspan="3">
	<table align="center" width="100%" name="AuthT" id="AuthT" style="display:none" >
	<tr>
	    <td height="20" width="45%" align="right" id="WebAuth_Switch">&nbsp;<%write(web_authType);%>
	    </td>
	    <td width="2%">&nbsp;
	    </td>
	    <td align="left" width="53%">
	<input type="radio" name="authType" id="authType" value="localAuth" onClick="changeSelTab(this.form,this.value) " checked>&nbsp;<%write(web_authlocal);%>
	    </td>
	</tr>
	<tr>
	    <td height="20" width="45%" align="right">&nbsp;</td>
	    <td width="2%">&nbsp;</td>
	    <td align="left" width="53%"><input type="radio" name="authType" id="authType" value="remoteAuth" onClick="changeSelTab(this.form,this.value) ">&nbsp;<%write(web_authremote);%></td>
	</tr>
	</table>
	</td>
    </tr>

<tr  id="idcardbox_td" name="idcardbox_td" style="display:none;">
<td height="20" width="45%" align="right"><%write(web_idcard);%>
</td>
<td width="2%">&nbsp;
</td>
<td align="left" width="53%"><input type="checkbox" name="idcardEnabled" id="idcardEnabled" onClick="idcartAuth();" >
</td>
</tr>
<tr>
	<td colspan="3">
	<table align="center" width="100%" name="Select" id="Select" style="display:" >
<tr id="AvtivePic" style="display:">
<td height="20" width="45%" align="right"><%write(web_enpicture);%></td>
<td width="2%">&nbsp;</td>
<td align="left" width="53%"><input type="checkbox" name="ActivePic" id="ActivePic"></td>
</tr>

<tr id="SelfHelp" style="display:">
<td height="20" width="45%" align="right"><%write(web_pwdMod);%></td>
<td width="2%">&nbsp;</td>
<td align="left" width="53%"><input type="checkbox" name="selfenabled" id="selfenabled"></td>
</tr>

<tr id="Stale" style="display:">
<td height="20" width="45%" align="right"><%write(web_staletime);%></td>
<td width="2%">&nbsp;</td>
<td align="left" width="53%"><input type="text" name="staletime" id="staletime" value="10" style="width:70px"/><%write(web_minutes);%></td>
</tr>

<tr id="disIpGroup" style="display" >
<td width="45%" height="20" align="right" id = "exceptAddGroup"><%write(web_exceptipgroup);%>
</td>
<td width="2%">&nbsp;</td>
<td align="left" height="20"  width="53%">
<select name="exceptIpGroup" id="exceptIpGroup" class="input" style="width:70px">
<option value=""><%write(web_none);%></option>
<script language="javascript">
for(var i=0;i<addrGroupName.length;i++)
{
    document.write('<option value='+addrGroupName[i]+'>'+addrGroupName[i]+'</option>');
}
</script>	      
</select>
</td>
</tr>
<tr name="smsShow" id="smsShow" style="display:none">
<td colspan=3>
    <table align="center" width="100%">
<tr>
         <td width="45%" height="25" align="right">短信发送</td>
         <td width="2%" height="25">&nbsp;</td>
         <td width="53%" height="25"><div align="left"> <input name="smsEnable" type="checkbox" id="smsEnable" onClick="" size="25"> </div></td> 
        </tr>
<tr style="display:none">
         <td width="45%" height="25" align="right">移动运营商</td>
         <td width="2%" height="25">&nbsp;</td>
         <td width="53%" height="25"><div align="left"> <input type="radio" name="cdmaType" id="cdmaType" value="CDMA2000" onClick="cdmaChange()" checked>&nbsp;CDMA2000
         <input type="radio" name="cdmaType" id="cdmaType" value="WCDMA" onClick="cdmaChange()" checked>&nbsp;WCDMA
         <input type="radio" name="cdmaType" id="cdmaType" value="TD-CDMA" onClick="cdmaChange()" checked>&nbsp;TD-CDMA </div></td> 
        </tr>
    <tr style="display:none">
  <td width="45%" height="25" align="right" valign="middle" id="domain">省市选择</td>
  <td width="2%">&nbsp;</td>
  <td align="left" width="53%" valign="middle">
  <select name="province" id="province" class="list"  onChange="getCity()">
   <option value="0" id="cmc">请选择</option>
  </select>
  <select name="city" id="city" class="list"  onChange="getNumber()">
   <option value="0" id="cmc">请选择</option>
  </select>
  <input type="hidden" name="sms_number" id="sms_number">
  </td> 
</tr>
    </table>
</td>
</tr>
<tr id="webConfig1" style="display">
<td height="20" width="45%" align="right" id="Picture_Switch"><%write(web_page);%></td>
<td width="2%">&nbsp;<input type="hidden" name="hidenpicture" id="hidenpicture"></td>
<td align="left" width="53%"><input type="radio" name="enpicture" id="simplesel" value="1" onClick="hidEnOrBan(this)" >&nbsp;<%write(web_simpleConf);%></td>
</tr>
<tr id="webConfig2" style="display">
<td height="20" width="45%" align="right" ></td>
<td width="2%">&nbsp;</td>
<td align="left" width="53%"><input type="radio" name="enpicture" id="advancesel" value="0" onClick="hidEnOrBan(this)">&nbsp;<%write(web_advanceConf);%></td>
</tr>
	</table>
	</td>
</tr>

<tr>
<td colspan="3">
<table align="center" width="100%" name="simple" id="simple" style="display" >

<tr id="tipstitle1" style="display:none">
<td width="45%" height="20" align="right" id = "TipsTitle"><%write(web_tipstitle);%></td>
<td width="2%">&nbsp;</td>
<td align="left" height="20"  width="53%">
<input name="tipstitle"  type="text" class="input2" id="tipstitle" maxlength="32" size="20" ></td>
</tr>

<tr id="tipsinfo1"  style="display:none">
<td width="45%" height="20" align="right" id = "TipsInfo"><%write(web_tipsinfo);%></td>
<td width="2%">&nbsp;</td>
<td align="left" height="20"  width="53%">
<input name="tipsinfo"  type="text"  class="input2" id="tipsinfo"  maxlength="64" size="20" >
</td>
</tr>

<tr>
<td height="20" colspan="3" align="center"><%write(web_contact);%></td>
</tr>
<tr>
<td colspan="3" align="center"> 
<textarea id="textarea" name="textarea" style="font-size:15px; font-family:'新宋体'; width:500px; height:100px"><%aspOutputContactData();%></textarea><input type="hidden" name="hidcontact" id="hidcontact" value=""></td>
</tr>

</table>
</td>
</tr>

<tr>
    <td colspan="3">
	<table align="center" width="100%" name="ActTab" id="ActTab" style="display:none" >
	    <tr>
	    <td height="20" width="45%" align="right"><% write(SerNum); %>:</td></td>
	    <td width="2%">&nbsp;</td>
	    <td align="left" width="53%">
		<script>
		    document.write(productIDs);
		</script>
	    </td>
	    </tr>
	    <tr>
	    <td height="20" width="45%" align="right">&nbsp;<%write(com_actCode);%>:</td></td>
	    <td width="2%">&nbsp;</td>
	    <td align="left" width="53%">
		<script>
		    document.write(ActCode);
		</script>
	    </td>
	    </tr>
	</table>
    </td>
</tr>

<tr>
<td colspan="3">
<table width="30%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
<td width="45%" >&nbsp;</td>
<td width="2%" >&nbsp;</td>
<td height="30" align="left" valign="top">
<input type="hidden" name="WebAuthEnblew">
<input type="hidden" name="WebAuthEnbleOld">
<input type="button" class="input-button" id="uttsave" value="<%write(com_save);%>"  onClick="save()">
<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#WebAuthGlobalEdit')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</td>
</tr>
</table>
</td>
</tr>
<tr id="advanceTable" style="display:none"><td colspan="3"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr>
         <td width="45%" height="30" align="right" ><div style="position:relative; z-index:2;"><a href="javascript:clickSenior(adv1)" style="color:#000000;text-decoration:underline;"><strong>高级选项</strong></a></div></td>
         <td width="2%" >&nbsp;</td>
         <td width="30%"></td>
         <td width="23%"></td></tr>
	<tr id="adv1" style="display:none">
         <td colspan="4" >
        <table width="100%" border="0" cellpadding="0" cellspacing="0" >
  <tr>
  <td width="45%" height="25" align="right" valign="middle" id="domain">域名名称</td>
  <td width="2%">&nbsp;</td>
  <td align="left" width="53%" valign="middle"><input name="addHostFilter"  class="input"type="text" size="45" maxlength="49">&nbsp;&nbsp;<input type="button" value="添加" onClick="sure()" class="input-button" id="add"></td> 
</tr>
<tr style="display:">
   <td width="30%" height="30" align="right" valign="middle" > </td>
     <td width="3%">&nbsp;</td>
     <td colspan="4" height="20" class="helpfont" >域名白名单用于设置免认证的域名或IP，如要在认证通过之前正常访问http://www.com.cn，加入域名白名单列表即可</td>
   </tr>
  <tr>
       <tr><td colspan="4" height="20"><input type="hidden" name="delstr"></td></tr>
  <tr>
<tr>
    <td width="20%" height="30" align="right" valign="top" id="domain list">白名单列表</td>
	<td width="3%" height="40">&nbsp;</td>
	<td align="left" colspan="2"><select class="input" name="DnsList" size=5 multiple  id="dnslist"  color:blue  style="font-size:10pt;SCROLLBAR-FACE-COLOR: #ffffff;  SCROLLBAR-HIGHLIGHT-COLOR: #ffffff;  #c0ddff 1px solid; SCROLLBAR-SHADOW-COLOR: #ffffff; LINE-HEIGHT: 20px; SCROLLBAR-ARROW-COLOR: #c0ddff; SCROLLBAR-TRACK-COLOR: #ffffff; SCROLLBAR-DARKSHADOW-COLOR: #c0ddff; SCROLLBAR-BASE-COLOR: #c0ddff;border:1px solid #c0ddff; width:270px " border=0  >
    </select></td>
  </tr>
  <tr><td colspan="4" align="center" height="30" valign="bottom" >
  <table width="180" border="0" cellspacing="0" cellpadding="0">
		      <tr align="left">
			<td width="45%" height="40">&nbsp;</td>
			<td width="2%" height="40">&nbsp;</td>
		        <td >
			 <input type="button" value="删除" class="input-button" onClick="del()" id="delete">
		    	 <input type="button" value="清空" class="input-button" onClick="DelAll()" id="delete all">
		        </td>
		      </tr>
		    </table>
			</td></tr>

	                </table>        </td>
      </tr>		
	</table></td></tr>

<tr><td colspan="3" align="center" height="40"><table width=750 border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table>
</td></tr><input type="hidden" value="1" name="re">
<!--  wifidog use tips  -->
  <tr name="wifidogTip" id="wifidogTip" style="display:none" ><td colspan="3" align="center" height="40"><table width=750 border="0" cellspacing="0" cellpadding="0">
  <tr height="30" align="center">
   <td width="50" align="right"><strong><%write(web_tip);%>:</strong></td>
   <td align="left">&nbsp;</td>
   <td>&nbsp;</td>
  </tr>
  <tr height="30" align="center">
   <td width="50" align="right">&nbsp;</td>
   <td align="left"><%write(web_tips_1);%><a href="http://b.greenwifi.com.cn/register.jsp?<%outputwifiTip();%>" target="_blank"><%write(web_tips_2);%></a>。&nbsp;</td>
   <td>&nbsp;</td>
  </tr>
  <tr height="30" align="center">
   <td width="50" align="right">&nbsp;</td>
   <td align="left"><%write(web_tips_3);%><a href="http://b.greenwifi.com.cn/login.jsp" target="_blank"><%write(web_tips_4);%></a><%write(web_tips_5);%><a href="http://b.greenwifi.com.cn/binddevice.htm" target="_blank"><%write(web_tips_6);%></a>？&nbsp;</td>
   <td>&nbsp;</td>
  </tr>
</table>
</td></tr>
<!--  end wifidog tips  -->
</form>

<form name="WebAuthAdvPage" id="WebAuthAdvPage" style="display" method="post" enctype="multipart/form-data">
<table border="0" align="center" width="100%" name="advance" id="advance" style="display">
<tr>
<td width="36%" align="right"><%write(web_advanceConf);%></td>
<td width="2%">&nbsp;</td>
<td width="62%" align="left">
<input name="default_page" id="default_page" type="button" class="input-button" value="<%write(web_useDefPage);%>" onClick="activeDefPage()" ></td>
</tr>	
<tr >
<td width="36%"></td>
<td width="2%">&nbsp;</td>
<td width="62%" align="left">	
<input name="download_page" id="download_page" type="button" class="input-button" value="<%write(web_downloadPage);%>" onClick="downlaodPage()" >
</td>
</tr>
<tr>
<td width="36%" ></td>
<td width="2%">&nbsp;</td>
<td width="62%" align="left"><input name="importpage" type="file" id="importpage"><input name="saveimportPage" type="button" class="input-button"  value="<%write(web_input);%>" onClick="saveImportPage()" id="saveimportPage"><script type="text/javascript">document.write(webFileNames);</script>
</td>
</tr>
<tr><td colspan="3" align="center" height="40"><table width=750 border="0" cellspacing="0" cellpadding="0">
<tr>
<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
</tr>
</table>
</td></tr>
</table>
</form>

<form name="WebAuthWechatPic"  id ="WebAuthWechatPic" method="post" enctype="multipart/form-data">
<table border="0"  align='center' width="80%" >
<tr id="wechatPic_td" style="display:none;">
<td height="20" width="36%" align="right">微信二维码图片
</td>
<td align="left"><input type="file" name="wechatPicFile" id="wechatPicFile" ><input type="button" class="input-button" name="saveWechatPic" id="saveWechatPic" value="导入" onclick="saveWechatImport()">
</td>
</tr>
</table>
</form>

<form name="WebAuthJumpUrl"  id ="WebAuthJumpUrl" method="post" enctype="multipart/form-data">
<table border="0"  align='center' width="80%" >
<tr id="jumpurl_tr" name="jumpurl_tr" style="display:none">
<td width="36%" align="right" ><%write(web_jumpurl);%></td>
<td height="20" align="left" style="valign:baseline"><input name="jumpurl" type="text" id="jumpurl"><input name="clearjumpurl" type="button" class="input-button"  value="<%write(web_clear);%>" onClick="clearJumpUrl()" id="clearjumpurl"><input name="savejumpurl" type="button" class="input-button"  value="<%write(com_save);%>" onClick="saveJumpUrl()" id="savejumpurl"></td>
</tr>
</table>
</form>

<form name="WebAuthpicture"  id ="WebAuthpicture" method="post" enctype="multipart/form-data">
<table name="BackPic" id="BackPic" style="display:" border="0"  align='center' width="80%" >
<tr>
<td width="36%" align="right" ><%write(web_bgPic);%></td>
<td height="20" align="left"><input name="selectpic" id="netpicturesel" type="radio" value="0" onClick="showImportOrNet(this)" checked><%write(web_netUrl);%></td>
</tr>
<tr id="webConfig3" style="display">
<td width="36%" align="right" ></td>
<td height="20" align="left"><input name="selectpic" id="selfpicturesel" type="radio" value="1" onClick="showImportOrNet(this)"><%write(web_selfDef);%></td>
</tr>
<tr id="selfpicture" name="selfpicture" style="display:">
<td width="36%" align="right" ></td>
<td height="20" align="left"><input name="importpicture" type="file" id="importpicture"><input name="saveimport" type="button" class="input-button"  value="<%write(web_input);%>" onClick="saveImport()" id="import"></td>
</tr>
<tr id="netpicture" name="netpicture" style="display:">
<td width="36%" align="right" ></td>
<td height="20" align="left"><input name="importpictureurl" type="text" id="importpictureurl"><input name="saveimport" type="button" class="input-button"  value="<%write(com_save);%>" onClick="saveImportUrl()" id="saveimport"></td>
</tr>
<tr id="selfpicinfo" height="30" style="display">
<td></td>
<td class="helpfont"  align="left" id="zhuyi"><strong><%write(web_note3);%></strong><%write(web_note4);%></td>
</tr>
<tr><td></td>
<td align="left"><input name="preview" type="button" class="input-button" value="<%write(web_preview);%>" onClick="preWindow()" id="preview"></td>
</tr>
</table>


</form>


</table></td>
</tr>
</table>

</td>
</tr>
<script language="javascript">init();</script>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>


