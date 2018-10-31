<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<!-- <%langTran("Remote");%> -->
<title>AP状态</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script>
<style type="text/css">
	.tc,.tc>*{
		text-align: center !important;
	}
	.icons-group{
		position: relative;
		word-break: keep-all;
		word-wrap: normal;
		white-space: nowrap;
	}
	.icons-group>img{
		display: inline;
		cursor: pointer;
		cursor: -ms-pointer;
		margin: 0px 2px;
	}
</style>
</head>
<script type="text/javascript">
<%
aspOutPutApList();
%>

function init(){
	$('#apState_table').on('click','img[data-type]',function(event){
		var e = event || window.event;
		var tar = e.target || e.srcElement;
		
		var dataType = $(tar).attr('data-type');
		switch(dataType){
			case 'update':
				showUpdate(function(){
					alert(1)
				},function(){
					alert(2)
				})
				break;
			case 'restart':
				showConfirm('是否重启？',
					function(){
						alert(1)
					},
					function(){
						alert(2)
					});
				break;
			case 'reset':
				showConfirm('是否恢复出厂？',
					function(){
						alert(1)
					},
					function(){
						alert(2)
					});
				break;
		}
	});
	

	$('#fresh_all').click(function(){
		window.location.reload();
	});
	
	$('#update_all').click(function(){
		showUpdate(function(){
			alert(1)
		},function(){
			alert(2)
		})
	});
	
	$('#restart_all').click(function(){
		showConfirm('是否重启？',
			function(){
				alert(1)
			},
			function(){
				alert(2)
			});
	});
	
	$('#reset_all').click(function(){
		showConfirm('是否恢复出厂？',
			function(){
				alert(1)
			},
			function(){
				alert(2)
			});
	});
	
	$('#allcheck_btn').click(function(){
		var ischecked = $(this).is(':checked');
		$(this).parent().parent().nextAll().children().children('[type="checkbox"]').prop('checked',ischecked);
	})

}
function save(){
//  var f=Remote_Config;
//  if(!checkNumber_P(f.OutPort,1,65535,"<%write(OutPort);%>",0)) return; 
//  
//  var enabled = $("#HttpEnable").val();
//  f.HttpEnable.value=(enabled=="on"?"1":"0"); 
//  
//  Remote_Config.action ="/goform/formRemoteControl";
//  Remote_Config.submit();
}

$(function(){
    init();
});
</script>
<body>
<div style="width:800px; margin:0 auto; text-align:center;position: relative;">

<br>
	<form action="" method="post" name="">
    
	<table border="0" cellpadding="0" cellspacing="1" id="apState_table" style="background:#5e5e5e;" class="group" width="100%">
    	<tr class="tab_info_list tc" id = "apStateTr">
    		<th><input type="checkbox"  id="allcheck_btn" /></th>
    		<th>ID</th>
    		<th>AP型号</th>
    		<th>序列号</th>
    		<th>IP地址</th>
    		<th>MAC地址</th>
    		<th>信道</th>
    		<th>在线/离线</th>
    		<th>在线时长</th>
    		<th>操作</th>
    	</tr>
    	<script>
	    	if(totalrecs > 0){
				//转换数据格式
				var apStateArr1 = [];
				for(var i = 0;i< totalrecs;i++){
					apStateArr1.push([(i+1),types[i],serialno[i],iparray[i],macarray[i],channel[i],apStatus[i],i]);
				}
				//如果不为空，则遍历数组，生成HTML
					apStateArr1.forEach(function(obj,i){
						var newhtml = '';
						newhtml+= '<tr class="tab_info_list tc">';
						newhtml += '<td><input type="checkbox" /></td>'
						
						apStateArr1[i].forEach(function(obj1,j){
							newhtml += '<td>'+apStateArr1[i][j]+'</td>';
						});
						newhtml += '<td class="icons-group">';
						newhtml	+=	'<img src="./images_Sui/update.png" data-type="update"/>';
						newhtml	+=	'<img src="./images_Sui/restart.png" data-type="restart"/>';
						newhtml	+=	'<img src="./images_Sui/reset.png" data-type="reset"/>';
						newhtml	+=  '</td>';
						newhtml += '</tr>';
						document.write(newhtml);
					})			
			}else{
				//如果内容为空，则显示：无信息
				htmlstr += '<tr class="tab_info_list tc"><td  colspan="10" style="center">暂无信息</td></tr>';
				document.write(htmlstr);
			}
    	</script>

		<!--
    	<tr class="tab_info_list tc" >
	
    		<td><input type="checkbox" /></td>
    		<td>1</td>
    		<td>WAN12312</td>
    		<td>12312123</td>
    		<td>192.222.222.222</td>
    		<td>32:32:32:32:32:32</td>
    		<td>5</td>
    		<td>在线</td>
    		<td>1222天23时21分23秒</td>
    		<td class="icons-group">
    			<img src="./images_Sui/update.png" data-type="update"/>
    			<img src="./images_Sui/restart.png" data-type="restart"/>
    			<img src="./images_Sui/reset.png" data-type="reset"/>
    		</td>
    	</tr>-->
    	<tr class="tab_info">
        	<td colspan="10" height="60px" style="text-align: right;">
        		<button type="button" id="fresh_all" class="btn" style="margin: 20px 3px">刷新</button>
        		<button type="button" id="update_all" class="btn" style="margin: 20px 3px">升级</button>
        		<button type="button" id="restart_all" class="btn" style="margin: 20px 3px">重启</button>
        		<button type="button" id="reset_all" class="btn" style="margin: 20px 3px;margin-right:10px">恢复出厂设置</button>
        	</td>
        </tr>
    </table>
    </form>
<br>
<br>
</div>
</body>
</html>
