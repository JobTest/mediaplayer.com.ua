<div id="<?php print $block_html_id; ?>" class="<?php print $classes; ?>"<?php print $attributes; ?>>
   
   <?php print render($title_prefix); ?>
      <h2 style="display:none;">Сравнение товаров</h2>
   <?php print render($title_suffix); ?>
   
   <div class="content"<?php print $content_attributes; ?>>
      <?php print $content; ?>
   </div>
  
</div><!-- /.block -->
