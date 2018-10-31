 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<% aspOutWatchdogEn(); %>
function init(){
	var f=watchdog_config;
	if(wdgEn == "on")
	    f.watchdog.checked= true;
}
function save(){
	watchdog_config.action ="/goform/formConfigWatchdog";
	watchdog_config.submit();
}
</script>
<body>
callstack:
<% aspOutCallTrace(); %>
<form action=""  name="watchdog_config" method="post" >
open/close watchdog
<input  type="checkbox" name="watchdog"><input type="hidden" name="watchdog-hidden">
<input type="button" class="input-button" value="保存"  onClick="save()" id="uttsave">
<script language="javascript">init();</script>
</form>
</body>
</html>

