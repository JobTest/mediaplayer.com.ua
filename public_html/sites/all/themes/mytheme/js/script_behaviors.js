(function ($) {  Drupal.behaviors.mytheme = { attach: function (context, settings) {

/**********************************************************************************************************/   

//$('a.tooltip:not(.my_processed)', context).addClass('my_processed').click(function() {
/**********************************************************************************************************/   

$('#quotes-pane a').click(function(){
  window.location = $(this).attr('href');	
  return false;	
});


/**********************************************************************************************************/   

$('a.flag-link-toggle:not(.my_processed)', context).addClass('my_processed').bind('flagGlobalAfterLinkUpdate', function() {
	$('#block-views-sravnenia-block-1 .block-refresh-button').trigger('click');
});

/**********************************************************************************************************/   
$('#colorbox').ajaxComplete(function() {
   setTimeout(function() {
	  if ($('#cart-form-pane').length) { 
	    var y = $('#cart-form-pane').height()+25;
	    var y2 = $('#mycart-komplect-wrapper').height();
		if (y2) y2 += 25;
        $.colorbox.resize({height:y+y2}); 
	  } else {
        $.colorbox.resize(); 
	  }
  }, 200);	
  
});	 


$('#cboxContent').filter(function (index) { //Фильтр элементов, с проверкой каждого
    if ($(this).find('.jwplayer-video').length == true) {
      $(this).find('#cboxNext').css({bottom:"30px", top: "inherit"});
      $(this).find('#cboxPrevious').css({bottom:"30px", top: "inherit"});	
	} else {
      $(this).find('#cboxNext').css({bottom:"inherit", top: "50%"});
      $(this).find('#cboxPrevious').css({bottom:"inherit", top: "50%"});	
	}
});


/**********************************************************************************************************/   

function hovatu_lejbl(wrapper, input) {
	var inputu = wrapper.find(input);
	var label  = inputu.closest('.form-item').find('label');
    inputu.each(function() {
		    if ($(this).val() != '') {
		         $(this).closest('.form-item').find('label').hide();
			}
    });	 
    inputu.blur  (function() {
		    if ($(this).val() == '') {
				$(this).closest('.form-item').find('label').show();
			}
    });
	inputu.focus  (function() {
		    $(this).closest('.form-item').find('label').hide();
    }); 
	label.click  (function() {
		    $(this).hide();
			$(this).closest('.form-item').find(input).focus();
    });
}

$('#simplenews-block-form-1:not(.my_processed)', context).addClass('my_processed').each(function() {
  hovatu_lejbl($(this), 'input[type="text"]'); 
});
$('#webform-client-form-11--2:not(.my_processed), #webform-client-form-11:not(.my_processed)', context).addClass('my_processed').each(function() {
  hovatu_lejbl($(this), 'input[type="text"], textarea'); 
});
$('#webform-client-form-164--2:not(.my_processed), #webform-client-form-164:not(.my_processed)', context).addClass('my_processed').each(function() {
  hovatu_lejbl($(this), 'input[type="text"], textarea');
});
$('#webform-client-form-277--2:not(.my_processed), #webform-client-form-277:not(.my_processed)', context).addClass('my_processed').each(function() {
  hovatu_lejbl($(this), 'input[type="text"], textarea');
});

/**********************************************************************************************************/   
$('input.node-add-to-cart:not(.my_processed)', context).addClass('my_processed').click(function() {
   setTimeout(function() {$('#popap-linka-kart').click();}, 1000);
});

$('#cart-block-contents-ajax .cart').click(function() {
  $('#popap-linka-kart').click();
});

/**********************************************************************************************************/   

$('#uc-cart-view-form .form-actions a').click(function() {
   $.colorbox.close();	
   return false;
});
/**********************************************************************************************************/ 
  
if ($('#mysravnenia_table').length) {
   var count_td = $('#mysravnenia_table tbody tr:first > td').length;
   var width_td = 196;
   $('#mysravnenia_table table').width(width_td*count_td);
   $('#mysravnenia_table').jScrollPane({
      showArrows: false,
      hideFocus: true,
   });
   
}

/**********************************************************************************************************/   
}};})(jQuery);