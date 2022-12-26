<?php


 


function myoption_page_add($term) {
  $vid = variable_get('myoption_taxonomy_vid', '');
  if ($term->vid != $vid) {
	 drupal_not_found(); 
  }
  global $user;
  $myoption = entity_create('myoption', array('uid' => $user->uid, 'tid' => $term->tid));
  return drupal_get_form('myoption_form', $myoption, 'create');
}

function myoption_page_edit($myoption) {
  drupal_set_title(t('<em>Edit</em> @title', array('@title' => $myoption->id)), PASS_THROUGH);
  return drupal_get_form('myoption_form', $myoption);
}

function myoption_form($form, &$form_state, $myoption, $op = 'update') {
  $form['#attributes']['class'][] = 'myoption-form';
  $form['myoption'] = array(
    '#type' => 'value',
    '#value' => $myoption,
  ); 
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
  );
  if ($op == 'update' && myoption_access('delete', $myoption, NULL, 'myoption')) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 15,
      '#submit' => array('myoption_form_delete_submit'),
    );
  }
  // Прикрепляем все созданные поля сущности к форме.
  field_attach_form('myoption', $myoption, $form, $form_state);
  return $form;
}
 
function myoption_form_submit($form, &$form_state) {
  $myoption = $form_state['values']['myoption'];
  $myoption->changed = REQUEST_TIME;
  field_attach_submit('myoption', $myoption, $form, $form_state);
  $is_new = !empty($myoption->is_new) ? TRUE : FALSE;
  if ($is_new) {
	 $myoption->created = REQUEST_TIME;  
	 global $user;
	 $myoption->uid = $user->uid;
  }
  $myoption->save();
  if ($is_new) {
    drupal_set_message(t('%title has been created.', array('%title' => $myoption->id)));
  } else {
    drupal_set_message(t('%title has been updated.', array('%title' => $myoption->id)));
  }
  $form_state['redirect'] = array('taxonomy/term/'.$myoption->tid.'/myoption_list');
} 

function myoption_form_delete_submit($form, &$form_state) {
  $destination = array();
  if (isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  $myoption = $form_state['values']['myoption'];
  $form_state['redirect'] = array('myoption/'.$myoption->id.'/delete', array('query' => $destination));
}


function myoption_delete_confirm($form, &$form_state, $myoption) {
  $form['myoption_id'] = array('#type' => 'value', '#value' => $myoption->id);
  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $myoption->id)),
    'myoptions',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}
 
function myoption_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $myoption = myoption_load($form_state['values']['myoption_id']);
    myoption_delete($form_state['values']['myoption_id']);
    drupal_set_message(t('%title has been deleted.', array('%title' => $myoption->id)));
  }
  $form_state['redirect'] = 'taxonomy/term/'.$myoption->tid.'/myoption_list';
}



