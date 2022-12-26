<?php

class MyoptionInlineEntityFormController extends EntityInlineEntityFormController {

  public function defaultLabels() {
    $labels = array(
      'singular' => t('myoption'),
      'plural' => t('myoptions'),
    );
    return $labels;
  }

  public function tableFields($bundles) {
    $fields = array();
    $info = entity_get_info($this->entityType);
    $metadata = entity_get_property_info($this->entityType);
    $label_key = $info['entity keys']['label'];
    $fields[$label_key] = array(
      'type' => 'property',
      'label' => $metadata ? $metadata['properties'][$label_key]['label'] : t('Label'),
      'weight' => 1,
    );
    return $fields;
  }

  
  public function entityForm($entity_form, &$form_state) {
    $myoption = $entity_form['#entity'];
    global $user;
	$time = time();
    $defaults = array(
      'uid' => $user->uid,
      'created' => $time,
      'changed' => $time,
      'tid' => NULL,
    );
    foreach ($defaults as $key => $value) {
      if (!isset($myoption->$key)) {
        $myoption->$key = $value;
      }
    }
	$myoption->changed = $time;
    $langcode = entity_language('myoption', $myoption);
    field_attach_form('myoption', $myoption, $entity_form, $form_state, $langcode);
    return $entity_form;
  }
}
