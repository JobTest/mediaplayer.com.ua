<?php

function myapi_callback() {
  $output  = '';
  $output .= '<p>Налаштування сайту '.variable_get('site_name', '').'.</p>';
  $parent = menu_link_get_preferred($_GET['q']);
  $parameters = array(
    'active_trail' => array($parent['plid']),
    'only_active_trail' => FALSE,
    'min_depth' => $parent['depth']+1,
    'max_depth' => $parent['depth']+1,
    'conditions' => array('plid' => $parent['mlid']),
  );
  $children = menu_build_tree($parent['menu_name'], $parameters);
  if ($children) {
	$menu = menu_tree_output($children);
    $output .=  '<div class="content-sub-menu">' . drupal_render($menu) . '</div>';
  }
  return $output;
}




function myapi_settings_contacts_form() {
  $form['myapi_settings_contacts_1_1'] = array(
    '#type' => 'textarea',
    '#title' => 'В шапці 1',
    '#default_value' => variable_get('myapi_settings_contacts_1_1', ''),
	'#attributes' => array('class' => array('coll-1')),
  );
  $form['myapi_settings_contacts_1_2'] = array(
    '#type' => 'textarea',
    '#title' => 'В шапці 1',
    '#default_value' => variable_get('myapi_settings_contacts_1_2', ''),
	'#attributes' => array('class' => array('coll-2')),
  );
  $form['myapi_settings_contacts_2_1'] = array(
    '#type' => 'textarea',
    '#title' => 'В шапці 2',
    '#default_value' => variable_get('myapi_settings_contacts_2_1', ''),
	'#attributes' => array('class' => array('coll-1')),
  );
  $form['myapi_settings_contacts_2_2'] = array(
    '#type' => 'textarea',
    '#title' => 'В шапці 2',
    '#default_value' => variable_get('myapi_settings_contacts_2_2', ''),
	'#attributes' => array('class' => array('coll-2')),
  );
  $form['myapi_settings_contacts_3_1'] = array(
    '#type' => 'textarea',
    '#title' => 'У підвалі',
    '#default_value' => variable_get('myapi_settings_contacts_3_1', ''),
	'#attributes' => array('class' => array('coll-1')),
  );
  $form['myapi_settings_contacts_3_2'] = array(
    '#type' => 'textarea',
    '#title' => 'У підвалі',
    '#default_value' => variable_get('myapi_settings_contacts_3_2', ''),
	'#attributes' => array('class' => array('coll-2')),
  );
  $form['myapi_settings_contacts_4_1'] = array(
    '#type' => 'textarea',
    '#title' => 'Сторінка контакти',
    '#default_value' => variable_get('myapi_settings_contacts_4_1', ''),
	'#attributes' => array('class' => array('coll-1')),
  );
  $form['myapi_settings_contacts_4_2'] = array(
    '#type' => 'textarea',
    '#title' => 'Сторінка контакти',
    '#default_value' => variable_get('myapi_settings_contacts_4_2', ''),
	'#attributes' => array('class' => array('coll-2')),
  );
  return system_settings_form($form);
}
