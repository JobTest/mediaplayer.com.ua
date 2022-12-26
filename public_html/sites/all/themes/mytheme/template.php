<?php

function mytheme_node_view_alter(&$build) {
  if ($build['#entity_type'] == 'node' && ($build['#bundle'] == 'product' || $build['#bundle'] == 'aksessuar' || $build['#bundle'] == 'product_action')) {
	 //Для всех стилей вывода
	 $build['cost']['#access'] = TRUE;
     if ((float)$build['cost']['#value'] < 1) {
	   $build['cost'] = array();
     }
     unset($build['cost']['#title']);
     unset($build['sell_price']['#title']);
	// dsm($build);

	 
     switch ($build['#view_mode']) {
       case 'full': //Повна нода

	     //dsm($build);
         break;
       case 'teaser': //Тизер
	     //dsm($build);

         break;
     }
  }
  ///////////////////////////
}


function mytheme_preprocess_html(&$variables) {
  //dsm($variables);	
  $arg = arg();
  if (  (isset($arg[1]) && !isset($arg[2]) && $arg[0] == 'cart' && $arg[1] == 'checkout') || 
        (isset($arg[0]) && !isset($arg[1]) && $arg[0] == 'sravnenija') ) {
    $key = array_search('one-sidebar sidebar-first', $variables['classes_array']);
    if (!is_null($key) && !$key === false) {
      unset($variables['classes_array'][$key]);
      $variables['page']['sidebar_first'] = array();
    }	
  }
}

function mytheme_preprocess_page(&$variables, $hook) {
  $arg = arg();
  //dsm($variables); 
  
  if ($variables['is_front']) {
	$element = array(
          '#tag' => 'meta',
          '#attributes' => array( 
            'name' => 'yandex-verification',
            'content' => '7ba666d12c6d99ef', 
          ),
    );
    drupal_add_html_head($element, 'meta_yandex_verification');
	$element = array(
          '#tag' => 'meta',
          '#attributes' => array( 
            'name' => 'google-site-verification',
            'content' => 'o5mtCGLFXacHWRYZIw0cNILOb66aJ1lZwUq1NRo0Np8', 
          ),
    );
    drupal_add_html_head($element, 'meta_google_verification');
  }
  
  
  // Удалить сайдбары
  if (  (isset($arg[1]) && !isset($arg[2]) && $arg[0] == 'cart' && $arg[1] == 'checkout')  || 
        (isset($arg[0]) && !isset($arg[1]) && $arg[0] == 'sravnenija')) {
      $variables['page']['sidebar_first'] = array();
  }
  // КОНЕЦ Удалить сайдбары
     
  // Показывать хлебные крошки и дефолтные
  $breadcrumb = drupal_get_breadcrumb();
  if (count($breadcrumb) < 2 ) {
	 $breadcrumb[] = drupal_get_title();
	 drupal_set_breadcrumb($breadcrumb);
  }

  $variables['breadcrumb_view'] = true;
  if ($variables['is_front']) {
     $variables['breadcrumb_view'] = false;
  } elseif (isset($variables['node'])) {
	  $node = $variables['node'];
  }
  // КОНЕЦ Показывать хлебные крошки и дефолтные
  
  // Показывать заголовок и загловок
  $variables['title_view'] = true;
  if ($variables['is_front']) {
     $variables['title_view'] = false;
  } elseif (isset($variables['node'])) {
	  $node = $variables['node'];
  } elseif ($arg[0] == 'product' && !isset($arg[1])) {
     $variables['title_view'] = false;
  } elseif ($arg[0] == 'aksessuar' && !isset($arg[1])) {
     $variables['title_view'] = false;
  } elseif ($arg[0] == 'sravnenija' && !isset($arg[1]) ) {
     $variables['title_view'] = false;
  }
  // КОНЕЦ Показывать заголовок и загловок

  // Подзаголовок и Топ-заголовок
  $title = '';
  if (isset($variables['title']) && $variables['title']) {
	 $title = $variables['title'];
  } else {
	 $title = drupal_get_title();
  }   
  
  $variables['top_title'] = '';
  if (!$variables['is_front']) {
     $variables['top_title'] = $title;
  }
  if (isset($variables['node'])) {
	 $node = $variables['node'];
	 if ($node->type == 'page') {
       switch($node->nid){
   	     case 6:
           $title = 'Наши '.$title;
		   $variables['top_title'] = 'Контактная информация';
   	       break;
	     case 28:
           $title = 'Страница '.$title;
		   $variables['top_title'] = 'Страница не найдена';
   	       break;
	     case 29:
           $title = 'У вас '.$title;
		   $variables['top_title'] = 'У вас нет прав';
   	       break;
	     case 48:
		   $variables['top_title'] = 'Наши награды';
   	       break;
	     case 50:
	     case 135:
		   $title = '&nbsp; '.$title;
   	       break;
       }
	 } elseif ($node->type == 'news') {
           $title = 'Новости';
           $title = 'Последние '.$title;
		   $variables['top_title'] = 'Новости и события';
	 } elseif ($node->type == 'meroprijatija') {
           $title = 'Мероприятия';
           $title = 'Ближайшие '.$title;
		   $variables['top_title'] = 'Мероприятия и события';
	 } elseif ($node->type == 'video') {
           $title = 'Обзор';
           $title = 'Обзоры Продуктов';
		   $variables['top_title'] = 'Обзоры';
	 } elseif ($node->type == 'product') {
		   $variables['top_title'] = 'HD-Медиаплееры';
	 } elseif ($node->type == 'promoaction') {
		   $variables['top_title'] = 'Акционные предложения';
           $title = 'Акция '.$title;
	 } elseif ($node->type == 'action') {
		   $variables['top_title'] = 'Акционное предложение';
	 }
  } elseif ($arg[0] == 'news' && !isset($arg[1])) {
           $title = 'Последние '.$title;
		   $variables['top_title'] = 'Новости и события';
  } elseif ($arg[0] == 'meroprijatija' && !isset($arg[1])) {
           $title = 'Ближайшие '.$title;
		   $variables['top_title'] = 'Мероприятия и события';
  } elseif ($arg[0] == 'video' && !isset($arg[1])) {
           $title = 'Обзоры Продуктов';
		   $variables['top_title'] = 'Обзоры';
  } elseif ($arg[0] == 'cart' && !isset($arg[1])) {
           $title = 'Ваша '.$title;
		   $variables['top_title'] = 'Ваша корзина';
  } 
  
  if ($title) {
    $variables['title'] = myapi_title2line($title);
  } 
  
  // КОНЕЦ Подзаголовок и Топ-заголовок
  
  // Показывать addthis и addthis
  $addthis_view = TRUE;
  $url_img = '';
  if (isset($variables['node'])) {
	 $node = $variables['node'];
	 if ($node->type == 'page') {
       switch($node->nid){	  
	     case 28:
	     case 29:
           $addthis_view = FALSE;
   	       break;
       }
	 } 
	 if (isset($node->uc_product_image['und'][0]['uri'])){
		$uri = $node->uc_product_image['und'][0]['uri']; 
		$url_img = file_create_url($uri);
	 }

	 
	 
  } elseif ($arg[0] == 'cart') {
           $addthis_view = FALSE;
  }
  
  
  
  
  $addthis = '';
  if ($addthis_view) {
    $block = block_load('block', '10');
    $block = _block_render_blocks(array($block));
    $block_build = _block_get_renderable_array($block);
    $addthis = drupal_render($block_build);
	if (!$url_img) {
	  $url_img = 'http://'.$_SERVER['HTTP_HOST'].'/sites/default/files/node/product/logo-defolt.png';	
	}
	$element = array(
          '#tag' => 'meta',
          '#attributes' => array( 
            'property' => 'og:image',
            'content' => $url_img, 
          ),
    );
    drupal_add_html_head($element, 'meta_tef_for_soc_cet');
  }
  $variables['addthis_view'] = $addthis_view;
  $variables['addthis'] = $addthis;  
  // КОНЕЦ Показывать addthis и addthis
  
}

function mytheme_preprocess_node(&$variables, $hook) {
  $function = __FUNCTION__ . '_' . $variables['node']->type;
  if (function_exists($function)) {
    $function($variables, $hook);
  }
}

function mytheme_preprocess_node_page(&$variables, $hook) {
  
}


function mytheme_uc_price($variables) {
  $price_text = uc_currency_format($variables['price']);
  $price_text = str_replace('грн.', '<span class="suf">грн.</span>', $price_text);
  $output = '<span class="uc-price">' .$price_text. '</span>';
  if (!empty($variables['suffixes'])) {
    $output .= '<span class="price-suffixes">' . implode(' ', $variables['suffixes']) . '</span>';
  }
  return $output;
}






function mytheme_uc_liqpay_payment_method_title() {
  $title = 'Оплата онлайн (через Liqpay)';
  return $title;
}





function mytheme_uc_cart_checkout_review($variables) {
  $panes = $variables['panes'];
  $form = $variables['form']; 
  //dsm($panes);
  drupal_add_css(drupal_get_path('module', 'uc_cart') . '/uc_cart.css');

  $output = '<div id="review-instructions">' . filter_xss_admin(variable_get('uc_checkout_review_instructions', uc_get_message('review_instructions'))) . '</div>';

  $output .= '<table class="order-review-table">';


  foreach ($panes as $title => $data) {
	if ($title != "Рассчитать стоимость доставки") {
		$output .= '<tr class="pane-title-row">';
		$output .= '<td colspan="2">' . $title . '</td>';
		$output .= '</tr>';
		if (is_array($data)) {
		  foreach ($data as $row) {
			if (is_array($row)) {
			  if (isset($row['border'])) {
				$border = ' class="row-border-' . $row['border'] . '"';
			  }
			  else {
				$border = '';
			  }
			  $output .= '<tr' . $border . '>';
			  $output .= '<td class="title-col">' . $row['title'] . ':</td>';
			  $output .= '<td class="data-col">' . $row['data'] . '</td>';
			  $output .= '</tr>';
			}
			else {
			  $output .= '<tr><td colspan="2">' . $row . '</td></tr>';
			}
		  }
		}
		else {
		  $output .= '<tr><td colspan="2">' . $data . '</td></tr>';
		}
	}
  }

  $output .= '<tr class="review-button-row">';
  $output .= '<td colspan="2">' . drupal_render($form) . '</td>';
  $output .= '</tr>';

  $output .= '</table>';

  return $output;
}



function mytheme_menu_link__main_menu(array $variables) {
  //dsm($variables);

  $element = $variables['element'];
  $sub_menu = '';
  if ($element['#href'] == '<front>') {
	$sub_menu .= '<ul class="menu">';
	$terms = taxonomy_get_tree(6);
	$query_yes = "SELECT n.nid
	          FROM {node} n 
			  INNER JOIN {field_data_field_category} c ON n.nid = c.entity_id AND (c.entity_type = 'node' AND c.deleted = '0')
			  WHERE c.field_category_tid = :tid AND n.status = 1 AND n.type = 'product' LIMIT 0, 1";	
	foreach ($terms as $term) {
      $result_yes = db_query($query_yes, array(':tid' => $term->tid)); 
	  if ($result_yes->fetchField()) {
	    $sub_menu .= '<li class="menu__item">'.l($term->name, 'taxonomy/term/'.$term->tid).'</li>';
	  }
	}
	$sub_menu .= '</ul>';
  } elseif ($element['#below']) {
    $sub_menu = drupal_render($element['#below']);
  }
  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}