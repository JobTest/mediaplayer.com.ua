<?php

function mybatch_export_hotline_form($form, &$form_state) {
  $form['title'] = array(
    '#markup' => '<p>Експорт товарів для hotline в файл xls.</p>',
  );
  $form['action'] = array(
    '#type' => 'actions',
  );
  $form['action']['submit'] = array(
    '#type' => 'submit',
	'#value' => 'Експорт',
  );
  return $form;
}


function mybatch_export_hotline_form_submit($form, &$form_state) {
  $batch = array(
    'operations' => array(
	  array('mybatch_export_hotline_operation', array()),
	),
    'finished' => 'mybatch_export_hotline_finished',
    'title' => 'Експорт',
    'init_message' => 'Ініціалізація експорту.',
    'progress_message' => 'Експорт.',
    'error_message' => 'Помилка експорту',
	'file' => drupal_get_path('module', 'mybatch') . '/includes/mybatch.hotline_export.inc',
  );
  batch_set($batch);
}



function mybatch_export_hotline_operation(&$context){
  $limit = 20;
  $context['finished'] = 0;
  module_load_include('inc','phpexcel');
  if (!isset($context['sandbox']['file'])) {
    $filename = 'hotline_' . date('d_m_Y', REQUEST_TIME) . '.xls';
	$directory = variable_get('file_public_path', conf_path() . '/files') . '/mybatch/export';
	$file_path = $directory . $filename;
	file_prepare_directory($directory, FILE_MODIFY_PERMISSIONS | FILE_CREATE_DIRECTORY);
	unlink($file_path);
    $context['sandbox']['file'] = $file_path;
    $context['sandbox']['subscribers'] = mybatch_export_hotline_list_subscribers();
    $context['sandbox']['subscribers_total'] = count($context['sandbox']['subscribers']) - 1;
    $context['results']['filename'] = $filename;
    $context['results']['file'] = $file_path;
	$context['results']['count'] = 0;
  }

  $data = array();
  for ($i = 0; $i < $limit; $i++) {
    $number_remaining = count($context['sandbox']['subscribers']);
    if ($number_remaining) {
      $id = $context['sandbox']['subscribers'][$context['results']['count']];
      $data[] = mybatch_export_hotline_data_row($id);
      unset($context['sandbox']['subscribers'][$context['results']['count']]);
      $context['results']['count']++;
      $context['finished'] = $context['results']['count'] / $context['sandbox']['subscribers_total'];
    } else {
      $context['finished'] = 1;
      break;
    }
  }

  $options = array('format' => 'xls');
  $headers = mybatch_export_hotline_data_row();
  $result = phpexcel_export($headers, $data, $context['sandbox']['file'], $options);

  $context['message'] = t('Exported @count of @total subscribers.', array(
    '@count' => $context['results']['count'],
    '@total' => $context['sandbox']['subscribers_total'],
  ));
}


function mybatch_export_hotline_finished($success, $results, $operations) {
  if ($success) {
	$path = $results['file'];
    $message  = $results['count'] . ' оновлено.</br>';
	$message .= l('Завантажити', $path) . '<br />';
	$_POST['path'] = $path;

  } else {
    $error_operation = reset($operations);
    $message = 'An error occurred while processing ' . $error_operation[0] . ' with arguments :' . print_r($error_operation[0], TRUE);
  }
  drupal_set_message($message, 'warning');
}






function mybatch_export_hotline_list_subscribers() {
  $query = db_select('node', 'n');
  $query->distinct();
  $query->leftJoin('uc_products', 'p', "n.vid = p.vid");
  $query->fields('n', array('nid'));
  $query->condition('n.type', 'product');
  $query->condition('n.status', 1);
  $query->condition('p.sell_price', 0, '>');
  $query->orderBy('n.nid', 'ASC');
  // $query->range(0, 5); //
  $result = $query->execute();
  $nids = $result->fetchCol();
  return $nids;
}




function mybatch_export_hotline_data_row($nid = NULL) {
  if (!$nid) {
	return array(
      // 'Nid',
	  'Категорія товару',
	  'Виробник',
	  'Найменування товару',
	  'Опис товару',
	  'Ціна розн., грн.',
	  'Гарантія',
	  'Наявність',
	  'Посилання на товар',
	  'Посилання на зображення',
    );
  }

  $node = node_load($nid);
  $node_wrapper = entity_metadata_wrapper('node', $node);

  $term_hotline = $node_wrapper->field_product_category_hotline->value();
  $category = '';
  if (!empty($term_hotline->name)) {
    $category = $term_hotline->name;
  }

  $term_brand = $node_wrapper->field_product_brand->value();
  $brand = '';
  if (!empty($term_brand->name)) {
    $brand = $term_brand->name;
  }

  $course = mycart_get_course_usd($node);
  $price = $node->sell_price * $course;

  $warranty = '';
  $myoption = $node_wrapper->field_myoption->value();
  if (!empty($myoption->field_s_warranty)) {
	$myoption_wrapper = entity_metadata_wrapper('myoption', $myoption);
	$warranty = $myoption_wrapper->field_s_warranty->value();
  }

  $nalichie = $node_wrapper->field_product_nalichie->value();
  if (!empty($nalichie)) {
	$nalichie = 'є на складі';
  }
  else {
	$nalichie = 'немає на складі';
  }

  $image_url = '';
  $images = $node_wrapper->uc_product_image->value();
  if (!empty($images[0]['uri'])) {
	$image_url = file_create_url($images[0]['uri']);
  }

  return array(
    // $node->nid,
	$category,
	$brand,
	$node_wrapper->field_product_name_hotline->value(),
	$node_wrapper->field_product_desc_hotline->value(),
	round($price),
	$warranty,
	$nalichie,
	url('node/' . $nid, array('absolute' => TRUE)),
	$image_url,
  );
}






function mybatch_phpexcel_hotline_export() {
  module_load_include('inc','phpexcel');
  $directory = 'public://mybatch/export';
  if (!file_prepare_directory($directory, FILE_MODIFY_PERMISSIONS | FILE_CREATE_DIRECTORY)) {
    drupal_set_message(t('Failed to create %directory.', array('%directory' => $directory)), 'error');
    return FALSE;
  }
  $filename = 'hotline_' . date('d_m_Y', REQUEST_TIME) . '.xls';
  $path = $directory . '/' . $filename;

  $options = array('format' => 'xls');
  $headers = array('Header 1', 'Header 2');
  $data = array(
	array('Data 1.1', 'Data 1.2'),
	array('Data 2.1', 'Data 2.2'),
  );
  $result = phpexcel_export($headers, $data, $path, $options);


  // Формируем заголовки, для того, чтобы сразу же скачать созданный файл.
  $http_headers = array(
    'Content-Type' => 'application/vnd.ms-excel',
    'Content-Disposition' => 'attachment; filename="' . drupal_basename($path) . '"',
    'Content-Length' => filesize($path),
  );
  if (strpos($_SERVER['HTTP_USER_AGENT'], 'MSIE')) {
    $http_headers['Cache-Control'] = 'must-revalidate, post-check=0, pre-check=0';
    $http_headers['Pragma'] = 'public';
  }
  else {
    $http_headers['Pragma'] = 'no-cache';
  }
  file_transfer($path, $http_headers);

}











































































