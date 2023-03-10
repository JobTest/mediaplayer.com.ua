<?php

/**
 * @file
 * Contains the administrative page and form callbacks.
 *
 * @author Jim Berry ("solotandem", http://drupal.org/user/240748)
 */

/**
 * Form constructor for the module settings form.
 *
 * @see google_tag_settings_form_validate()
 * @see google_tag_settings_form_submit()
 *
 * @ingroup forms
 */
function google_tag_settings_form($form, &$form_state) {
  module_load_include('inc', 'google_tag', 'includes/variable');

  // Gather data.
  // @todo Set this on validation errors.
  $default_tab = !empty($form_state['default_tab']) ? $form_state['default_tab'] : '';

  $description = t('On this and the next two tabs, specify the conditions on which the GTM JavaScript snippet will either be included in or excluded from the page response, thereby enabling or disabling tracking and other analytics.');
  $description .= t(' All conditions must be satisfied for the snippet to be included. The snippet will be excluded if any condition is not met.<br /><br />');
  $description .= t(' On this tab, specify the path condition.');

  $groups = array(
    'general' => array('title' => t('General'), 'collapse' => FALSE),
    'path' => array('title' => t('Page paths'), 'description' => $description),
    'role' => array('title' => t('User roles'), 'description' => t('On this tab, specify the user role condition.')),
    'status' => array('title' => t('Response statuses'), 'description' => t('On this tab, specify the page response status condition.')),
    'advanced' => array('title' => t('Advanced')),
  );

  // Build form elements.
  $form['tabs'] = array(
    '#type' => 'vertical_tabs',
    '#default_tab' => $default_tab ? $default_tab : 'edit-general',
    '#attributes' => array('class' => array('google-tag')),
    '#attached' => array(
      'css' => array(drupal_get_path('module', 'google_tag') . '/css/google_tag.admin.css'),
      'js' => array(drupal_get_path('module', 'google_tag') . '/js/google_tag.admin.js'),
    ),
  );
  foreach ($groups as $group => $items) {
    $form['tabs'][$group] = google_tag_fieldset($group, $items);
  }
  $form['#after_build'] = array('google_tag_settings_form_after_build');

  return system_settings_form($form);
}

/**
 * Fieldset builder for the module settings form.
 */
function google_tag_fieldset($group, $items) {
  // Gather data.
  $function = "_google_tag_variable_info_$group";
  $variables = $function(array());
  $items += array('description' => '', 'collapse' => TRUE);

  // Build form elements.
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => $items['title'],
    '#description' => $items['description'],
    '#collapsible' => $items['collapse'],
    '#collapsed' => $items['collapse'],
    '#tree' => FALSE,
  );
  $fieldset += google_tag_form_elements($variables);

  return $fieldset;
}

/**
 * Returns form elements from variable definitions.
 *
 * @param array $variables
 *   Associative array of variable definitions.
 *
 * @return array
 *   Associative array of form elements.
 */
function google_tag_form_elements($variables) {
  static $keys = array(
    'type' => '#type',
    'title' => '#title',
    'description' => '#description',
    'options' => '#options',
    'default' => '#default_value',
  );
  $elements = array();
  foreach ($variables as $name => $variable) {
    $element = array();
    foreach ($keys as $key => $property) {
      if (isset($variable[$key])) {
        $element[$property] = $variable[$key];
      }
    }
    $element['#type'] = google_tag_form_element_type($element['#type']);
    $element['#default_value'] = variable_get($name, $variable['default']);
    $element += isset($variable['element']) ? $variable['element'] : array();
    $elements[$name] = $element;
  }

  return $elements;
}

/**
 * Converts variable type to form element type.
 *
 * @param string $type
 *   Variable type.
 *
 * @return string
 *   Form element type.
 */
function google_tag_form_element_type($type) {
  static $keys = array(
    'string' => 'textfield',
    'select' => 'radios',
    'text' => 'textarea',
    'options' => 'checkboxes',
    'boolean' => 'checkbox',
  );
  return isset($keys[$type]) ? $keys[$type] : 'textfield';
}

/**
 * Element after build callback for google_tag_settings_form().
 */
function google_tag_settings_form_after_build($element, &$form_state) {
  // This will follow:
  // variable_realm_variable_settings_form_submit
  // variable_settings_form_submit
  $element['#submit'][] = 'google_tag_settings_form_submit';
  return $element;
}

/**
 * Form validation handler for google_tag_settings_form().
 */
function google_tag_settings_form_validate($form, &$form_state) {
  $values = &$form_state['values'];

  // Trim the text values.
  $values['google_tag_container_id'] = trim($values['google_tag_container_id']);
  $values['google_tag_data_layer'] = trim($values['google_tag_data_layer']);
  google_tag_text_clean($values['google_tag_path_list']);
  google_tag_text_clean($values['google_tag_status_list']);
  google_tag_text_clean($values['google_tag_whitelist_classes']);
  google_tag_text_clean($values['google_tag_blacklist_classes']);

  // Replace all types of dashes (n-dash, m-dash, minus) with a normal dash.
  $values['google_tag_container_id'] = str_replace(array('???', '???', '???'), '-', $values['google_tag_container_id']);
  $values['google_tag_environment_id'] = str_replace(array('???', '???', '???'), '-', $values['google_tag_environment_id']);

  if (!preg_match('/^GTM-\w{4,}$/', $values['google_tag_container_id'])) {
    // @todo Is there a more specific regular expression that applies?
    // @todo Is there a way to "test the connection" to determine a valid ID for
    // a container? It may be valid but not the correct one for the website.
    form_set_error('google_tag_container_id', t('A valid container ID is case sensitive and formatted like GTM-xxxxxx.'));
  }
  if ($values['google_tag_include_environment'] && !preg_match('/^env-\d{1,}$/', $values['google_tag_environment_id'])) {
    form_set_error('google_tag_environment_id', t('A valid environment ID is case sensitive and formatted like env-x.'));
  }
  if ($message = _google_tag_data_layer_verify($values['google_tag_data_layer'])) {
    form_set_error('google_tag_data_layer', $message);
  }
  if ($values['google_tag_include_classes']) {
    if (empty($values['google_tag_whitelist_classes']) && empty($values['google_tag_blacklist_classes'])) {
      form_set_error('google_tag_include_classes', t('Enter listed classes in at least one field, or uncheck the box.'));
      form_set_error('google_tag_whitelist_classes', '');
      form_set_error('google_tag_blacklist_classes', '');
    }
  }
}

/**
 * Form submission handler for google_tag_settings_form().
 */
function google_tag_settings_form_submit($form, &$form_state) {
  if (module_exists('variable_realm') && module_exists('variable_store')) {
    // When this handler runs, $GLOBALS['conf'] will have been updated by the
    // form submitted values (this includes realm and non-realm specific
    // variables). However, the variable stores maintained by variable_realm
    // will not reflect these changes. To counteract this behaviour refresh the
    // internal variable stores.
    //
    // Example: Assume a language realm with 'en' (default) and 'fr' keys.
    //
    // Because of how variable_realm_switch() works, the values for one or more
    // realm:key pairs will include the OLD global (i.e. not realm-specific)
    // values in effect at form build not the NEW values included with the form
    // submission. Whether one or more is stale depends on the realm of the URL:
    //
    //   default: /admin/config/system/google_tag
    //   realm: /fr/admin/config/system/google_tag
    //
    // The result does not depend on whether any google_tag variables are
    // configured to be realm-specific.
    //
    // The global:default snippet files always reflect the NEW global values
    // plus the realm-specific values for the realm of the URL. If the form is
    // submitted from the 'en' (or default) URL, then these files have the 'en'
    // values.
    //
    // The language:en snippet files have OLD global values if the realm URL is
    // language:fr (indicated by 'fr' prefix), NEW values otherwise. Any realm-
    // specific values are from language:en.
    //
    // The language:fr snippet files always have OLD global values. Any realm-
    // specific values are from language:fr.

    // Load variables from cache or database.
    // Merge with current global to get values from settings.php.
    $variables = variable_initialize();
    $variables = array_merge($GLOBALS['conf'], $variables); // recursive?

    // Reset internal store with updated variables.
    $global = variable_realm_controller('global');
    $global->addStore('default', $variables);
  }

  // Set global to avoid message display to non-admin users on cache flush or cron.
  global $google_tag_display_message;
  $google_tag_display_message = TRUE;
  _google_tag_assets_create();
}

/**
 * Saves snippet files and data layer classes based on current settings.
 *
 * Use case: variable_realm is not enabled
 * - one set of snippet files stored in public://google_tag
 *
 * Use case: variable_realm is enabled
 * - multiple sets of snippet files stored in public://google_tag/{realm}
 * - sub case: no google_tag variables are realm-specific
 *   update all snippet files based on global default variables
 * - sub case: some google_tag variables are realm-specific
 *   update all snippet files based on global and realm-specific variables
 */
function _google_tag_assets_create() {
  if (module_exists('variable_realm') && module_exists('variable_store')) {
    // i18n_variable module depends on variable_realm, variable_store
    $realms = variable_realm_list();
    foreach ($realms as $realm_name => $realm_title) {
      $keys = variable_realm_keys($realm_name);
      foreach ($keys as $key_name => $key_title) {
        drupal_set_message(t('realm:key = @realm:@key', array('@realm' => $realm_name, '@key' => $key_name)));
        variable_realm_switch($realm_name, $key_name);
        if (_google_tag_directory_prepare($realm_name)) {
          _google_tag_snippets_save($realm_name, $key_name);
          _google_tag_classes_save($realm_name, $key_name);
        }
      }
    }
  }
  else {
    if (_google_tag_directory_prepare()) {
      _google_tag_snippets_save();
      _google_tag_classes_save();
    }
  }

  _drupal_flush_css_js();
  drupal_clear_js_cache();
}

/**
 * Prepares directory for base or realm specific snippet files.
 *
 * @return bool
 *   Whether the directory was prepared.
 */
function _google_tag_directory_prepare($realm_name = '') {
  // From google_tag_requirements(); this should be a helper function (in core).
  $directory = 'public://google_tag';
  $directory .= $realm_name ? "/$realm_name" : '';
  if (is_dir($directory) && _google_tag_is_writable($directory) && _google_tag_is_executable($directory)) {
    return TRUE;
  }
  if (__file_prepare_directory($directory, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    return TRUE;
  }

  // The snippet directory does not exist or is not writable or searchable.

  // If applicable, get the directory path of stream wrapper.
  $wrapper = file_stream_wrapper_get_instance_by_uri($directory);
  if (method_exists($wrapper, 'getDirectoryPath') && ($path = $wrapper->getDirectoryPath())) {
    // getDirectoryPath() is not defined in StreamWrapperInterface; it
    // exists in LocalStream and the local storage replacement classes in
    // google_appengine; s3fs returns an empty string.
    $path .= str_replace('public://', '/', $directory);
  }
  elseif (!($path = $wrapper->getExternalUrl())) {
    $path = $directory;
  }

  $args = array('%directory' => $path);
  $message = 'The directory %directory could not be prepared for use, possibly due to file system permissions. The directory either does not exist, or is not writable or searchable.';
  _google_tag_message_display($message, $args, 'error');
  watchdog('google_tag', $message, $args, WATCHDOG_ERROR);
  return FALSE;
}

/**
 * Saves JS snippet files based on current settings.
 *
 * @return bool
 *   Whether the files were saved.
 */
function _google_tag_snippets_save($realm_name = '', $realm_key = '') {
  $realm_string = $realm_name ? "$realm_name:$realm_key" : 'default';
  $realm_name .= $realm_name ? '/' : '';
  $realm_key .= $realm_key ? '.' : '';
  // Save the altered snippets after hook_google_tag_snippets_alter().
  module_load_include('inc', 'google_tag', 'includes/snippet');
  $result = TRUE;
  $snippets = google_tag_snippets();
  foreach ($snippets as $type => $snippet) {
    $path = file_unmanaged_save_data($snippet, "public://google_tag/{$realm_name}google_tag.$realm_key$type.js", FILE_EXISTS_REPLACE);
    $result = !$path ? FALSE : $result;
  }

  $args = array('@count' => count($snippets));
  if ($realm_name) {
    $args += array('@realm' => $realm_string);
    $message = $result
      ? 'Created @count snippet files for @realm realm based on configuration.'
      : 'An error occurred saving @count snippet files for @realm realm. Contact the site administrator if this persists.';
  }
  else {
    $message = $result
      ? 'Created @count snippet files based on configuration.'
      : 'An error occurred saving @count snippet files. Contact the site administrator if this persists.';
  }
  _google_tag_message_display($message, $args, $result ? 'status' : 'error');
  if (!$result) {
    watchdog('google_tag', $message, $args, WATCHDOG_ERROR);
  }
}

/**
 * Stores data layer classes based on current settings.
 */
function _google_tag_classes_save($realm_name = '', $realm_key = '') {
  $data_layer = variable_get('google_tag_data_layer', 'dataLayer');
  if (module_exists('datalayer') && $data_layer == 'dataLayer') {
    // Save classes for data layer.
    // @todo How to or can we set the name of the data layer?
    // This is an open issue on datalayer project; after it is implemented in
    // datalayer then remove second condition on if block.
    _google_tag_data_layer_snippet($classes);
    if ($realm_name) {
      variable_realm_set($realm_name, $realm_key, 'google_tag_data_layer_classes', $classes);
    }
    else {
      variable_set('google_tag_data_layer_classes', $classes);
    }
  }
}

/**
 * Cleans a string representing a list of items.
 *
 * @param string $text
 *   The string to clean.
 * @param string $format
 *   The final format of $text, either 'string' or 'array'.
 */
function google_tag_text_clean(&$text, $format = 'string') {
  $text = explode("\n", $text);
  $text = array_map('trim', $text);
  $text = array_filter($text, 'trim');
  if ($format == 'string') {
    $text = implode("\n", $text);
  }
}

/**
 * Verifies presence of dataLayer module and compares name of data layer.
 *
 * @return bool|null
 *   Whether data layer name is incompatible with dataLayer module, if present.
 */
function _google_tag_data_layer_verify($value) {
  if (module_exists('datalayer') && $value != 'dataLayer') {
    // @todo Setting form error does not allow user to save a different name,
    // i.e. to do what message text says.
    return t('The 1.1 release of the dataLayer module does not support a data layer name other than "dataLayer." If you need a different layer name, then either disable the dataLayer module or alter the JavaScript added to the page response.');
  }
}

/**
 * Checks that the directory exists and is writable.
 *
 * @todo Remove this function if core is updated to check the executable bit.
 *
 * @see file_prepare_directory()
 */
function __file_prepare_directory(&$directory, $options = FILE_MODIFY_PERMISSIONS) {
  if (!file_stream_wrapper_valid_scheme(file_uri_scheme($directory))) {
    // Only trim if we're not dealing with a stream.
    $directory = rtrim($directory, '/\\');
  }

  // Check if directory exists.
  if (!is_dir($directory)) {
    // Let mkdir() recursively create directories and use the default directory
    // permissions.
    if (($options & FILE_CREATE_DIRECTORY) && @drupal_mkdir($directory, NULL, TRUE)) {
      return drupal_chmod($directory);
    }
    return FALSE;
  }
  // The directory exists, so check to see if it is writable.
  $writable = _google_tag_is_writable($directory) && _google_tag_is_executable($directory);
  if (!$writable && ($options & FILE_MODIFY_PERMISSIONS)) {
    return drupal_chmod($directory);
  }

  return $writable;
}

/**
 * Determines whether a directory is writable.
 *
 * Remove this if PHP is_writable() is changed to respect ACLS on a 'local'
 * stream wrapper other than the local file wrapper provided by PHP.
 *
 * @param string $uri
 *   A directory path or stream wrapper URI.
 *
 * @return bool
 *   Whether the directory is writable.
 */
function _google_tag_is_writable($uri) {
  // Use the local path, if applicable, since PHP only checks ACLs on its local
  // file wrapper.
  $realpath = FALSE;
  if ($wrapper = file_stream_wrapper_get_instance_by_uri($uri)) {
    $realpath = $wrapper->realpath($uri);
  }
  return is_writable($realpath ? $realpath : $uri);
}

/**
 * Determines whether a directory is searchable.
 *
 * Remove this if PHP is_executable() is changed to not return FALSE simply
 * because the URI points to a directory (not a file) in a stream wrapper other
 * than the local file wrapper provided by PHP.
 *
 * @param string $uri
 *   A directory path or stream wrapper URI.
 *
 * @return bool
 *   Whether the directory is searchable.
 */
function _google_tag_is_executable($uri) {
  if ($wrapper = file_stream_wrapper_get_instance_by_uri($uri)) {
    if ($realpath = $wrapper->realpath($uri)) {
      // The URI is a local stream wrapper.
      // Use local path since PHP only checks ACLs on its local file wrapper.
      return is_executable($realpath);
    }

    // The URI is a remote stream wrapper.
    if (!($stat = $wrapper->url_stat($uri, 0))) {
      return FALSE;
    }

    // Determine the appropriate permissions bit mask as an octal.
    // The stat array is likely to have uid=gid=0 so that the mask is octal 01.
    // This is true for Amazon S3 and Google Cloud Storage.
    $mask = 1;
    if ($stat['uid'] == posix_getuid()) {
      $mask = $mask << 6;
    }
    elseif ($stat['gid'] == posix_getgid()) {
      $mask = $mask << 3;
    }
    return ($stat['mode'] & $mask) != 0;
  }
  else {
    // The URI is a local path.
    return is_executable($uri);
  }
}

/**
 * Displays a message to admin users.
 *
 * @see arguments to t() and drupal_set_message()
 */
function _google_tag_message_display($message, $args = array(), $type = 'status') {
  global $google_tag_display_message;
  if ($google_tag_display_message) {
    drupal_set_message(t($message, $args), $type);
  }
}
