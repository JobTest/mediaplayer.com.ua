(function ($) {

  Drupal.behaviors.jQueryFormStyler = {
    attach: function (context, settings) {
      var selector = Drupal.settings.jqueryformstyler['class'];
      $(selector, context).styler();
    }
  };

})(jQuery);
