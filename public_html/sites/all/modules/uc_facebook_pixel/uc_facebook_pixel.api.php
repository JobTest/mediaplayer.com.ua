<?php

/**
 * @file
 * API documentation for Ubercart Facebook Pixel.
 */

/**
 * Alters the final array of data items to be pushed.
 *
 * Modules can implement hook_uc_facebook_pixel_data_ACTION_alter() to modify
 * data sent to Facebook Pixel for a specific action.
 *
 * Possible actions:
 *  - Purchase
 *  - AddToCart
 *  - InitiateCheckout
 *  - CompleteRegistration
 *  - ViewContent
 *
 * @param array &$data
 *   By reference. An array of all encoded data elements.
 * 
 * @param int $order_id
 *   Function hook_uc_facebook_pixel_data_ACTION_alter order_id.
 */
function hook_uc_facebook_pixel_data_ACTION_alter(array &$data, $order_id) {
  // Example for 'Purchase' action: Add order id to the purchase data.
  $data += array(
    'order_id' => $order_id,
  );
}
