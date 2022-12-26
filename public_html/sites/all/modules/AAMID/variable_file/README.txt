
Drupal module: Variable File
===========================

Variable File module will provide uploadfile type for Variable module.
Variable File allow you upload file as configuration, support thumbnail when
file has filemime is image/jpeg.

Note: when you load vairable with type uploadfile, it just a file id, you need
use file_load to load other properties of file.

How?
===========================
Implement hook_variable_info() with type is uploadfile.

/**
 * Implements hook_variable_info().
 */
function hook_variable_info($options) {
  $variables['myuploadfile'] = array(
    'type'              => 'uploadfile',
    'title'             => t('My Upload File', array(), $options),
    'description'       => t('Upload my config file.', array(), $options),
    'required'          => TRUE,
    'upload_location'   => 'public://myuploaddir',
    'upload_validators' => array(
      'file_validate_extensions' => array('gif png jpg jpeg'),
    ),
    'group'             => 'variable_example',
  );

  return $variables;
}

More information about hook_variable_info and config you can look at Variable
module.

Module developed by zipme_hkt (Huynh Khac Thao).
Email: flasvnn@gmail.com.
