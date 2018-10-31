(function($){
		$.fn.selectbox = function(data,current){
			//用变量idm存储select的id或name
			//alert("data:"+data);
			var idm = $(this).attr("id") || $(this).attr("name");	
       
			var divHtml ='<div id="'+idm+'divinput" style="height:24px;width:125px; border:1px solid #7f9db9; padding-top:2px;margin-right:20px;  color:#807a62; cursor:pointer ; position:relative;"><input type="text"  maxlength="17"  value="" style="width:100px; height:17px;padding-left:2px; margin-left:2px;padding-right:2px; background:#fff; padding-top:5px; border:0"/><span style="height:19px;width:15px;background:#cde2fc url(../images_Sui/arrow.gif) no-repeat center right  ; position:absolute; border:1px solid #90abe0;">&nbsp;</span></div>';
					if(data!="" || data!=null || data!="undefined"){
						divHtml += '<div style="position:absolute;" id='+idm+'"divlist"><ul style="width:125px; background:#e8f5fe; margin-top:2px; border:1px solid #a9c9e2; position:absolute; display:none">';
						$.each(data, function(index, value) { 
							//alert(index + ': ' + value);
							divHtml +='<li style="height:24px; line-height:24px; text-indent:10px"><a style="display:block; height:24px;color: #383838;text-decoration:none; cursor:pointer;" rel="'+value+'">'+value+'</a></li>'; 
						}); 
						divHtml +='</ul></div>';	
					}
				//alert("divHtml:"+$("#"+idm));
				$("#"+idm).append(divHtml);
				//alert("current:");

				if(current!=""){
					$("#"+idm+" input").val(current);	
				}
				$("#"+idm+"divinput span").click(function(){
					
					
					var ul = $("#"+idm+" ul");
					if(ul.css("display")=="none"){
						ul.slideDown("fast");
						
					}else{
						ul.slideUp("fast");
					
					}
				});
			
				var value=$("#txt").val();
				$("#"+idm+" ul li a").click(function(){
					var txt = $(this).text();
					$("#"+idm+" input").val(txt);
					value = $(this).attr("rel");
					$("#"+idm+" ul").hide();
					$("#"+idm+"divinput").css({"border":"1px solid #7f9db9"});
					
				});
			
				$("#"+idm+" ul li a").hover(function(){
				
						$(this).addClass("selectedit");	
					
				},function(){
						$(this).removeClass("selectedit");	
				});
		
				$(document).bind('click',function(e){
					var e = e || window.event; //浏览器兼容性
					var elem = e.target || e.srcElement;
					while (elem) { //循环判断至跟节点，防止点击的是div子元素
						if (elem.id && (elem.id==idm+'divlist'||elem.id== idm+'divinput')) {
							return;
						}
						elem = elem.parentNode;
					}
	
					var ul = $("#"+idm+" ul");
					//if(ul.css("display")=="none"){
				//		ul.slideDown("fast");
				//	}else{
						ul.slideUp("fast");
				//	} //点击的不是div或其子元素
				});
				
			return value;
		}
		$.fn.selectboxval=function(){
				var idm = $(this).attr("id") || $(this).attr("name");	
				return $("#"+idm+" input").val();
		}
})(jQuery);
