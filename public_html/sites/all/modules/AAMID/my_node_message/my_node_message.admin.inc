<?php

function my_node_message_table_callback() {
  $header = array(
    array('data' => 'Тип', 'field' => 'm.type'),
    array('data' => 'Ім'я відправника', 'field' => 'm.name_from'),
    array('data' => 'Е-мейл от', 'field' => 'm.mail_from'),
    array('data' => 'Е-мейл кому', 'field' => 'm.mail_to'),
    array('data' => 'Шаблон листа', 'field' => 'm.body'),
    array('data' => 'Змінити',),
    array('data' => 'Видалити',),
  );
  $query = db_select('my_node_message', 'm');
  $query->fields('m', array('type', 'name_from', 'mail_from', 'mail_to', 'body'));
  $query = $query->extend('PagerDefault')->limit(20);
  $query = $query->extend('TableSort')->orderByHeader($header);
  $result = $query->execute();

  $par['max_length'] = 300; //количество символов
  $par['word_boundary'] = TRUE; //обрезать только целые слова
  $par['ellipsis'] = TRUE; //добавить многоточие
  $par['html'] = TRUE; //строка может содержать html

  $rows = array();
  foreach ($result as $value) {
    $rows[] = array(
      $value->type,
      $value->name_from,
      $value->mail_from,
      views_trim_text($par, $value->mail_to),
	  views_trim_text($par, $value->body),
      l('Змінити', 'admin/myapi/my_node_message/add/'.$value->type),
      l('Видалити', 'admin/myapi/my_node_message/delete/'.$value->type),
    );
  }

  $output  = theme('table', array('header' => $header, 'rows' => $rows));
  $output .= theme('pager');

  return $output;
}


function my_node_message_add_form($form, &$form_state, $message = NULL) {
  $default_name_from = '';
  $default_title_from = '';
  $default_mail_from = '';
  $default_mail_to = '';
  $default_body = '';
  if ($message) {
	$type = $message->type;
    $default_name_from = $message->name_from;
    $default_title_from = $message->title_from;
    $default_mail_from = $message->mail_from;
    $default_mail_to = $message->mail_to;
    $default_body = $message->body;
    $form['type'] = array(
      '#type' => 'value',
      '#value' => $type,
    );
  } else {
	$node_types = node_type_get_types();
	$message_types = my_node_message_types_load();
	$types = array();
	foreach ($node_types as $node_type) {
	  if (!in_array($node_type->type, $message_types)) {
	    $types[$node_type->type] = $node_type->name;
	  }
	}
    $form['type'] = array(
	    '#type' => 'select',
		'#title' => 'Тип нод',
        '#options' => $types,
        '#default_value' => '',
    );
  }


  $form['name_from'] = array(
      '#type' => 'textfield',
      '#title' => 'Ім'я відправника',
      '#required' => TRUE,
	  '#default_value' => $default_name_from,
  );
  $form['title_from'] = array(
      '#type' => 'textfield',
      '#title' => 'Тема листа',
      '#required' => TRUE,
	  '#default_value' => $default_title_from,
  );
  $form['mail_from'] = array(
      '#type' => 'textfield',
      '#title' => 'Е-мейл від імені якого надсилається лист',
      '#required' => TRUE,
	  '#default_value' => $default_mail_from,
  );
  $form['mail_to'] = array(
      '#type' => 'textarea',
      '#title' => 'Е-мейл кому надсилати лист',
      '#required' => TRUE,
	  '#description' => 'Розділювач кома, приклад: <em>duna@duna.com, duna@duna.ru</em>',
	  '#default_value' => $default_mail_to,
  );
  $form['body'] = array(
      '#type' => 'textarea',
      '#title' => 'Шаблон листа',
      '#required' => TRUE,
	  '#description' => '',
	  '#default_value' => $default_body,
	  '#rows' => 20,
  );
  if(module_exists("token")) {
    $form['token_help'] = array(
      '#token_types' => array('site', 'node'),
      '#theme' => 'token_tree',
      '#click_insert' => TRUE,
      '#dialog' => FALSE,
    );
  }

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Зберегти',
      '#weight' => 10
  );
  return $form;
}


function my_node_message_add_form_validate($form, &$form_state) {
  $mail_from  = $form_state['values']['mail_from'];
  if (!valid_email_address($mail_from)) {
    form_set_error('mail_from', t('The e-mail address %mail is not valid.', array('%mail' => $mail_from)));
  }
}


function my_node_message_add_form_submit($form, &$form_state) {
  $type       = $form_state['values']['type'];
  $name_from  = $form_state['values']['name_from'];
  $title_from = $form_state['values']['title_from'];
  $mail_from  = $form_state['values']['mail_from'];
  $mail_to    = $form_state['values']['mail_to'];
  $body       = $form_state['values']['body'];
  db_merge('my_node_message')
  ->key(array('type' => $type))
  ->fields(array(
    'name_from' => $name_from,
    'title_from' => $title_from,
    'mail_from' => $mail_from,
    'mail_to' => $mail_to,
    'body' => $body,
	))
  ->execute();
  drupal_set_message('Рассылку обновлено.');
  drupal_goto('admin/myapi/my_node_message');
}




// Delete
function my_node_message_delete_confirm($form, &$form_state, $message) {
  $form['type'] = array(
    '#type' => 'value',
	'#value' => $message->type
  );
  return confirm_form($form,
    'Ви впевнені, що хочете видалити розсилку з <strong>type = '.$message->type.'</strong>',
    'admin/myapi/my_node_message',
    'Цю дію не можна скасувати.',
    t('Delete'),
    t('Cancel')
  );
}
function my_node_message_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    db_query('DELETE FROM {my_node_message} WHERE type = :type', array(':type' => $form_state['values']['type']));
    drupal_set_message('Розсилку видалено');
  }
  drupal_goto('admin/myapi/my_node_message');
}


























