<?php

function jqueryformstyler_settings_form() {
  $form = array();
  
  $form['jqueryformstyler_class'] = array(
    '#title' => t('Class'),
    '#description' => t('The HTML class that is to be used to apply jQueryFormStyler.'),
    '#type' => 'textarea',
    '#required' => TRUE,
    '#default_value' => variable_get('jqueryformstyler_class', '.jqueryformstyler'),
  );
  
  return system_settings_form($form);
}