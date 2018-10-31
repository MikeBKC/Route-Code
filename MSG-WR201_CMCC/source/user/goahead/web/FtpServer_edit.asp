<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("FtpServer_edit"); %>
<% langTran("common"); %>
<title><% write(FtpEditTitle); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript">

//var filename="folder";
var alldirEnable=0;
var path="";
var volumeName="";
//var filedirectories = new Array("演讲","ReOS1","工作","NetFilter","演讲2","ReOS2","工作4","NetFilter3","演讲3","ReOS3","工作5","NetFilter4");
var errorstr="";
var editname="";

<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!=""){ 
        aspOutputFtpServerDirOneInfo(edit_Name);//需要修改
    }
    else
    {
        aspOutputFtpServerLocalDir();
    }
%>
var edit_Names = "<% write(Form("id"));%>";


/**

 * 分页函数

 * pno--页数

 * psize--每页显示记录数

 * 分页部分是从真实数据行开始，因而存在加减某个常数，以确定真正的记录数

 * 纯js分页实质是数据行全部加载，通过是否显示属性完成分页功能

 **/

function goPage(pno,psize){

	//var itable = document.getElementById("idData");

	var num = filedirectories.length;//表格行数

	var totalPage = 0;//总页数

	var pageSize = psize;//每页显示行数

	if((num-1)/pageSize > parseInt((num-1)/pageSize)){   

   		totalPage=parseInt((num-1)/pageSize)+1;   

   	}else{   

   		totalPage=parseInt((num-1)/pageSize);   

   	}   

	var currentPage = pno;//当前页数
	
	var startRow = (currentPage - 1) * pageSize+1;//开始显示的行   

   	var endRow = currentPage * pageSize+1;//结束显示的行   

   	endRow = (endRow > num)? num : endRow;

	writefile(startRow);
	writePage(totalPage);
	document.getElementById("filenamepage").value=currentPage;
	if(Ftpserver_conf.allselect.checked){
			 var temp = document.getElementsByName("file_dir_radio");
			 var tempdir = document.getElementsByName("dirname");
			  for(var i=0;i<temp.length;i++)
			  {
				 temp[i].disabled=true;
				 tempdir[i].disabled=true;  
			  }	
		}
	
	
}
function writefile(pno){
	document.getElementById("filenamelist").innerHTML="";
		var filestr='<table cellpadding="0" border="0" cellspacing="0" width="100%" >';
	if(filedirectories.length!=0){
		
		for(var i=pno;i<=filedirectories.length;i++){
			
			filestr +="<tr height='28'>";
			filestr +='<td width="8%" align="left"><input type="radio" value="'+filedirectories[i-1]+'" class="checkbox" id="file_dir'+i+'" name="file_dir_radio" /></td><td width="2%">&nbsp;</td><td align="left" valign="middle" ><a href="#" name="dirname" onclick="linkdir('+(i-1)+')">'+filedirectories[i-1]+'</a></td>';
			filestr +="</tr>";
			if(i%10==0)break;
		   }
		   
	   }
	   filestr +="</table>"
	  
	 // alert("filestr:"+filestr);

	document.getElementById("filenamelist").innerHTML = filestr;
	
	
}
function writePage(totalPage){
		 var sel = '<select id="filenamepage" onchange="goPage(this.value,10);">';

	   for(var i=1;i<=totalPage;i++){
		
		  		
		   			sel+='<option value='+i+'>'+i+'</option>';
			
		   }
       if(totalPage==0){
        sel+='<option value="1">1</option>';
       }
		sel +='</select>';
		document.getElementById("selectpage").innerHTML=sel;
}
function changeAllDir(){
	if(Ftpserver_conf.allselect.checked){
			var temp = document.getElementsByName("file_dir_radio");
			 var tempdir = document.getElementsByName("dirname");
			  for(var i=0;i<temp.length;i++)
			  {
				 temp[i].checked=false;
				 temp[i].disabled=true;
				 tempdir[i].disabled=true;  
			  }	
	}else{
		var temp = document.getElementsByName("file_dir_radio");
			 var tempdir = document.getElementsByName("dirname");
			  for(var i=0;i<temp.length;i++)
			  {
				 temp[i].disabled=false;
				 tempdir[i].disabled=false;  
			  }	
	}	
}
function linkdir(index){
	var filename = 	filedirectories[index];
	var filedirval = document.getElementById("file_path").value;
	
	document.getElementById("file_link_path").value=filedirval+"/"+filename;
	Ftpserver_conf.action="/goform/formFtpServerShareDirSelcet";
	Ftpserver_conf.submit();
	
}
function gobackdir(){
	Ftpserver_conf.action="/goform/formFtpServerShareDirGoBack";
	Ftpserver_conf.submit();
		
}


function save(){
	var f = Ftpserver_conf;
	if(!checkName(f.filename,11,"<% write(name); %>",0)) return false;

	if( !filedirectories.length ) return false;

	 var temp = document.getElementsByName("file_dir_radio");
     var file = "";
	  for(var i=0;i<temp.length;i++)
	  {
		 if(temp[i].checked){
			   file = temp[i].value;
         }
	  }
//alert(file);
//	  var fileallpath=document.getElementById("file_path").value+"/"+file;
	  var fileallpath=document.getElementById("file_path").value;
//alert(fileallpath);
      if( fileallpath != "" ){
          fileallpath = fileallpath + "/" + file;
      }else{
          fileallpath = file;
      }

	  document.getElementById("file_save_path").value=fileallpath;
	 
	Ftpserver_conf.action="/goform/formFtpServerDirConfig";
	Ftpserver_conf.submit();
}
function init(){
		goPage(1,10);
		document.getElementById("directory").innerHTML=path;
		document.getElementById("file_path").value=path;
		document.getElementById("volume_name").value=volumeName;
		Ftpserver_conf.allselect.checked=(alldirEnable==1?true:false);
        Ftpserver_conf.filename.value=filename;
//        Ftpserver_conf.id11.value=edit_Names;
//        alert("id value:"+ Ftpserver_conf.id11.value);
        if(edit_Names!=""&&edit_Names!=null){//alert();
            Ftpserver_conf.Action.value="modify";
            Ftpserver_conf.oldfilename.value=filename;
        }
        if(editname!=""&&editname!=null){//alert();
            Ftpserver_conf.Action.value="modify";
            Ftpserver_conf.oldfilename.value=editname;
        }
	if(filedirectories[0]=="sda1"||filedirectories[1]=="sda1"
		||filedirectories[0]=="sdb1"||filedirectories[1]=="sdb1"
		||filedirectories[0]=="mmcblk0p1"||filedirectories[1]=="mmcblk0p1"){              
	    document.getElementById("allselect").disabled=true;                   
	}                                 
	else {                                                              
	    document.getElementById("allselect").disabled=false;                     
	}               
}
</script>
</head>

<body background="images/body_main.png" onLoad="init();">
	<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td>
  	<script language="javascript">//showSecondTitle("");</script>
  	<table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   		<script language="javascript">//showerror(errorstr,80);</script>
  		<tr valign="top">
    		<td>
  				<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
				<p>&nbsp;</p>
				<tr>
    				<td align="center"  class="style7">
                    	<table width="100%" border="0" cellspacing="0" cellpadding="0">
  						<tr>
    					<td>
                        	<table border="0" width="80%" align='center'>
                            	<form action="" onSubmit="return " name="Ftpserver_conf" method="post">
                                	<tr>
                                    	<td align="center">
                                        	<table width="100%" border="0">
                                             <tr><td colspan="4">&nbsp;</td></tr>
                                            	<tr>
                                                	<td width="45%" align="right"><% write(name); %></td>
                                                    <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
                                                    <td align="left"><input type="text" value="" class="input" name="filename" /></td>
                                                    <td align="left"><input type="hidden" value="" class="input" name="oldfilename" /></td>
                                                    <td id="nametip"></td>                                                    
                                                </tr>
                                                <tr>
                                                	<td align="right"><% write(folder); %></td>
                                                    <td>&nbsp;</td>
                                                     <td colspan="2 " align="left"><table cellpadding="0" cellspacing="0">
                                                     	<tr>
                                                        	
                                                            <td align="left" width="4"><input type="checkbox" value="" onclick="changeAllDir();" class="checkbox" name="allselect" id="allselect" /></td>
                                                            <td align="left"><% write(selectAll); %></td>
                                                        </tr>
                                                     </table></td>                                                     
                                                </tr>
                                                <tr>
                                                	<td align="right"><% write(folderPosition); %></td>
                                                    <td>&nbsp;</td>
                                                    <td align="left"><span id="directory">/</span></td>
                                                    <td></td>                                                    
                                                </tr>
                                                <tr>
                                                	<td align="right"><% write(selectFolder); %></td>
                                                    <td>&nbsp;</td>
                                                    <td align="left"><input type="button" class="input-button" value="<% write(upLevel); %>" onClick="gobackdir();" /></td>
                                                    <td></td>                                                    
                                                </tr>
                                                  <tr>
                                                	<td align="right"></td>
                                                    <td>&nbsp;</td>
                                                     <td colspan="2 " align="left" id="filenamelist">
                                                     
                                                     </td>                                                     
                                                </tr>
                                                  <tr>
                                                	<td align="right"></td>
                                                    <td></td>
                                                     <td colspan="2 " align="left" >
                                                     	<table>
                                                        	<tr>
                                                            	<td width="30%" align="right"><% write(of); %></td>
                                                                <td width="30%" id="selectpage"></td>
                                                                <td width="30%" align="left"><% write(page); %></td>
                                                            </tr>
                                                        </table>
                                                     </td>                                                     
                                                </tr>
                                                <tr><td colspan="4" align="center"></td></tr>
                                                 <tr><td colspan="4">&nbsp;</td></tr>
                                                 <tr>
                                                	<td colspan="4" align="center"><input type="button" onclick="save();" class="input-button" value="<% write(com_save); %>"/>&nbsp;&nbsp;&nbsp;&nbsp;<!--<input type="reset" class="input-button" value="重置"/>&nbsp;&nbsp;&nbsp;&nbsp;--><input type="button" class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#FtpServer')"/>&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>" onClick="GoBack_Page('FtpServer.asp')"/></td>
                                                                                                    
                                                </tr>
                                                <tr><td colspan="4">&nbsp;</td></tr>
                                            </table>
                                        </td>
                                    </tr>
                                    <input type="hidden" name="file_path" id="file_path" value="" />
                                    <input type="hidden" name="file_link_path" id="file_link_path" value="" />
                                    <input type="hidden" name="file_save_path" id="file_save_path" value="" />
                                    <input type="hidden" name="volume_name" id="volume_name" value="" />
                                    <input type="hidden" name="Action" value="add">
<!--
                                    <input type="hidden" name="id11" value="kk">
-->
                                </form>
                            </table>
                        </td>
                        </tr>
                        </table>
                    </td>
                 </tr>
                 </table>
             </td>
          </tr>
      </table>
    </td>
   <td width="15"></td>
   </tr>
  </table>
</body>
</html>
