<?php



 

/**
 * Implements hook_variable_info().
 */
function myvariable_variable_info($options) {

    $variables['myvariable_text'] = array(
    'type' => 'text',
    'title' => t('Simple text', array(), $options),
    'default' => 'Example text.',
    'description' => t('Example of text variable.', array(), $options),
    'required' => TRUE,
    'group' => 'myvariable',
  );

  $variables['myvariable_number'] = array(
    'type' => 'number',
    'title' => t('Number', array(), $options),
    'default' => 0,
    'description' => t('Example of numeric variable.', array(), $options),
    'required' => TRUE,
    'group' => 'myvariable',
  );



  $variables['myvariable_file'] = array(
    'type'              => 'uploadfile',
    'title'             => t('My Upload File', array(), $options),
    'description'       => t('Upload my config file.', array(), $options),
   // 'required'          => TRUE,
    'upload_location'   => 'public://myuploaddir',
    'upload_validators' => array(
      'file_validate_extensions' => array('gif png jpg jpeg'),
    ),
    'group'             => 'myvariable',
  );




  
 /*
   $variables['myvariable_string'] = array(
    'type' => 'string',
    'title' => t('string', array(), $options),
    'default' => '',
    'description' => t('Example of string variable.', array(), $options),
    'required' => TRUE,
    'group' => 'myvariable',
  );  
 
 
  $variables['myvariable_text_format'] = array(
    'type' => 'text_format',
    'title' => t('Text format', array(), $options),
    'default' => 'Example text with default format',
    'description' => t('Example of text variable with text format.', array(), $options),
    'required' => TRUE,
    'group' => 'myvariable',
  );
  $variables['myvariable_mail_[mail_part]'] = array(
    'type' => 'mail_text',
    'title' => t('Example mail', array(), $options),
    'default' => array(
      'subject' => t('Example mail subject', array(), $options),
      'body' => t('Example mail body.', array(), $options),
    ),
    'description' => t('Example mail variable with subject and body.', array(), $options),
    'required' => TRUE,
    'group' => 'myvariable',
  );
  */
  return $variables;
}

/**
 * Implements hook_variable_group_info().
 */
function myvariable_variable_group_info() {
  $groups['myvariable'] = array(
    'title' => t('My variable group'),
    'description' => t('My variable group.'),
    'access' => 'myvariable edit',
    'path' => array('admin/config/system/variable/example'),
  );
  return $groups;
}

