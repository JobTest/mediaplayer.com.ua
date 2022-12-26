<?php



function mycart_set_course_form() {
  $form['mycart_course_usd'] = array(
    '#type' => 'textfield',
    '#title' => 'Курс долара',
    '#default_value' => variable_get('mycart_course_usd', 1),
	'#element_validate' =>  array('element_validate_number'),
  );
  return system_settings_form($form);
}


?>