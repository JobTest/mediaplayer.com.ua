<?php

define('FEEDS_MYOPTION_BATCH_SIZE', 50);

/**
 * Creates myoptions from feed items.
 */
class FeedsMyoptionProcessor extends FeedsProcessor {
  /**
   * Define entity type.
   */
  public function entityType() {
    return 'myoption';
  }

  /**
   * Implements parent::entityInfo().
   */
  protected function entityInfo() {
    $info = parent::entityInfo();
    $info['label plural'] = t('Myoptions');
    return $info;
  }

  /**
   * Creates a new myoption in memory and returns it.
   */
  protected function newEntity(FeedsSource $source) {
    $myoption = new stdClass();
    $myoption->id = 0;
    $myoption->uid = $this->config['author'];
    $myoption->tid = $this->bundle(); // в myoption tid и есть bundle
    $myoption->created = REQUEST_TIME;
    $myoption->changed = REQUEST_TIME;
	$myoption->is_new = TRUE;
    return $myoption;
  }

  /**
   * Loads an existing myoption.
   */
  protected function entityLoad(FeedsSource $source, $id) {
    return myoption_load($id);
  }

  /**
   * Validates a myoption.
   */
  protected function entityValidate($myoption) {
    if (!isset($myoption->uid) || !is_numeric($myoption->uid)) {
       $myoption->uid = $this->config['author'];
    }
  }
  

  /**
   * Save a myoption.
   */
  public function entitySave($myoption) {
    myoption_save($myoption);
  }

  /**
   * Delete multiple myoptions.
   */
  protected function entityDeleteMultiple($ids) {
    myoption_delete_multiple($ids);
  }

  /**
   * Implement expire().
   */
  public function expire($time = NULL) {
    if ($time === NULL) {
      $time = $this->expiryTime();
    }
    if ($time == FEEDS_EXPIRE_NEVER) {
      return;
    }
    $myoptions = db_query_range("
	   SELECT o.id FROM {myoption} o 
	   JOIN {feeds_item} fi ON fi.entity_type = 'myoption' AND o.id = fi.entity_id 
	   WHERE fi.id = :id AND o.created < :created
	   ", 0, variable_get('feeds_myoption_batch_size', FEEDS_MYOPTION_BATCH_SIZE), array(':id' => $this->id, ':created' => REQUEST_TIME - $time));
    $ids = array();
    foreach ($myoptions as $myoption) {
      $ids[$myoption->id] = $myoption->id;
    }
    $this->entityDeleteMultiple($ids);
    if (db_query_range("
	    SELECT 1 FROM {myoption} o 
		JOIN {feeds_item} fi ON fi.entity_type = 'myoption' AND o.id = fi.entity_id 
		WHERE fi.id = :id AND o.created < :created
		", 0, 1, array(':id' => $this->id, ':created' => REQUEST_TIME - $time))->fetchField()) {
      return FEEDS_BATCH_ACTIVE;
    }
    return FEEDS_BATCH_COMPLETE;
  }


  /**
   * Return expiry time.
   */
  public function expiryTime() {
    return $this->config['expire'];
  }

  /**
   * Override parent::configDefaults().
   */
  public function configDefaults() {
    $defaults = parent::configDefaults();
    $defaults['expire'] = FEEDS_EXPIRE_NEVER;
    $defaults['author'] = 0;
    return $defaults;
  }

  /**
   * Override parent::configForm().
   */
  public function configForm(&$form_state) {
    $form = parent::configForm($form_state);
    $author = user_load($this->config['author']);
    $form['author'] = array(
      '#type' => 'textfield',
      '#title' => t('Author'),
      '#description' => t('Select the author of the myoptions to be created - leave empty to assign "anonymous".'),
      '#autocomplete_path' => 'user/autocomplete',
      '#default_value' => empty($author->name) ?  'anonymous' : check_plain($author->name),
    );
    $period = drupal_map_assoc(array(FEEDS_EXPIRE_NEVER, 3600, 10800, 21600, 43200, 86400, 259200, 604800, 604800 * 4, 604800 * 12, 604800 * 24, 31536000), 'feeds_format_expire');
    $form['expire'] = array(
      '#type' => 'select',
      '#title' => t('Expire myoptions'),
      '#options' => $period,
      '#description' => t('Select after how much time myoptions should be deleted. The myoption\'s published date will be used for determining the myoption\'s age, see Mapping settings.'),
      '#default_value' => $this->config['expire'],
    );
    return $form;
  }

  /**
   * Override parent::configFormValidate().
   */
  public function configFormValidate(&$values) {
    if ($author = user_load_by_name($values['author'])) {
      $values['author'] = $author->uid;
    } else {
      $values['author'] = 0;
    }
  }

  /**
   * Override setTargetElement to operate on a target item that is a myoption.
   */
  public function setTargetElement(FeedsSource $source, $target_myoption, $target_element, $value) {
    switch ($target_element) {
      default:
        parent::setTargetElement($source, $target_myoption, $target_element, $value);
        break;
    }
  }

  /**
   * Return available mapping targets.
   */
  public function getMappingTargets() {
	$targets = parent::getMappingTargets();  
    $targets['id'] = array(
      'name' => t('myoption ID'),
      'description' => t('The id of the myoption. NOTE: use this feature with care, myoption ids are usually assigned by Drupal.'),
      'optional_unique' => TRUE,
    );
    $targets['uid'] = array(
      'name' => t('User ID'),
      'description' => t('The Drupal user ID of the myoption author.'),
    );
    $targets['created'] = array(
      'name' => t('Published date'),
      'description' => t('The UNIX time when a myoption has been published.'),
    );
	

	
	
    // Let other modules expose mapping targets.
    self::loadMappers();
    $entity_type = $this->entityType();
    $bundle = $this->bundle();
    drupal_alter('feeds_processor_targets', $targets, $entity_type, $bundle);

    return $targets;
  }

  /**
   * Get cid of an existing feed item myoption if available.
   */
  protected function existingEntityId(FeedsSource $source, FeedsParserResult $result) {
    if ($id = parent::existingEntityId($source, $result)) {
      return $id;
    }

    // Iterate through all unique targets and test whether they do already
    // exist in the database.
    foreach ($this->uniqueTargets($source, $result) as $target => $value) {
      switch ($target) {
        case 'id':
          $oid = db_query("SELECT id FROM {myoption} WHERE id = :id", array(':id' => $value))->fetchField();
          break;
      }
      if (isset($oid) && $oid) {
        // Return with the first oid found.
        return $oid;
      }
    }
    return 0;
  }
}
