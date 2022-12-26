<div id="<?php print $block_html_id; ?>" class="<?php print $classes; ?>"<?php print $attributes; ?>>

<?php 
$dva_line = true;
?>

<?php print render($title_prefix); ?>
<?php 
     if ($title) {
		$title = trim($title); 
	    if ($dva_line) {
			$pos = mb_strpos($title, ' ');
			if ($pos === FALSE) {
			   print '<h2 '.$title_attributes.'>'.$title.'</h2>';
		    } else {
			   $srt1 = drupal_substr($title, 0, $pos);
			   $srt2 = drupal_substr($title, $pos+1);
			   print '<h2 class="block-title title-2-line"><span>'.$srt1.' </span> '.$srt2.'</h2>';
			}
		} else {
			print '<h2 '.$title_attributes.'>'.$title.'</h2>';
	    }	 
	 } 
?>
<?php print render($title_suffix); ?>

<?php print $content; ?>

</div><!-- /.block -->
