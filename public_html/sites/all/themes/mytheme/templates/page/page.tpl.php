
<div class="site-wrapper">
  <div class="site-wrapper-shadow">

<div id="header-wrapper">
  <header id="header" role="banner">
    <div class="top">
	<?php if ($logo): ?>
      <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" id="logo">
         <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
      </a>
    <?php endif; ?>
    <?php print render($page['header']); ?>
    </div>
    <div id="navigation"><?php print render($page['navigation']); ?></div><!-- /#navigation -->
  </header>
</div>


  

<div id="pagetop-wrapper"><div id="pagetop">
    <?php if ($page['pagetop']) print render($page['pagetop']); ?>
    <?php if ($top_title): ?><div class="top_title"><?php print $top_title; ?></div><?php endif; ?>
</div></div>
 


<?php if ($breadcrumb && $breadcrumb_view): ?>
  <div id="breadcrumb">
    <?php print $breadcrumb; ?>
  </div>
<?php endif; ?>

<div id="main-wrapper">
  <div id="main">
    <div id="content" class="column" role="main">
      <?php print render($page['highlighted']); ?>   
      
      <?php if ($title && $title_view): ?>
         <div id="page-title-wrapper">
            <?php if ($addthis_view) print $addthis; ?>
            <h1 id="page-title" class="title title-2-line"><?php print $title; ?></h1>            
         </div>
	  <?php endif; ?>
    
      <?php if ($messages): ?><div id="page-messages"><?php print $messages; ?></div><?php endif; ?>
      <?php print render($tabs); ?>
      <?php print render($page['help']); ?>
      <?php if ($action_links): ?><ul class="action-links"><?php print render($action_links); ?></ul><?php endif; ?>
   
      <?php $content_top = render($page['content_top']); if ($content_top): ?>
        <div id="content_top"><?php print $content_top; ?></div>
      <?php endif; ?>  
      
      <div id="content-content"><?php print render($page['content']); ?></div>
  
      <?php $content_bottom = render($page['content_bottom']); if ($content_bottom): ?>
        <div id="content_bottom"><?php print $content_bottom; ?></div>
      <?php endif; ?>  

      <?php 
	  if (arg(0) != 'cart') {
		print l('', 'cart', array(
		      'attributes' => array(
		        'id' => 'popap-linka-kart',
		        'class' => array('cart', 'colorbox-node'),
				'rel' => array('cart'),
			  ),
			  'query'=>array('width'=>'680', 'height'=>'800'),
			));
	  }
	  ?> 
    </div><!-- /#content -->

    <?php
      $sidebar_first  = render($page['sidebar_first']);
      $sidebar_second = render($page['sidebar_second']);
    ?>

    <?php if ($sidebar_first || $sidebar_second): ?>
      <aside class="sidebars">
        <?php print $sidebar_first; ?>
        <?php print $sidebar_second; ?>
      </aside><!-- /.sidebars -->
    <?php endif; ?>

  </div><!-- /#main -->  
</div>

<?php $contentbottom = render($page['contentbottom']); if ($contentbottom): ?>
  <div id="contentbottom-wrapper"><div id="contentbottom"><?php print $contentbottom; ?></div></div>
<?php endif; ?> 

<?php $pagebottom = render($page['pagebottom']); if ($pagebottom): ?>
  <div id="pagebottom-wrapper"><div id="pagebottom"><?php print $pagebottom; ?></div></div>
<?php endif; ?>  

<div id="footer-wrapper">
  <?php print render($page['footer']); ?>
</div>
  
<div id="copyright-wrapper"><div id="copyright"><?php print render($page['copyright']); ?></div></div>


<?php print render($page['bottom']); ?>

  </div>
</div>
