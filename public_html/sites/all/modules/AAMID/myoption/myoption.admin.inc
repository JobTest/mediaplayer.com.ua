<?php

function myoption_settings_form() {
  $vocabularies = taxonomy_get_vocabularies();
  $options = array();
  $options[] = '-- Не выбрано --';
  foreach ($vocabularies as $vocabularie) {
	 $options[$vocabularie->vid] = $vocabularie->name;
  }
  $form['settings']['myoption_taxonomy_vid'] = array(
    '#type' => 'select',
    '#title' => 'Словник категорій',
    '#options' => $options,
    '#default_value' => variable_get('myoption_taxonomy_vid', ''),
  );
  return system_settings_form($form);
}



function myoption_admin_myoptions($term) {
   $vid = variable_get('myoption_taxonomy_vid', '');
   if ($term->vid != $vid) {
	  drupal_not_found();
   }
   $header = array(
    'id' => array('data' => t('Id'), 'field' => 'o.id'),
    'author' => t('Author'),
    'created' => array('data' => t('Created'), 'field' => 'o.created', 'sort' => 'desc'),
    'changed' => array('data' => t('Updated'), 'field' => 'o.changed', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );
  $query = db_select('myoption', 'o')
           ->condition('o.tid', $term->tid)
		   ->extend('PagerDefault')
		   ->extend('TableSort');
  $myoption_ids = $query->fields('o', array('id'))
    ->limit(50)
    ->orderByHeader($header)
    ->execute()
    ->fetchCol();
  $myoptions = myoption_load_multiple($myoption_ids);
  $destination = drupal_get_destination();
  $rows = array();
  foreach ($myoptions as $myoption) {
    if ($myoption->uid) {
      $account = user_load($myoption->uid);
    } else {
      $account = variable_get('anonymous', t('Anonymous'));
    }
    $rows[$myoption->id] = array(
      'id' => $myoption->id,
      'author' => theme('username', array('account' => $account)),
      'created' => format_date($myoption->created, 'short'),
      'changed' => format_date($myoption->changed, 'short'),
    );

    // Формируем список доступных операций над текущим продуктом.
    $operations = array();
    if (myoption_access('update', $myoption, NULL, 'myoption')) {
      $operations['edit'] = array(
        'title' => t('edit'),
        'href' => 'myoption/'.$myoption->id.'/edit',
        'query' => $destination,
      );
    }
    if (myoption_access('delete', $myoption, NULL, 'myoption')) {
      $operations['delete'] = array(
        'title' => t('delete'),
        'href' => 'myoption/'.$myoption->id.'/delete',
        'query' => $destination,
      );
    }

    $rows[$myoption->id]['operations'] = array();
    if (count($operations) > 1) {
      // Выводим операции в виде списка.
      $rows[$myoption->id]['operations'] = array(
        'data' => array(
          '#theme' => 'links__myoption_operations',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
        ),
      );
    } elseif (!empty($operations)) {
      // Выводим первую и единственную операцию.
      $link = reset($operations);
      $rows[$myoption->id]['operations'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => $link['title'],
          '#href' => $link['href'],
          '#options' => array('query' => $link['query']),
        ),
      );
    }
  }

  $page = array();
  $page['myoptions'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No myoptions available.'),
  );

  $page['pager'] = array('#markup' => theme('pager'));
  return $page;
}

