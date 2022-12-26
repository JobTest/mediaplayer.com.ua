<?php

module_load_include('inc', 'uc_cart', 'uc_cart.pages');

function mycart_checkout_callback() {
  global $user;

  $items = uc_cart_get_contents();
  if (count($items) == 0 || !variable_get('uc_checkout_enabled', TRUE)) {
    drupal_goto('cart');
  }

  if (($min = variable_get('uc_minimum_subtotal', 0)) > 0) {
    $subtotal = 0;
    if (is_array($items) && count($items) > 0) {
      foreach ($items as $item) {
        $data = module_invoke($item->module, 'uc_cart_display', $item);
        if (!empty($data)) {
          $subtotal += $data['#total'];
        }
      }
    }

    if ($subtotal < $min) {
      drupal_set_message(t('The minimum order subtotal for checkout is @min.', array('@min' => uc_currency_format($min))), 'error');
      drupal_goto('cart');
    }
  }

  // Send anonymous users to login page when anonymous checkout is disabled.
  if (!$user->uid && !variable_get('uc_checkout_anonymous', TRUE)) {
    drupal_set_message(t('You must login before you can proceed to checkout.'));
    if (variable_get('user_register', 1) != 0) {
      drupal_set_message(t('If you do not have an account yet, you should <a href="!url">register now</a>.', array('!url' => url('user/register', array('query' => drupal_get_destination())))));
    }
    drupal_goto('user', array('query' => drupal_get_destination()));
  }

  // Load an order from the session, if available.
  if (isset($_SESSION['cart_order'])) {
    $order = uc_order_load($_SESSION['cart_order']);
    if ($order) {
      // Don't use an existing order if it has changed status or owner, or if
      // there has been no activity for 10 minutes (to prevent identity theft).
      if (uc_order_status_data($order->order_status, 'state') != 'in_checkout' ||
          ($user->uid > 0 && $user->uid != $order->uid) ||
          $order->modified < REQUEST_TIME - UC_CART_ORDER_TIMEOUT) {
        if (uc_order_status_data($order->order_status, 'state') == 'in_checkout' && $order->modified < REQUEST_TIME - UC_CART_ORDER_TIMEOUT) {
          // Mark expired orders as abandoned.
          uc_order_update_status($order->order_id, 'abandoned');
        }
        unset($order);
      }
    }
    else {
      // Ghost session.
      unset($_SESSION['cart_order']);
      drupal_set_message(t('Your session has expired or is no longer valid.  Please review your order and try again.'));
      drupal_goto('cart');
    }
  }

  // Determine whether the form is being submitted or built for the first time.
  if (isset($_POST['form_id']) && $_POST['form_id'] == 'uc_cart_checkout_form') {
    // If this is a form submission, make sure the cart order is still valid.
    if (!isset($order)) {
      drupal_set_message(t('Your session has expired or is no longer valid.  Please review your order and try again.'));
      drupal_goto('cart');
    }
    elseif (!empty($_SESSION['uc_cart_order_rebuild'])) {
      //drupal_set_message(t('Your shopping cart contents have changed. Please review your order and try again.'));
      //drupal_goto('cart/checkout');
      ////My code
 	  $efq = new EntityFieldQuery();
      $result = $efq->entityCondition('entity_type', 'uc_order_product')
        ->propertyCondition('order_id', $order->order_id)
        ->execute();
      if (!empty($result['uc_order_product'])) {
        $product_ids = array_keys($result['uc_order_product']);
        uc_order_product_delete_multiple($product_ids);
      }
      uc_order_delete_line_item($order->order_id, TRUE);
      $order->products = uc_cart_get_contents();
	  $order->line_items = uc_order_load_line_items($order);
      uc_order_save($order);
	  unset($_SESSION['uc_cart_order_rebuild']);
	  ////	  
    }
  }
  else {
    // Prepare the cart order.
    $rebuild = FALSE;
    if (!isset($order)) {
      // Create a new order if necessary.
      $order = uc_order_new($user->uid);
      $_SESSION['cart_order'] = $order->order_id;
      $rebuild = TRUE;
    }
    elseif (!empty($_SESSION['uc_cart_order_rebuild'])) {
      // Or, if the cart has changed, then remove old products and line items.
      $efq = new EntityFieldQuery();
      $result = $efq->entityCondition('entity_type', 'uc_order_product')
        ->propertyCondition('order_id', $order->order_id)
        ->execute();
      if (!empty($result['uc_order_product'])) {
        $product_ids = array_keys($result['uc_order_product']);
        uc_order_product_delete_multiple($product_ids);
      }
      uc_order_delete_line_item($order->order_id, TRUE);
      $rebuild = TRUE;
    }

    if ($rebuild) {
      // Copy the cart contents to the cart order.
      $order->products = uc_cart_get_contents();
      unset($_SESSION['uc_cart_order_rebuild']);
    }
    elseif (!uc_order_product_revive($order->products)) {
      drupal_set_message(t('Some of the products in this order are no longer available.'), 'error');
      drupal_goto('cart');
    }
  }

  // Trigger the "Customer starts checkout" hook and event.
  module_invoke_all('uc_cart_checkout_start', $order);
  rules_invoke_event('uc_cart_checkout_start', $order);

  return drupal_get_form('uc_cart_checkout_form', $order);
}
