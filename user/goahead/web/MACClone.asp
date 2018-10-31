 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

Butterlate.setTextDomain("internet");
var errorstr="";
var macCloneMac1s="";
var macCloneMac2s="";
var macCloneMac2s=""; 
var macCloneMac3s="";
var macCloneMac4s="";
<%outputWanMacClone();%>
function initTranslation()
{
	var e;
/*
	e= document.getElementById("MACcopy");
	e.innerHTML= _("MACcopy");

	e= document.getElementById("uttsave");
	e.value= _("save");
	e = document.getElementById("uttreset");
	e.value= _("reset");
	e = document.getElementById("save2");
	e.value= _("save");
	e = document.getElementById("reset2");
	e.value= _("reset");
	e = document.getElementById("update");
	e.value= _("update");*/
}

<!--
var ports = "4";
function save(){
	var f=portsno;
		portsno.submit();
 
}  

function initTranslation()
{
	var e;
 
}

function init()
{
	portsno.portnumbers.value = ports
}

function Reset(){
	MacColonConfig.reset();
	init();
}
//-->
</script>
</head>
<body onLoad="initTranslation()" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("WANConfig"));</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						new Array(_("WANConfig"),'WANConfig.asp',1,0),
						new Array("动态端口配置",'MACClone.asp',1,1)
		);
		showThreeMenu();
	</script>
	
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
      <tr>
         <td align="center" class="three_td" valign="top">
		<table width="600" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><table border="0" width="80%" align="center">
                  <form  name="portsno" method="post"><tr><td colspan="4" height="20">&nbsp;</td></tr>
                   
				     <tr>
    <td width="46%" align="right" id="portno">最大WAN口数：</td>
  	<td width="1%"></td>
	<td width="52%"><input id="portnumbers"  type="text" maxlength="1" size="3" onChange="showfiledtip(re_checkName(this,'动态端口数'),yutd1)"></td><td width="1%"  id="yutd1"></td>
  </tr>
				   
					
                    <tr>
                      <td  colspan="4"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
					       <tr><td colspan="3" height="10">&nbsp;</td></tr>
                          <tr>
                            <td  height="20" align="center" valign="bottom"><input name="button" type="button" class="input-button" id="save" onClick="save()" value="保存">
                              &nbsp;&nbsp;&nbsp;
                              <input name="button" type="button" class="input-button" id="reset2" onClick="Reset()" value="重填"></td>
                          </tr>
						       <tr><td colspan="3" height="20">&nbsp;</td></tr>
                      </table></td>
                    </tr>
                  </form>
              </table></td>
            </tr>
        </table></td>
      </tr>
	  <script language="javascript">init();</script>
    </table>
	</td></tr></table></td>
    <!--////end-->
  <td width="15"></td>
  </tr>
</table></td></tr></table>
</body>
</html>
