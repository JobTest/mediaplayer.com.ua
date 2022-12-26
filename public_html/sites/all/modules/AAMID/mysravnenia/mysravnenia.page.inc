<?php

function mysravnenia_page_callback() {
   drupal_add_library('system', 'drupal.ajax');
   drupal_add_library('system', 'jquery.form');	
   $output = '<div id="mysravnenia-page-content">
             '.mysravnenia_page_content().'
              </div>';
  return $output;	
}

function mysravnenia_page_content($cat_dedault = '') {

  drupal_page_is_cacheable(FALSE);	
  global $user;
  $uid = intval($user->uid);
  $sid = flag_get_sid();
  $query = db_select('node', 'n');
  $query->innerJoin('flagging', 'f', "n.nid = f.entity_id AND (f.fid = '1' AND f.uid = '".$uid."' AND f.sid = '".$sid."')");
  $query->fields('n', array('nid'));
  $query->condition('n.status', 1);
  $query->condition('n.type', 'product');
  $query->orderBy('n.created', 'DESC');
  $result = $query->execute();
  $rows = array();
  $nids = array();
  if (!$cat_dedault) {
    if (isset($_GET['cats'])) {
	  $cat_dedault = $_GET['cats'];  
    } 
  }
  $cats = array();
  foreach ($result as $value) {
	$nid = $value->nid;
	$nids[$nid] = $nid;
	$node = node_load($nid);
	if (!$node->nid) continue;
	$course = mycart_get_course_usd($node);
	
    //
    $categorie_tid = $node->field_category['und'][0]['tid'];
	if (!$cat_dedault) {
		$cat_dedault = $categorie_tid;
    }
    $categorie_term = taxonomy_term_load($categorie_tid);
    $cats[$categorie_tid] = $categorie_term;	
	$cats[$categorie_tid]->count += 1; 
	if ($categorie_tid != $cat_dedault) {
		continue;
	}
	//
	$form = drupal_get_form('uc_product_add_to_cart_form_'.$nid, $node);
    $add_to_cart = drupal_render($form);
	if (!$add_to_cart) continue;
	//
	
	//
	$rows[0]['title'] = array(
	   'label' => '<span>Сравнение </span> товаров',
	   'weight' => -9999, 
	   'show' => TRUE, 
	);
	$sort['title'] = -9999;
	$rows[$nid]['title'] = l(myapi_title2line($node->title), 'node/'.$nid, array('html' => TRUE));
	$image = '';
	if(isset($node->uc_product_image['und'][0]['uri'])) {
		$image = theme('image_style', array(
                'style_name' => 'sravnenija', 
				'path' => $node->uc_product_image['und'][0]['uri'],
				'alt' => $node->title, 
                'title' => $node->title,
		));	
		$image = l($image, 'node/'.$nid, array('html' => TRUE));
	}
	//
	$rows[0]['foto'] = array(
	   'label' => 'Фото',
	   'weight' => -9998, 
	   'show' => TRUE, 
	);
	$sort['foto'] = -9998;
	$rows[$nid]['foto'] = '<div class="linka-foto"><div class="linka">'.l('Удалить из сравнения', 'ajax/sravnenija/flag/delete/'.$nid.'/'.$cat_dedault, array(
       'attributes' => array(
         'class' => array('use-ajax'),
       )
    )).'</div><div class="image">'.$image.'</div></div>';
	//
	$rows[0]['sell_price'] = array(
	   'label' => 'Стоимость',
	   'weight' => -9997, 
	   'show' => TRUE, 
	);
	$sort['sell_price'] = -9997;
	$rows[$nid]['sell_price'] = theme('uc_price', array('price' => $node->sell_price*$course));
	//
	$rows[0]['add_to_cart'] = array(
	   'label' => '',
	   'weight' => -9996, 
	   'show' => TRUE, 
	);
	$sort['add_to_cart'] = -9996;
	$rows[$nid]['add_to_cart'] = $add_to_cart;
	//
	if (isset($node->field_myoption['und'][0]['target_id']) && $node->field_myoption['und'][0]['target_id']){
      $myoption_id = $node->field_myoption['und'][0]['target_id'];
      $myoption = myoption_load($myoption_id);
	  if (isset($myoption->tid)) {
	    $myoption_fields = field_info_instances('myoption', $myoption->tid);
	    foreach ($myoption as $field_name => $line) {
	      if (isset($myoption_fields[$field_name])) {
		    $rows[0][$field_name] = array(
	          'label' => $myoption_fields[$field_name]['label'],
	          'weight' => (int) $myoption_fields[$field_name]['display']['default']['weight'],
	        );
		    $sort[$field_name] = (int) $myoption_fields[$field_name]['display']['default']['weight'];
		    if (!isset($line['und'][0]['value'])) {
		  	  $line['und'][0]['value'] = ''; 
		    } else {
			  $rows[0][$field_name]['show'] = TRUE;
			}
	        $rows[$nid][$field_name] = $line['und'][0]['value'];
	      }	
        }
	  }
	}

    //
	$rows[0]['sell_price2'] = array(
	   'label' => 'Стоимость',
	   'weight' => 9998, 
	   'show' => TRUE, 
	);
	$sort['sell_price2'] = 9998;
	$rows[$nid]['sell_price2'] = theme('uc_price', array('price' => $node->sell_price*$course));
	//
	$rows[0]['add_to_cart2'] = array(
	   'label' => '',
	   'weight' => 9999, 
	   'show' => TRUE, 
	);
	$sort['add_to_cart2'] = 9999;
	$rows[$nid]['add_to_cart2'] = $add_to_cart;
	//
  }
  ////////////////////////////////
  $table = '';
  if ($rows) {
      $table_rows = array();
	  $i = 0; 
	  $header = $rows[0];
      array_multisort($sort, SORT_NUMERIC, $header);
	  foreach ($header as $name => $value) {
		if (!isset($value['show']) || !$value['show']) continue;
		$j = 0;  
		foreach ($rows as $row) {
		  if (!isset($row[$name])) $row[$name] = '';
		  if ($j == 0 && $row[$name]) $row[$name] = '<span class="col-1">'.$row[$name]['label'].'</span>';	
		  $table_rows[$i][] = $row[$name];
		  $j++;
		}
		$i++;
	  }
	  $table = theme('table', array(
		'header' => array(),
		'rows' => $table_rows,
	  )); 
  }
  ////////////////////////////////
  $form = drupal_get_form('mysravnenia_form_select', $cat_dedault);
  $select = drupal_render($form);
  //////////////////////////////////
  $output = '';
  $output .= '<div class="top-dani">';
  $output .= $select;
  if (count($cats) > 1) {
    $links = array();
	foreach ($cats as $cat) {
	  if ($cat->tid == $cat_dedault) {
		 $selected	= array('selected'); 
	  }	else {
		 $selected	= array(); 
	  }
	  $query = array('cats' => $cat->tid); 
	  $links[] = array(
	    'data' => l('<strong>'.$cat->name.' <span>('.$cat->count.')</span></strong>', 'sravnenija', array('html'=>TRUE, 'query' => $query)),
		'tabindex' => '-1',
		'class' => $selected,
	  );
	}
    $output .= '<div class = "mysravnenia_filter">'.theme('item_list', array('items' => $links, 'attributes' => array('class' => array('horizontal-tabs-list')))).'</div>';
  }
  $output .= '<div style = "clear: both;"></div>';
  $output .= '</div>';
  
  $output .= '<div id="mysravnenia_table">'.$table.'</div>';
  return $output;	
}

function mysravnenia_form_select($form, &$form_state, $cat) {
  global $user;
  $uid = intval($user->uid);
  $sid = flag_get_sid();
  $query = db_select('node', 'n');
  $query->leftJoin('flagging', 'f', "n.nid = f.entity_id AND (f.fid = '1' AND f.uid = '".$uid."' AND f.sid = '".$sid."')");
  $query->innerJoin('field_data_field_category', 'c', "n.nid = c.entity_id AND (c.entity_type = 'node' AND c.deleted = '0')");
  $query->fields('n', array('nid', 'title'));
  $query->condition('n.status', 1);
  $query->condition('n.type', 'product');
  $query->condition('c.field_category_tid', $cat);
  $query->isNull('f.entity_id');
  $query->orderBy('n.created', 'DESC');
  $result = $query->execute();
  $options = array();
  $options[0] = 'Добавить модель';
  foreach ($result as $value) {
	$options[$value->nid] = $value->title; 
  }
  $form['cat'] = array(
    '#type' => 'value', 
	'#value' => $cat,
  );
  if (count($options)>1) {
     $form['nid'] = array(
       '#type' => 'select',
       '#title' => '',
       '#options' => $options,
       '#default_value' => 0,
       '#description' => '',
	   '#ajax' => array(
             'callback' => 'mysravnenia_form_select_ajax',
	   ),  	  
     );
   }
   return $form;
}


function mysravnenia_flag_ajax_callback($nid, $cat) {
    if (!$nid) return array();
	flag('unflag', 'sravnenia', $nid);
	$content = mysravnenia_page_content($cat);
	$commands[] = ajax_command_html('#mysravnenia-page-content', $content);
	return array('#type' => 'ajax', '#commands' => $commands);		
}

function mysravnenia_form_select_ajax($form, $form_state) {
	$cat = $form_state['values']['cat'];
	$nid = $form_state['values']['nid'];
	if (!$nid) return ;
	flag('flag', 'sravnenia', $nid);
	$content = mysravnenia_page_content($cat);
	$commands[] = ajax_command_html('#mysravnenia-page-content', $content);
	return array('#type' => 'ajax', '#commands' => $commands);	
}