<table width="95%" border="0" cellspacing="0" cellpadding="1" align="center" bgcolor="#006699" style="font-family: verdana, arial, helvetica; font-size: small;">
  <tr>
    <td>
      <table width="100%" border="0" cellspacing="0" cellpadding="5" align="center" bgcolor="#FFFFFF" style="font-family: verdana, arial, helvetica; font-size: small;">


        <tr valign="top">
          <td>

            <p>
               <b>Добрый день, <?php print $order_first_name; ?>!</b>
               <br />Благодарим Вас за покупку в нашем магазине <?php print $store_link; ?>!
            </p>

            <table cellpadding="4" cellspacing="0" border="0" width="100%" style="font-family: verdana, arial, helvetica; font-size: small;">
             
              <tr>
                <td colspan="2" bgcolor="#006699" style="color: white;"><b><?php print t('Purchasing Information:'); ?></b></td>
              </tr>
              
              <tr>
                <td nowrap="nowrap"><b><?php print t('E-mail Address:'); ?></b></td>
                <td width="98%"><?php print $order_email; ?></td>
              </tr>            

              <tr>
                 <td nowrap="nowrap"><b>Телефон:</b></td>
                 <td width="98%"><?php print $order_billing_phone; ?></td>
              </tr>

              <tr>
                 <td nowrap="nowrap"><b>Доставка:</b></td>
                 <td width="98%"><?php print $order_shipping_method; ?></td>
              </tr>
             
              <tr>
                <td nowrap="nowrap"><b>Оплата:</b></td>
                <td width="98%"><?php print $order_payment_method; ?></td>
              </tr>

              <tr>
                 <td nowrap="nowrap"><b>Адрес доставки:</b></td>
                 <td width="98%"><?php print $order_billing_address; ?></td>
              </tr>

              <tr>
                <td colspan="2" bgcolor="#006699" style="color: white;">
                  <b><?php print t('Order Summary:'); ?></b>
                </td>
              </tr>



              <tr>
                <td colspan="2">

                  <table border="0" cellpadding="1" cellspacing="0" width="100%" style="font-family: verdana, arial, helvetica; font-size: small;">
                    <tr>
                      <td nowrap="nowrap"><b><?php print t('Order #:'); ?></b></td>
                      <td width="98%"><?php print $order->order_id; ?></td>
                    </tr>

                    <tr>
                      <td nowrap="nowrap"><b><?php print t('Order Date: '); ?></b></td>
                      <td width="98%"><?php print $order_created; ?></td>
                    </tr>

                    <tr>
                      <td nowrap="nowrap"><?php print t('Products Subtotal:'); ?>&nbsp;</td>
                      <td width="98%"><?php print $order_subtotal; ?></td>
                    </tr>

                    <?php foreach ($line_items as $item): ?>
                    <?php if ($item['type'] == 'subtotal' || $item['type'] == 'total' || $item['type'] == 'shipping')  continue; ?>

                    <tr>
                      <td nowrap="nowrap">
                        <?php print $item['title']; ?>:
                      </td>
                      <td>
                        <?php print $item['formatted_amount']; ?>
                      </td>
                    </tr>

                    <?php endforeach; ?>

                    <tr>
                      <td>&nbsp;</td>
                      <td>------</td>
                    </tr>

                    <tr>
                      <td nowrap="nowrap">
                        <b><?php print t('Total for this Order:'); ?>&nbsp;</b>
                      </td>
                      <td>
                        <b><?php print $order_total; ?></b>
                      </td>
                    </tr>

                    <tr>
                      <td colspan="2">
                        <br /><br /><b><?php print t('Products on order:'); ?>&nbsp;</b>

                        <table width="100%" style="font-family: verdana, arial, helvetica; font-size: small;">

                          <?php foreach ($products as $product): ?>
                          <tr>
                            <td valign="top" nowrap="nowrap">
                              <b><?php print $product->qty; ?> x </b>
                            </td>
                            <td width="98%">
                              <b><?php print $product->title; ?> - <?php print $product->total_price; ?></b>
                              <?php print $product->individual_price; ?><br />
                              <?php print t('SKU'); ?>: <?php print $product->model; ?><br />
                              <?php print $product->details; ?>
                            </td>
                          </tr>
                          <?php endforeach; ?>
                        </table>

                      </td>
                    </tr>
                  </table>

                </td>
              </tr>



              <tr>
                <td colspan="2">
                  <hr noshade="noshade" size="1" /><br />
                  <p><em>
                  Примечание: это сообщение создано автоматически. Пожалуйста, не отвечайте на него. Ещё раз спасибо за покупку в нашем магазине!
                  </em></p> 
                  <p>С уважением, Команда компании <?php print $store_link; ?></p>          
                </td>
              </tr>



            </table>
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>
