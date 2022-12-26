<?php

$nid_tovar = $fields['nid']->raw;
$nid_acsesuar = $fields['nid_1']->raw;
$node_tovar = node_load($nid_tovar); 
$node_acsesuar = node_load($nid_acsesuar); 
if (!$node_tovar->nid) {
   return FALSE;
}
if (!$node_acsesuar->nid) {
   return FALSE;
}


$form = drupal_get_form(
   'mykomplekt_add_to_cart_form_'.$nid_tovar.'_'.$nid_acsesuar, 
   $nid_tovar, 
   $nid_acsesuar
);
$add = drupal_render($form);


$tovar_sell_price = $node_tovar->sell_price * mycart_get_course_usd($node_tovar);
$acsesuar_sell_price = $node_acsesuar->sell_price * mycart_get_course_usd($node_acsesuar);
$acsesuar_sell_price_new = mykomplekt_get_price_by_nodes($node_tovar, $node_acsesuar) * mycart_get_course_usd($node_acsesuar);
$prise_komplect = $acsesuar_sell_price_new + $tovar_sell_price;

$tovar_image = theme('image_style', array(
                'style_name' => 'komplect', 
				'path' => $node_tovar->uc_product_image['und'][0]['uri'],
				'alt' => $node_tovar->title, 
                'title' => $node_tovar->title,
		));	
$tovar_image = l($tovar_image, 'node/'.$nid_tovar, array('html'=>TRUE));
	
$acsesuar_image = theme('image_style', array(
                'style_name' => 'komplect', 
				'path' => $node_acsesuar->uc_product_image['und'][0]['uri'],
				'alt' => $node_acsesuar->title, 
                'title' => $node_acsesuar->title,
		));	
$acsesuar_image = l($acsesuar_image, 'node/'.$nid_acsesuar, array('html'=>TRUE));
?>

<div class="komplect-wraper">

  <div class="komplect1">
     <div class="title"><?php print l(myapi_title2line($node_tovar->title), 'node/'.$nid_tovar, array('html'=>TRUE)); ?></div>
     <div class="img"><?php print $tovar_image; ?></div>
     <div class="prise"><?php print theme('uc_price', array('price' => $tovar_sell_price)); ?></div>
  </div>
  <div class="komplect2">
     <div class="title"><?php print l(myapi_title2line($node_acsesuar->title), 'node/'.$nid_acsesuar, array('html'=>TRUE)); ?></div>
     <div class="img"><?php print $acsesuar_image; ?></div>
     <div class="prise">
	    <div class="prise-nova"><?php print theme('uc_price', array('price' => $acsesuar_sell_price_new)); ?></div>
	    <div class="prise-stara"><?php print theme('uc_price', array('price' => $acsesuar_sell_price)); ?></div>
     </div>
  </div>
  <div class="komplect3">
     <div class="prise_komplect"><?php print theme('uc_price', array('price' => $prise_komplect)); ?></div>
     <div class="add_komplect"><?php print $add; ?></div>
  </div>
</div>









