(function ($, Drupal, window, document, undefined) {

$(function() {

/**********************************************************************************************************/  



setTimeout(function(){
  var text = $('#skype-dune').remove();	
  $('#block-block-2 div.skype a img').after(text);
}, 2000);

/*

var a = $('a[rel="galery-product"').length;
alert(a);

$('a[rel="galery-product"').live( 'click', function () {
  var h = $(this).attr('href');
  console.log(h);
});

*/
/**********************************************************************************************************/  
 
$("#block-menu-menu-okompanii > ul a.active").closest('li').prev('li').addClass('no-border');
$("#block-menu-menu-okompanii > ul li").hover(function() {
	$(this).prev("li").addClass('no-border');
}, function() {
	$(this).prev("li").removeClass('no-border');
});




/**********************************************************************************************************/   


if ($('#page-messages').length) {
//  $.colorbox({href:"#page-messages", open:true, inline:true, width:400});  
}




/**********************************************************************************************************/   

$('.view-aksessuar.view-display-id-page_1 .group-aksessuar-kupit .sell-price, .view-product.view-display-id-page_1 .group-product-kupit .sell-price, .view-product.view-display-id-block_5 .group-product-kupit .sell-price').click(function(){
	$(this).parent().find('.add-to-cart input[type="submit"]').click();
});

/**********************************************************************************************************/   
setTimeout(function(){
  $("#navigation #block-system-main-menu > ul.menu > li").each(function(){
	var liwidth = $(this).width();
	$(this).find("ul.menu").css({
	  'min-width' : liwidth
    });
  });	
}, 1000);



/**********************************************************************************************************/  




/**********************************************************************************************************/  













































 

/**********************************************************************************************************/   

});

})(jQuery, Drupal, this, this.document);

