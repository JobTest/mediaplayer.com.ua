<div id="myoption-<?php print $myoption->id; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
 
  <?php print render($title_prefix); ?>
  <?php print render($title_suffix); ?>


  <div class="content"<?php print $content_attributes; ?>>
    <?php 
//dsm($content);
	//print drupal_render($ty);
//	dsm(_field_invoke('', 'myoption', $myoption));
	
	//$output = _field_invoke_default('view', 'myoption', $myoption);
	//$entity = $myoption;
//	_field_invoke('view', 'myoption', $entity);
	//dsm($entity);
	
	
	
	//if ($field = field_info_field($field_name)) {
	
	/*	
  $options = array(
    'default' => FALSE,
    'deleted' => FALSE,
    'language' => NULL,
  );
  list(, , $bundle) = entity_extract_ids('myoption', $myoption);
  $instances = _field_invoke_get_instances('myoption', $bundle, $options);
  dsm($instances);
  foreach ($instances as $instance) {
    $field = field_info_field_by_id($instance['field_id']);
	dsm($field);
    $field_name = $field['field_name'];
	print render(field_view_field('myoption', $myoption, $field_name, array()));

	
	
	
  }
	*/
	
	
	/*
	
  if (!isset($langcode)) {
    global $language;
    $langcode = $language->language;
  } 
  $view_mode = 'full';
  field_attach_prepare_view('myoption', array($myoption->id => $myoption), $view_mode, $langcode);	
  $a = field_attach_view('myoption', $myoption, $view_mode, $langcode);
dsm($myoption);
	*/
	
	
	print render($content); ?>
  </div>
 
</div>