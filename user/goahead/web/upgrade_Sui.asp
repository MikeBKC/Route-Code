<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<META http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel=stylesheet type="text/css" href="css/style_new.css" title="cool" id="lnk">
<% langTran("SoftwareUpdate"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<meta HTTP-EQUIV="refresh" CONTENT="90;url=/redirect.asp">
<!--
<meta HTTP-EQUIV="refresh" CONTENT="90;url=/FirmwareUpgrade_Sui.asp">
-->
<link href="css/default.css" rel="stylesheet" type="text/css"/>

<script type="text/javascript" src="script/jquery-1.8.1.min.js" ></script>
<script type="text/javascript" src="script/jquery-ui-1.8.12.custom.min.js"></script> 
<script type="text/javascript" src="script/jquery.artDialog.js" ></script>
</head>
<style type="text/css">
body{background:#eee;font-size: 14px; font-family: Microsoft Yahei, Verdana, Arial, Sans-Serif; color: #383838/*#666*/;margin:0;padding:0}
.example{background:#FFF;width:500px;font-size:80%;border:1px #000 solid;margin:20px auto;padding:15px;position:relative;-moz-border-radius: 3px;-webkit-border-radius: 3px}
h3 {text-align:center}

.pbar .ui-progressbar-value {display:block !important}
.pbar {overflow: hidden}
.percent {position:relative;text-align: right;}
.elapsed {position:relative;text-align: right;}
</style>
<script type="text/javascript">
$(document).ready(function(){
    jQuery.fn.anim_progressbar = function (aOptions) {
        // def values
        var iCms = 1000;
        var iMms = 60 * iCms;
        var iHms = 3600 * iCms;
        var iDms = 24 * 3600 * iCms;

        // def options
        var aDefOpts = {
            start: new Date(), // now
            finish: new Date().setTime(new Date().getTime() + 60 * iCms), // now + 60 sec
            interval: 100
        }
        var aOpts = jQuery.extend(aDefOpts, aOptions);
        var vPb = this;

        // each progress bar
        return this.each(
            function() {
                var iDuration = aOpts.finish - aOpts.start;

                // calling original progressbar
                $(vPb).children('.pbar').progressbar();

                // looping process
                var vInterval = setInterval(
                    function(){
                        var iLeftMs = aOpts.finish - new Date(); // left time in MS
                        var iElapsedMs = new Date() - aOpts.start, // elapsed time in MS
                            iDays = parseInt(iLeftMs / iDms), // elapsed days
                            iHours = parseInt((iLeftMs - (iDays * iDms)) / iHms), // elapsed hours
                            iMin = parseInt((iLeftMs - (iDays * iDms) - (iHours * iHms)) / iMms), // elapsed minutes
                            iSec = parseInt((iLeftMs - (iDays * iDms) - (iMin * iMms) - (iHours * iHms)) / iCms), // elapsed seconds
                            iPerc = (iElapsedMs > 0) ? iElapsedMs / iDuration * 100 : 0; // percentages

                        // display current positions and progress
                        $(vPb).children('.percent').html('<b>'+iPerc.toFixed(0)+'%</b>');
						if(iMin==-59 || iSec==-59){
							iMin=0;	iSec=0;
						}
                        $(vPb).children('.elapsed').html(iMin+'<%write(com_minute);%>'+iSec+'<%write(com_second);%></b>');
                        $(vPb).children('.pbar').children('.ui-progressbar-value').css('width', iPerc+'%');

                        // in case of Finish
                        if (iPerc >= 100) {
                            clearInterval(vInterval);
                            $(vPb).children('.percent').html('<b>100%</b>');
                            $(vPb).children('.elapsed').html('<%write(com_complete);%>');
							
							//art.dialog({icon:"succeed",id:"wating_box",title:false,content:'操作生效,重启成功！'}).lock().time(4);
				
							//window.setTimeout('window.location.href="home.html";',3000); 
			
							window.parent.art.dialog.list['func_box'].close();
							


                        }
                    } ,aOpts.interval
                );
            }
        );
    }

    // default mode
   // $('#progress1').anim_progressbar();

    // from second #5 till 15
    var iNow = new Date().setTime(new Date().getTime() + 0 * 1000); // now plus 0secs
    var iEnd = new Date().setTime(new Date().getTime() + 90 * 1000); // now plus 90 secs
    $('#progress').anim_progressbar({start: iNow, finish: iEnd, interval: 100});

});

</script>
<body>
<div class="example">
 <div id="progress">
            <div class="percent"></div>
            <div class="pbar"></div>
            <div class="elapsed"></div>
            <div class="tip"> 	&diams;&nbsp;<%write(upg_warn6);%><br>
 	&diams;&nbsp;<%write(upg_warn7);%></div>
 </div>
 </div>
</body>
</html>
