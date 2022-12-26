<?php

/**
 * @file
 * {@inheritdoc}
 */

/**
 * Implements hook_admin_settings().
 */
function uc_facebook_pixel_admin_settings_form($form_state) {
  $form = array();

  $form['uc_facebook_pixel_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Pixel ID'),
    '#required' => TRUE,
    '#default_value' => variable_get('uc_facebook_pixel_id'),
  );

  $form['uc_facebook_pixel_exclude_admin_paths'] = array(
    '#type' => 'checkbox',
    '#title' => t('Exclude from admin pages'),
    '#description' => t('The pixel tracking code will not be added to admin pages.'),
    '#default_value' => variable_get('uc_facebook_pixel_exclude_admin_paths', 1),
  );

  // Visibility configurations by Role.
  $roles = user_roles();
  $role_options = array();
  foreach ($roles as $rid => $name) {
    $role_options[$rid] = $name;
  }
  $form['uc_facebook_pixel_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Remove Facebook Pixel Tracking for specific roles'),
    '#default_value' => variable_get('uc_facebook_pixel_roles', array()),
    '#options' => $role_options,
    '#description' => t('Remove tracking for the selected role(s).'),
  );

  return system_settings_form($form);
}
