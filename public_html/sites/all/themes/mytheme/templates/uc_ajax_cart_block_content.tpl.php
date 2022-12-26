<?php
$qtys = 0;
foreach ($items as $item) {
  $qtys += $item['qty'];	
}

?>

<div id="cart-block-contents-ajax">
  <div class="hover">
    <div class="cart-block-summary-items">
        <div class="cart"><?php print $qtys; ?></div>
        <?php print l('Оформить заказ', 'cart/checkout', array('attributes'=>array('class'=>array('cart-checkout')))); ?>
    </div> 
  </div>
  <div class="hiden">
     <div class="title">В Вашей корзине</div>
     <div class="cont">
	    <?php print $qtys.' '.myapi_sklon($qtys, array('товар', 'товара', 'товаров')); ?>
        на сумму <?php print theme('uc_price', array('price' => $total)); ?>
        <?php print l('Оформить заказ', 'cart/checkout', array('attributes'=>array('class'=>array('cart-checkout')))); ?>
     </div>
  </div>    
</div>